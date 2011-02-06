//! \file IterativeWarp.cxx
//! Basic illustration of the vtkPolyDataIterativeWarp filter
//! in an infinite loop.
//!
//! \author Jerome Velut
//! \date February 2011


#include "vtkPolyDataIterativeWarp.h"

#include "vtkSmartPointer.h"
#include "vtkPointSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"


int main( int argc, char** argv )
{

   // Build visualisation pipeline
   vtkSmartPointer<vtkPointSource> pointSource;
   pointSource = vtkSmartPointer<vtkPointSource>::New( );

   vtkSmartPointer<vtkPolyDataIterativeWarp> iterativeWarp;
   iterativeWarp = vtkSmartPointer<vtkPolyDataIterativeWarp>::New();
   
   vtkSmartPointer<vtkPolyDataMapper> mapper;
   mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
   
   vtkSmartPointer<vtkActor> actor;
   actor = vtkSmartPointer<vtkActor>::New();
   
   vtkSmartPointer<vtkRenderer> renderer;
   renderer = vtkSmartPointer<vtkRenderer>::New();
   
   vtkSmartPointer<vtkRenderWindow> renWin;
   renWin = vtkSmartPointer<vtkRenderWindow>::New();
   
   iterativeWarp->SetInputConnection(pointSource->GetOutputPort());
   mapper->SetInputConnection(iterativeWarp->GetOutputPort());
   actor->SetMapper(mapper);
   renderer->AddActor(actor);
   
   renWin->AddRenderer(renderer);
   
   // Configure PointSource
   pointSource->SetNumberOfPoints(10000);
   pointSource->SetRadius(1);
   
   // Configure iterativeWarp
   iterativeWarp->IterateFromZeroOff();
   iterativeWarp->SetScaleFactor(.01);
   
   while( 1 )
   {
     iterativeWarp->SetNumberOfIterations(iterativeWarp->GetNumberOfIterations()+1);
     renWin->Render();
   }

   return( 0 );
}


