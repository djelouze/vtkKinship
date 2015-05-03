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

#include <vtkSymmetricRecursivePolyDataFilter.h>

#include <vtkIdList.h>
#include <vtkCellArray.h>
#include <vtkMath.h>
#include <vtkSmartPointer.h>
#include <vtkPointData.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkObjectFactory.h>

vtkCxxRevisionMacro(vtkSymmetricRecursivePolyDataFilter, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkSymmetricRecursivePolyDataFilter);

class SphericSymmetryHelper
{
public:
  SphericSymmetryHelper();
  virtual ~SphericSymmetryHelper(){};
  
  void InsertId( vtkIdType id ); // Insert id in poleIds and insert null pole and valence
  void AccumulatePole( vtkIdType ptId, double* data ); // add data to pole if ptId exists
  int GetPole( const vtkIdType poleId, vtkIdType& pointId, double* data );
  void Finalize( ); // divide each pole accumulation by its corresponding valence
  void Reset( ); // empty arrays

private:
  vtkSmartPointer<vtkIdList> poleIds;
  vtkSmartPointer<vtkIdList> polesValence;
  vtkSmartPointer<vtkPoints> meanPoles;
};

SphericSymmetryHelper::SphericSymmetryHelper()
{
   this->poleIds = vtkSmartPointer<vtkIdList>::New( );
   this->polesValence = vtkSmartPointer<vtkIdList>::New( );
   this->meanPoles = vtkSmartPointer<vtkPoints>::New();
}

int SphericSymmetryHelper::GetPole(const vtkIdType poleId, vtkIdType& pointId, double* data)
{
   if( poleId >= this->poleIds->GetNumberOfIds() )
     return( -1 );
   
   pointId = this->poleIds->GetId(poleId);
   this->meanPoles->GetPoint(poleId,data);
}


void SphericSymmetryHelper::Finalize()
{
      for( unsigned int poleId = 0; poleId < this->poleIds->GetNumberOfIds();poleId++)
      {
	double sumPoles[3];
	this->meanPoles->GetPoint(poleId, sumPoles);

	for( int comp = 0 ; comp < 3 ; comp++ )
            sumPoles[comp] /= this->polesValence->GetId(poleId);

	this->meanPoles->SetPoint(poleId, sumPoles);
      }
}



void SphericSymmetryHelper::Reset()
{
   this->poleIds->Reset();
   this->polesValence->Reset();
   this->meanPoles->Reset();
}

void SphericSymmetryHelper::InsertId( vtkIdType id )
{
   if( this->poleIds->IsId(id) == -1 ) 
   {
                this->poleIds->InsertNextId(id);
		this->meanPoles->InsertNextPoint(0,0,0);
		this->polesValence->InsertNextId(0);
   }  
}

void SphericSymmetryHelper::AccumulatePole(vtkIdType pointId, double* data)
{
    vtkIdType poleId = this->poleIds->IsId(pointId);
    if( poleId != -1 )
    {
      vtkIdType valence = this->polesValence->GetId(poleId) + 1;
        this->polesValence->SetId(poleId,valence);
	
	double meanPole[3];
	this->meanPoles->GetPoint(poleId,meanPole);

	for( int comp = 0; comp < 3; comp ++)
            meanPole[comp] += data[comp];

	this->meanPoles->SetPoint(poleId,meanPole);      
    }
}

vtkSymmetricRecursivePolyDataFilter::vtkSymmetricRecursivePolyDataFilter()
{
    this->causalMesh = vtkPolyData::New( );
    this->SignalSource = 0;
    this->Causality = 0;
    this->KeepSignals = 0;

    for( int i = 0; i < 3 ; i++ )
    {
        this->b[i] = (i==0)?1:0;
        this->a[i] = (i==0)?1:0;
    }

    this->symmetricCellPoints = 0x0; //!< used in the spherical symmetry. Changes at each different polyline filtering
    this->nbSymmetricCellPoints = 0;
    this->symmetricCellPosition = 0;
    
    this->sphericHelper = new SphericSymmetryHelper();
}

vtkSymmetricRecursivePolyDataFilter::~vtkSymmetricRecursivePolyDataFilter()
{
    this->causalMesh->Delete( );
    delete this->sphericHelper;
}

