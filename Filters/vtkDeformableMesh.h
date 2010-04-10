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

//! \class vtkDeformableMesh
//! \brief Implements a regularized deformation of a vtkPolyData
//!
//! This class implements an iterative mechanism that deforms
//! the input given a number of iterations. The output is used
//! as input until the property Reset is set to 1.
//! 
//! At each iteration, the input probes the vector of the image
//! (second input). The regularization is performed as a smoothing of
//! these displacement vectors ( Default is a laplacian smoothing). 
//! Finally, the input mesh is warped along the regularized deformation
//! vector (vtkWarpVector)
//!
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


protected:
  vtkDeformableMesh();
  ~vtkDeformableMesh() {};

  void IterativeRequestData( vtkInformationVector** );
  void Reset( vtkInformationVector** );
  int FillInputPortInformation(int port, vtkInformation *info);

private:
  vtkDeformableMesh(const vtkDeformableMesh&);  // Not implemented.
  void operator=(const vtkDeformableMesh&);  // Not implemented.


  vtkWarpVector* WarpFilter; //!< deformation filter
  vtkProbeFilter* ProbeFilter; //!< get the deformation from the image
};

#endif
