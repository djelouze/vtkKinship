#include "vtkThickTubeFilter.h"

#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"

vtkCxxRevisionMacro(vtkThickTubeFilter, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkThickTubeFilter);

vtkThickTubeFilter::vtkThickTubeFilter()
{
   this->NumberOfSides = 6;
   this->InnerRadius = 1;
   this->OuterRadius = 2;
}


void vtkThickTubeFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

int vtkThickTubeFilter::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
   // get the info objects
   vtkInformation *inMeshInfo = inputVector[0]->GetInformationObject(0);
   vtkInformation *outMeshInfo = outputVector->GetInformationObject(0);

   vtkPolyData* inputMesh = vtkPolyData::SafeDownCast(
    inMeshInfo->Get(vtkDataObject::DATA_OBJECT()));
   vtkPolyData* outputMesh = vtkPolyData::SafeDownCast(
    outMeshInfo->Get(vtkDataObject::DATA_OBJECT()));

   vtkPolyData* shallowInput = vtkPolyData::New( );
   shallowInput->ShallowCopy( inputMesh );

   vtkTubeFilter* innerTubeFilter = vtkTubeFilter::New( );
   innerTubeFilter->SetInput( shallowInput );
   innerTubeFilter->SetRadius( this->InnerRadius );
   innerTubeFilter->SetNumberOfSides( this->NumberOfSides );
   innerTubeFilter->CappingOff( );

   vtkTubeFilter* outerTubeFilter = vtkTubeFilter::New( );
   outerTubeFilter->SetInput( shallowInput );
   outerTubeFilter->SetRadius( this->OuterRadius );
   outerTubeFilter->SetNumberOfSides( this->NumberOfSides );
   outerTubeFilter->CappingOff( );

   vtkAppendPolyData* append = vtkAppendPolyData::New( );
   append->AddInputConnection( innerTubeFilter->GetOutputPort( ));   
   append->AddInputConnection( outerTubeFilter->GetOutputPort( ));   
   append->Update( );

   // At this point, outputMesh contains two non-connected tubes
   outputMesh->DeepCopy( append->GetOutput( ) );

   // Get the number of point of a cylinder ('spine' length x num. sides)
   vtkPolyData* innerTube = static_cast<vtkPolyData*>(innerTubeFilter->GetOutput( ));
   int length = innerTube->GetNumberOfPoints( );

   vtkCellArray* outputTriangles = outputMesh->GetPolys( );
   for( int ptId = 0; ptId < this->NumberOfSides; ptId ++)
   {
      // Triangle one extremity
      vtkIdType triangle[3];
      triangle[0] = ptId;
      triangle[1] = ptId + length;
      triangle[2] = (ptId + 1)%this->NumberOfSides + length;
      outputTriangles->InsertNextCell( 3, triangle );

      triangle[1] = (ptId+1)%this->NumberOfSides;
      triangle[2] = ptId;
      triangle[0] = (ptId + 1)%this->NumberOfSides + length;
      outputTriangles->InsertNextCell( 3, triangle );
 
      // Triangle the other extremity
      int offset = length - this->NumberOfSides;
      triangle[0] = ptId + offset;
      triangle[1] = ptId + +offset + length;
      triangle[2] = (ptId + 1)%this->NumberOfSides + offset + length;
      outputTriangles->InsertNextCell( 3, triangle );

      triangle[1] = (ptId+1)%this->NumberOfSides + offset;
      triangle[2] = ptId + offset;
      triangle[0] = (ptId + 1)%this->NumberOfSides + length + offset;
      outputTriangles->InsertNextCell( 3, triangle );
   }

   return( 1 );
}
