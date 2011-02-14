//! \file testDeformableMesh.cxx
//!
//! \author Jerome Velut
//! \date February 2011


#include "vtkDeformableMesh.h"

#include "vtkSmartPointer.h"

#include "vtkSphereSource.h"
#include "vtkImageEllipsoidSource.h"
#include "vtkImageSobel3D.h"
#include "vtkImageMagnitude.h"

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

   double center[3] = {31,31,31};
   double radii[3] = {18,14,18};
   // Configure PointSource
   ellipsoidSource->SetWholeExtent(0,63,0,63,0,63);
   ellipsoidSource->SetCenter(center);
   ellipsoidSource->SetRadius(radii);

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
   deformableMesh->SetScaleFactor(.0003);
   deformableMesh->SetNumberOfIterations( 150 );

   // Model
   deformableMesh->SetInputConnection(0, sphereSource->GetOutputPort( 0 ));

   // Deformation forces
   deformableMesh->SetInputConnection(1, sobel2->GetOutputPort( 0 ));
   deformableMesh->SetInputArrayToProcess( 
      0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, "Magnitude" );
   
   cerr << "Updating deformableMesh..." << endl;
   deformableMesh->Update();
   cerr << "Done." << endl;
   
   double bounds[6];
   vtkPolyData::SafeDownCast(deformableMesh->GetOutputDataObject(0))->GetBounds( bounds );
   
   // Check if the bounds of the deformable mesh are closed to
   // the ellipsoid border (less than 1 voxel is SUCCESS)
   if( fabs((center[0]-radii[0])-bounds[0]) < 1 
     && fabs((center[0]+radii[0])-bounds[1]) < 1 
     && fabs((center[1]-radii[1])-bounds[2]) < 1 
     && fabs((center[1]+radii[1])-bounds[3]) < 1 
     && fabs((center[2]-radii[2])-bounds[4]) < 1 
     && fabs((center[2]+radii[2])-bounds[5]) < 1 ) 
     
     return( EXIT_SUCCESS );
   
   return( EXIT_FAILURE );
}


