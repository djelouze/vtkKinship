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

   this->CachedInput = vtkSmartPointer<vtkPolyData>::New( );
   this->WarpFilter = vtkSmartPointer<vtkWarpVector>::New( );
   this->ProbeFilter = vtkSmartPointer<vtkProbeFilter>::New( );
}


void vtkDeformableMesh::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);


}


int vtkDeformableMesh::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
   // get the info objects
   vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
   vtkInformation *outInfo = outputVector->GetInformationObject(0);

   if( this->IterateFromZero == 1 || this->CurrentIteration == 0 )
   {
      // Copy input
      // this->CachedInput->DeepCopy( 
      // Reset current iteration
      this->CurrentIteration = 0;
   }
      
   while( this->CurrentIteration < this->NumberOfIterations )
   {
      

      
   }    
  

}
