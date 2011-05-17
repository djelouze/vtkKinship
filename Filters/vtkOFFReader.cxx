#include "vtkOFFReader.h"

#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkFloatArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPolyData.h"

#include <fstream>
#include <string>

vtkStandardNewMacro(vtkOFFReader);


// Construct object with merging set to true.
vtkOFFReader::vtkOFFReader()
{
  this->FileName = NULL;

  this->SetNumberOfInputPorts(0);
}

vtkOFFReader::~vtkOFFReader()
{
  if (this->FileName)
    {
    delete [] this->FileName;
    }
}

int vtkOFFReader::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **vtkNotUsed(inputVector),
  vtkInformationVector *outputVector)
{
  // get the info object
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  // get the ouptut
  vtkPolyData *output = vtkPolyData::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

  int i, j, k;
  int numPts=0, numPolys=0;

  if (!this->FileName)
    {
    vtkErrorMacro(<<"A File Name must be specified.");
    return 0;
    }

  // open a OFF file for reading
  std::ifstream offFile(this->FileName);
  std::string line;
  offFile >> line;
  cout << line << endl;
  if(line.compare("OFF") != 0)
  {
    vtkErrorMacro(<<"Not an OFF file"); // case already checked by CanReaFile
    return( 0 );
  }
  
  unsigned int nbVertex, nbFaces, nbEdges;
  offFile >> nbVertex  >> nbFaces >> nbEdges;
  cout << nbVertex << "; " << nbFaces << "; " << nbEdges <<endl;

  vtkPoints* points = vtkPoints::New( );
  for( int v = 0; v < nbVertex ; v++ )
  {
    double pt[3];
    offFile >> pt[0] >> pt[1] >> pt[2];
    points->InsertNextPoint(pt);
  }
    
  vtkCellArray* polys = vtkCellArray::New();
  for( int f = 0; f < nbFaces ; f++ )
  {
    int fDim;
    offFile >> fDim;
    vtkIdType* fPoints = new vtkIdType[fDim];
    for( int fptIdx = 0; fptIdx < fDim;fptIdx++)
      offFile >> fPoints[fptIdx];

    polys->InsertNextCell(fDim, fPoints);
  }
  
  output->SetPolys(polys);
  output->SetPoints(points);
  
  return 1;
}

int vtkOFFReader::CanReadFile(const char *filename)
{
  std::ifstream offFile(filename);
  std::string line;
  offFile >> line;
  offFile.close();
  return(line.compare("OFF")==0);
}

void vtkOFFReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "File Name: " 
     << (this->FileName ? this->FileName : "(none)") << "\n";

}
