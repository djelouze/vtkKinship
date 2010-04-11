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


#include "vtkImageConvolution.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"

vtkCxxRevisionMacro(vtkImageConvolution, "$Revision: 0.1 $");
vtkStandardNewMacro(vtkImageConvolution);

//----------------------------------------------------------------------------
// Construct an instance of vtkImageConvolution filter.
vtkImageConvolution::vtkImageConvolution()
{
  this->OutputDataName = 0;

  this->SetNumberOfInputPorts( 2 );
}

//----------------------------------------------------------------------------
// Destructor
vtkImageConvolution::~vtkImageConvolution()
{
}

//----------------------------------------------------------------------------
void vtkImageConvolution::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
// Specify a source object at a specified table location.
void vtkImageConvolution::SetKernelConnection(int id, vtkAlgorithmOutput* algOutput)
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


//----------------------------------------------------------------------------
int vtkImageConvolution::RequestUpdateExtent(vtkInformation*,
                                         vtkInformationVector** inputVector,
                                         vtkInformationVector* outputVector)
{
  // Get input and output pipeline information.
  vtkInformation* outInfo = outputVector->GetInformationObject(0);
  vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation* kernelInfo = inputVector[1]->GetInformationObject(0);

  // Get the input whole extent.
  int wExtent[6];
  kernelInfo->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), wExtent);
  kernelInfo->Set(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT(), wExtent, 6);
  
  return( 1 );
}

//----------------------------------------------------------------------------
int vtkImageConvolution::RequestInformation(vtkInformation*,
                                         vtkInformationVector** inputVector,
                                         vtkInformationVector* outputVector)
{
  // Get input and output pipeline information.
  vtkInformation* outInfo = outputVector->GetInformationObject(0);
  vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation* kernelInfo = inputVector[1]->GetInformationObject(0);

  vtkImageData *inImage = vtkImageData::SafeDownCast( inInfo->Get(vtkDataObject::DATA_OBJECT()));
  int inNumComps = inImage->GetNumberOfScalarComponents( );

  vtkImageData *kernelImage = vtkImageData::SafeDownCast( kernelInfo->Get(vtkDataObject::DATA_OBJECT()));
  int krnlNumComps = kernelImage->GetNumberOfScalarComponents( );
  // Set the number of point data components to the number of
  // components in the convolution kernel times the number of 
  // input components.
  vtkDataObject::SetPointDataActiveScalarInfo(outInfo, 
                                              VTK_DOUBLE,
                                              krnlNumComps * inNumComps);

  return 1;
}

