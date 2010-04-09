//! \class vtkIterativePolyDataAlgorithm
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

#ifndef __vtkIterativePolyDataAlgorithm_h
#define __vtkIterativePolyDataAlgorithm_h

#include "vtkPolyDataAlgorithm.h"


class VTK_EXPORT vtkIterativePolyDataAlgorithm : public vtkPolyDataAlgorithm
{
public:
  vtkTypeRevisionMacro(vtkIterativePolyDataAlgorithm,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  static vtkIterativePolyDataAlgorithm *New();

  vtkSetMacro( NumberOfIterations, int );
  vtkGetMacro( NumberOfIterations, int );

  vtkSetMacro( IterateFromZero, int );
  vtkGetMacro( IterateFromZero, int );
  vtkBooleanMacro( IterateFromZero, int );

protected:
  vtkIterativePolyDataAlgorithm();
  ~vtkIterativePolyDataAlgorithm() {};

  int RequestData( vtkInformation*, 
                   vtkInformationVector**, 
                   vtkInformationVector*);
  int FillInputPortInformation(int port, vtkInformation *info);

  virtual void IterativeRequestData( vtkInformationVector** ){};

  virtual void Reset( vtkInformationVector** ){};

  vtkPolyData* GetCachedInput( ){return this->CachedInput; };
  vtkPolyData* SetIterativeOutput( vtkPolyData* io){this->IterativeOutput = io;};

private:
  vtkIterativePolyDataAlgorithm(const vtkIterativePolyDataAlgorithm&);  // Not implemented.
  void operator=(const vtkIterativePolyDataAlgorithm&);  // Not implemented.


  vtkPolyData* CachedInput; //!< mesh that is iterated
  vtkPolyData* IterativeOutput; //!< output of one iteration

  int NumberOfIterations; //!< Number of iterations to reached
  int CurrentIteration; //!< Actual iteration
  int IterateFromZero; //!< If 1, the input will be copied over the cached 
                       //!< input at each RequestData
};

#endif
