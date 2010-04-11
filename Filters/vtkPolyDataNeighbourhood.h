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
