// Copyright (c) 2010, Jérôme Velut
// All rights reserved.
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of the University of California, Berkeley nor the
//       names of its contributors may be used to endorse or promote products
//       derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

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

