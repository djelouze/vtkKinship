//! \file SplineDrivenImageSlicer.cxx
//!
//! \author Jerome Velut
//! \date February 2011

#include "vtkDualPolyDataFilter.h"

#include "vtkSmartPointer.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkXMLPolyDataReader.h"
#include "vtkXMLImageDataWriter.h"

int main( int argc, char** argv )
{
   vtkSmartPointer<vtkXMLPolyDataReader> reader;
   reader = vtkSmartPointer<vtkXMLPolyDataReader>::New();
   
   vtkSmartPointer<vtkXMLImageDataWriter> writer;
   writer = vtkSmartPointer<vtkXMLImageDataWriter>::New();

   
   reader->SetFileName("c:/Users/Jerome/Desktop/probe.vtp");
   reader->Update();

   vtkImageData* img = vtkImageData::New();
   img->SetExtent( -50, 50, -50, 50, 0, 0 );
   img->SetSpacing( .1, .1, 0 );
   vtkDataObject::SetPointDataActiveScalarInfo(
     img->GetInformation(), VTK_FLOAT, 1 );
   img->GetPointData()->SetScalars( reader->GetOutput()->GetPointData()->GetScalars() );
   
   writer->SetFileName("c:/Users/Jerome/Desktop/img_probe.vti");
   writer->SetInputData(img);
   writer->Write( );

   return( 0 );
}


