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


//! \class vtkImageLocalConvolution
//! \brief Convolution of an image with a kernel.
//!
//! vtkImageLocalConvolution convolves the image with a kernel of type double.
//! The kernel input is a vtkImageData of dimension i,j,k and possibly multi-
//! components. The image input is a vtkImageData. 
//! The convolution is computed at sites given through the PointSet input.
//! \note vtkPolyDataSource are of interest for such input. One can use a vtkPointSource
//! and computes a convolution at discrete sites inside a sphere
//!
//! \author Jerome Velut
//! \date jan 2010

#ifndef __VTKLOCALIMAGECONVOLUTION_H__
#define __VTKLOCALIMAGECONVOLUTION_H__

#include "vtkPointSetAlgorithm.h"
#include "vtkImageData.h"

class VTK_EXPORT vtkImageLocalConvolution : public vtkPointSetAlgorithm
{
public:
  //! Construct an instance of vtkImageLocalConvolution filter.
  static vtkImageLocalConvolution *New();
  vtkTypeRevisionMacro(vtkImageLocalConvolution,vtkPointSetAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  //! Get the kernel
  double* GetKernel();
  //! Specify a Kernel object at a specified table location. New style.
  //! Kernel connection is stored in port 2. This method is equivalent
  //! to SetInputConnection(2, id, outputPort).
  void SetKernelConnection(int id, vtkAlgorithmOutput* algOutput);
  //! Specify a Kernel object at a specified table location. New style.
  //! Kernel connection is stored in port 2. This method is equivalent
  //! to SetInputConnection(2, id, outputPort).
  void SetKernelConnection(vtkAlgorithmOutput* algOutput)
    {
      this->SetKernelConnection(0, algOutput);
    }

  //! Specify an input image at a specified table location.
  void SetImageConnection(int id, vtkAlgorithmOutput* algOutput);
  //! Specify an input image at a specified table location.
  void SetImageConnection(vtkAlgorithmOutput* algOutput)
    {
      this->SetImageConnection(0, algOutput);
    }

  vtkSetStringMacro( OutputDataName );
  vtkGetStringMacro( OutputDataName );

  vtkSetMacro( NormalizedIntensities, int );
  vtkGetMacro( NormalizedIntensities, int );
  vtkBooleanMacro( NormalizedIntensities, int );

  static void GetGridLocation( vtkImageData* img, vtkIdType ptId, int* ijk );

protected:
  vtkImageLocalConvolution();
  virtual ~vtkImageLocalConvolution();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  virtual int FillInputPortInformation(int port, vtkInformation *info);
  virtual int FillOutputPortInformation(int port, vtkInformation *info);

  
   char* OutputDataName;
  
private:
  vtkImageLocalConvolution(const vtkImageLocalConvolution&);  // Not implemented.
  void operator=(const vtkImageLocalConvolution&);  // Not implemented.

  int NormalizedIntensities; //! if 1, the image intensities I are mapped in [0,1] 
                            //! regarding values inside the kernel window
};

#endif //__VTKLOCALIMAGECONVOLUTION_H



