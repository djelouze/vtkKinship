//! \file SplineDrivenImageSlicer.cxx
//!
//! \author Jerome Velut
//! \date February 2011

#include "vtkDualPolyDataFilter.h"

#include "vtkSmartPointer.h"

#include "vtkXMLPolyDataReader.h"
#include "vtkTriangleFilter.h"
#include "vtkXMLPolyDataWriter.h"

int main( int argc, char** argv )
{
   vtkSmartPointer<vtkXMLPolyDataReader> reader;
   reader = vtkSmartPointer<vtkXMLPolyDataReader>::New();
   
   vtkSmartPointer<vtkTriangleFilter> triangle;
   triangle = vtkSmartPointer<vtkTriangleFilter>::New();
   
   vtkSmartPointer<vtkDualPolyDataFilter> dual;
   dual = vtkSmartPointer<vtkDualPolyDataFilter>::New();
   
   vtkSmartPointer<vtkXMLPolyDataWriter> writer;
   writer = vtkSmartPointer<vtkXMLPolyDataWriter>::New();

   
   reader->SetFileName(argv[1]);
   
   triangle->SetInputConnection(reader->GetOutputPort() );
   dual->SetInputConnection(triangle->GetOutputPort());
   
   writer->SetFileName(argv[2]);
   writer->SetInputConnection(dual->GetOutputPort(0));
   writer->Write( );

   return( 0 );
}


