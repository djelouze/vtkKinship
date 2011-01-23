// Copyright (c) 2010, Jérôme Velut
// All rights reserved.
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT OWNER ``AS IS'' AND ANY EXPRESS 
// OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN 
// NO EVENT SHALL THE COPYRIGHT OWNER BE LIABLE FOR ANY DIRECT, INDIRECT, 
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
// OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "vtkStraightenedCurvedPlanarReformation.h"

#include "vtkXMLPolyDataReader.h"
#include "vtkXMLImageDataReader.h"
#include "vtkXMLImageDataWriter.h"
#include "vtkImageEllipsoidSource.h"
#include "vtkLineSource.h"
#include "vtkSmartPointer.h"

int main(int argc, char** argv )
{

    vtkSmartPointer<vtkImageAlgorithm> volumeProducer;
    vtkSmartPointer<vtkPolyDataAlgorithm> splineProducer;

    if( argc == 2)
    {
       cout << "\ttestStraightenedCurvedPlanarReformation [volume] [spline]"
            << endl;
       cout << "[volume] is a xml vtkImageData (vti) volume" << endl;
       cout << "[spline] is an xml vtkPolyData (vtp) polyline" << endl;
       return( 0 );
    }
    else if( argc == 3 )
    {
    vtkSmartPointer<vtkXMLPolyDataReader> splineReader;
    splineReader = vtkSmartPointer<vtkXMLPolyDataReader>::New( );
    splineReader->SetFileName( argv[2] );
    splineProducer = vtkPolyDataAlgorithm::SafeDownCast( splineReader );
 
    vtkSmartPointer<vtkXMLImageDataReader> volumeReader;
    volumeReader = vtkSmartPointer<vtkXMLImageDataReader>::New( );
    volumeReader->SetFileName( argv[1] );
    volumeProducer = vtkImageAlgorithm::SafeDownCast( volumeReader );
    }
    else if( argc == 1 )
    {
       vtkSmartPointer<vtkImageEllipsoidSource> ellipsoid;
       ellipsoid = vtkSmartPointer<vtkImageEllipsoidSource>::New( );
       int extent[6];
       extent[0] = 0;
       extent[2] = 0;
       extent[4] = 0;
       extent[1] = 50;
       extent[3] = 50;
       extent[5] = 50;
       ellipsoid->SetCenter( 25, 25, 25 );
       ellipsoid->SetRadius( 5, 5, 10 );
       ellipsoid->SetWholeExtent( extent );

       vtkSmartPointer<vtkLineSource> line;
       line = vtkSmartPointer<vtkLineSource>::New( );
       line->SetPoint1(extent[0], extent[2], extent[4] );
       line->SetPoint2(extent[1], extent[3], extent[5] );
       line->SetResolution( 100 );
       line->Update( );
       cout << endl <<line->GetOutput( )->GetLines( )->GetNumberOfCells( ) << endl;
 
       splineProducer = vtkPolyDataAlgorithm::SafeDownCast( line );
       volumeProducer = vtkImageAlgorithm::SafeDownCast( ellipsoid );     
       
    }

    vtkSmartPointer<vtkStraightenedCurvedPlanarReformation> cprFilter;
    cprFilter = vtkSmartPointer<vtkStraightenedCurvedPlanarReformation>::New( );
    
    cprFilter->SetInput( volumeProducer->GetOutputDataObject(0 ) );
    cprFilter->SetInput( 1, splineProducer->GetOutput( ) );
    cprFilter->Update( );

    vtkSmartPointer<vtkXMLImageDataWriter> volumeWriter;
    volumeWriter = vtkSmartPointer<vtkXMLImageDataWriter>::New( );
    volumeWriter->SetFileName( "output.vti"  );
    volumeWriter->SetInputConnection( cprFilter->GetOutputPort() );
    volumeWriter->Write( );
    
}
