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

//! \class vtkThickTubeFilter
//! \brief Implements an iterative interface for polydata algorithm
//!
//! This class provides functionalities to process a vtkPolyData
//! iteratively. The proposed mechanism allows one to stop the
//! process at one iteration and to restart at the last iteration
//! on next Update( ).
//!
//! If IterateFromZero is On, then the stop-and-start hints is 
//! disable. The filter iterates from 0 to NumberOfIterations.
//! Otherwise, the current iteration is considered as filter
//! input at next update, and the iterations run until the new
//! NumberOfIterations, if it changed. 
//!
//! One application is a real-time animation with ParaView: just
//! set an animation key on NumberOfIterations from 0 to what you want.
//! the incremented NumberOfIterations will triggered an update of
//! the filter from the last time-step num. iteration to the new one.
//!
//! \author Jerome Velut
//! \date 9 apr 2010

#ifndef __vtkThickTubeFilter_h
#define __vtkThickTubeFilter_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkTubeFilter.h"
#include "vtkAppendPolyData.h"
#include "vtkCellArray.h"


class VTK_EXPORT vtkThickTubeFilter : public vtkPolyDataAlgorithm
{
public:
  vtkTypeRevisionMacro(vtkThickTubeFilter,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  static vtkThickTubeFilter *New();

  vtkSetMacro( InnerRadius, double );
  vtkGetMacro( InnerRadius, double );

  vtkSetMacro( OuterRadius, double );
  vtkGetMacro( OuterRadius, double );

  vtkSetMacro( NumberOfSides, int );
  vtkGetMacro( NumberOfSides, int );
  

protected:
  vtkThickTubeFilter();
  ~vtkThickTubeFilter() {};

  int RequestData( vtkInformation*, 
                   vtkInformationVector**, 
                   vtkInformationVector*);

private:
  vtkThickTubeFilter(const vtkThickTubeFilter&);  // Not implemented.
  void operator=(const vtkThickTubeFilter&);  // Not implemented.


  int NumberOfSides; //!< TubeFilter parameter
  double InnerRadius; //!< 
  double OuterRadius; //!< 
};

#endif
