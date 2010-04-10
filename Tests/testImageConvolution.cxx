// Copyright (c) 2010, Jérôme Velut
// All rights reserved.
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of the University of California, Berkeley nor the
//       names of its contributors may be used to endorse or promote products
//       derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.



//include <vtkImageConvolution.h>

//include <vtkSmartPointer.h>
//include <vtkImageGaussianSource.h>
//include <vtkImageCanvasSource2D.h>
//include <vtkJPEGWriter.h>
//include <vtkImageCast.h>
//include <vtkImageData.h>

int main( int argc, char* argv[] )
{
   // Objects instanciation
   vtkSmartPointer<vtkImageCanvasSource2D> image = vtkSmartPointer<vtkImageCanvasSource2D>::New( );
   vtkSmartPointer<vtkImageGaussianSource> kernel = vtkSmartPointer<vtkImageGaussianSource>::New( );
   vtkSmartPointer<vtkImageConvolution> convolutionFilter = vtkSmartPointer<vtkImageConvolution>::New( );
   vtkSmartPointer<vtkImageCast> castFilter = vtkSmartPointer<vtkImageCast>::New( );
   vtkSmartPointer<vtkJPEGWriter> writer = vtkSmartPointer<vtkJPEGWriter>::New( );

   // Pipeline construction
   convolutionFilter->SetInputConnection( image->GetOutputPort( ) );
   convolutionFilter->SetKernelConnection( kernel->GetOutputPort( ) );
   castFilter->SetInputConnection( convolutionFilter->GetOutputPort( ));
   writer->SetInputConnection( castFilter->GetOutputPort( ));

   // Setting the image
   image->SetExtent( 0, 255, 0, 255, 0, 0 );
   image->SetDrawColor( 255,255,255,255 );
   image->SetScalarTypeToUnsignedChar( );
   image->DrawPoint( 127, 127 );
   image->DrawPoint( 12, 127 );
   image->DrawPoint( 127, 12 );
   image->DrawPoint( 100, 100 );

   // Setting the kernel
   kernel->SetWholeExtent( 0, 18, 0, 18, 0, 0 );
   kernel->SetCenter( 8, 8, 0 );
   kernel->SetStandardDeviation( 3 );

   // casting the convolved image to unsigned char (suitable for jpg
   castFilter->SetOutputScalarTypeToUnsignedChar( );

   // Setting the writer
   writer->SetFileName("test.jpg" );

   writer->Write( );
}
