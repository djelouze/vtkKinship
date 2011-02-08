// Copyright (c) 2010, Jérôme Velut
// All rights reserved.
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT OWNER ``AS IS'' AND ANY EXPRESS 
// OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN 
// NO EVENT SHALL THE COPYRIGHT OWNER BE LIABLE FOR ANY DIRECT, INDIRECT, 
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
// OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


#include "vtkIterativePolyDataAlgorithm.h"

#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"

vtkCxxRevisionMacro(vtkIterativePolyDataAlgorithm, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkIterativePolyDataAlgorithm);

vtkIterativePolyDataAlgorithm::vtkIterativePolyDataAlgorithm()
{
   this->IterateFromZero = 1;
   this->NumberOfIterations = 0;
   this->CurrentIteration = 0;

   this->CachedInput = vtkSmartPointer<vtkPolyData>::New( );

   // Default behaviour: iterating on itself.
   this->SetIterativeOutput( this->CachedInput );
}


void vtkIterativePolyDataAlgorithm::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "IterateFromZero: " << this->IterateFromZero << endl;
  os << indent << "NumberOfIterations: " << this->NumberOfIterations << endl;
  os << indent << "CurrentIteration: " << this->CurrentIteration << endl;
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
  
  if( this->NumberOfIterations == 0 )
    outputMesh->DeepCopy( inputMesh );
  else
   outputMesh->DeepCopy( this->IterativeOutput );

   return( 1 );
}
