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

  vtkCellData* sourceCellData = source->GetCellData();
  vtkPointData* sourcePointData = source->GetPointData();
  
  vtkSmartPointer<vtkDoubleArray> distArray = vtkSmartPointer<vtkDoubleArray>::New();
  distArray->SetName("Copy");
  distArray->SetNumberOfComponents(1);
  
  vtkSmartPointer<vtkCellLocator> cellLocator = vtkSmartPointer<vtkCellLocator>::New();
  cellLocator->SetDataSet( source );

  vtkSmartPointer<vtkPointLocator> pointLocator = vtkSmartPointer<vtkPointLocator>::New();
  pointLocator->SetDataSet( source );  
  
  vtkSmartPointer<vtkGenericCell> sourceCell = vtkSmartPointer<vtkGenericCell>::New();

  vtkIdType nbOfTargetedCells = input->GetNumberOfCells();
  for( vtkIdType cellId = 0; cellId < nbOfTargetedCells ; cellId++ )
  {
      double cellCenter[3], closestTargetPoint[3], distance;
      this->GetCellCenter(input, cellId, cellCenter );
      vtkIdType sourceCellId;
      int subId;
      //cellLocator->FindClosestPoint(cellCenter,closestTargetPoint,sourceCell,sourceCellId,subId,distance);
      vtkIdType ptId = pointLocator->FindClosestPoint(cellCenter);
      
      distArray->InsertNextValue(sourcePointData->GetArray(0)->GetComponent(ptId,0));      
  }

  output->DeepCopy(input);
  output->GetCellData()->AddArray(distArray);
  
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
