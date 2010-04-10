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

//! \class vtkImageMomentKernelSource
//! \brief Outputs a vtkImageData with geometric moment kernel values
//!
//! component:   0    1    2    3    4    5    6    7    8    9
//! moment :   M000 M100 M010 M001 M110 M101 M011 M200 M020 M002
//! If one single of these moments is of interest, use vtkImageExtractComponents
//! from the vtk library.
//!
//! \author Jerome Velut
//! \date 8 apr 2010

#ifndef __VTKIMAGEMOMENTKERNELSOURCE_H
#define __VTKIMAGEMOMENTKERNELSOURCE_H

#include "vtkImageAlgorithm.h"

class VTK_EXPORT vtkImageMomentKernelSource : public vtkImageAlgorithm
{
public:
  //! VTK Object factory
  static vtkImageMomentKernelSource *New();
  vtkTypeRevisionMacro(vtkImageMomentKernelSource,vtkImageAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  //! Set/Get the kernel size. Has to be an odd value
  void SetKernelSize( int kernelSize );
  //! Set/Get the kernel size. Has to be an odd value
  int GetKernelSize( ){return ( this->KernelSize );};

  //! Set/Get the subsampling for the spherical neighbourhood
  vtkSetMacro( SubSampling, int );
  //! Set/Get the subsampling for the spherical neighbourhood
  vtkGetMacro( SubSampling, int );

  //! Computes automatically (1) or not (0) the subsampling
  vtkBooleanMacro( AutoSubSampling, int );
  vtkSetMacro( AutoSubSampling, int );
  vtkGetMacro( AutoSubSampling, int );

protected:
  //! constructor
  vtkImageMomentKernelSource();
  ~vtkImageMomentKernelSource() {};

  int KernelSize; //!< Moment kernel size. Equal to output extent
  int SubSampling; //!< Depending on AutoSubSampling value, it has 2 meanings:
                   //!< 0: SubSampling will give the absolute subsampling of each voxel
                   //!< 1: Subsampling is relative to the kernel size (recommanded)
  int TrueSubSampling; //!< Internal use : it is the closest divisor of SubSampling according to KernelSize
  int AutoSubSampling; //!< specify wether the voxel subsampling is the same for any kernel size or if
                       //!< it depends on it.

  //! VTK Pipelining functions
  virtual int RequestInformation (vtkInformation *, vtkInformationVector**, vtkInformationVector *);
  //! VTK Pipelining functions
  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  
private:
  vtkImageMomentKernelSource(const vtkImageMomentKernelSource&);  // Not implemented.
  void operator=(const vtkImageMomentKernelSource&);  // Not implemented.
};


#endif //__VTKIMAGEMOMENTKERNELSOURCE_H