int vtkSymmetricRecursivePolyDataFilter::FillInputPortInformation(int port, vtkInformation *info)
{
    if( port == 0 ) // input mesh port
        info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData");
    return 1;
}


int vtkSymmetricRecursivePolyDataFilter::RequestData(
    vtkInformation *vtkNotUsed(request),
    vtkInformationVector **inputVector,
    vtkInformationVector *outputVector)
{
    // get the info objects
    vtkInformation *inMeshInfo = inputVector[0]->GetInformationObject(0);
    vtkInformation *outMeshInfo = outputVector->GetInformationObject(0);

    vtkPolyData* in = vtkPolyData::SafeDownCast(
                          inMeshInfo->Get(vtkDataObject::DATA_OBJECT()));
    vtkPolyData* out = vtkPolyData::SafeDownCast(
                           outMeshInfo->Get(vtkDataObject::DATA_OBJECT()));

    // output has the same structure as input
    // i.e pointId <-> PolyLines
    this->causalMesh->DeepCopy( in );
    out->DeepCopy( in );

    vtkCellArray* inPolyLines = this->GetSignals(in);
    vtkCellArray* outPolyLines = this->GetSignals(out);
    vtkCellArray* causalMeshPolyLines = this->GetSignals(this->causalMesh);
    vtkPoints* outPoints = out->GetPoints( );

    this->sphericHelper->Reset();
    
    // For each polyline/polyvertex cellId, filter the corresponding signal.
    inPolyLines->InitTraversal( );
    int cellPosition = 0;
    for( int cellId = 0; cellId < inPolyLines->GetNumberOfCells( ); cellId ++ )
    {
        // Get the cell content ( i.e the list of points )
        vtkIdType nbPoints;
        vtkIdType* points;

        cellPosition = inPolyLines->GetTraversalLocation( );
        inPolyLines->GetNextCell( nbPoints , points );

        if( this->ExtrapolationMode == 0 )
            this->SetSymmetricCell( cellPosition );

        int isPole = this->DefinePointRole( points[0] );
//         if( this->ExtrapolationMode == 1 && isPole <= 2
//                 || this->ExtrapolationMode == 0 && isPole > 2 )
        {
            for( this->Causality = 0; this->Causality < 2; this->Causality++)
            {
	        this->sphericHelper->InsertId( points[0] );

                vtkPolyData* from,* to;
                if( this->Causality )
                {
                    from = this->causalMesh;
                    to = out;
                }
                else
                {
                    from = in;
                    to = this->causalMesh;
                }

                inPolyLines->GetCell( cellPosition, nbPoints, points );
                int closed = (points[0] == points[nbPoints-1]) || (inPolyLines->GetNumberOfCells( )==1);
                double initPoint[3];

                // Gives an approximation of the first point and adds it to the output
                this->ComputeFilterCoefficients( in, cellPosition, 0  );
                if( !this->Causality )
                    this->CausalImpulseResponseApproximation( in, cellPosition, 0 , initPoint );
                else
                    this->SymmetricImpulseResponseApproximation( in, cellPosition, 0 , initPoint );

                this->SetData( to, points[0],initPoint );

                // Gives an approximation of the second point and adds it to the output
                this->ComputeFilterCoefficients( in, cellPosition, 1  );
                if( !this->Causality )
                    this->CausalImpulseResponseApproximation( in, cellPosition, 1 , initPoint );
                else
                    this->SymmetricImpulseResponseApproximation( in, cellPosition, 1 , initPoint );
                this->SetData( to, points[1],initPoint );

		// Beginning of the recursive filtering
                for( int pointId = 2 ; pointId < nbPoints ; pointId ++ )
                {
                    // gi is the considered point. gdi the last and gddi the one before gdi.
                    double gi[3];
                    double gdi[3];
                    double gddi[3];
                    this->GetSignalComponents( from, cellPosition , pointId , gi );
                    this->GetSignalComponents( from, cellPosition , pointId - 1 , gdi );
                    this->GetSignalComponents( from, cellPosition , pointId - 2 , gddi );

                    // yi is the filtered point. ydi the last filtered and yddi the one before ydi
                    // It does not matter if we are in antiSymmetric or Symmetric mode : output is filled
                    // from point Id '0' to 'nbPoints' (that is why we use the 'count' var to tell
                    // which point to read.
                    double yi[3];
                    double ydi[3];
                    double yddi[3];
                    this->GetSignalComponents( to, cellPosition , pointId - 1 , ydi );
                    this->GetSignalComponents( to, cellPosition , pointId - 2 , yddi );

                    // Computes the recursive function for each component of the point
                    this->ComputeFilterCoefficients( in, cellPosition, pointId);
                    for( int comp = 0; comp < 3 ; comp++ )
                    {
                        yi[ comp ] = (( b0( pointId )*gi[ comp ]
                                        + b1( pointId )*gdi[ comp ]
                                        + b2( pointId )*gddi[ comp ] ) -
                                      ( a1( pointId )*ydi[ comp ]
                                        + a2( pointId )*yddi[ comp ] )) / a0( pointId );
                    }

                    this->SetData(to, points[pointId], yi );
                }

                causalMeshPolyLines->ReverseCell( cellPosition );
                outPolyLines->ReverseCell( cellPosition );
                inPolyLines->ReverseCell( cellPosition );
                if( !closed )
                {
                    causalMeshPolyLines->ReverseCell( this->symmetricCellPosition );
                    outPolyLines->ReverseCell( this->symmetricCellPosition  );
                    inPolyLines->ReverseCell( this->symmetricCellPosition  );
                }
            }
        }
    } // end for each cell

    if( this->ExtrapolationMode == 0 )
    {
      this->sphericHelper->Finalize();
      
      int poleId = 0;
      double point[3];
      vtkIdType pointId;
      while( this->sphericHelper->GetPole( poleId, pointId, point ) != -1 )
      {
	this->SetData( out, pointId, point );
	poleId++;
      }
    }
    
    if( !this->KeepSignals )
    {
      if( this->SignalContainer == 1 )
         out->SetLines( 0x0 );
      else 
	out->SetVerts( 0x0 );
    }

    return( 1 );
}

