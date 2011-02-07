//! \class vtkSplineDrivenImageReslice
//! \brief Reslicing of a volume along a path
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
//! \date 6 february 2011

#ifndef __vtkSplineDrivenImageReslice_h__
#define __vtkSplineDrivenImageReslice_h__

#include"vtkImageAlgorithm.h"

class vtkFrenetSerretFrame;
class vtkImageReslice;

class VTK_EXPORT vtkSplineDrivenImageReslice : public vtkImageAlgorithm
{
public:
   vtkTypeRevisionMacro(vtkSplineDrivenImageReslice,vtkImageAlgorithm);
   static vtkSplineDrivenImageReslice* New();

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

   vtkSetMacro( SliceSpacing, double );
   vtkGetMacro( SliceSpacing, double );

   vtkSetMacro( OffsetPoint, vtkIdType );
   vtkGetMacro( OffsetPoint, vtkIdType );

   vtkSetMacro( OffsetLine, vtkIdType );
   vtkGetMacro( OffsetLine, vtkIdType );

   vtkSetMacro( Incidence, double );
   vtkGetMacro( Incidence, double );


protected:
   vtkSplineDrivenImageReslice();
   ~vtkSplineDrivenImageReslice();

   virtual int RequestData(vtkInformation *, vtkInformationVector **, 
	                                         vtkInformationVector *);

   virtual int FillInputPortInformation(int port, vtkInformation *info);
   virtual int RequestInformation(vtkInformation*, vtkInformationVector**, 
	                                                vtkInformationVector*);
private:
   vtkSplineDrivenImageReslice(const vtkSplineDrivenImageReslice&);  // Not implemented.
   void operator=(const vtkSplineDrivenImageReslice&);  // Not implemented.

   vtkFrenetSerretFrame* localFrenetFrames; //!< computes local tangent along path input
   vtkImageReslice* reslicer; //!< Reslicers array

   vtkIdType SliceSize; //!< Number of pixels around the center points
   double SliceSpacing; //!< Pixel size
   double Incidence; //!< Rotation of the initial normal vector.
   
   vtkIdType OffsetPoint; //!< Id of the point where the reslicer proceed
   vtkIdType OffsetLine; //!< Id of the line cell where to get the reslice center
};

#endif //__vtkSplineDrivenImageReslice_h__
