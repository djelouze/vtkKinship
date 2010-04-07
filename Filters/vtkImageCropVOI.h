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
