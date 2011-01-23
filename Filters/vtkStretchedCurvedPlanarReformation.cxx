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

#include "vtkStretchedCurvedPlanarReformation.h"
#include "vtkImageReslice.h"
#include "vtkImageData.h"
#include "vtkImageAppend.h"
#include "vtkDoubleArray.h"
#include "vtkPointData.h"
#include "vtkMath.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"


vtkCxxRevisionMacro(vtkStretchedCurvedPlanarReformation, "$Revision: 1.31 $");
vtkStandardNewMacro(vtkStretchedCurvedPlanarReformation);

vtkStretchedCurvedPlanarReformation::vtkStretchedCurvedPlanarReformation( )
{
    this->OutputThickness = 15;
    this->XYSpacing = 1;
    this->ZSpacing = 1;
    this->SetxAxis( 1,0,0 );
    this->SetyAxis( 0,1,0 );

    this->SetNumberOfInputPorts( 2 );
}

vtkStretchedCurvedPlanarReformation::~vtkStretchedCurvedPlanarReformation( )
{

}

//----------------------------------------------------------------------------
// Specify a source object at a specified table location.
void vtkStretchedCurvedPlanarReformation::SetPathConnection(int id, vtkAlgorithmOutput* algOutput)
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
int vtkStretchedCurvedPlanarReformation::FillInputPortInformation(int port, vtkInformation *info)
{
    if( port == 0 )
        info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkImageData");
    else
        info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData");

    return 1;
}

int vtkStretchedCurvedPlanarReformation::RequestUpdateExtent (
    vtkInformation * vtkNotUsed(request),
    vtkInformationVector** inputVector,
    vtkInformationVector *outputVector)
{
    // get the info objects
    vtkInformation* outInfo = outputVector->GetInformationObject(0);
    vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);

    int outExtent[6], inExtent[6];
    outInfo->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), outExtent);
    inInfo->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), inExtent);

    for( int i=0; i < 6; i+=2 )
    {
        if( inExtent[i] > outExtent[i] )
            inExtent[i] = outExtent[i];
        if( inExtent[i+1] < outExtent[i+1] )
            inExtent[i+1] = outExtent[i+1];
    }

    outInfo->Set(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT(), inExtent, 6);

    return( 1 );
}



int vtkStretchedCurvedPlanarReformation::RequestInformation (
    vtkInformation * vtkNotUsed(request),
    vtkInformationVector** inputVector,
    vtkInformationVector *outputVector)
{

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

    int extent[6] = {0, 511,
                     0, this->OutputThickness  / this->XYSpacing - 1,
                     0, numberOfLinePoints - 1
                    };
    double spacing[3] = {this->XYSpacing,this->XYSpacing,this->ZSpacing};


    outInfo->Set(vtkDataObject::SPACING(), spacing, 3);
    outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(),
                 extent,6);
    return 1;
}

//----------------------------------------------------------------------------
int vtkStretchedCurvedPlanarReformation::FillOutputPortInformation (
    vtkInformation * vtkNotUsed(request),
    vtkInformationVector** inputVector,
    vtkInformationVector *outputVector)
{
    vtkInformation* outInfo = outputVector->GetInformationObject(0);
    outInfo->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkImageData");
    return 1;
}


//! RequestData is called by the pipeline process.
int vtkStretchedCurvedPlanarReformation::RequestData(
    vtkInformation *vtkNotUsed(request),
    vtkInformationVector **inputVector,
    vtkInformationVector *outputVector)
{
    // get the info objects
    vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
    vtkInformation *pathInfo = inputVector[1]->GetInformationObject(0);
    vtkInformation *outInfo = outputVector->GetInformationObject(0);

    // get the input and ouptut
    vtkImageData *input = vtkImageData::SafeDownCast(
                              inInfo->Get(vtkDataObject::DATA_OBJECT()));
    vtkPolyData *path = vtkPolyData::SafeDownCast(
                            pathInfo->Get(vtkDataObject::DATA_OBJECT()));
    vtkImageData *output = vtkImageData::SafeDownCast(
                               outInfo->Get(vtkDataObject::DATA_OBJECT()));

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
    // Their will be as many slicers as input points in the polyline
    this->reslicers = new (vtkImageReslice*[numberOfPoints]);

    // The reslicers outputs will be appended in a new volume along the z-axis
    vtkImageAppend* append = vtkImageAppend::New( );
    append->SetAppendAxis( 2 );

    // for each line
    lines->InitTraversal( );
    for( int cellIdx = 0, reslicerId = 0; cellIdx < lines->GetNumberOfCells( ); cellIdx++ )
    {
        vtkIdType nbCellPoints;
        vtkIdType* points;

        lines->GetNextCell( nbCellPoints, points);

        // for each line point
        for( int i = 0 ; i < nbCellPoints; i++, reslicerId++)
        {
            int ptId = points[i];
            // Build a new reslicer with image input as input too.
            this->reslicers[reslicerId] = vtkImageReslice::New( );
            this->reslicers[reslicerId]->SetInputConnection(
                this->GetInputConnection( 0, 0 ) );

            // - position (center)
            //! \todo Make it dependent on xAxis and yAxis.
            double* center = path->GetPoints( )->GetPoint( ptId );
            double origin[3] = {0,0,0};
            origin[0] = center[0] -  this->OutputThickness/2.;
            origin[1] = center[1];
            origin[2] = 0;

            double zAxis[3];
            vtkMath::Cross( this->xAxis, this->yAxis, zAxis );

            this->reslicers[reslicerId]->SetResliceAxesDirectionCosines( this->xAxis, this->yAxis, zAxis);
            this->reslicers[reslicerId]->SetResliceAxesOrigin( origin);
            this->reslicers[reslicerId]->SetInformationInput( input );
            this->reslicers[reslicerId]->SetInterpolationModeToCubic( );

            this->reslicers[reslicerId]->SetOutputDimensionality( 2 );
            this->reslicers[reslicerId]->SetOutputOrigin(0, 0, 0);
            this->reslicers[reslicerId]->SetOutputExtent( 0, 511,
                    0, this->OutputThickness / this->XYSpacing - 1,
                    0, 0);
            this->reslicers[reslicerId]->SetOutputSpacing(this->XYSpacing, this->XYSpacing, this->ZSpacing);
            this->reslicers[reslicerId]->Update( );

            append->AddInputConnection( 0, this->reslicers[reslicerId]->GetOutputPort( 0 ) );

            this->reslicers[reslicerId]->Delete( );
        }
    }
    append->Update( );
    output->DeepCopy( append->GetOutputDataObject( 0 ) );
    output->GetPointData( )->GetScalars( )->SetName( "StretchedCPR" );
    append->Delete( );
    delete this->reslicers;

    return 1;

}