void vtkSymmetricRecursivePolyDataFilter::SetB( double ub0, double ub1, double ub2 )
{
    this->b[0] = ub0;
    this->b[1] = ub1;
    this->b[2] = ub2;
    this->Modified( );
}

void vtkSymmetricRecursivePolyDataFilter::SetA( double ua0, double ua1, double ua2 )
{
    this->a[0] = ua0;
    this->a[1] = ua1;
    this->a[2] = ua2;
    this->Modified( );
}


vtkCellArray* vtkSymmetricRecursivePolyDataFilter::GetSignals( vtkPolyData* from )
{
    if( this->SignalContainer == 0 )
        return( from->GetVerts( ) );
    else if( this->SignalContainer == 1 )
        return( from->GetLines( ) );
}

double vtkSymmetricRecursivePolyDataFilter::GetPointSampling( vtkPolyData* mesh, int cellLocation, int curveCoord )
{
    double pointBefore[3], pointAfter[3], point[3];

    //// Warning: GetSignalComponents gives vectors comp if SetFilterVectors is true. So...
    int memSignalSource = this->SignalSource;
    this->SignalSource = 0;
    this->GetSignalComponents(mesh, cellLocation, curveCoord-1, pointBefore);
    this->GetSignalComponents(mesh, cellLocation, curveCoord, point);
    this->GetSignalComponents(mesh, cellLocation, curveCoord+1, pointAfter);
    this->SignalSource = memSignalSource;

    return( ( sqrt( (double)vtkMath::Distance2BetweenPoints( point, pointBefore ) )
              +sqrt( (double)vtkMath::Distance2BetweenPoints( point, pointAfter ) ) ) / 2.0 );
}

double vtkSymmetricRecursivePolyDataFilter::GetMeanSampling( vtkPolyData* mesh, int cellLocation, int curveCoord, int nbPoint )
{

    double meanSampling = 0.0;
    for(int i = 0; i<=nbPoint; i++)
    {
        meanSampling += GetPointSampling(mesh, cellLocation, curveCoord+i)/(2.0*(nbPoint+1));
        meanSampling += GetPointSampling(mesh, cellLocation, curveCoord-i)/(2.0*(nbPoint+1));
    }
    return(meanSampling);
}

