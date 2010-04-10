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

//! \class vtkImageCropVOI
//! \brief Extract a VOI from a volume according to second input's bounding box
//!
//! This filter derives from vtkExtractVOI. It simply adds a second input which
//! bounding box is computed in RequestInformation. The parent's function 
//! SetVOI is then called according to the bounding box, the input spacing
//! and the margin.
//!
//! \author Jerome Velut
//! \date 7 apr 2010

#ifndef __VTKIMAGECROPVOI_H__
#define __VTKIMAGECROPVOI_H__

#include "vtkExtractVOI.h"

class VTK_EXPORT vtkImageCropVOI : public vtkExtractVOI
{
public:
  vtkTypeRevisionMacro(vtkImageCropVOI,vtkExtractVOI);
  static vtkImageCropVOI* New();
 
  //! Enlarge the VOI 
  vtkSetMacro( Margin, int );
  vtkGetMacro( Margin, int );

  //! Set the dataset that define the VOI 
  void SetBoundsConnection(int id, vtkAlgorithmOutput* algOutput);
  void SetBoundsConnection(vtkAlgorithmOutput* algOutput)
    {
      this->SetBoundsConnection(0, algOutput);
    }

protected:
  vtkImageCropVOI();
  ~vtkImageCropVOI();

  virtual int RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  virtual int FillInputPortInformation(int port, vtkInformation *info);
  
private:
  vtkImageCropVOI(const vtkImageCropVOI&);  // Not implemented.
  void operator=(const vtkImageCropVOI&);  // Not implemented.

  int Margin;
};

#endif //__VTKIMAGECROPVOI_H__
