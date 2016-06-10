#include "vtkHistoryManager.h"

#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"

vtkStandardNewMacro(vtkHistoryManager);

vtkHistoryManager::vtkHistoryManager()
{
  this->SetNumberOfInputPorts( 0 );
  this->CurrentOutput = -1;
}


void vtkHistoryManager::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "CurrentOutput: " << this->CurrentOutput << endl;
}



void vtkHistoryManager::AddInput( vtkPolyData* newInput )
{
  vtkSmartPointer<vtkPolyData> localNewInput;
  localNewInput = vtkSmartPointer<vtkPolyData>::New();
  localNewInput->Register( this );
  localNewInput->DeepCopy( newInput );
  this->Inputs.push_back( localNewInput );
  this->CurrentOutput = this->Inputs.size() - 1;
  this->Modified();
}


void vtkHistoryManager::ResetInputs()
{
  for( vtkPolyData* mesh : this->Inputs )
  {
    mesh->Delete();
  }
  this->Inputs.clear();
  this->CurrentOutput = -1;
}

int vtkHistoryManager::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
   // get the info objects
   vtkInformation *outMeshInfo = outputVector->GetInformationObject(0);

   vtkPolyData* outputMesh = vtkPolyData::SafeDownCast(
    outMeshInfo->Get(vtkDataObject::DATA_OBJECT()));

   if( this->CurrentOutput < this->Inputs.size() && this->CurrentOutput >= 0)
   {
     outputMesh->ShallowCopy( this->Inputs[this->CurrentOutput] );
   }



   return( 1 );
}
