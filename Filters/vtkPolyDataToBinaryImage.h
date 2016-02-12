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

//! \class vtkPolyDataToBinaryImage
//! \brief Makes VTK polydata to image stencil filter usable through paraview
//!
//! The vtkPolyDataToImageStencil class does not show good connections for a
//! Paraview pipelining. This class fix that.
//!
//! \author Jerome Velut
//! \date 2010-2011

#ifndef __vtkPolyDataToBinaryImage_h
#define __vtkPolyDataToBinaryImage_h

#include "vtkImageAlgorithm.h"

#include "vtkInformation.h"
#include "vtkInformationVector.h"

class vtkPolyDataToImageStencil;
class vtkImageStencilToImage;

class VTK_EXPORT vtkPolyDataToBinaryImage : public vtkImageAlgorithm
{
public:
    static vtkPolyDataToBinaryImage *New();
    vtkTypeMacro(vtkPolyDataToBinaryImage, vtkImageAlgorithm);

protected:
    vtkPolyDataToBinaryImage();
    ~vtkPolyDataToBinaryImage();
    virtual int FillInputPortInformation(int, vtkInformation* );
    virtual int RequestData( vtkInformation*, vtkInformationVector**, vtkInformationVector* );
    virtual int RequestInformation( vtkInformation*, vtkInformationVector**, vtkInformationVector* );

private:
    vtkPolyDataToBinaryImage(const vtkPolyDataToBinaryImage&);  // Not implemented.
    void operator=(const vtkPolyDataToBinaryImage&);  // Not implemented.

    vtkPolyDataToImageStencil* polyDataToImageStencil;
    vtkImageStencilToImage* imageStencilToImage;

};

#endif //__vtkPolyDataToBinaryImage_h
