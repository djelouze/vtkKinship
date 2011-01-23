//! \class vtkStraightenedCurvedPlanarReformation
//! \brief Orthogonal reslicing of a volume along a path
//!
//! Straightened Curved Planar Reformation (Stretched-CPR) builds a 2D image 
//! from an input path and an input volume. Each point of the path is 
//! considered as the center of a 2D vtkImageReslicer. Reslicers axes are set
//! orthogonal to the path. Reslicers output are appended on the z axis. Thus
//! the output of this filter is a volume with central XZ- and YZ-slices 
//! corresponding to the Straightened-CPR.
//!
//! Input: vtkImageData (InputConnection) and vtkPolyData (PathConnection) 
//! one polyline representing the path. Typically, the output of vtkSpline can
//! be used as path input.
//!
//! \see Kanitsar et al. "CPR - Curved Planar Reformation", Proc. IEEE  Visualization, 2002, 37-44
//! \author Jerome Velut
//! \author LTSI
//! \date 2008-2009

#ifndef __VTKSTRAIGHTENEDCURVEDPLANARREFORMATION_H__
#define __VTKSTRAIGHTENEDCURVEDPLANARREFORMATION_H__

#include"vtkPoints.h"
#include"vtkPolyDataAlgorithm.h"
#include"vtkPolyData.h"
#include"vtkCellArray.h"
#include "vtkFrenetSerretFrame.h"
#include "vtkImageReslice.h"

#if defined(NO_DLL)  && defined (WIN32)
   #undef VTK_EXPORT
   #define VTK_EXPORT
#endif

class VTK_EXPORT vtkStraightenedCurvedPlanarReformation : public vtkPolyDataAlgorithm
{
public:
   vtkTypeRevisionMacro(vtkStraightenedCurvedPlanarReformation,vtkPolyDataAlgorithm);
   static vtkStraightenedCurvedPlanarReformation* New();

  //! Specify the path represented by a vtkPolyData wich contains PolyLines
  void SetPathConnection(int id, vtkAlgorithmOutput* algOutput);
  void SetPathConnection(vtkAlgorithmOutput* algOutput)
    {
      this->SetPathConnection(0, algOutput);
    };
  vtkAlgorithmOutput* GetPathConnection( )
                                 {return( this->GetInputConnection( 1, 0 ) );};

   vtkSetMacro( SliceSize, double );
   vtkGetMacro( SliceSize, double );

   vtkSetMacro( XYSpacing, double );
   vtkGetMacro( XYSpacing, double );

   vtkSetMacro( ZSpacing, double );
   vtkGetMacro( ZSpacing, double );

   vtkSetMacro( AutoZSpacing, int );
   vtkGetMacro( AutoZSpacing, int );
   vtkBooleanMacro( AutoZSpacing, int );

   vtkSetMacro( Incidence, double );
   vtkGetMacro( Incidence, double );


protected:
   vtkStraightenedCurvedPlanarReformation();
   ~vtkStraightenedCurvedPlanarReformation();

   virtual int RequestData(vtkInformation *, vtkInformationVector **, 
	                                         vtkInformationVector *);
      virtual int RequestUpdateExtent(vtkInformation *, vtkInformationVector **, 
                                            vtkInformationVector *);
   virtual int FillInputPortInformation(int port, vtkInformation *info);
   virtual int RequestInformation(vtkInformation*, vtkInformationVector**, 
	                                                vtkInformationVector*);
   virtual int FillOutputPortInformation( vtkInformation*,
                                          vtkInformationVector**,
                                          vtkInformationVector*);

private:
   vtkStraightenedCurvedPlanarReformation(const vtkStraightenedCurvedPlanarReformation&);  // Not implemented.
   void operator=(const vtkStraightenedCurvedPlanarReformation&);  // Not implemented.

   vtkFrenetSerretFrame* localFrenetFrames; //!< computes local tangent along path input
   vtkImageReslice** reslicers; //!< Reslicers array

   double SliceSize; //!< Number of pixels around the center points
   double XYSpacing; //!< Pixel size
   double ZSpacing;  //!< Distance between slices
   int AutoZSpacing; //!< Specify if ZSpacing is computed from input sampling
   double Incidence; //!< Rotation of the initial normal vector.

};

#endif //__VTKSTRAIGHTENEDCURVEDPLANARREFORMATION_H__
