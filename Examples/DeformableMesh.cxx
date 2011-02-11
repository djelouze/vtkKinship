//! \file DeformableMesh.cxx
//! Basic illustration of the vtkDeformableMesh filter
//! in an infinite loop.
//!
//! \author Jerome Velut
//! \date February 2011


#include "vtkDeformableMesh.h"

#include "vtkSmartPointer.h"

#include "vtkSphereSource.h"
#include "vtkImageEllipsoidSource.h"
#include "vtkImageSobel3D.h"
#include "vtkImageMagnitude.h"

#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkProperty.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"


int main( int argc, char** argv )
{

   // Build data processing pipeline
   vtkSmartPointer<vtkSphereSource> sphereSource;
   sphereSource = vtkSmartPointer<vtkSphereSource>::New( );

   vtkSmartPointer<vtkDeformableMesh> deformableMesh;
   deformableMesh = vtkSmartPointer<vtkDeformableMesh>::New();

   vtkSmartPointer<vtkImageEllipsoidSource> ellipsoidSource;
   ellipsoidSource = vtkSmartPointer<vtkImageEllipsoidSource>::New();

   vtkSmartPointer<vtkImageSobel3D> sobel1;
   sobel1 = vtkSmartPointer<vtkImageSobel3D>::New();

   vtkSmartPointer<vtkImageMagnitude> magnitude;
   magnitude = vtkSmartPointer<vtkImageMagnitude>::New();
   
   vtkSmartPointer<vtkImageSobel3D> sobel2;
   sobel2 = vtkSmartPointer<vtkImageSobel3D>::New();

   // Configure PointSource
   ellipsoidSource->SetWholeExtent(0,63,0,63,0,63);
   ellipsoidSource->SetCenter(31,31,31);
   ellipsoidSource->SetRadius(18, 14, 18);

   // External forces computation
   sobel1->SetInputConnection(ellipsoidSource->GetOutputPort( ));
   magnitude->SetInputConnection(sobel1->GetOutputPort());
   sobel2->SetInputConnection(magnitude->GetOutputPort());
   
   // Deformable model initialisation
   sphereSource->SetCenter(31,31,31);
   sphereSource->SetRadius(16);
   sphereSource->SetPhiResolution(36);
   sphereSource->SetThetaResolution(36);
   
   // Configure deformableMesh
   deformableMesh->IterateFromZeroOff();
   deformableMesh->SetScaleFactor(.0003);
   // Model
   deformableMesh->SetInputConnection(0, sphereSource->GetOutputPort());
   // Deformation forces
   deformableMesh->SetInputConnection(1, sobel2->GetOutputPort( ));
   deformableMesh->SetInputArrayToProcess( 
      0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, "Magnitude" );
   
   // Visualisation pipeline
   vtkSmartPointer<vtkPolyDataMapper> mapper;
   mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
   
   vtkSmartPointer<vtkActor> actor;
   actor = vtkSmartPointer<vtkActor>::New();
   
   vtkSmartPointer<vtkRenderer> renderer;
   renderer = vtkSmartPointer<vtkRenderer>::New();
   
   vtkSmartPointer<vtkRenderWindow> renWin;
   renWin = vtkSmartPointer<vtkRenderWindow>::New();
   
   mapper->SetInputConnection(deformableMesh->GetOutputPort());
   mapper->SetScalarVisibility(0);

   actor->SetMapper(mapper);
   actor->GetProperty()->SetRepresentationToWireframe();
   renderer->AddActor(actor);
   
   renWin->AddRenderer(renderer);
   
   // Iterative loop: 
   // - Increment the number of iteration
   // - Render -> it will trigger only one iteration in the DeformableMesh.
   while( 1 ) 
   {
     deformableMesh->SetNumberOfIterations(
              deformableMesh->GetNumberOfIterations()+1);
     renWin->Render();
   }

   return( 0 );
}


