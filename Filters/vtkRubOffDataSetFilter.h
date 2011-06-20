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

//! \class vtkRubOffDataSetFilter
//! \brief Rub off data from source on input
//!
//! This filter iterates over input cells/points and finds the closest 
//! cell/point in source. Output contains cell and point data from those
//! closest target elements.
//! 
//! \warning Proof of concept class: it simply set on celldata the id of the
//! closest source point from the input cell centers.
//! \bug vtkCellLocator::FindClosestPoint(...) crashes on GetBucketNeighbors

#ifndef __vtkRubOffDataSetFilter_h
#define __vtkRubOffDataSetFilter_h

#include <vtkDataSetAlgorithm.h>

class VTK_EXPORT vtkRubOffDataSetFilter : public vtkDataSetAlgorithm
{
public:
  static vtkRubOffDataSetFilter *New();
  vtkTypeMacro(vtkRubOffDataSetFilter,vtkDataSetAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  void SetSourceConnection(vtkAlgorithmOutput* algOutput);

protected:
  vtkRubOffDataSetFilter();
  ~vtkRubOffDataSetFilter();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, 
    vtkInformationVector *);
  virtual int RequestInformation(vtkInformation *, vtkInformationVector **, 
    vtkInformationVector *);
  virtual int RequestUpdateExtent(vtkInformation *, vtkInformationVector **, 
    vtkInformationVector *);

private:
  vtkRubOffDataSetFilter(const vtkRubOffDataSetFilter&);  // Not implemented.
  void operator=(const vtkRubOffDataSetFilter&);  // Not implemented.

  void GetCellCenter( vtkDataSet* container, vtkIdType cellId, double center[3] );

};

#endif //__vtkRubOffDataSetFilter_h
