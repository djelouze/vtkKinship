//! \class vtkImageConvolution
//! \brief Convolution of an image with a kernel.
//!
//! vtkImageConvolution convolves the image with a kernel of type ImageData.
//! The kernel input is a vtkImageData of dimension i,j,k and possibly multi-
//! components.
//!
//! \todo Add the possibilty to not checking the boundary conditions (faster).
//!
//! \author Jerome Velut
//! \date jan 2010

#ifndef __VTKEXTENDEDIMAGECONVOLUTION_H__
#define __VTKEXTENDEDIMAGECONVOLUTION_H__

#include "vtkThreadedImageAlgorithm.h"

class VTK_EXPORT vtkImageConvolution : public vtkThreadedImageAlgorithm
{
public:
  // Description:
  // Construct an instance of vtkImageConvolution filter.
  static vtkImageConvolution *New();
  vtkTypeRevisionMacro(vtkImageConvolution,vtkThreadedImageAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  vtkSetStringMacro( OutputDataName );
  vtkGetStringMacro( OutputDataName );

  // Description:
  // Specify the kernel input
  void SetKernelConnection(int id, vtkAlgorithmOutput* algOutput);
  void SetKernelConnection(vtkAlgorithmOutput* algOutput)
    {
      this->SetKernelConnection(0, algOutput);
    }
  
protected:
  vtkImageConvolution();
  ~vtkImageConvolution();

  virtual void ThreadedRequestData(vtkInformation *request,
                           vtkInformationVector **inputVector,
                           vtkInformationVector *outputVector,
                           vtkImageData ***inData, vtkImageData **outData,
                           int outExt[6], int id);
  virtual int RequestUpdateExtent(vtkInformation*,
                         vtkInformationVector** inputVector,
                         vtkInformationVector* outputVector);
                         
  virtual int RequestInformation(vtkInformation*,
                         vtkInformationVector** inputVector,
                         vtkInformationVector* outputVector);

  char* OutputDataName;
  
private:
  vtkImageConvolution(const vtkImageConvolution&);  // Not implemented.
  void operator=(const vtkImageConvolution&);  // Not implemented.
};

#endif //__VTKEXTENDEDIMAGECONVOLUTION_H



