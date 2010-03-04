#include "vtkPolyDataNeighbourhood.h"

#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkCellLocator.h"
#include "vtkFloatArray.h"
#include "vtkMath.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkPolygon.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkTriangleFilter.h"

vtkCxxRevisionMacro(vtkPolyDataNeighbourhood, "$Revision: 1.46 $");
vtkStandardNewMacro(vtkPolyDataNeighbourhood);


// The following code defines methods for the vtkPolyDataNeighbourhood class
//

// Construct object with number of iterations 20; relaxation factor .01;
// feature edge smoothing turned off; feature 
// angle 45 degrees; edge angle 15 degrees; and boundary smoothing turned 
// on. Error scalars and vectors are not generated (by default). The 
// convergence criterion is 0.0 of the bounding box diagonal.
vtkPolyDataNeighbourhood::vtkPolyDataNeighbourhood()
{
   this->VerticeNeighbourhood = 0;
}

#define VTK_SIMPLE_VERTEX 0
#define VTK_FIXED_VERTEX 1
#define VTK_FEATURE_EDGE_VERTEX 2
#define VTK_BOUNDARY_EDGE_VERTEX 3

void vtkPolyDataNeighbourhood::BuildNeighbourhood( )
{
  // Peform topological analysis. What we're gonna do is build a connectivity
  // array of connected vertices. The outcome will be one of three
  // classifications for a vertex: VTK_SIMPLE_VERTEX, VTK_FIXED_VERTEX. or
  // VTK_EDGE_VERTEX. Simple vertices are smoothed using all connected 
  // vertices. FIXED vertices are never smoothed. Edge vertices are smoothed
  // using a subset of the attached vertices.
  //
  vtkDebugMacro(<<"Analyzing topology...");
  vtkPolyData* input = vtkPolyData::SafeDownCast( this->GetInput( ) );

  vtkIdType numPts, numCells, i, numPolys, numStrips;
  int j, k;
  vtkIdType npts = 0;
  vtkIdType *pts = 0;
  vtkIdType p1, p2;
  double x3[3], x2[3];

  vtkIdType numSimple=0, numBEdges=0, numFixed=0, numFEdges=0;
  vtkPolyData *inMesh, *Mesh;
  vtkPoints *inPts;
  vtkTriangleFilter *toTris=NULL;
  vtkCellArray *inVerts, *inLines, *inPolys, *inStrips;

  numPts = input->GetNumberOfPoints( );
  vtkDebugMacro(<<"Instanciate VerticeNeighbourhood...");
  if( !this->VerticeNeighbourhood )
     this->VerticeNeighbourhood = new VertexEdge[numPts];
  else
  {
      //! \todo reinitialize this->VerticeNeighbourhood iff input topology changed
  }

  vtkDebugMacro(<<"Analyzing topology...");
  for (i=0; i<numPts; i++)
    {
    this->VerticeNeighbourhood[i].type = VTK_SIMPLE_VERTEX; //can smooth
    this->VerticeNeighbourhood[i].edges = NULL;
    }

  inPts = input->GetPoints();
  
  // check vertices first. Vertices are never smoothed_--------------
  for (inVerts=input->GetVerts(), inVerts->InitTraversal(); 
  inVerts->GetNextCell(npts,pts); )
    {
    for (j=0; j<npts; j++)
      {
      this->VerticeNeighbourhood[pts[j]].type = VTK_FIXED_VERTEX;
      }
    }
  this->UpdateProgress(0.10);

  // now check lines. Only manifold lines can be smoothed------------
  for (inLines=input->GetLines(), inLines->InitTraversal(); 
  inLines->GetNextCell(npts,pts); )
    {
    for (j=0; j<npts; j++)
      {
      if ( this->VerticeNeighbourhood[pts[j]].type == VTK_SIMPLE_VERTEX )
        {
        if ( j == (npts-1) ) //end-of-line marked FIXED
          {
          this->VerticeNeighbourhood[pts[j]].type = VTK_FIXED_VERTEX;
          }
        else if ( j == 0 ) //beginning-of-line marked FIXED
          {
          this->VerticeNeighbourhood[pts[0]].type = VTK_FIXED_VERTEX;
          inPts->GetPoint(pts[0],x2);
          inPts->GetPoint(pts[1],x3);
          }
        else //is edge vertex (unless already edge vertex!)
          {
          this->VerticeNeighbourhood[pts[j]].type = VTK_FEATURE_EDGE_VERTEX;
          this->VerticeNeighbourhood[pts[j]].edges = vtkIdList::New();
          this->VerticeNeighbourhood[pts[j]].edges->SetNumberOfIds(2);
          this->VerticeNeighbourhood[pts[j]].edges->SetId(0,pts[j-1]);
          this->VerticeNeighbourhood[pts[j]].edges->SetId(1,pts[j+1]);
          }
        } //if simple vertex

      else if ( this->VerticeNeighbourhood[pts[j]].type == VTK_FEATURE_EDGE_VERTEX )
        { //multiply connected, becomes fixed!
        this->VerticeNeighbourhood[pts[j]].type = VTK_FIXED_VERTEX;
        this->VerticeNeighbourhood[pts[j]].edges->Delete();
        this->VerticeNeighbourhood[pts[j]].edges = NULL;
        }

      } //for all points in this line
    } //for all lines
  this->UpdateProgress(0.25);

  // now polygons and triangle strips-------------------------------
  inPolys=input->GetPolys();
  numPolys = inPolys->GetNumberOfCells();
  inStrips=input->GetStrips();
  numStrips = inStrips->GetNumberOfCells();

  if ( numPolys > 0 || numStrips > 0 )
    { //build cell structure
    vtkCellArray *polys;
    vtkIdType cellId;
    int numNei, nei, edge;
    vtkIdType numNeiPts;
    vtkIdType *neiPts;
    double normal[3], neiNormal[3];
    vtkIdList *neighbors;

    neighbors = vtkIdList::New();
    neighbors->Allocate(VTK_CELL_SIZE);

    inMesh = vtkPolyData::New();
    inMesh->SetPoints(inPts);
    inMesh->SetPolys(inPolys);
    Mesh = inMesh;

    if ( (numStrips = inStrips->GetNumberOfCells()) > 0 )
      { // convert data to triangles
      inMesh->SetStrips(inStrips);
      toTris = vtkTriangleFilter::New();
      toTris->SetInput(inMesh);
      toTris->Update();
      Mesh = toTris->GetOutput();
      }

    Mesh->BuildLinks(); //to do neighborhood searching
    polys = Mesh->GetPolys();
    this->UpdateProgress(0.375);

    for (cellId=0, polys->InitTraversal(); polys->GetNextCell(npts,pts); 
    cellId++)
      {
      for (i=0; i < npts; i++) 
        {
        p1 = pts[i];
        p2 = pts[(i+1)%npts];

        if ( this->VerticeNeighbourhood[p1].edges == NULL )
          {
          this->VerticeNeighbourhood[p1].edges = vtkIdList::New();
          this->VerticeNeighbourhood[p1].edges->Allocate(16,6);
          }
        if ( this->VerticeNeighbourhood[p2].edges == NULL )
          {
          this->VerticeNeighbourhood[p2].edges = vtkIdList::New();
          this->VerticeNeighbourhood[p2].edges->Allocate(16,6);
          }

        Mesh->GetCellEdgeNeighbors(cellId,p1,p2,neighbors);
        numNei = neighbors->GetNumberOfIds();

        edge = VTK_SIMPLE_VERTEX;
        if ( numNei == 0 )
          {
          edge = VTK_BOUNDARY_EDGE_VERTEX;
          }

        else if ( numNei >= 2 )
          {
          // check to make sure that this edge hasn't been marked already
          for (j=0; j < numNei; j++)
            {
            if ( neighbors->GetId(j) < cellId )
              {
              break;
              }
            }
          if ( j >= numNei )
            {
            edge = VTK_FEATURE_EDGE_VERTEX;
            }
          }

        else if ( numNei == 1 && (nei=neighbors->GetId(0)) > cellId ) 
          {
            edge = VTK_FEATURE_EDGE_VERTEX;
          }
        else // a visited edge; skip rest of analysis
          {
          continue;
          }

        if ( edge && this->VerticeNeighbourhood[p1].type == VTK_SIMPLE_VERTEX )
          {
          this->VerticeNeighbourhood[p1].edges->Reset();
          this->VerticeNeighbourhood[p1].edges->InsertNextId(p2);
          this->VerticeNeighbourhood[p1].type = edge;
          }
        else if ( (edge && this->VerticeNeighbourhood[p1].type == VTK_BOUNDARY_EDGE_VERTEX) ||
        (edge && this->VerticeNeighbourhood[p1].type == VTK_FEATURE_EDGE_VERTEX) ||
        (!edge && this->VerticeNeighbourhood[p1].type == VTK_SIMPLE_VERTEX ) )
          {
          this->VerticeNeighbourhood[p1].edges->InsertNextId(p2);
          if ( this->VerticeNeighbourhood[p1].type && edge == VTK_BOUNDARY_EDGE_VERTEX )
            {
            this->VerticeNeighbourhood[p1].type = VTK_BOUNDARY_EDGE_VERTEX;
            }
          }

        if ( edge && this->VerticeNeighbourhood[p2].type == VTK_SIMPLE_VERTEX )
          {
          this->VerticeNeighbourhood[p2].edges->Reset();
          this->VerticeNeighbourhood[p2].edges->InsertNextId(p1);
          this->VerticeNeighbourhood[p2].type = edge;
          }
        else if ( (edge && this->VerticeNeighbourhood[p2].type == VTK_BOUNDARY_EDGE_VERTEX ) ||
        (edge && this->VerticeNeighbourhood[p2].type == VTK_FEATURE_EDGE_VERTEX) ||
        (!edge && this->VerticeNeighbourhood[p2].type == VTK_SIMPLE_VERTEX ) )
          {
          this->VerticeNeighbourhood[p2].edges->InsertNextId(p1);
          if ( this->VerticeNeighbourhood[p2].type && edge == VTK_BOUNDARY_EDGE_VERTEX )
            {
            this->VerticeNeighbourhood[p2].type = VTK_BOUNDARY_EDGE_VERTEX;
            }
          }
        }
      }

    inMesh->Delete();
    if (toTris) {toTris->Delete();}
    
    neighbors->Delete();
    }//if strips or polys

  this->UpdateProgress(0.50);
}
    
