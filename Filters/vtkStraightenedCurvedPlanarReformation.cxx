#include "vtkStraightenedCurvedPlanarReformation.h"
#include "vtkFrenetSerretFrame.h"
#include "vtkPlaneSource.h"
#include "vtkImageData.h"
#include "vtkMatrix4x4.h"
#include "vtkImageAppend.h"
#include "vtkDoubleArray.h"
#include "vtkPointData.h"
#include "vtkMath.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"
#include "vtkStreamingDemandDrivenPipeline.h"


vtkCxxRevisionMacro(vtkStraightenedCurvedPlanarReformation, "$Revision: 1.31 $");
vtkStandardNewMacro(vtkStraightenedCurvedPlanarReformation);

vtkStraightenedCurvedPlanarReformation::vtkStraightenedCurvedPlanarReformation( )
{
   this->localFrenetFrames = vtkFrenetSerretFrame::New( );
   this->SliceSize = 15;
   this->XYSpacing = 1;
   this->ZSpacing = 1;
   this->AutoZSpacing = 0;
   this->SetNumberOfInputPorts( 2 );
   this->DebugOn( );
}

vtkStraightenedCurvedPlanarReformation::~vtkStraightenedCurvedPlanarReformation( )
{
	this->localFrenetFrames->Delete( );
}

//----------------------------------------------------------------------------
// Specify a source object at a specified table location.
void vtkStraightenedCurvedPlanarReformation::SetPathConnection(int id, vtkAlgorithmOutput* algOutput)
{
  if (id < 0)
    {
    vtkErrorMacro("Bad index " << id << " for source.");
    return;
    }

  int numConnections = this->GetNumberOfInputConnections(1);
  if (id < numConnections)
    {
    this->SetNthInputConnection(1, id, algOutput);
    }
  else if (id == numConnections && algOutput)
    {
    this->AddInputConnection(1, algOutput);
    }
  else if (algOutput)
    {
    vtkWarningMacro("The source id provided is larger than the maximum "
                    "source id, using " << numConnections << " instead.");
    this->AddInputConnection(1, algOutput);
    }
}

//---------------------------------------------------------------------------
int vtkStraightenedCurvedPlanarReformation::FillInputPortInformation(int port, vtkInformation *info)
{
  if( port == 0 )
     info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkImageData");
   else
     info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData");

  return 1;
}

int vtkStraightenedCurvedPlanarReformation::RequestUpdateExtent (
  vtkInformation * vtkNotUsed(request),
  vtkInformationVector** inputVector,
  vtkInformationVector *outputVector)
{
  vtkDebugMacro( <<"RequestUpdateExtent"<<endl);
  //! \bug RequestUpdateExtent set the filter to Modified each time it is 
  //! called. One effect is to call RequestData when slicing through the
  //! output.
 
  // get the info objects
  vtkInformation* outInfo = outputVector->GetInformationObject(0);
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);

  int outExtent[6];
  outInfo->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), outExtent);
  outInfo->Set(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT(), outExtent, 6);
  inInfo->Set(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT(), outExtent, 6);
  
   return( 1 );
}

