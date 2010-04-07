#include "vtkImageCropVOI.h"

#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPolyData.h"
#include "vtkImageData.h"

vtkCxxRevisionMacro(vtkImageCropVOI, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkImageCropVOI);

vtkImageCropVOI::vtkImageCropVOI()
{
   this->SetNumberOfInputPorts( 2 );
   this->Margin = 0;
}


vtkImageCropVOI::~vtkImageCropVOI()
{

}


int vtkImageCropVOI::FillInputPortInformation(int port, vtkInformation *info)
{
  if( port == 0 )
     info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkImageData");
  else if( port == 1 )
     info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataSet");
  else
     return 0;
  return 1;
}

void vtkImageCropVOI::SetBoundsConnection(int id, vtkAlgorithmOutput* algOutput)
{
  if (id < 0)
    {
    vtkErrorMacro("Bad index " << id << " for source.");
    return;
    }

  int numConnections = this->GetNumberOfInputConnections(1);
  if (id < numConnections)
    {
    this->SetNthInputConnection(1, id, algOutput);
    }
  else if (id == numConnections && algOutput)
    {
    this->AddInputConnection(1, algOutput);
    }
  else if (algOutput)
    {
    vtkWarningMacro("The source id provided is larger than the maximum "
                    "source id, using " << numConnections << " instead.");
    this->AddInputConnection(1, algOutput);
    }
}


int vtkImageCropVOI::RequestInformation(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *boundsInfo = inputVector[1]->GetInformationObject(0);

  // get the input and ouptut
  vtkImageData *input = vtkImageData::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkDataSet *bounds = vtkDataSet::SafeDownCast(
    boundsInfo->Get(vtkDataObject::DATA_OBJECT()));
 
  double boundingBox[6];
  bounds->ComputeBounds( );
  bounds->GetBounds( boundingBox );
  
  double spacing[3], origin[3];
  input->GetSpacing( spacing );
  input->GetOrigin( origin );
  
  int voi[6];
  for( int comp = 0; comp < 6 ; comp += 2 )
     voi[comp] = ( boundingBox[comp] - origin[(int)(comp / 2)]  ) / spacing[(int)(comp / 2)] - Margin ;
  for( int comp = 1; comp < 6 ; comp += 2 )
     voi[comp] = ( boundingBox[comp] - origin[(int)(comp / 2)] ) / spacing[(int)(comp / 2)] + Margin ;

  this->SetVOI( voi );

  vtkInformation* request = 0; // null initialization to avoid warning at build time
  this->Superclass::RequestInformation( request, inputVector, outputVector );
  
  return 1;
}

