#include "vtkDualPolyDataFilter.h"
#include "vtkDoubleArray.h"
#include "vtkFloatArray.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkMath.h"
#include "vtkExtractEdges.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"

#include <set>

vtkStandardNewMacro(vtkDualPolyDataFilter);

vtkDualPolyDataFilter::vtkDualPolyDataFilter( )
{
}

vtkDualPolyDataFilter::~vtkDualPolyDataFilter( )
{

}
//---------------------------------------------------------------------------
int vtkDualPolyDataFilter::FillInputPortInformation(int port, vtkInformation *info)
{
    if( port == 0 )
        info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData");
    return 1;
}

int vtkDualPolyDataFilter::RequestData(
    vtkInformation *vtkNotUsed(request),
    vtkInformationVector **inputVector,
    vtkInformationVector *outputVector)
{
    // get the info objects
    vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
    vtkInformation *outInfo = outputVector->GetInformationObject(0);

    // get the input and ouptut
    vtkPolyData *input = vtkPolyData::SafeDownCast(
                             inInfo->Get(vtkDataObject::DATA_OBJECT()));
    vtkPolyData *output = vtkPolyData::SafeDownCast(
                              outInfo->Get(vtkDataObject::DATA_OBJECT()));

    vtkPolyData* shallowInput = vtkPolyData::New();
    shallowInput->ShallowCopy( input );
    shallowInput->BuildLinks();
    shallowInput->BuildCells();
    vtkCellArray* inTri = shallowInput->GetPolys();
    inTri->InitTraversal();

    vtkPoints* inPts = input->GetPoints();
    vtkSmartPointer<vtkPoints> outPts;
    outPts = vtkSmartPointer<vtkPoints>::New();
    outPts->SetNumberOfPoints(inTri->GetNumberOfCells());
    output->SetPoints( outPts );

    std::set<Edge> visited;

    vtkSmartPointer<vtkIdList> triPts = vtkSmartPointer<vtkIdList>::New();
    vtkSmartPointer<vtkIdList> cellIds1 = vtkSmartPointer<vtkIdList>::New();
    vtkSmartPointer<vtkIdList> cellIds2 = vtkSmartPointer<vtkIdList>::New();
    vtkSmartPointer<vtkCellArray> edges = vtkSmartPointer<vtkCellArray>::New();

    int triId = 0;
    while( inTri->GetNextCell( triPts ) )
    {
      if( triPts->GetNumberOfIds() != 3 )
      {
        // not a triangle!!
        continue;
      }

      double triCenter[3]={0,0,0};
      double pt1[3], pt2[3], pt3[3];
      inPts->GetPoint(triPts->GetId(0), pt1);
      inPts->GetPoint(triPts->GetId(1), pt2);
      inPts->GetPoint(triPts->GetId(2), pt3);
      vtkMath::Add(pt1,pt2,triCenter);
      vtkMath::Add(pt3,triCenter,triCenter);
      vtkMath::MultiplyScalar(triCenter,1/3.);
      outPts->SetPoint(triId, triCenter);
     
      // For each vertex of the current triangle
      for( int vertex = 0; vertex < 3; vertex++ )
      {
        bool borderEdge = false;

        // Create a new edge with consecutive vertices
        std::pair<std::set<Edge>::iterator, bool> ret;
        Edge newEdge( triPts->GetId( vertex ), triPts->GetId( (vertex + 1) % 3 ) );
        ret = visited.insert(newEdge);
        if( ret.second == false )
        {
          continue;
        }

        shallowInput->GetPointCells( newEdge.p1, cellIds1 );
        shallowInput->GetPointCells( newEdge.p2, cellIds2 );
        cellIds1->IntersectWith( cellIds2 );
        if( cellIds1->GetNumberOfIds() > 2 )
        {
          std::cerr << "Not a 2-manifold" << std::endl;
          continue;
        }
        if( cellIds1->GetNumberOfIds() == 1 )
        {
          borderEdge = true;
        }

        if( !borderEdge )
        {
          edges->InsertNextCell( 2 );
          edges->InsertCellPoint(cellIds1->GetId(0));
          edges->InsertCellPoint( cellIds1->GetId(1) );
        }

      }
      triId++;
    }

    output->SetLines( edges );
 
    return 1;
}
