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


void vtkDeformableMesh::Reset( vtkInformationVector** inputVector )
{
   // get the info objects
   vtkInformation *inMeshInfo = inputVector[0]->GetInformationObject(0);
   vtkInformation *inImageInfo = inputVector[1]->GetInformationObject(0);

   vtkPolyData* inputMesh = vtkPolyData::SafeDownCast(
    inMeshInfo->Get(vtkDataObject::DATA_OBJECT()));
   vtkImageData* inputImage = vtkImageData::SafeDownCast(
    inImageInfo->Get(vtkDataObject::DATA_OBJECT()));

   vtkImageData* cachedImage = vtkImageData::New( );
   cachedImage->ShallowCopy( inputImage );
   
   this->ProbeFilter->SetInput( this->GetCachedInput() );
   this->ProbeFilter->SetSource( cachedImage );

   this->WarpFilter->SetInputArrayToProcess( 0,
                              this->ProbeFilter->GetOutputPortInformation( 0 ) );
   this->SetIterativeOutput( static_cast<vtkPolyData*>(this->WarpFilter->GetOutput( )) );
}

void vtkDeformableMesh::IterativeRequestData(
  vtkInformationVector **inputVector)
{
   this->WarpFilter->Update( );
   this->ProbeFilter->Modified( );
}

