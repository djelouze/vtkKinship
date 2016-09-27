//! \file DeformableMesh.cxx
//! Basic illustration of the vtkDeformableMesh filter
//! in an infinite loop.
//!
//! \author Jerome Velut
//! \date February 2011


#include "vtkPolyDataToBinaryImage.h"

#include "vtkSmartPointer.h"
#include "vtkImageData.h"
#include "vtkSTLReader.h"
#include "vtkXMLImageDataWriter.h"


int main( int argc, char** argv )
{

   // Build data processing pipeline
  vtkSmartPointer<vtkSTLReader> reader;
  reader = vtkSmartPointer<vtkSTLReader>::New();

  vtkSmartPointer<vtkPolyDataToBinaryImage> pdToImg;
  pdToImg = vtkSmartPointer<vtkPolyDataToBinaryImage>::New();

  vtkSmartPointer<vtkXMLImageDataWriter> writer;
  writer = vtkSmartPointer<vtkXMLImageDataWriter>::New();

  pdToImg->SetInputConnection( reader->GetOutputPort() );
  writer->SetInputConnection( pdToImg->GetOutputPort() );

  reader->SetFileName( argv[1] );
  std::cerr << "reader" << std::endl;
  reader->Update();
  std::cerr << "pdToImg" << std::endl;
  pdToImg->SetInsideValue( 255 );
  pdToImg->SetOutputSpacing( 0.3, 0.3, 0.3 );
  pdToImg->Update();
  std::cerr << "writer" << std::endl;
  writer->SetFileName( "artery_cube.vti" );
  writer->Write();


  return( 0 );
}


