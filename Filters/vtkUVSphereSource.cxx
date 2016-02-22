#include <vtkUVSphereSource.h>

#include "vtkCellArray.h"
#include "vtkIdList.h"
#include "vtkCellTypes.h"
#include "vtkDoubleArray.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkMath.h"
#include <math.h>
#include <vtkPointData.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkObjectFactory.h>

vtkStandardNewMacro(vtkUVSphereSource);

#define PI vtkMath::Pi( )
#define RANDOM (double)rand()/RAND_MAX*this->NoiseLevel - this->NoiseLevel/2.0

vtkUVSphereSource::vtkUVSphereSource()
{
   this->MeridianResolution = 20;
   this->ParallelResolution = 20;
   this->Radius = 50;
   this->NoiseLevel = 0;
   this->ModulationAmplitude = 0;
   this->NumberOfModulations = 0;
   this->BuildVectorData = 0;

   this->Center[0] = this->Center[1] = this->Center[2] = 0.0;
   this->SetNumberOfInputPorts(0);
}


int vtkUVSphereSource::RequestData(
    vtkInformation *vtkNotUsed(request),
    vtkInformationVector **inputVector,
    vtkInformationVector *outputVector)
{
   vtkDebugMacro( << " PolyLineSphereSource Execute ");
   
   vtkPoints *newPoints; 
   vtkIdList *newPolys,*newVertices;
   vtkDoubleArray *vectors;
    vtkInformation *outMeshInfo = outputVector->GetInformationObject(0);

    vtkPolyData* output = vtkPolyData::SafeDownCast(
                           outMeshInfo->Get(vtkDataObject::DATA_OBJECT()));
   output->Reset( );
   
   int numPts = this->ParallelResolution*this->MeridianResolution*2;

   vtkCellArray* isoLines = vtkCellArray::New( );
   vtkCellArray* integralLines = vtkCellArray::New( );
   vtkCellArray* triangles = vtkCellArray::New( );      
   output->SetLines(integralLines);
   output->SetVerts(isoLines);
   output->SetPolys(triangles);
   
   newPoints = vtkPoints::New();
  
   newPolys = vtkIdList::New();
   newVertices = vtkIdList::New();
   vectors = vtkDoubleArray::New( );

   vectors->SetNumberOfComponents( 3 );

   newVertices->InsertNextId(0);
   int nbVerts = 0;

   double vector[3], offset;

   // Generate points and point data
   //

   // inserting south pole :
   if( this->BuildVectorData )
   {
      offset = 0;
      vector[0] = RANDOM;
      vector[1] = RANDOM;
      vector[2] = RANDOM;
   }
   else
      offset = RANDOM;

   vtkIdType southPole = newPoints->InsertNextPoint(this->Center[0],
      this->Center[1],
      this->Center[2]-this->Radius+offset);
   if( this->BuildVectorData )
      vectors->InsertNextTuple( vector );

   newVertices->InsertNextId( southPole );
   nbVerts++;

   // inserting north pole : 
   vtkIdType northPole = newPoints->InsertNextPoint(this->Center[0],
      this->Center[1],
      this->Center[2]+this->Radius+offset);
   if( this->BuildVectorData )
      vectors->InsertNextTuple( vector );

   newVertices->InsertNextId( northPole );
   nbVerts++;

   vtkIdType** meridianPolyLines = new vtkIdType*[this->ParallelResolution];
   for( int i = 0; i < this->ParallelResolution;i++ )
   {
      meridianPolyLines[i] = new vtkIdType[this->MeridianResolution+1];
      // setting northPole as first point of each meridian
      meridianPolyLines[i][0] = northPole;
   }

   int phi, theta;
   vtkDebugMacro( << " \t Building PolyLine and Triangles ");
   for ( phi = 1 ; phi<this->MeridianResolution ; phi++ )
   {
      // Building one parallele line ///////////////////////////////////
      newPolys->Reset( );
      vtkIdType extremity;
      for( theta = 0 ; theta<this->ParallelResolution ; theta++ )
      {

         if( this->BuildVectorData == 1 )
         {
            offset = 0;
            vector[0] = RANDOM;
            vector[1] = RANDOM;
            vector[2] = RANDOM;
         }
         else if( this->BuildVectorData == 0 )
         {
            offset = RANDOM;
         }
         else
         {
            offset = (this->Radius/10)*( sin( theta*2*PI/this->ParallelResolution*100 )+
                                       sin( phi*2*PI / this->MeridianResolution*100  ) );
         }

         vtkIdType idp = newPoints->InsertNextPoint((this->Radius+offset)*cos(theta*2*PI/this->ParallelResolution)*sin(phi*PI/this->MeridianResolution)+this->Center[0],
            (this->Radius+offset)*sin(theta*2*PI/this->ParallelResolution)*sin(phi*PI/this->MeridianResolution)+this->Center[1],
            (this->Radius+offset)*cos(phi*PI/this->MeridianResolution)+this->Center[2]);

         if( this->BuildVectorData )
            vectors->InsertNextTuple( vector );

         newPolys->InsertNextId( idp );
         newVertices->InsertNextId( idp );
         nbVerts++;
         if( theta == 0)
            extremity = idp;
         meridianPolyLines[theta][phi] = idp;

         ///////////// INSERT 2 TRIANGLES (as a quad)
         if( theta > 0 )
         {
            vtkIdType triangleInf[3];
            triangleInf[0] = meridianPolyLines[theta-1][phi-1];
            triangleInf[1] = meridianPolyLines[theta%this->ParallelResolution][phi];
            triangleInf[2] = meridianPolyLines[theta-1][phi];
            triangles->InsertNextCell( 3, triangleInf);
            if( phi > 1 )
            {
               vtkIdType triangleSup[3];
               triangleSup[0] = meridianPolyLines[theta-1][phi-1];
               triangleSup[1] = meridianPolyLines[theta%this->ParallelResolution][phi-1];
               triangleSup[2] = meridianPolyLines[theta%this->ParallelResolution][phi];
               triangles->InsertNextCell( 3, triangleSup);
            }
         }         
      }
      newPolys->InsertNextId( extremity );
      
      /////////////// Close the triangulation along the parallels
      vtkIdType triangleInf[3];
      triangleInf[0] = meridianPolyLines[theta-1][phi-1];
      triangleInf[1] = meridianPolyLines[theta%this->ParallelResolution][phi];
      triangleInf[2] = meridianPolyLines[theta-1][phi];
      triangles->InsertNextCell( 3, triangleInf);

      vtkIdType triangleSup[3];
      triangleSup[0] = meridianPolyLines[theta-1][phi-1];
      triangleSup[1] = meridianPolyLines[theta%this->ParallelResolution][phi-1];
      triangleSup[2] = meridianPolyLines[theta%this->ParallelResolution][phi];
      triangles->InsertNextCell( 3, triangleSup);      
      
      isoLines->InsertNextCell( this->ParallelResolution+1, newPolys->GetPointer( 0 ) );

      /////////////////////////////////////////////////////////////////
   }

   for( int i = 0; i < this->ParallelResolution;i++ )
   {
      // setting southPole as last point of each meridian
      meridianPolyLines[i][this->MeridianResolution] = southPole;

      integralLines->InsertNextCell( this->MeridianResolution+1, meridianPolyLines[i]);
      /////////////// Closed the triangulation along the meridians
      vtkIdType triangleInf[3];
      triangleInf[0] = meridianPolyLines[i][this->MeridianResolution-1];
      triangleInf[1] = meridianPolyLines[(i+1)%this->ParallelResolution][this->MeridianResolution-1];
      triangleInf[2] = meridianPolyLines[i][this->MeridianResolution];
      triangles->InsertNextCell( 3, triangleInf);  
   }

   vtkDebugMacro( << " \t Releasing memory ");
   // Update ourselves and release memory
   output->SetPoints(newPoints);
   newPoints->Delete( );
   
   vectors->Squeeze( );
   if( this->BuildVectorData )
       output->GetPointData( )->SetVectors( vectors );
   vectors->Delete( );

   newPolys->Delete();
   newVertices->Delete();

   for( int i = 0; i < this->ParallelResolution;i++ )
      delete[] meridianPolyLines[i];
   delete[] meridianPolyLines;


   output->GetLines()->Delete( );
   output->GetVerts()->Delete( );
   output->GetPolys()->Delete( );
 return( 1 );
}


//----------------------------------------------------------------------------
void vtkUVSphereSource::SetOutput(vtkPolyData *output)
{
//  this->GetExecutive()->SetOutputData(0, output);
}

void vtkUVSphereSource::PrintSelf(ostream& os, vtkIndent indent)
{
   this->Superclass::PrintSelf(os,indent);

   os << indent << "Center: (" << this->Center[0] << ", "
      << this->Center[1] << ", "
      << this->Center[2] << ")\n";
}
