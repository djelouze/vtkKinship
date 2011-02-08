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
#include "vtkSmartPointer.h"

class VTK_EXPORT vtkIterativePolyDataAlgorithm : public vtkPolyDataAlgorithm
{
public:
  vtkTypeRevisionMacro(vtkIterativePolyDataAlgorithm,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  static vtkIterativePolyDataAlgorithm *New();

  //! NumberOfIterations accessors
  vtkSetMacro( NumberOfIterations, int );
  //! NumberOfIterations accessors
  vtkGetMacro( NumberOfIterations, int );

  //! IterateFromZero accessors
  vtkSetMacro( IterateFromZero, int );
  //! IterateFromZero accessors
  vtkGetMacro( IterateFromZero, int );
  //! IterateFromZero accessors
  vtkBooleanMacro( IterateFromZero, int );

protected:
  //! constructor
  vtkIterativePolyDataAlgorithm();
  //! destructor
  virtual ~vtkIterativePolyDataAlgorithm() {};

  //! VTK Pipeline function
  int RequestData( vtkInformation*, 
                   vtkInformationVector**, 
                   vtkInformationVector*);
  //! VTK Pipeline function
  int FillInputPortInformation(int port, vtkInformation *info);

  //! Implementation of one iteration. Children should override.
  virtual void IterativeRequestData( vtkInformationVector** ){};

  //! Description of the initial state (before first iteration).
  //! Children should override.
  virtual void Reset( vtkInformationVector** ){};

  //! Get the cached input that can be modified.
  vtkPolyData* GetCachedInput( ){return this->CachedInput; };

  //! The IterativeOutput will be copied to the filter's output after
  //! last iteration. Children have to use it accordingly.
  void SetIterativeOutput( vtkPolyData* io){this->IterativeOutput = io;};

  //! The IterativeOutput will be copied to the filter's output after
  //! last iteration. Children have to use it accordingly.
  vtkPolyData* GetIterativeOutput( ){return this->IterativeOutput;};

private:
  vtkIterativePolyDataAlgorithm(const vtkIterativePolyDataAlgorithm&);  // Not implemented.
  void operator=(const vtkIterativePolyDataAlgorithm&);  // Not implemented.


  //BTX
  vtkSmartPointer<vtkPolyData> CachedInput; //!< mesh that is iterated
  vtkSmartPointer<vtkPolyData> IterativeOutput; //!< output of one iteration
  //ETX

  unsigned int NumberOfIterations; //!< Number of iterations to reached
  unsigned int CurrentIteration; //!< Actual iteration
  int IterateFromZero; //!< If 1, the input will be copied over the cached 
                       //!< input at each RequestData
};

#endif
