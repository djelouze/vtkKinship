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


//! \class vtkImageEigenElements
//! \brief Compute the eigen elements of 3x3 symmetric matrix defined at each voxel
//!
//! This filter computes the eigen vectors and values of each tensor defined
//! at voxels. The tensor is of the form:
//!    M11 M12 M13
//!    M12 M22 M23
//!    M13 M23 M33
//!
//! Input image must have a scalar array with 7 components:
//! comp1: maskvalue (0 or 1)
//! comp2->7: M11, M12, M13, M22, M23, M33
//!
//! The output will provide 6 arrays containing the eigen elements of the
//! input tensor. The vectors and eigen values are multiplied with mask values
//! to ensure that eigen elements are computed from non valid tensors.
//!
//! \author Jerome Velut
//! \date mar 2011

#ifndef __vtkImageEigenElements_h
#define __vtkImageEigenElements_h

#include "vtkThreadedImageAlgorithm.h"

class VTK_EXPORT vtkImageEigenElements : public vtkThreadedImageAlgorithm
{
public:
  // Description:
  // Construct an instance of vtkImageEigenElements filter.
  static vtkImageEigenElements *New();
  vtkTypeRevisionMacro(vtkImageEigenElements,vtkThreadedImageAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

protected:
  vtkImageEigenElements();
  ~vtkImageEigenElements();

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
  vtkImageEigenElements(const vtkImageEigenElements&);  // Not implemented.
  void operator=(const vtkImageEigenElements&);  // Not implemented.
};

#endif //__vtkImageEigenElements_h



