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
