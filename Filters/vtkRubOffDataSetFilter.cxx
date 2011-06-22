// Copyright (c) 2011, Jérôme Velut
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

#include <vtkRubOffDataSetFilter.h>

#include <vtkCellData.h>
#include <vtkPointData.h>
#include <vtkCellLocator.h>
#include <vtkPointLocator.h>
#include <vtkDataSet.h>
#include <vtkSmartPointer.h>
#include <vtkIdList.h>
#include <vtkGenericCell.h>
#include <vtkDoubleArray.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkObjectFactory.h>
#include <vtkStreamingDemandDrivenPipeline.h>

vtkStandardNewMacro(vtkRubOffDataSetFilter);


vtkRubOffDataSetFilter::vtkRubOffDataSetFilter()
{
  this->RubOffCellData = 1;
  this->RubOffPointData = 1;
  this->SetNumberOfInputPorts(2);
}

vtkRubOffDataSetFilter::~vtkRubOffDataSetFilter()
{
}

void vtkRubOffDataSetFilter::SetSourceConnection(vtkAlgorithmOutput* algOutput)
{
  this->SetInputConnection(1, algOutput);
}


//----------------------------------------------------------------------------
int vtkRubOffDataSetFilter::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *sourceInfo = inputVector[1]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  // get the input and output
  vtkDataSet *input = vtkDataSet::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkDataSet *source = vtkDataSet::SafeDownCast(
    sourceInfo->Get(vtkDataObject::DATA_OBJECT()));
  
  vtkDataSet *output = vtkDataSet::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

  // First copy the input on the output Geometry, Topology and attributes
  output->DeepCopy( input );

  // If RubOffPointData is On, 
  vtkSmartPointer<vtkPointLocator> pointLocator = vtkSmartPointer<vtkPointLocator>::New();  
  if( this->RubOffPointData != 0 )
  {
     // First put the point data arrays existing in the source in the output
     vtkPointData* sourcePointData = 0;
     sourcePointData = source->GetPointData();
     for( int i = 0; i < sourcePointData->GetNumberOfArrays();i++)
     {
       vtkDataArray* sourceArray = sourcePointData->GetArray(i);
       if( sourceArray != 0x0 )
       {
	 // Create a new instance without a knowledge of the type
	 vtkDataArray* array = sourceArray->NewInstance(); 
	 // Simple reset
         array->SetNumberOfTuples(0);
	 array->SetNumberOfComponents(sourceArray->GetNumberOfComponents());
	 // Name seems to not be set by NewInstance (that makes sense)
	 array->SetName(sourceArray->GetName());
	 // FInally add the new array to the output point data.
	 output->GetPointData()->AddArray( array );
       }
     }

     // Initialize a PointLocator on the source
     pointLocator->SetDataSet( source ); 
     pointLocator->BuildLocator();
     
     // For each output points ...
     vtkIdType nbOfTargetedPoints = output->GetNumberOfPoints();
     double point[3], closestSourcePoint[3];
     vtkIdType sourcePointId;
     for( vtkIdType pointId = 0; pointId < nbOfTargetedPoints ; pointId++ )
     {
        // ... find the closest point in the source
        input->GetPoint(pointId, point);
        sourcePointId = pointLocator->FindClosestPoint(point);
	
	// and set the output arrays to the corresponding value
	for( int i = 0; i < source->GetPointData()->GetNumberOfArrays(); i++ )
	{
	  vtkDataArray* sourceArray = source->GetPointData()->GetArray(i);
	  vtkDataArray* targetArray = output->GetPointData()->GetArray( sourceArray->GetName() );
	  if( targetArray != 0x0 )
	  {
	    targetArray->InsertNextTuple( sourcePointId, sourceArray );
	  }
	      
	    
	}
     }     
  }

  // If RubOffCellData is On, 
  vtkSmartPointer<vtkCellLocator> cellLocator = vtkSmartPointer<vtkCellLocator>::New();
  if( this->RubOffCellData != 0 )
  {
     // First put the cell data arrays existing in the source in the output
     vtkCellData* sourceCellData = 0;
     sourceCellData = source->GetCellData();
     for( int i = 0; i < sourceCellData->GetNumberOfArrays();i++)
     {
       vtkDataArray* sourceArray = sourceCellData->GetArray(i);
       if( sourceArray != 0x0 )
       {
	 // Create a new instance without a knowledge of the type
	 vtkDataArray* array = sourceArray->NewInstance();
	 // Simple reset
         array->SetNumberOfTuples(0);
	 array->SetNumberOfComponents(sourceArray->GetNumberOfComponents());
	 // Name seems to not be set by NewInstance (that makes sense)
	 array->SetName(sourceArray->GetName());
	 // Finally add the new array to the output cell data.
	 output->GetCellData()->AddArray( array );
       }
     }

     // Initialize a PointLocator on the source
     cellLocator->SetDataSet( source );
     cellLocator->BuildLocator();
     
     // For each output cells ...
     vtkSmartPointer<vtkGenericCell> sourceCell = vtkSmartPointer<vtkGenericCell>::New();
     vtkIdType nbOfTargetedCells = input->GetNumberOfCells();
     double cellCenter[3], closestTargetPoint[3], distance;
     vtkIdType sourceCellId;
     int subId;
     for( vtkIdType cellId = 0; cellId < nbOfTargetedCells ; cellId++ )
     {

        // ... find the closest cell in the source
        // Maybe a check of the type of the cell would be better...
        this->GetCellCenter(input, cellId, cellCenter );
        cellLocator->FindClosestPoint(cellCenter,closestTargetPoint,sourceCell,sourceCellId,subId,distance);
	
	// and set the output arrays to the corresponding value
	for( int i = 0; i < source->GetCellData()->GetNumberOfArrays(); i++ )
	{
	  vtkDataArray* sourceArray = source->GetCellData()->GetArray(i);
	  vtkDataArray* targetArray = output->GetCellData()->GetArray( sourceArray->GetName() );
	  if( targetArray != 0x0 )
	      targetArray->InsertNextTuple( sourceCellId, sourceArray );
	}
     }
  }

  return 1;
}


