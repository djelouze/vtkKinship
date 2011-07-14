//! \file StraightenedCurvedPlanarReformation.cxx
//!
//! \author Jerome Velut
//! \date February 2011

#include "vtkSplineDrivenImageSlicer.h"
#include "vtkImageAppend.h"

#include "vtkSmartPointer.h"
#include "vtkPolyData.h"
#include "vtkImageData.h"

#include "vtkXMLPolyDataReader.h"
#include "vtkXMLImageDataReader.h"
#include "vtkXMLImageDataWriter.h"

int main( int argc, char** argv )
{
   vtkSmartPointer<vtkXMLImageDataReader> imgReader;
   imgReader = vtkSmartPointer<vtkXMLImageDataReader>::New();

   vtkSmartPointer<vtkXMLPolyDataReader> pathReader;
   pathReader = vtkSmartPointer<vtkXMLPolyDataReader>::New();
   
   vtkSmartPointer<vtkSplineDrivenImageSlicer> reslicer;
   reslicer = vtkSmartPointer<vtkSplineDrivenImageSlicer>::New();
   
   vtkSmartPointer<vtkImageAppend> append;
   append = vtkSmartPointer<vtkImageAppend>::New();
   
   vtkSmartPointer<vtkXMLImageDataWriter> imgWriter;
   imgWriter = vtkSmartPointer<vtkXMLImageDataWriter>::New();
   
   imgReader->SetFileName(argv[1]);
   imgReader->Update();
   pathReader->SetFileName(argv[2]);
   pathReader->Update();
   
   reslicer->SetInputConnection(imgReader->GetOutputPort() );
   reslicer->SetPathConnection(pathReader->GetOutputPort());
   reslicer->SetSliceExtent( 250,200 );
   reslicer->SetSliceSpacing( 0.2,0.1 );
   reslicer->SetSliceThickness( 0.4 );
   
   // Get number of input points
   int nbPoints = pathReader->GetOutput( )->GetNumberOfPoints();
   for( int ptId = 0; ptId < nbPoints; ptId++ )
   {
     reslicer->SetOffsetPoint( ptId );
     reslicer->Update();
     
     vtkSmartPointer<vtkImageData> tempSlice;
     tempSlice = vtkSmartPointer<vtkImageData>::New();
     tempSlice->DeepCopy( reslicer->GetOutput( 0 ));
     
     append->AddInput( tempSlice );
   }
   append->SetAppendAxis(2);
   append->Update( );
   
   imgWriter->SetFileName(argv[3]);
   imgWriter->SetInputConnection(append->GetOutputPort());
   imgWriter->Write( );
   
   return( 0 );
}


