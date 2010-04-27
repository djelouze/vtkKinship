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


#include "vtkSmoothPolyDataVectors.h"

#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"
#include "vtkPointData.h"

vtkCxxRevisionMacro(vtkSmoothPolyDataVectors, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkSmoothPolyDataVectors);

vtkSmoothPolyDataVectors::vtkSmoothPolyDataVectors()
{
   this->SmoothingFilter = vtkSmartPointer<vtkSmoothPolyDataFilter>::New( );
}


void vtkSmoothPolyDataVectors::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

void vtkSmoothPolyDataVectors::SetNumberOfIterations( int nbIte )
{
   this->SmoothingFilter->SetNumberOfIterations( nbIte );
   this->Modified( );
}


int vtkSmoothPolyDataVectors::FillInputPortInformation(int port, vtkInformation *info)
{
  if( port == 0 ) // input mesh port
     info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData");
  return 1;
}


int vtkSmoothPolyDataVectors::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
   // get the info objects
   vtkInformation *inMeshInfo = inputVector[0]->GetInformationObject(0);
   vtkInformation *outMeshInfo = outputVector->GetInformationObject(0);

   vtkPolyData* inputMesh = vtkPolyData::SafeDownCast(
    inMeshInfo->Get(vtkDataObject::DATA_OBJECT()));
   vtkPolyData* outputMesh = vtkPolyData::SafeDownCast(
    outMeshInfo->Get(vtkDataObject::DATA_OBJECT()));

   // mesh geometry and topology won't change 
   outputMesh->DeepCopy( inputMesh );

   // if no smoothing filter is set, return.
   if( !this->SmoothingFilter )
      return( 1 );


   // Set input vectors as temporary geometry
   vtkSmartPointer<vtkPolyData> vectorsToPoints;
   vectorsToPoints = vtkSmartPointer<vtkPolyData>::New( );
   vectorsToPoints->DeepCopy( inputMesh );

   vtkDataArray* vectors = this->GetInputArrayToProcess( 1, inputVector );
   if( !vectors )
      return( 0 );

   for( int pointId = 0; 
        pointId < inputMesh->GetNumberOfPoints();
        pointId++ )
   {
      double vector[3];
      vectors->GetTuple( pointId, vector );
      vectorsToPoints->GetPoints()->SetPoint( pointId, vector );
   }

   // smooth this temp geometry
   this->SmoothingFilter->SetInput( vectorsToPoints );
   this->SmoothingFilter->Update( );

   vtkPolyData* pointsToVectors = this->SmoothingFilter->GetOutput( );

   vtkSmartPointer<vtkDoubleArray> smoothedVectors;
   smoothedVectors = vtkSmartPointer<vtkDoubleArray>::New( );
   smoothedVectors->SetNumberOfComponents( 3 );
   smoothedVectors->SetNumberOfTuples( inputMesh->GetNumberOfPoints() );
   smoothedVectors->SetName( "SmoothedVectors" );

   vtkSmartPointer<vtkPoints> copyInPoints = vtkSmartPointer<vtkPoints>::New( );
 
   for( int pointId = 0; 
        pointId < inputMesh->GetNumberOfPoints();
        pointId++ )
   {
      double vector[3];
      pointsToVectors->GetPoints()->GetPoint( pointId, vector );
      smoothedVectors->SetTuple( pointId, vector );
      copyInPoints->InsertNextPoint( inputMesh->GetPoints()->GetPoint( pointId ) );
   }

   outputMesh->DeepCopy( pointsToVectors );
   outputMesh->SetPoints( copyInPoints );
   outputMesh->GetPointData()->AddArray( smoothedVectors );
 

   return( 1 );
}
