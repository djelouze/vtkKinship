//! \file SplineDrivenImageSlicer.cxx
//!
//! \author Jerome Velut
//! \date February 2011

#include "vtkSplineDrivenImageSlicer.h"

#include "vtkSmartPointer.h"

#include "vtkXMLPolyDataReader.h"
#include "vtkXMLImageDataReader.h"
#include "vtkXMLImageDataWriter.h"
#include "vtkXMLPolyDataWriter.h"

int main( int argc, char** argv )
{
   vtkSmartPointer<vtkXMLImageDataReader> imgReader;
   imgReader = vtkSmartPointer<vtkXMLImageDataReader>::New();

   vtkSmartPointer<vtkXMLPolyDataReader> pathReader;
   pathReader = vtkSmartPointer<vtkXMLPolyDataReader>::New();
   
   vtkSmartPointer<vtkSplineDrivenImageSlicer> reslicer;
   reslicer = vtkSmartPointer<vtkSplineDrivenImageSlicer>::New();
   
   vtkSmartPointer<vtkXMLImageDataWriter> imgWriter;
   imgWriter = vtkSmartPointer<vtkXMLImageDataWriter>::New();
   
   vtkSmartPointer<vtkXMLPolyDataWriter> sliceWriter;
   sliceWriter = vtkSmartPointer<vtkXMLPolyDataWriter>::New();

   
   imgReader->SetFileName(argv[1]);
   pathReader->SetFileName(argv[2]);
   
   imgReader->Update();
   pathReader->Update();
   
   reslicer->SetInputConnection(imgReader->GetOutputPort() );
   reslicer->SetPathConnection(pathReader->GetOutputPort());
   reslicer->SetOffsetPoint( 30 );
   
   imgWriter->SetFileName(argv[3]);
   imgWriter->SetInputConnection(reslicer->GetOutputPort(0));
   imgWriter->Write( );

   sliceWriter->SetFileName(argv[4]);
   sliceWriter->SetInputConnection(reslicer->GetOutputPort(1));
   sliceWriter->Write( );
   
   
   return( 0 );
}


