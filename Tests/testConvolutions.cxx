#include <vtkImageConvolution.h>

#include <vtkSmartPointer.h>
#include <vtkImageGaussianSource.h>
#include <vtkImageCanvasSource2D.h>
#include <vtkJPEGWriter.h>
#include <vtkImageCast.h>
#include <vtkImageData.h>

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