int vtkStraightenedCurvedPlanarReformation::RequestInformation (
  vtkInformation * vtkNotUsed(request),
  vtkInformationVector** inputVector,
  vtkInformationVector *outputVector)
{

  vtkDebugMacro( <<"RequestInformation"<<endl);
  // get the info objects
  vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation* pathInfo = inputVector[1]->GetInformationObject(0);
  vtkInformation* outInfo = outputVector->GetInformationObject(0);

   // Get the path object
  vtkPolyData *path = vtkPolyData::SafeDownCast( pathInfo->Get(vtkDataObject::DATA_OBJECT()));
   // from path, get the cells representing PolyLines
   vtkCellArray* lines = path->GetLines( );
   lines->InitTraversal( );
   int numberOfLinePoints = 0;
   // Count how many points are used in the cells
   // In case of loop, points may be used several times
   for( int cellIdx = 0; cellIdx < lines->GetNumberOfCells( ); cellIdx++ )
   {
      vtkIdType nbPoints;
      vtkIdType* points;
      lines->GetNextCell( nbPoints, points);
      numberOfLinePoints += nbPoints;
   }

   if( this->AutoZSpacing )
   {
      lines->InitTraversal( );
      vtkIdType nbPoints;
      vtkIdType* points;
      lines->GetNextCell( nbPoints, points);
	  double pt0[3], pt1[3];
	  // Spline filter gives a true sampling after point 1 
	  // ||pt0,pt1|| is often less than mean sampling because of clamping stuff
	  path->GetPoint( points[1], pt0 );
	  path->GetPoint( points[2], pt1 );
	  this->ZSpacing = vtkMath::Distance2BetweenPoints( pt0, pt1 );
	  this->ZSpacing = sqrt( this->ZSpacing );
   }

  int extent[6] = {0, this->SliceSize / this->XYSpacing - 1,
                   0, this->SliceSize / this->XYSpacing - 1, 
                   0, numberOfLinePoints - 1};
  double spacing[3] = {this->XYSpacing,this->XYSpacing,this->ZSpacing};


  outInfo->Set(vtkDataObject::SPACING(), spacing, 3);
  outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(),
               extent,6);

  //int outExtent[6], inExtent[6], uExtent[6];
  //outInfo->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), outExtent);
  //outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT(), uExtent);
  //inInfo->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), inExtent);

  //for( int i=0; i < 6; i+=2 )
  //{
  //   if( inExtent[i] > outExtent[i] )
  //      inExtent[i] = outExtent[i];
  //   if( inExtent[i+1] < outExtent[i+1] )
  //      inExtent[i+1] = outExtent[i+1];
  //}

  //outInfo->Set(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT(), inExtent, 6);
  return 1;
}

//----------------------------------------------------------------------------
int vtkStraightenedCurvedPlanarReformation::FillOutputPortInformation (
  vtkInformation * vtkNotUsed(request),
  vtkInformationVector** inputVector,
  vtkInformationVector *outputVector)
{

  vtkDebugMacro( <<"FillOutputPortInformation"<<endl);
  vtkInformation* outInfo = outputVector->GetInformationObject(0);
  outInfo->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkPolyData");
  return 1;
}


//! RequestData is called by the pipeline process. 
int vtkStraightenedCurvedPlanarReformation::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  vtkDebugMacro( <<"RequestData"<<endl);
  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *pathInfo = inputVector[1]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  // get the input and ouptut
  vtkImageData *input = vtkImageData::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));
//  vtkImageData *inputCopy = vtkImageData::New( );
//  inputCopy->ShallowCopy( input );
  vtkPolyData *inputPath = vtkPolyData::SafeDownCast(
    pathInfo->Get(vtkDataObject::DATA_OBJECT()));
