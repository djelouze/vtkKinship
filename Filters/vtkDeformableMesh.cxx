#include "vtkDeformableMesh.h"

#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"

vtkCxxRevisionMacro(vtkDeformableMesh, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkDeformableMesh);

vtkDeformableMesh::vtkDeformableMesh()
{
   this->SetNumberOfInputPorts( 2 );

   this->IterateFromZero = 1;
   this->NumberOfIterations = 0;
   this->CurrentIteration = 0;

   this->CachedInput = vtkPolyData::New( );
   this->WarpFilter = vtkWarpVector::New( );
   this->ProbeFilter = vtkProbeFilter::New( );
   
   this->WarpFilter->SetInputConnection( this->ProbeFilter->GetOutputPort( ) );
}


void vtkDeformableMesh::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);


}

//---------------------------------------------------------------------------
int vtkDeformableMesh::FillInputPortInformation(int port, vtkInformation *info)
{
  if( port == 0 ) // input mesh port
     info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData");
  else if( port == 1 ) // image port
     info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkImageData");
  return 1;
}

int vtkDeformableMesh::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
   // get the info objects
   vtkInformation *inMeshInfo = inputVector[0]->GetInformationObject(0);
   vtkInformation *inImageInfo = inputVector[1]->GetInformationObject(0);

   vtkInformation *outMeshInfo = outputVector->GetInformationObject(0);

   vtkPolyData* inputMesh = vtkPolyData::SafeDownCast(
    inMeshInfo->Get(vtkDataObject::DATA_OBJECT()));
   vtkImageData* inputImage = vtkImageData::SafeDownCast(
    inImageInfo->Get(vtkDataObject::DATA_OBJECT()));
   vtkPolyData* outputMesh = vtkPolyData::SafeDownCast(
    outMeshInfo->Get(vtkDataObject::DATA_OBJECT()));

   vtkImageData* cachedImage = vtkImageData::New( );
   cachedImage->ShallowCopy( inputImage );

   if( this->IterateFromZero == 1 || this->CurrentIteration == 0 )
   {
      // Copy input
      this->CachedInput->DeepCopy( inputMesh );
      // Reset current iteration
      this->CurrentIteration = 0;
   }

   this->ProbeFilter->SetInput( this->CachedInput );
   this->ProbeFilter->SetSource( cachedImage );

   this->WarpFilter->SetInputArrayToProcess( 0,
                              this->ProbeFilter->GetOutputPortInformation( 0 ) );

   while( this->CurrentIteration < this->NumberOfIterations )
   {
      this->WarpFilter->Update( );
      
      this->CachedInput->DeepCopy( static_cast<vtkPolyData*>(this->WarpFilter
                                                             ->GetOutput( )) );
      this->ProbeFilter->Modified( );
      this->CurrentIteration ++;      
   }    
  
   outputMesh->DeepCopy( static_cast<vtkPolyData*>(this->WarpFilter
                                                       ->GetOutput( )) );

   return( 1 );
}