void vtkSymmetricRecursivePolyDataFilter::GetSignalComponents( vtkPolyData* mesh, int cellLocation, int curveCoord, double* signalComp )
{
    vtkCellArray* lines = this->GetSignals( mesh );

    vtkIdType nbPoints;
    vtkIdType* points;
    lines->GetCell( cellLocation , nbPoints , points );

    int newCurveCoord;
    if( this->ExtrapolationMode == 1 )
    //if( (points[0] == points[nbPoints-1]) || (lines->GetNumberOfCells( )==1)) // closed curve
    {
        // closed curve, last point is the same as first
        nbPoints = nbPoints - 1;
        if( curveCoord < 0 )
            newCurveCoord = (nbPoints+curveCoord%(-nbPoints))%(nbPoints);
        else
            newCurveCoord = curveCoord%nbPoints;
    }
    else if( this->ExtrapolationMode == 0 )// behaviour to be defined (sphere? open surface?)
    {
        if( curveCoord >= 0 )
        {
            if( ( curveCoord %( nbPoints*2 ) ) < nbPoints )
                newCurveCoord = curveCoord%nbPoints;
            else
            {
                // change point list (i.e cell) here
                lines->GetCell( this->symmetricCellPosition , nbPoints , points );
                newCurveCoord = nbPoints-1-curveCoord%(nbPoints);
            }
        }
        else
        {
            int sec = -curveCoord / (nbPoints-1);
            if( sec % 2 == 0 )
            {
                // change point list here
                lines->GetCell( this->symmetricCellPosition , nbPoints , points );
                newCurveCoord = (-curveCoord+sec)%nbPoints;
            }
            else
            {
                newCurveCoord = nbPoints - (-curveCoord + sec)%nbPoints - 1 ;
            }
        }

    }

    if( this->SignalSource == 0 )
        mesh->GetPoints( )->GetPoint( points[newCurveCoord], signalComp );
    else
        mesh->GetPointData( )->GetVectors( )->GetTuple( points[newCurveCoord], signalComp );
}

void vtkSymmetricRecursivePolyDataFilter::SetSymmetricCell( vtkIdType orgCellId )
{
    vtkIdType orgNbPoints;
    vtkIdType* orgPoints;

    vtkPolyData* mesh = vtkPolyData::SafeDownCast(this->GetInputDataObject(0,0 ));
    vtkPoints* meshPoints = mesh->GetPoints( );
    vtkCellArray* polyLines = this->GetSignals( mesh );

    // orgCellLocation++ : manual management of cell traversal. RTS 'GetNextCell' in vtkCellArray for details
    polyLines->GetCell( orgCellId++ , orgNbPoints , orgPoints );

    this->symmetricCellPoints = orgPoints; // Initialize symmetricCellPoint 

    polyLines->InitTraversal( );
    double cosAlphaMin = 1;
    int cellId;
    for( cellId = 0; cellId < polyLines->GetNumberOfCells( ); cellId ++ )
    {
        vtkIdType nbPoints;
        vtkIdType* points;
        vtkIdType position = polyLines->GetTraversalLocation( );
        polyLines->GetNextCell( nbPoints, points );

	// Check if both cells share the same extremity
        if( points[ 0 ] == orgPoints[ 0 ] )
        {
	    // Get the first edge of processed cell and candidate cell
            double pointOrg[3], pointDest[3], pole[3];
            meshPoints->GetPoint( orgPoints[0], pole);
            meshPoints->GetPoint( orgPoints[1], pointOrg);
            meshPoints->GetPoint( points[1], pointDest);

	    double vectorOrg[3], vectorDest[3];
            for( int i = 0; i < 3 ; i++ )
            {
                vectorOrg[i] = pointOrg[i] - pole[i];
                vectorDest[i] = pointDest[i] - pole[i];
            }

            // Compute the cos between the two edges
            double cosAlpha =   vtkMath::Dot( vectorOrg , vectorDest )
                                /(vtkMath::Norm( vectorOrg )*vtkMath::Norm( vectorDest ) );

            // The more suited cell is the one with cos that tends to -1
            // ie the most "in front of" the processed cell 
            if( cosAlpha <= cosAlphaMin  )
            {
                cosAlphaMin = cosAlpha;
                this->symmetricCellPoints = points;
                this->nbSymmetricCellPoints = nbPoints;
                this->symmetricCellPosition = position;
            }
        }
    }
    polyLines->SetTraversalLocation( orgCellId + orgNbPoints );
}