//  vtkPolyData *pathCopy = vtkPolyData::New( );
//  pathCopy->ShallowCopy( inputPath );
   vtkImageData *output = vtkImageData::SafeDownCast(
   outInfo->Get(vtkDataObject::DATA_OBJECT()));

   // Compute the local normal and tangent to the path
   //this->localFrenetFrames->SetInput( pathCopy );
   this->localFrenetFrames->SetInputConnection( this->GetPathConnection( ) );
   this->localFrenetFrames->SetViewUp( this->Incidence );
   this->localFrenetFrames->Update( );

   // path will contain PointData array "Tangents" and "Vectors"
   vtkPolyData* path = static_cast<vtkPolyData*>
                      (this->localFrenetFrames->GetOutputDataObject( 0 ));
   // Count how many points are used in the cells
   // In case of loop, points may be used several times
   // (note: not using NumberOfPoints because we want only LINES points...)
   vtkCellArray* lines = path->GetLines( );
   lines->InitTraversal( );
   int numberOfPoints = 0;
   for( int cellIdx = 0; cellIdx < lines->GetNumberOfCells( ); cellIdx++ )
   {
      vtkIdType nbCellPoints;
      vtkIdType* points;
      lines->GetNextCell( nbCellPoints, points);
      numberOfPoints += nbCellPoints;
   }

   vtkSmartPointer<vtkPlaneSource> plane;
   plane = vtkSmartPointer<vtkPlaneSource>::New( );

   // for each line
   lines->InitTraversal( );
   for( int cellIdx = 0, reslicerId = 0; cellIdx < lines->GetNumberOfCells( ); cellIdx++ )
   {
      vtkIdType nbCellPoints;
      vtkIdType* points;

      lines->GetNextCell( nbCellPoints, points);

      // for each line point
      for( int i = 0 ;i < nbCellPoints;i++, reslicerId++)
      {
         int ptId = points[i];
         // Build a new reslicer with image input as input too.
         this->reslicers[reslicerId] = vtkImageReslice::New( );
         //this->reslicers[reslicerId]->SetInput( inputCopy ); 
         this->reslicers[reslicerId]->SetInputConnection( this->GetInputConnection( 0, 0) ); 

         // Get the Frenet-Serret chart at point ptId:
         // - position (center)
         // - tangent T
         // - normal N
         double center[3];
         path->GetPoints( )->GetPoint( ptId, center );
         plane->SetCenter( center );
         vtkDoubleArray* pathTangents = static_cast<vtkDoubleArray*>
                                  (path->GetPointData( )->GetArray( "Tangents" ));
         double tangent[3];
         pathTangents->GetTuple( ptId, tangent );
         plane->SetNormal( tangent );

         vtkDoubleArray* pathNormals = static_cast<vtkDoubleArray*>
                                  (path->GetPointData( )->GetArray( "Normals" ));
         double normal[3];
         pathNormals->GetTuple( ptId, normal );

         // the Frenet-Serret chart is made of T, N and B = T ^ N
         double crossProduct[3];
         vtkMath::Cross( tangent, normal, crossProduct );

         // Build the transformation matrix (inspired from vtkImagePlaneWidget)
         vtkMatrix4x4* resliceAxes = vtkMatrix4x4::New( );
         resliceAxes->Identity();
         double origin[4];
         // According to vtkImageReslice API:
         // - 1st column contains the resliced image x-axis
         // - 2nd column contains the resliced image y-axis
         // - 3rd column contains the normal of the resliced image plane
         // -> 1st column is normal to the path
         // -> 3nd column is tangent to the path
         // -> 2nd column is B = T^N
         for ( int comp = 0; comp < 3; comp++ )
         {
            resliceAxes->SetElement(0,comp,normal[comp]);
            resliceAxes->SetElement(1,comp,crossProduct[comp]);
            resliceAxes->SetElement(2,comp,tangent[comp]);

            origin[comp] = center[comp] - normal[comp]*this->SliceSize/2.
                               - crossProduct[comp]*this->SliceSize/2.;
         }

         //! Transform the origin in the homogeneous coordinate space. 
         //! \todo See why !
         origin[3] = 1.0;
         double originXYZW[4];
         resliceAxes->MultiplyPoint(origin, originXYZW);

         //! Get the new origin from the transposed matrix. 
         //! \todo See why !
         resliceAxes->Transpose();
         double neworiginXYZW[4];
         resliceAxes->MultiplyPoint(originXYZW, neworiginXYZW);

         resliceAxes->SetElement(0,3,neworiginXYZW[0]);
         resliceAxes->SetElement(1,3,neworiginXYZW[1]);
         resliceAxes->SetElement(2,3,neworiginXYZW[2]);
 
         this->reslicers[reslicerId]->SetResliceAxes( resliceAxes );
         this->reslicers[reslicerId]->SetInformationInput( input );
         this->reslicers[reslicerId]->SetInterpolationModeToCubic( );

         this->reslicers[reslicerId]->SetOutputDimensionality( 2 );
         this->reslicers[reslicerId]->SetOutputOrigin(0, 0, 0);
         this->reslicers[reslicerId]->SetOutputExtent( 0, this->SliceSize / this->XYSpacing - 1, 
                                              0, this->SliceSize / this->XYSpacing - 1, 
                                              0, 0);
		 this->reslicers[reslicerId]->SetOutputSpacing(this->XYSpacing, this->XYSpacing, this->ZSpacing);
         this->reslicers[reslicerId]->Update( );

         //append->AddInputConnection( 0, this->reslicers[reslicerId]->GetOutputPort( 0 ) );

         resliceAxes->Delete( );
         this->reslicers[reslicerId]->Delete( );
      }
   }
   //append->Update( );
   //output->DeepCopy( append->GetOutputDataObject( 0 ) );
   output->GetPointData( )->GetScalars( )->SetName( "StraightenedCPR" );
   //append->Delete( );
   delete this->reslicers;

   return 1;

}

