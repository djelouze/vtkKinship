#include "vtkIterativePolyDataAlgorithm.h"

#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"

vtkCxxRevisionMacro(vtkIterativePolyDataAlgorithm, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkIterativePolyDataAlgorithm);

vtkIterativePolyDataAlgorithm::vtkIterativePolyDataAlgorithm()
{
   this->SetNumberOfInputPorts( 2 );

   this->IterateFromZero = 1;
   this->NumberOfIterations = 0;
   this->CurrentIteration = 0;

   this->CachedInput = vtkPolyData::New( );
   // Default behaviour: iterating on itself.
   this->SetIterativeOutput( this->CachedInput );
}


void vtkIterativePolyDataAlgorithm::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);


}

//---------------------------------------------------------------------------
int vtkIterativePolyDataAlgorithm::FillInputPortInformation(int port, vtkInformation *info)
{
  if( port == 0 ) // input mesh port
     info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData");
  return 1;
}

int vtkIterativePolyDataAlgorithm::RequestData(
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

   if(   this->IterateFromZero == 1  // Explicit user reset
      || this->CurrentIteration == 0 // First iteration
      || this->NumberOfIterations < this->CurrentIteration // Overflow
      || this->NumberOfIterations == 0 // Animation reset
     )
   {
      // Copy input
      this->CachedInput->DeepCopy( inputMesh );
      // Reset current iteration
      this->CurrentIteration = 0;
      // User define initial condition
      this->Reset( inputVector );
   }


   while( this->CurrentIteration < this->NumberOfIterations )
   {
      // Effective call to the iterative algorithm. Child classes
      // should override this function
      this->IterativeRequestData( inputVector ); 

      this->CachedInput->DeepCopy( this->IterativeOutput );
      this->CurrentIteration ++;      
   }    
  
   outputMesh->DeepCopy( this->IterativeOutput );

   return( 1 );
}
