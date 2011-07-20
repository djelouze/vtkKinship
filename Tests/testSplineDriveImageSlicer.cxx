// Copyright (c) 2011, Jérôme Velut
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

#include <vtkSplineDrivenImageSlicer.h>

#include <vtkImageEllipsoidSource.h>
#include <vtkImageData.h>
#include <vtkLineSource.h>
#include <vtkSmartPointer.h>


int main(int argc, char** argv )
{
    // Create an ellipsoid
    vtkSmartPointer<vtkImageEllipsoidSource> imgSource = vtkSmartPointer<vtkImageEllipsoidSource>::New( );
    imgSource->SetWholeExtent( 0, 255, 0, 255, 0, 255 );
    imgSource->SetCenter( 127, 127, 127 );
    imgSource->SetRadius( 50, 50, 50 );
    imgSource->Update();
    double imgBB[6];
    imgSource->GetOutput()->GetBounds( imgBB );
    
    // Create a spline
    vtkSmartPointer<vtkLineSource> linSource = vtkSmartPointer<vtkLineSource>::New( );
    linSource->SetPoint1( imgBB[0], imgBB[2],imgBB[4]);
    linSource->SetPoint2( imgBB[1], imgBB[3],imgBB[5]);
    linSource->SetResolution( 20 );

    // tested object
    vtkSmartPointer<vtkSplineDrivenImageSlicer> sdis;
    sdis = vtkSmartPointer<vtkSplineDrivenImageSlicer>::New( );
    sdis->SetInputConnection( 0, imgSource->GetOutputPort( ));
    sdis->SetInputConnection( 1, linSource->GetOutputPort( ));
    sdis->SetOffsetPoint( 10 );
    sdis->SetSliceExtent( 200, 200 );
    sdis->SetSliceSpacing( 0.5,0.5 );

    sdis->Update( );
  
    // Check output scalar range:
    double scalarRange[2];
    sdis->GetOutput(0)->GetScalarRange(scalarRange);
    if( scalarRange[0] != 0 || scalarRange[1] != 255 )
       return( EXIT_FAILURE );
    
    // Check plane output bounds
    static_cast<vtkPolyData*>(sdis->GetOutputDataObject(1))->ComputeBounds( );
    double bounds[6];
    static_cast<vtkPolyData*>(sdis->GetOutputDataObject(1))->GetBounds( bounds );

    // Bounds centroid:
    double sum = 0;
    for(int i = 0; i < 6; i++ )
        sum += bounds[i]/6.0;
   
    double epsilon = 2;
    if( fabs(sum - 127) > epsilon )
        return( EXIT_FAILURE ); 

    return( EXIT_SUCCESS );
}

