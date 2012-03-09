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

#include "vtkRegularizedDeformableMesh.h"

#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"

vtkCxxRevisionMacro(vtkRegularizedDeformableMesh, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkRegularizedDeformableMesh);

vtkRegularizedDeformableMesh::vtkRegularizedDeformableMesh()
{
   this->SetNumberOfInputPorts( 2 );

   this->RegularizationFilter = vtkSmartPointer<vtkSmoothPolyDataVectors>::New( );
   this->Normals = vtkSmartPointer<vtkPolyDataNormals>::New( );
   this->WarpFilter = vtkSmartPointer<vtkWarpVector>::New( );
   this->ProbeFilter = vtkSmartPointer<vtkProbeFilter>::New( );
   

   this->Normals->SetInputConnection( this->ProbeFilter->GetOutputPort( ) );
   this->RegularizationFilter->SetInputConnection( this->Normals->GetOutputPort( ) );
   this->WarpFilter->SetInputConnection( this->RegularizationFilter->GetOutputPort( ) );
}


void vtkRegularizedDeformableMesh::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);


}

void vtkRegularizedDeformableMesh::SetNumberOfSmoothingIterations(int nbIte )
{
   this->RegularizationFilter->SetNumberOfIterations( nbIte );
   this->Modified( );
}

//---------------------------------------------------------------------------
int vtkRegularizedDeformableMesh::FillInputPortInformation(int port, vtkInformation *info)
{
  if( port == 0 ) // input mesh port
     info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData");
  else if( port == 1 ) // image port
     info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkImageData");
  return 1;
}


void vtkRegularizedDeformableMesh::Reset( vtkInformationVector** inputVector )
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
   
   this->ProbeFilter->SetInputData( this->GetCachedInput() );
   this->ProbeFilter->SetInputArrayToProcess( 0, inImageInfo ); 
   this->ProbeFilter->SetSourceData( cachedImage );

   this->RegularizationFilter
       ->SetInputArrayToProcess( 0, this->ProbeFilter
                                        ->GetOutputPortInformation( 0 ) );


   this->WarpFilter
       ->SetInputArrayToProcess( 0, 0, 0, 0, "SmoothedVectors");

   this->SetIterativeOutput( static_cast<vtkPolyData*>(this->WarpFilter
                                                           ->GetOutput( )) );
}

void vtkRegularizedDeformableMesh::IterativeRequestData(
  vtkInformationVector **inputVector)
{
   this->WarpFilter->SetScaleFactor( this->GetScaleFactor( ) );
   this->WarpFilter->Update( );
   this->ProbeFilter->Modified( );
}

