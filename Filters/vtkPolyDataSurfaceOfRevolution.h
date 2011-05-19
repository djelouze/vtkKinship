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

//! \class vtkPolyDataSurfaceOfRevolution
//! \brief 
//!
//! 
//!
//! \seealso vtkIterativePolyDataFilter
//! \author Jerome Velut
//! \date 18 may 2010

#ifndef __vtkPolyDataSurfaceOfRevolution_h
#define __vtkPolyDataSurfaceOfRevolution_h

#include "vtkIterativePolyDataAlgorithm.h"

class VTK_EXPORT vtkPolyDataSurfaceOfRevolution : public vtkIterativePolyDataAlgorithm
{
public:
  vtkTypeRevisionMacro(vtkPolyDataSurfaceOfRevolution,vtkIterativePolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  static vtkPolyDataSurfaceOfRevolution *New();

  //! Set the total revolution angle
  vtkSetMacro( Theta, double );
  //! Get the total revolution angle
  vtkGetMacro( Theta, double );

protected:
  vtkPolyDataSurfaceOfRevolution();
  ~vtkPolyDataSurfaceOfRevolution() {};

  virtual void IterativeRequestData( vtkInformationVector** );
  virtual void Reset( vtkInformationVector** );
  virtual int FillInputPortInformation(int port, vtkInformation *info);

private:
  vtkPolyDataSurfaceOfRevolution(const vtkPolyDataSurfaceOfRevolution&);  // Not implemented.
  void operator=(const vtkPolyDataSurfaceOfRevolution&);  // Not implemented.

  double Theta; //!< Total revolution angle
  double StepAngle; //!< Rotation angle at each iteration
};

#endif
