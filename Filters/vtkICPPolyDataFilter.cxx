// Copyright (c) 2010-2011, Jérôme Velut
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

#include <vtkICPPolyDataFilter.h>

#include <vtkObjectFactory.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkSmartPointer.h>
#include <vtkIterativeClosestPointTransform.h>
#include <vtkLandmarkTransform.h>


vtkStandardNewMacro(vtkICPPolyDataFilter);
vtkCxxRevisionMacro(vtkICPPolyDataFilter, "$Revision$");


vtkICPPolyDataFilter::vtkICPPolyDataFilter()
{

  this->Transform = vtkIterativeClosestPointTransform::New();
  this->SetNumberOfInputPorts(2);
}


vtkICPPolyDataFilter::~vtkICPPolyDataFilter()
{

  this->Transform->Delete();
}


void vtkICPPolyDataFilter::SetSourceConnection(vtkAlgorithmOutput* input)
{
  this->SetInputConnection(0, input);
}


void vtkICPPolyDataFilter::SetTargetConnection(vtkAlgorithmOutput* input)
{
  this->SetInputConnection(1, input);
}


int vtkICPPolyDataFilter::FillInputPortInformation( int port, vtkInformation* info )
{
  if (!this->Superclass::FillInputPortInformation(port, info))
    {
    return 0;
    }
  if ( port == 0 )
    {
    info->Set( vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData" );
    return 1;
    }
  else if ( port == 1 )
    {
    info->Set( vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData" );
    return 1;
    }
  return 0;
}


int vtkICPPolyDataFilter::RequestData(vtkInformation *vtkNotUsed(request),
                              vtkInformationVector **inputVector,
                              vtkInformationVector *outputVector)
{
  // info objects
  vtkInformation *sourceInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *targetInfo = inputVector[1]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  vtkPolyData *source = vtkPolyData::SafeDownCast(
                                                 sourceInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkPolyData *target = vtkPolyData::SafeDownCast(
                                                 targetInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkPolyData *output = vtkPolyData::SafeDownCast(
                                                  outInfo->Get(vtkDataObject::DATA_OBJECT()));

  vtkSmartPointer<vtkPolyData> shallowSource = vtkSmartPointer<vtkPolyData>::New( );
  shallowSource->ShallowCopy( source ); 

  this->Transform->SetSource(source);
  this->Transform->SetTarget(target);
  this->Transform->GetLandmarkTransform()->SetModeToRigidBody();
  this->Transform->Update();

  //bring the source to the target
  vtkSmartPointer<vtkTransformPolyDataFilter> TransformFilter;
  TransformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  
  TransformFilter->SetInput( shallowSource );
  TransformFilter->SetTransform(this->Transform);
  TransformFilter->Update();

  output->DeepCopy(TransformFilter->GetOutput());

  return( 1 );
}


vtkIterativeClosestPointTransform* vtkICPPolyDataFilter::GetTransform( )
{
  return( this->Transform );
}


void vtkICPPolyDataFilter::PrintSelf(ostream &os, vtkIndent indent)
{
    this->Transform->PrintSelf( os, indent );
}
