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


#include "vtkImageCropVOI.h"

#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkImageData.h"
#include "vtkSphere.h"
#include "vtkCallbackCommand.h"
#include "vtkStreamingDemandDrivenPipeline.h"

#include <algorithm>

#include "vtkWin32OutputWindow.h"

vtkStandardNewMacro(vtkImageCropVOI);

vtkImageCropVOI::vtkImageCropVOI()
{
   this->Margin = 0;
   this->Sphere = 0x0;
   this->SphereModifiedCommand = vtkCallbackCommand::New();
   this->SphereModifiedCommand->SetClientData( this );
   this->SphereModifiedCommand->SetCallback( this->SphereModifiedCallback );

   vtkWin32OutputWindow::SafeDownCast( vtkOutputWindow::GetInstance() )->SendToStdErrOn();
}


vtkImageCropVOI::~vtkImageCropVOI()
{
  this->SphereModifiedCommand->Delete();
}

void vtkImageCropVOI::SetSphere(vtkSphere* s)
{
  if( this->Sphere )
  {
    this->Sphere->RemoveObservers(vtkCommand::ModifiedEvent, this->SphereModifiedCommand );
  }

  this->Sphere = s;
  this->Sphere->AddObserver( vtkCommand::ModifiedEvent, this->SphereModifiedCommand );
  this->Modified();
}

void vtkImageCropVOI::SphereModifiedCallback(
  vtkObject *caller,
  unsigned long eid,
  void *clientdata,
  void *calldata )
{
  vtkImageCropVOI* self = static_cast<vtkImageCropVOI*>(clientdata);
  self->Modified();
}

int vtkImageCropVOI::RequestUpdateExtent(
  vtkInformation *request,
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector )
{
  vtkInformation *inInfo = inputVector[0]->GetInformationObject( 0 );
  vtkInformation *outInfo = outputVector->GetInformationObject( 0 );
  int inExtent[6], outExtent[6], maxExtent[6];
  inInfo->Get( vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), inExtent );
  outInfo->Get( vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT(), outExtent );

  for( int i = 0; i < 3; i++ )
  {
    maxExtent[i*2] = std::max( inExtent[i*2], outExtent[i*2] );
    maxExtent[i*2 + 1] = std::min( inExtent[i*2 + 1], outExtent[i*2 + 1] );
  }

  inInfo->Set( vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT(), maxExtent, 6 );

  return(1);
}


int vtkImageCropVOI::RequestInformation(
  vtkInformation *request,
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject( 0 );

  // get the input and ouptut
  double spacing[3], origin[3];
  int wholeExtent[6];

  inInfo->Get( vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), wholeExtent );
  inInfo->Get( vtkDataObject::SPACING(), spacing );
  inInfo->Get( vtkDataObject::ORIGIN(), origin );
 
  double boundingBox[6];
  double center[3];
  this->Sphere->GetCenter(center);
  double radius = this->Sphere->GetRadius();
  for( int i = 0; i < 3; i++ )
  {
    boundingBox[i*2] = center[i] - radius;
    boundingBox[i*2 + 1] = center[i] + radius;
  }
  
  for( int comp = 0; comp < 3; comp++ )
  {
    this->VOI[comp*2] = (boundingBox[comp*2] - origin[comp]) / spacing[comp] - this->Margin;
    this->VOI[comp*2] = std::max( this->VOI[comp*2], wholeExtent[comp*2] );
    this->VOI[comp*2 + 1] = (boundingBox[comp*2 + 1] - origin[comp]) / spacing[comp] + this->Margin;
    this->VOI[comp*2 + 1] = std::min( this->VOI[comp*2 + 1], wholeExtent[comp*2 + 1] );
  }

  outInfo->Set( vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), this->VOI,6 );
  outInfo->Set( vtkDataObject::SPACING(), spacing, 3 );
  outInfo->Set( vtkDataObject::ORIGIN(), origin, 3 );
  return(1);
}

