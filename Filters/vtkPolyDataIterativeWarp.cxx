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

#include "vtkPolyDataIterativeWarp.h"

#include "vtkWarpVector.h"
#include "vtkBrownianPoints.h"

#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"

vtkCxxRevisionMacro(vtkPolyDataIterativeWarp, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkPolyDataIterativeWarp);

vtkPolyDataIterativeWarp::vtkPolyDataIterativeWarp()
{
   this->SetNumberOfInputPorts( 1 );

   this->WarpFilter = vtkSmartPointer<vtkWarpVector>::New( );
   this->BrownianPoints = vtkSmartPointer<vtkBrownianPoints>::New( );
   
   this->ScaleFactor = 1.0;
}


void vtkPolyDataIterativeWarp::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "ScaleFactor: " << this->GetScaleFactor() << endl;
}

//---------------------------------------------------------------------------
int vtkPolyDataIterativeWarp::FillInputPortInformation(int port, vtkInformation *info)
{
  if( port == 0 ) // input mesh port
     info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData");
  return 1;
}


void vtkPolyDataIterativeWarp::Reset( vtkInformationVector** inputVector )
{
   // get the info objects
   vtkInformation *inMeshInfo = inputVector[0]->GetInformationObject(0);

   vtkPolyData* inputMesh = vtkPolyData::SafeDownCast(
   inMeshInfo->Get(vtkDataObject::DATA_OBJECT()));

   // Prepare the internal pipeline
   this->BrownianPoints->SetInput( this->GetCachedInput() );
   this->WarpFilter->SetInputConnection( this->BrownianPoints->GetOutputPort( ) );
   // Set the IterativeOutput as the output of the warpfilter. Next iteration
   // will begin by a DeepCopy of this Output to the CachedInput
   this->SetIterativeOutput( static_cast<vtkPolyData*>(this->WarpFilter->GetOutput( )) );
}

void vtkPolyDataIterativeWarp::IterativeRequestData(
  vtkInformationVector **inputVector)
{
   // Iterative process : simple warp based on the brownian
   // vectors generated.
   // The CachedInput is modified at each iteration, making the 
   // VTK pipeline update mechanism effective.
   
   // The call to SetScaleFactor allows to change this parameter
   // while iterating.
   this->WarpFilter->SetScaleFactor( this->GetScaleFactor() );
   this->WarpFilter->Update( );
}

