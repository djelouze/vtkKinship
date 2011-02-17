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


#include "vtkPolyDataToBinaryImage.h"
#include "vtkPolyDataToImageStencil.h"
#include "vtkImageStencilToImage.h"
#include "vtkObjectFactory.h"
#include "vtkImageData.h"
#include "vtkPolyData.h"
#include "vtkDataObject.h"
#include "vtkAlgorithmOutput.h"
#include "vtkStreamingDemandDrivenPipeline.h"

vtkCxxRevisionMacro(vtkPolyDataToBinaryImage, "$Revision: 1.22 $");
vtkStandardNewMacro(vtkPolyDataToBinaryImage);

vtkPolyDataToBinaryImage::vtkPolyDataToBinaryImage( )
{
    this->SetNumberOfInputPorts( 2 );
    this->polyDataToImageStencil = vtkPolyDataToImageStencil::New();
    this->imageStencilToImage = vtkImageStencilToImage::New();
}

vtkPolyDataToBinaryImage::~vtkPolyDataToBinaryImage( )
{
    this->polyDataToImageStencil->Delete( );
    this->imageStencilToImage->Delete( );
}


//----------------------------------------------------------------------------
int vtkPolyDataToBinaryImage::FillInputPortInformation(int port, vtkInformation* info)
{
    if (port == 1)
    {
        info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkImageData");
        info->Set(vtkAlgorithm::INPUT_IS_OPTIONAL(), 1);
    }
    else
        info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData");

    return 1;
}

int vtkPolyDataToBinaryImage::RequestInformation(
    vtkInformation *vtkNotUsed(request),
    vtkInformationVector **inputVector,
    vtkInformationVector *outputVector)
{
    // get the info objects
    vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
    vtkInformation *imageInfo = inputVector[1]->GetInformationObject(0);
    vtkInformation *outInfo = outputVector->GetInformationObject(0);

    // get the input and ouptut
    vtkPolyData *input = vtkPolyData::SafeDownCast(
                             inInfo->Get(vtkDataObject::DATA_OBJECT()));


    if( imageInfo )
    {
        vtkImageData *info = vtkImageData::SafeDownCast(
                                 imageInfo->Get(vtkDataObject::DATA_OBJECT()));
        vtkImageData* shallowInfo = vtkImageData::New();
        shallowInfo->ShallowCopy( info );
        this->polyDataToImageStencil->SetInformationInput(shallowInfo);
        int extent[6];

        info->GetExtent(extent);
        outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(),
                     extent,6);
        outInfo->Set(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT(),
                     extent,6);
        vtkDataObject::SetPointDataActiveScalarInfo(outInfo, info->GetScalarType(), info->GetNumberOfScalarComponents());

    }
    else
    {
        double dextent[6];

        input->ComputeBounds();
        input->GetBounds(dextent);
        int extent[6];
        for( int i = 0; i< 6; i++)
            extent[i] = static_cast<int>(dextent[i]);

        this->polyDataToImageStencil->SetOutputWholeExtent(extent);

        outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(),
                     extent,6);
        outInfo->Set(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT(),
                     extent,6);
        vtkDataObject::SetPointDataActiveScalarInfo(outInfo, VTK_UNSIGNED_CHAR, 1);
    }
    return 1;
}

int vtkPolyDataToBinaryImage::RequestData(
    vtkInformation *vtkNotUsed(request),
    vtkInformationVector **inputVector,
    vtkInformationVector *outputVector)
{
    // get the info objects
    vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
    vtkInformation *imageInfo = inputVector[1]->GetInformationObject(0);
    vtkInformation *outInfo = outputVector->GetInformationObject(0);

    // get the input and ouptut
    vtkPolyData *input = vtkPolyData::SafeDownCast(
                             inInfo->Get(vtkDataObject::DATA_OBJECT()));
    vtkImageData *output = vtkImageData::SafeDownCast(
                               outInfo->Get(vtkDataObject::DATA_OBJECT()));

    vtkPolyData* shallowInput = vtkPolyData::New( );
    shallowInput->ShallowCopy( input );


    this->polyDataToImageStencil->SetInput( shallowInput );
    this->imageStencilToImage->SetInputConnection(
        this->polyDataToImageStencil->GetOutputPort());
    this->imageStencilToImage->Update( );
    output->DeepCopy( this->imageStencilToImage->GetOutput());

    return 1;
}