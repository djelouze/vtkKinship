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


//! \class vtkSmoothPolyDataVectors
//! \brief Smooth point vectors rather than geometry 
//!
//! \author Jerome Velut
//! \date 25 apr 2010

#ifndef __vtkSmoothPolyDataVectors_h
#define __vtkSmoothPolyDataVectors_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkSmoothPolyDataFilter.h"
#include "vtkCellArray.h"
#include "vtkDoubleArray.h"
#include "vtkSmartPointer.h"


class VTK_EXPORT vtkSmoothPolyDataVectors : public vtkPolyDataAlgorithm
{
public:
  vtkTypeMacro(vtkSmoothPolyDataVectors,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  static vtkSmoothPolyDataVectors *New();
  void SetNumberOfIterations( int );

protected:
  vtkSmoothPolyDataVectors();
  ~vtkSmoothPolyDataVectors() {};

  int RequestData( vtkInformation*, 
                   vtkInformationVector**, 
                   vtkInformationVector*);
  int FillInputPortInformation(int port, vtkInformation *info);

private:
  vtkSmoothPolyDataVectors(const vtkSmoothPolyDataVectors&);  // Not implemented.
  void operator=(const vtkSmoothPolyDataVectors&);  // Not implemented.
  //BTX
  vtkSmartPointer<vtkSmoothPolyDataFilter> SmoothingFilter; //!< smoothing method
  //ETX
};

#endif
