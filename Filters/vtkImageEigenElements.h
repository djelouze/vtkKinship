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
    vtkTypeMacro(vtkImageEigenElements,vtkThreadedImageAlgorithm);
    void PrintSelf(ostream& os, vtkIndent indent);

    //! map the input component to the tensor index
    void MapInputComponentsToTensor( int M11, int M12, int M13, int M22, int M23, int M33 );
    void GetInputComponentsToTensorMap( int& M11, int& M12, int& M13, int& M22, int& M23, int& M33 );
    //!
    vtkSetMacro( UseMask, int );
    vtkGetMacro( UseMask, int );
    vtkBooleanMacro( UseMask, int );

    vtkSetMacro( InMask, int );
    vtkGetMacro( InMask, int );

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

    int inM11; //!< input component corresponding to tensor index
    int inM12; //!< input component corresponding to tensor index
    int inM13; //!< input component corresponding to tensor index
    int inM22; //!< input component corresponding to tensor index
    int inM23; //!< input component corresponding to tensor index
    int inM33; //!< input component corresponding to tensor index
    int UseMask; //!< if 1, the eigen elements are multiplied with the mask defined in InMask
    int InMask; //!< tell which input component to use as a output multiplier
};

#endif //__vtkImageEigenElements_h



