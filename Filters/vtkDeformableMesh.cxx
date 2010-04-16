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

   this->WarpFilter = vtkSmartPointer<vtkWarpVector>::New( );
   this->ProbeFilter = vtkSmartPointer<vtkProbeFilter>::New( );
   
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

   vtkSmartPointer<vtkImageData> cachedImage = vtkSmartPointer<vtkImageData>::New( );
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
   this->WarpFilter->SetScaleFactor( this->GetScaleFactor( ) );
   this->WarpFilter->Update( );
   this->ProbeFilter->Modified( );
}

