
#ifndef __vtkPolyDataNeighbourhood_h
#define __vtkPolyDataNeighbourhood_h

#include "vtkPolyDataAlgorithm.h"

 // Special structure for marking vertices
typedef struct VertexEdge
{
char      type;
vtkIdList *edges; // connected edges (list of connected point ids)
} VertexEdge, *VertexEdgePtr;

class VTK_GRAPHICS_EXPORT vtkPolyDataNeighbourhood : public vtkPolyDataAlgorithm
{
public:
  vtkTypeRevisionMacro(vtkPolyDataNeighbourhood,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  static vtkPolyDataNeighbourhood *New();

protected:
  vtkPolyDataNeighbourhood();
  ~vtkPolyDataNeighbourhood() {};

  int RequestData( vtkInformation*, 
                   vtkInformationVector**, 
                   vtkInformationVector*);

  VertexEdgePtr VerticeNeighbourhood;
  void BuildNeighbourhood( );

private:
  vtkPolyDataNeighbourhood(const vtkPolyDataNeighbourhood&);  // Not implemented.
  void operator=(const vtkPolyDataNeighbourhood&);  // Not implemented.

  void OrientNeighbourhood( vtkIdType ptId );


};

#endif
