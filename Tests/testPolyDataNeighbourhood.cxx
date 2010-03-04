#include "vtkSphereSource.h"
#include "vtkPolyDataNeighbourhood.h"

int main( )
{

   vtkSphereSource* sphere = vtkSphereSource::New( );
   sphere->SetThetaResolution( 20 );
   sphere->SetPhiResolution( 20 );
   vtkPolyDataNeighbourhood* neighbourhood = vtkPolyDataNeighbourhood::New( );
   neighbourhood->DebugOn( );
   neighbourhood->SetInputConnection( sphere->GetOutputPort() );

   neighbourhood->Update( );
}