void vtkRubOffDataSetFilter::GetCellCenter( vtkDataSet* container, vtkIdType cellId, double center[3] )
{
    vtkSmartPointer<vtkIdList> ptIds = vtkSmartPointer<vtkIdList>::New();
    container->GetCellPoints(cellId,ptIds);
    
    center[0] = center[1] = center[2] = 0.0;
    for( vtkIdType ptId = 0; ptId < ptIds->GetNumberOfIds(); ptId++ )
    {
        double point[3];
	container->GetPoint(ptIds->GetId(ptId),point);
	for( int comp = 0; comp < 3; comp++ )
	{
	  center[comp] += point[comp]/ptIds->GetNumberOfIds();
	}
    }
}

//----------------------------------------------------------------------------
int vtkRubOffDataSetFilter::RequestInformation(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *sourceInfo = inputVector[1]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  outInfo->CopyEntry(sourceInfo, 
                     vtkStreamingDemandDrivenPipeline::TIME_STEPS());
  outInfo->CopyEntry(sourceInfo, 
                     vtkStreamingDemandDrivenPipeline::TIME_RANGE());

  outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(),
               inInfo->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT()),
               6);
  outInfo->Set(vtkStreamingDemandDrivenPipeline::MAXIMUM_NUMBER_OF_PIECES(),
               inInfo->Get(
                 vtkStreamingDemandDrivenPipeline::MAXIMUM_NUMBER_OF_PIECES()));

}

//----------------------------------------------------------------------------
int vtkRubOffDataSetFilter::RequestUpdateExtent(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *sourceInfo = inputVector[1]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  int usePiece = 0;

  // What ever happend to CopyUpdateExtent in vtkDataObject?
  // Copying both piece and extent could be bad.  Setting the piece
  // of a structured data set will affect the extent.
  vtkDataObject* output = outInfo->Get(vtkDataObject::DATA_OBJECT());
  if (output &&
      (!strcmp(output->GetClassName(), "vtkUnstructuredGrid") ||
       !strcmp(output->GetClassName(), "vtkPolyData")))
    {
    usePiece = 1;
    }
  
  inInfo->Set(vtkStreamingDemandDrivenPipeline::EXACT_EXTENT(), 1);
  
  sourceInfo->Set(
      vtkStreamingDemandDrivenPipeline::UPDATE_PIECE_NUMBER(), 0);
    sourceInfo->Set(
      vtkStreamingDemandDrivenPipeline::UPDATE_NUMBER_OF_PIECES(), 1);
    sourceInfo->Set(
      vtkStreamingDemandDrivenPipeline::UPDATE_NUMBER_OF_GHOST_LEVELS(), 0);

 
  return 1;
}

//----------------------------------------------------------------------------
void vtkRubOffDataSetFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
