#include "vtkHistoryManager.h"

#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"

vtkStandardNewMacro(vtkHistoryManager);

vtkHistoryManager::vtkHistoryManager()
{
   this->CurrentOutput = -1;
}


void vtkHistoryManager::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "CurrentOutput: " << this->CurrentOutput << endl;
}

//---------------------------------------------------------------------------
int vtkHistoryManager::FillInputPortInformation(int port, vtkInformation *info)
{
  if( port == 0 ) // input mesh port
     info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData");
  return 1;
}

void vtkHistoryManager::ResetInputs()
{
  for( vtkPolyData* mesh : this->Inputs )
  {
    mesh->Delete();
  }
  this->Inputs.clear();
}

int vtkHistoryManager::RequestData(
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

   if( this->CurrentOutput == -1 )
   {
     vtkSmartPointer<vtkPolyData> newInput;
     newInput = vtkSmartPointer<vtkPolyData>::New();
     newInput->Register( this );
     newInput->DeepCopy( inputMesh );
     this->Inputs.push_back( newInput );
     outputMesh->ShallowCopy( inputMesh );
     this->CurrentOutput = this->Inputs.size() - 1;
   }
   else
   {
     outputMesh->ShallowCopy( this->Inputs[this->CurrentOutput] );
   }


   return( 1 );
}
