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

#include <vtkFrenetSerretFrame.h>

#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkCellArray.h>
#include <vtkMath.h>
#include <vtkSmartPointer.h>

int TestArray( vtkPolyDataAlgorithm* algo,const char* name )
{
   vtkDoubleArray* array = 0x0;
   array = static_cast<vtkDoubleArray*>(algo->GetOutput()->GetPointData()->GetArray( name ));
   if( !array )
      return( EXIT_FAILURE );
   if( array->GetValueRange(0)[0] != -1 ) // Range of X component
      return( EXIT_FAILURE );

   if( array->GetValueRange(0)[1] != 1 )
      return( EXIT_FAILURE );

   return( EXIT_SUCCESS );
}

int main(int argc, char** argv )
{
    // Generate a circle:
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New( );
    vtkSmartPointer<vtkCellArray> line = vtkSmartPointer<vtkCellArray>::New( );
    line->InsertNextCell( 360 );
    vtkIdType ptId;
    for( int i = 0; i < 360; i++)
    {
       ptId = points->InsertNextPoint( cos( i*2*vtkMath::DoublePi()/360.0 ), sin( i*2*vtkMath::DoublePi()/360.0 ), 0 );
       line->InsertCellPoint( ptId );
    }


    vtkSmartPointer<vtkPolyData> circle = vtkSmartPointer<vtkPolyData>::New( );
    circle->SetLines( line );
    circle->SetPoints( points );
    
    // tested object
    vtkSmartPointer<vtkFrenetSerretFrame> frenetSerretFilter;
    frenetSerretFilter = vtkSmartPointer<vtkFrenetSerretFrame>::New( );
    
    frenetSerretFilter->SetInput( circle );
    frenetSerretFilter->Update( );

    // Roughly check the result
    if( TestArray( frenetSerretFilter, "FSTangents" ) == EXIT_FAILURE )
        return( EXIT_FAILURE );

    return( EXIT_SUCCESS );
}

