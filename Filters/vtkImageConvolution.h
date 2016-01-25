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


//! \class vtkImageConvolution
//! \brief Convolution of an image with a kernel.
//!
//! vtkImageConvolution convolves the image with a kernel of type ImageData.
//! The kernel input is a vtkImageData of dimension i,j,k and possibly multi-
//! components.
//!
//! \todo Add the possibilty to not checking the boundary conditions (faster).
//!
//! \author Jerome Velut
//! \date jan 2010

#ifndef __VTKEXTENDEDIMAGECONVOLUTION_H__
#define __VTKEXTENDEDIMAGECONVOLUTION_H__

#include "vtkThreadedImageAlgorithm.h"

class VTK_EXPORT vtkImageConvolution : public vtkThreadedImageAlgorithm
{
public:
  // Description:
  // Construct an instance of vtkImageConvolution filter.
  static vtkImageConvolution *New();
  vtkTypeMacro(vtkImageConvolution,vtkThreadedImageAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  vtkSetStringMacro( OutputDataName );
  vtkGetStringMacro( OutputDataName );

  // Description:
  // Specify the kernel input
  void SetKernelConnection(int id, vtkAlgorithmOutput* algOutput);
  void SetKernelConnection(vtkAlgorithmOutput* algOutput)
    {
      this->SetKernelConnection(0, algOutput);
    }
  
protected:
  vtkImageConvolution();
  ~vtkImageConvolution();

  virtual void ThreadedRequestData(vtkInformation *request,
                           vtkInformationVector **inputVector,
                           vtkInformationVector *outputVector,
                           vtkImageData ***inData, vtkImageData **outData,
                           int outExt[6], int id);
  virtual int RequestUpdateExtent(vtkInformation*,
                         vtkInformationVector** inputVector,
                         vtkInformationVector* outputVector);
                         
  virtual int RequestInformation(vtkInformation*,
                         vtkInformationVector** inputVector,
                         vtkInformationVector* outputVector);

  char* OutputDataName;
  
private:
  vtkImageConvolution(const vtkImageConvolution&);  // Not implemented.
  void operator=(const vtkImageConvolution&);  // Not implemented.
};

#endif //__VTKEXTENDEDIMAGECONVOLUTION_H



