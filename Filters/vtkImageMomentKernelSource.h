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