void vtkPolyDataNeighbourhood::OrientNeighbourhood( vtkIdType ptId)
{
    vtkIdList* unsortedNeighbourhood = this->VerticeNeighbourhood[ptId].edges;
    int numNei = unsortedNeighbourhood->GetNumberOfIds();
    vtkIdList* orientedNeighbourhood = vtkIdList::New( );
    
    // Get first neighbour and set it as chain begin
    int firstChainPtId = unsortedNeighbourhood->GetId( 0 );
    int currentChainPtId = firstChainPtId;
    do
    {
       orientedNeighbourhood->InsertNextId( currentChainPtId );

       int fromPoint = currentChainPtId;

       // Find common point between point neighbours and chain point neighbours
       vtkIdList* currentChainPtNei = this->VerticeNeighbourhood[currentChainPtId].edges;
       for( int chainNei = 0; chainNei < currentChainPtNei->GetNumberOfIds(); chainNei++ )
          for( int ptNei = 0; ptNei < numNei; ptNei++ )
          {
             if( currentChainPtNei->GetId( chainNei ) == unsortedNeighbourhood->GetId( ptNei) 
                && unsortedNeighbourhood->GetId( ptNei ) != fromPoint )
             {
                currentChainPtId = ptNei;
                break;
             }
          }
   }
   while( currentChainPtId != firstChainPtId );
   this->VerticeNeighbourhood[ptId].edges->Delete( );
   this->VerticeNeighbourhood[ptId].edges = orientedNeighbourhood;
}


void vtkPolyDataNeighbourhood::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);


}

int vtkPolyDataNeighbourhood::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);
  
  this->BuildNeighbourhood( );
  for( int i = 0; 
       i < static_cast<vtkPolyData*>(this->GetInput( ))->GetNumberOfPoints( );
       i ++)
  {
     //this->OrientNeighbourhood( i );
     cout << "pt["<<i<<"]: ";
     for( int nei = 0; nei < this->VerticeNeighbourhood[i].edges->GetNumberOfIds(); nei++ )
        cout << this->VerticeNeighbourhood[i].edges->GetId( nei ) << ", ";
     cout << endl;
     
  }
  return( 1 );

}