void vtkSymmetricRecursivePolyDataFilter::CausalImpulseResponseApproximation( vtkPolyData* mesh, int cellId, int curveCoord, double* approxPoint )
{
    int i = 0;
    approxPoint[0] = approxPoint[1] = approxPoint[2] = 0.0;
    do {
        double currentPoint[3];
        this->GetSignalComponents( mesh, cellId, curveCoord - i, currentPoint );

        for( int comp = 0; comp < 3 ; comp++)
            approxPoint[comp] += CausalImpulseResponse( i )*currentPoint[comp];
        i++;

    } while(! StopIRApproximation( i ) );
}

void vtkSymmetricRecursivePolyDataFilter::SymmetricImpulseResponseApproximation( vtkPolyData* mesh, int cellId, int curveCoord, double* approxPoint )
{
    int i = 1;
    
    double initPoint[3];
    this->GetSignalComponents( mesh, cellId, curveCoord, initPoint );
    
    for( int comp = 0; comp < 3 ; comp++)
        approxPoint[comp] = SymmetricImpulseResponse( 0 )*initPoint[comp];

    do {
        double currentCausalPoint[3], currentAntiCausalPoint[3];
        this->GetSignalComponents( mesh, cellId, curveCoord - i, currentCausalPoint );
        this->GetSignalComponents( mesh, cellId, curveCoord + i, currentAntiCausalPoint );

        for( int comp = 0; comp < 3 ; comp++)
            approxPoint[comp] += SymmetricImpulseResponse( i )*currentCausalPoint[comp]+
                                 SymmetricImpulseResponse( -i )*currentAntiCausalPoint[comp];
        i++;

    } while(! StopIRApproximation( i ) );
}


int vtkSymmetricRecursivePolyDataFilter::DefinePointRole( vtkIdType pointId )
{
    vtkCellArray* inPolyLines = this->GetSignals( vtkPolyData::SafeDownCast(this->GetInputDataObject(0,0 )) );

    int memTraversal = inPolyLines->GetTraversalLocation( ); // memorize

    inPolyLines->InitTraversal( );
    int nbExtremity = 0; // count how many endline the pointId represents
    int cellId, cellPosition;
    for( cellId = 0, cellPosition = 0; cellId < inPolyLines->GetNumberOfCells( ); cellId ++ )
    {
        // Get the cell content ( i.e the list of points )
        vtkIdType nbPoints;
        vtkIdType* points;

        cellPosition = inPolyLines->GetTraversalLocation( );
        inPolyLines->GetNextCell( nbPoints , points );
        if( points[0] == pointId )//|| points[nbPoints-1] == pointId)
            nbExtremity++;
    }

    inPolyLines->SetTraversalLocation( memTraversal ); //back to initial location
    return( nbExtremity );
}


void vtkSymmetricRecursivePolyDataFilter::SetData( vtkPolyData* mesh, vtkIdType pointId, double* data)
{
    if( this->SignalSource == 1 )
        mesh->GetPointData( )->GetVectors( )->SetTuple( pointId, data );
    else
        mesh->GetPoints( )->SetPoint(pointId, data );
    
    if( this->ExtrapolationMode == 0 && this->Causality == 1 )
    {
      this->sphericHelper->AccumulatePole(pointId, data);
    }
}

void vtkSymmetricRecursivePolyDataFilter::SetSignalSourceInternal(int s )
{
    this->SignalSource = s;
}


double vtkSymmetricRecursivePolyDataFilter::a0( int k )
{
    return( a[0] );
}

double vtkSymmetricRecursivePolyDataFilter::a1( int k )
{
    return( a[1] );
}

double vtkSymmetricRecursivePolyDataFilter::a2( int k )
{
    return( a[2] );
}

double vtkSymmetricRecursivePolyDataFilter::b0( int k )
{
    return( b[0] );
}

double vtkSymmetricRecursivePolyDataFilter::b1( int k )
{
    return( b[1] );
}

double vtkSymmetricRecursivePolyDataFilter::b2( int k )
{
    return( b[2] );
}
