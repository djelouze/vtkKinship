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

#include "yaap.h"

int main( int argc, char** argv )
{
  yaap::Parser parser(argc, argv,"Use of spline-driven image slicer for panoramic imaging");
  yaap::OptionArg<std::string>* inputImage = parser.AddOptionArg<std::string>('i', "Input image (vti) to reformat along the input path", 1, true);
  yaap::OptionArg<std::string>* inputPath = parser.AddOptionArg<std::string>('p', "Input path (vtp) along which the input image will be reformatted", 1, true);
  yaap::OptionArg<std::string>* outputImage = parser.AddOptionArg<std::string>('o', "Output reformatted image (vti)", 1, true);

  yaap::OptionArg<int>* extentOpt = parser.AddOptionArg<int>('e', "Extent of the ouput slices (default to 256x256)", 2, false);
  yaap::OptionArg<double>* spacingOpt = parser.AddOptionArg<double>('s', "X/Y spacing (default to 1)", 2, false);
  yaap::OptionArg<double>* thicknessOpt = parser.AddOptionArg<double>('t', "Thickness (Z-spacing - default to 1)", 1, false);
  
yaap::Option* helpOpt = parser.AddOption('h', "Show this message");

  if( !parser.IsCommandLineValid( ) || helpOpt->Exists() || argc ==  1)
  {
    parser.Usage( );
    return( 0 );
  }


   vtkSmartPointer<vtkXMLImageDataReader> imgReader;
   imgReader = vtkSmartPointer<vtkXMLImageDataReader>::New();

   vtkSmartPointer<vtkXMLPolyDataReader> pathReader;
   pathReader = vtkSmartPointer<vtkXMLPolyDataReader>::New();
   
   vtkSmartPointer<vtkSplineDrivenImageSlicer> reslicer;
   reslicer = vtkSmartPointer<vtkSplineDrivenImageSlicer>::New();
   
   vtkIdType extent[2] = {255,255};
   if( extentOpt->Exists() )
   {
     extent[0] = extentOpt->GetArgument(0);
     extent[1] = extentOpt->GetArgument(1);
   }
      
   double thickness = 1.0;
   if( thicknessOpt->Exists() )
   {
     thickness = thicknessOpt->GetValue();
   }

   double spacing[2] = {1,1};
   if( spacingOpt->Exists() )
   {
     spacing[0] = spacingOpt->GetArgument(0);
     spacing[1] = spacingOpt->GetArgument(1);
   }
 
   reslicer->SetSliceSpacing( spacing );
   reslicer->SetSliceThickness( thickness );
   reslicer->SetSliceExtent( extent );   

   vtkSmartPointer<vtkImageAppend> append;
   append = vtkSmartPointer<vtkImageAppend>::New();
   
   vtkSmartPointer<vtkXMLImageDataWriter> imgWriter;
   imgWriter = vtkSmartPointer<vtkXMLImageDataWriter>::New();
   
   imgReader->SetFileName(inputImage->GetValue().c_str());
   imgReader->Update();
   pathReader->SetFileName(inputPath->GetValue().c_str());
   pathReader->Update();
   
   reslicer->SetInputConnection(imgReader->GetOutputPort() );
   reslicer->SetPathConnection(pathReader->GetOutputPort());
   
   // Get number of input points
   int nbPoints = pathReader->GetOutput( )->GetNumberOfPoints();
   for( int ptId = 0; ptId < nbPoints; ptId++ )
   {
     reslicer->SetOffsetPoint( ptId );
     reslicer->Update();
     
     vtkSmartPointer<vtkImageData> tempSlice;
     tempSlice = vtkSmartPointer<vtkImageData>::New();
     tempSlice->DeepCopy( reslicer->GetOutput( 0 ));
     
     append->AddInputData( tempSlice );
   }
   append->SetAppendAxis(2);
   append->Update( );
   
   imgWriter->SetFileName(outputImage->GetValue().c_str());
   imgWriter->SetInputConnection(append->GetOutputPort());
   imgWriter->Write( );
   
   return( 0 );
}


