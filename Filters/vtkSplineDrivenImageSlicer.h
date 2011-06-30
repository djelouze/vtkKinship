//! \class vtkSplineDrivenImageSlicer
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

#ifndef __vtkSplineDrivenImageSlicer_h__
#define __vtkSplineDrivenImageSlicer_h__

#include"vtkImageAlgorithm.h"

class vtkFrenetSerretFrame;
class vtkImageReslice;

class VTK_EXPORT vtkSplineDrivenImageSlicer : public vtkImageAlgorithm
{
public:
   vtkTypeRevisionMacro(vtkSplineDrivenImageSlicer,vtkImageAlgorithm);
   static vtkSplineDrivenImageSlicer* New();

  //! Specify the path represented by a vtkPolyData wich contains PolyLines
  void SetPathConnection(int id, vtkAlgorithmOutput* algOutput);
  void SetPathConnection(vtkAlgorithmOutput* algOutput)
    {
      this->SetPathConnection(0, algOutput);
    };
  vtkAlgorithmOutput* GetPathConnection( )
                                 {return( this->GetInputConnection( 1, 0 ) );};

   vtkSetVector2Macro( SliceExtent, vtkIdType );
   vtkGetVector2Macro( SliceExtent, vtkIdType );

   vtkSetVector2Macro( SliceSpacing, double );
   vtkGetVector2Macro( SliceSpacing, double );

   vtkSetMacro( SliceThickness, double );
   vtkGetMacro( SliceThickness, double );

   vtkSetMacro( OffsetPoint, vtkIdType );
   vtkGetMacro( OffsetPoint, vtkIdType );

   vtkSetMacro( OffsetLine, vtkIdType );
   vtkGetMacro( OffsetLine, vtkIdType );

   vtkSetMacro( ProbeInput, vtkIdType );
   vtkGetMacro( ProbeInput, vtkIdType );
   vtkBooleanMacro( ProbeInput, vtkIdType );

   vtkSetMacro( Incidence, double );
   vtkGetMacro( Incidence, double );


protected:
   vtkSplineDrivenImageSlicer();
   ~vtkSplineDrivenImageSlicer();

   virtual int RequestData(vtkInformation *, vtkInformationVector **, 
	                                         vtkInformationVector *);

   virtual int FillInputPortInformation(int port, vtkInformation *info);
   virtual int FillOutputPortInformation( int, vtkInformation*);
   virtual int RequestInformation(vtkInformation*, vtkInformationVector**, 
	                                                vtkInformationVector*);
private:
   vtkSplineDrivenImageSlicer(const vtkSplineDrivenImageSlicer&);  // Not implemented.
   void operator=(const vtkSplineDrivenImageSlicer&);  // Not implemented.

   vtkFrenetSerretFrame* localFrenetFrames; //!< computes local tangent along path input
   vtkImageReslice* reslicer; //!< Reslicers array

   vtkIdType SliceExtent[2]; //!< Number of pixels nx, ny in the slice space around the center points
   double SliceSpacing[2]; //!< Pixel size sx, sy of the output slice
   double SliceThickness; //!< Slice thickness (useful for volumic reconstruction) 
   double Incidence; //!< Rotation of the initial normal vector.
   
   vtkIdType OffsetPoint; //!< Id of the point where the reslicer proceed
   vtkIdType OffsetLine; //!< Id of the line cell where to get the reslice center
   vtkIdType ProbeInput; //!< If true, the output plane (2nd output probes the input image)
};

#endif //__vtkSplineDrivenImageSlicer_h__
