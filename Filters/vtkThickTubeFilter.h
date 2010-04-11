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


//! \class vtkThickTubeFilter
//! \brief Build a tube around polyline with thick borders 
//!
//! This filter creates a ring with user-given InnerRadius and OuterRadius,
//! resolution being NumberOfSides. The ring is then extruded along the
//! input polyline to create a tube with thick border.
//!
//! It uses two vtkTubeFilters internally with CappingOff. The capping is made
//! manually by this vtkThickTubeFilter.
//!
//! \warning It works for polydata with only one polyline
//! \todo Handle several polylines as input
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

  //! Set the radius of the inner 'hole'
  vtkSetMacro( InnerRadius, double );
  //! Get the radius of the inner 'hole'
  vtkGetMacro( InnerRadius, double );

  //! Set the radius of the outer cylinder
  vtkSetMacro( OuterRadius, double );
  //! Get the radius of the outer cylinder
  vtkGetMacro( OuterRadius, double );

  //! Set the resolution of the cylinder (the number of each generating circles)
  vtkSetMacro( NumberOfSides, int );
  //! Get the resolution of the cylinder (the number of each generating circles)
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
  double InnerRadius; //!< Radius of the inner hole 
  double OuterRadius; //!< Radius of the outer cylinder
};

#endif