//----------------------------------------------------------------------------
// This templated function executes the filter on any region,
// whether it needs boundary checking or not.
// If the filter needs to be faster, the function could be duplicated
// for strictly center (no boundary) processing.
template <class T>
void vtkImageConvolutionExecute(vtkImageConvolution *self,
                             vtkImageData *inData, T *inPtr, 
                             vtkImageData *kernelData, double *kernelPtr, 
                             vtkImageData *outData, double *outPtr,
                             int outExt[6], int id,
                             vtkInformation *inInfo)
{
  int *kernelSize;
  int kernelMiddle[3];

  // For looping though output (and input) pixels.
  int outMin0, outMax0, outMin1, outMax1, outMin2, outMax2;
  int outIdx0, outIdx1, outIdx2;
  vtkIdType inInc0, inInc1, inInc2;
  vtkIdType outInc0, outInc1, outInc2;
  T *inPtr0, *inPtr1, *inPtr2;
  double *outPtr0, *outPtr1, *outPtr2;
  int numComps, outIdxC;

  // For looping through hood pixels
  int hoodMin0, hoodMax0, hoodMin1, hoodMax1, hoodMin2, hoodMax2;
  int hoodIdx0, hoodIdx1, hoodIdx2;
  T *hoodPtr0, *hoodPtr1, *hoodPtr2;

  // For looping through the kernel, and compute the kernel result
  int kernelIdx;
  double sum;

  // The extent of the whole input image
  int inImageExt[6];

  // to compute the range
  unsigned long count = 0;
  unsigned long target;

  // Get information to march through data
  inData->GetIncrements(inInc0, inInc1, inInc2);
  inInfo->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), inImageExt);
  outData->GetIncrements(outInc0, outInc1, outInc2); 
  outMin0 = outExt[0];   outMax0 = outExt[1];
  outMin1 = outExt[2];   outMax1 = outExt[3];
  outMin2 = outExt[4];   outMax2 = outExt[5];
  numComps = outData->GetNumberOfScalarComponents();
   
  // Get ivars of this object (easier than making friends)
  kernelSize = kernelData->GetDimensions( );

  kernelMiddle[0] = kernelSize[0] / 2;
  kernelMiddle[1] = kernelSize[1] / 2;
  kernelMiddle[2] = kernelSize[2] / 2;

  hoodMin0 = -kernelMiddle[0];
  hoodMin1 = -kernelMiddle[1];
  hoodMin2 = -kernelMiddle[2];

  hoodMax0 = hoodMin0 + kernelSize[0] - 1;
  hoodMax1 = hoodMin1 + kernelSize[1] - 1;
  hoodMax2 = hoodMin2 + kernelSize[2] - 1;

  // Get the kernel. 
  double *kernel = kernelPtr;

  // in and out should be marching through corresponding pixels.
  inPtr = static_cast<T *>(
    inData->GetScalarPointer(outMin0, outMin1, outMin2));

  target = static_cast<unsigned long>(numComps*(outMax2 - outMin2 + 1)*
                                      (outMax1 - outMin1 + 1)/50.0);
  target++;
  
  // loop through components
  for (outIdxC = 0; outIdxC < numComps; ++outIdxC)
    {
    // loop through pixels of output
    outPtr2 = outPtr;
    inPtr2 = inPtr;
    for (outIdx2 = outMin2; outIdx2 <= outMax2; ++outIdx2)
      {
      outPtr1 = outPtr2;
      inPtr1 = inPtr2;
      for (outIdx1 = outMin1; 
           outIdx1 <= outMax1 && !self->AbortExecute; 
           ++outIdx1)
        {
        if (!id)
          {
          if (!(count%target))
            {
            self->UpdateProgress(count/(50.0*target));
            }
          count++;
          }

        outPtr0 = outPtr1;
        inPtr0 = inPtr1;

        for (outIdx0 = outMin0; outIdx0 <= outMax0; ++outIdx0)
          {
          // Inner loop : effective convolution

          // Set the sum to zero
          sum = 0;

          // loop through neighborhood pixels
          // as sort of a hack to handle boundaries, 
          // input pointer will be marching through data that does not exist.
          hoodPtr2 = inPtr0 - kernelMiddle[0] * inInc0 
                            - kernelMiddle[1] * inInc1 
                            - kernelMiddle[2] * inInc2;

          // Set the kernel index to the starting position
          kernelIdx = outIdxC;

          for (hoodIdx2 = hoodMin2; hoodIdx2 <= hoodMax2; ++hoodIdx2)
            {
            hoodPtr1 = hoodPtr2;

            for (hoodIdx1 = hoodMin1; hoodIdx1 <= hoodMax1; ++hoodIdx1)
              {
              hoodPtr0 = hoodPtr1;

              for (hoodIdx0 = hoodMin0; hoodIdx0 <= hoodMax0; ++hoodIdx0)
                {
                // A quick but rather expensive way to handle boundaries
                // This assumes the boundary values are zero
                if (outIdx0 + hoodIdx0 >= inImageExt[0] &&
                    outIdx0 + hoodIdx0 <= inImageExt[1] &&
                    outIdx1 + hoodIdx1 >= inImageExt[2] &&
                    outIdx1 + hoodIdx1 <= inImageExt[3] &&
                    outIdx2 + hoodIdx2 >= inImageExt[4] &&
                    outIdx2 + hoodIdx2 <= inImageExt[5])
                  {
                  sum += *hoodPtr0 * kernel[kernelIdx];

                  }
                // Take the next position in the kernel
                kernelIdx += numComps;

                hoodPtr0 += inInc0;
                }

              hoodPtr1 += inInc1;
              }

            hoodPtr2 += inInc2;
            }

          // Set the output pixel to the correct value
          *outPtr0 = sum;

          inPtr0 += inInc0;
          outPtr0 += outInc0;
          }

        inPtr1 += inInc1;
        outPtr1 += outInc1;
        }

      inPtr2 += inInc2;
      outPtr2 += outInc2;
      }
    ++outPtr;
    ++inPtr;
    }
}

//----------------------------------------------------------------------------
// This method contains the first switch statement that calls the correct
// templated function for the input and output Data types.
// It hanldes image boundaries, so the image does not shrink.
void vtkImageConvolution::ThreadedRequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *vtkNotUsed(outputVector),
  vtkImageData ***inData,
  vtkImageData **outData,
  int outExt[6], int id)
{
  void *inPtr = inData[0][0]->GetScalarPointerForExtent(outExt);
  void *kernelPtr = inData[1][0]->GetScalarPointer();  
  void *outPtr = outData[0]->GetScalarPointerForExtent(outExt);

  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);

 
  switch (inData[0][0]->GetScalarType())
    {
    vtkTemplateMacro(
      vtkImageConvolutionExecute(this, 
	                          inData[0][0], static_cast<VTK_TT *>(inPtr), 
	                          inData[1][0], static_cast<double *>(kernelPtr), 
                              outData[0], static_cast<double *>(outPtr),
                              outExt, id, inInfo));

    default:
      vtkErrorMacro(<< "Execute: Unknown ScalarType");
      return;
    }
  outData[0]->GetPointData( )->GetScalars( )->SetName( this->OutputDataName );
}
