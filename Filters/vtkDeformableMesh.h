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

//! \class vtkDeformableMesh
//! \brief Implements a simple deformation of a polydata embedded in a vector
//! field
//!
//! This filter takes two inputs: 
//! - a PolyData (port 0) that will be iterated and copied to output after
//!   last iteration
//! - an ImageData (port 1) that contains vector data. The polydata is warped
//!   according to these vectors.
//!
//! The iterative process is a combination of vtkProbeFilter->vtkWarpVector.
//!
//! \seealso vtkIterativePolyDataFilter
//! \author Jerome Velut
//! \date 28 mar 2010

#ifndef __vtkDeformableMesh_h
#define __vtkDeformableMesh_h

#include "vtkIterativePolyDataAlgorithm.h"
#include "vtkWarpVector.h"
#include "vtkProbeFilter.h"
#include "vtkImageData.h"


class VTK_EXPORT vtkDeformableMesh : public vtkIterativePolyDataAlgorithm
{
public:
  vtkTypeRevisionMacro(vtkDeformableMesh,vtkIterativePolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  static vtkDeformableMesh *New();

  //! Set the scale factor of the vtkWarpVector
  vtkSetMacro( ScaleFactor, double );
  //! Get the scale factor of the vtkWarpVector
  vtkGetMacro( ScaleFactor, double );

protected:
  vtkDeformableMesh();
  ~vtkDeformableMesh() {};

  //! Effective implementation of an iteration
  virtual void IterativeRequestData( vtkInformationVector** );

  //! Effective implementation of the initial state
  virtual void Reset( vtkInformationVector** );

  //! VTK pipelining function
  virtual int FillInputPortInformation(int port, vtkInformation *info);

private:
  vtkDeformableMesh(const vtkDeformableMesh&);  // Not implemented.
  void operator=(const vtkDeformableMesh&);  // Not implemented.

  //BTX
  vtkSmartPointer<vtkWarpVector> WarpFilter; //!< deformation filter
  vtkSmartPointer<vtkProbeFilter> ProbeFilter; //!< get the deformation from the image
  //ETX

  double ScaleFactor; //!< scale applied to the probed vectors
};

#endif
