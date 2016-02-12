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

//! \class vtkICPPolyDataFilter
//! \brief Wrapper around vtkIterativeClosestPointTransform.
//!
//! The original way of registrating 2 point sets thanks to
//! vtkIterativeClosestPointTransform is simply put inside a
//! polydata filter, in order to have it enabled in ParaView.
//!
//! \author Jerome Velut
//! \date jun 2011

#ifndef __vtkICPPolyDataFilter_h
#define __vtkICPPolyDataFilter_h

#include <vtkPolyDataAlgorithm.h>

class vtkIterativeClosestPointTransform;

class vtkICPPolyDataFilter : public vtkPolyDataAlgorithm
{
 public:
  static vtkICPPolyDataFilter *New();
  vtkTypeMacro(vtkICPPolyDataFilter, vtkPolyDataAlgorithm);
  
  void PrintSelf(ostream &os, vtkIndent indent);

  void SetSourceConnection(vtkAlgorithmOutput* input);
  void SetTargetConnection(vtkAlgorithmOutput* input);
  
  vtkIterativeClosestPointTransform* GetTransform( );
  
 protected:
  vtkICPPolyDataFilter();
  virtual ~vtkICPPolyDataFilter();

  //! VTK Pipeline overridings
  virtual int FillInputPortInformation( int port, vtkInformation* info );
  //! VTK Pipeline overridings
  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

 private:

  vtkIterativeClosestPointTransform* Transform; //!< performs the ICP registration

};
#endif //__vtkICPPolyDataFilter_h

