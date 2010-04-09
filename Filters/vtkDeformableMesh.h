//! \class vtkDeformableMesh
//! \brief Implements a regularized deformation of a vtkPolyData
//!
//! This class implements an iterative mechanism that deforms
//! the input given a number of iterations. The output is used
//! as input until the property Reset is set to 1.
//! 
//! At each iteration, the input probes the vector of the image
//! (second input). The regularization is performed as a smoothing of
//! these displacement vectors ( Default is a laplacian smoothing). 
//! Finally, the input mesh is warped along the regularized deformation
//! vector (vtkWarpVector)
//!
//! \author Jerome Velut
//! \date 28 mar 2010

#ifndef __vtkDeformableMesh_h
#define __vtkDeformableMesh_h

#include "vtkIterativePolyDataAlgorithm.h"
#include "vtkWarpVector.h"
#include "vtkProbeFilter.h"
#include "vtkImageData.h"


class VTK_EXPORT vtkDeformableMesh : public vtkIterativePolyDataAlgorithm
{
public:
  vtkTypeRevisionMacro(vtkDeformableMesh,vtkIterativePolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  static vtkDeformableMesh *New();


protected:
  vtkDeformableMesh();
  ~vtkDeformableMesh() {};

  void IterativeRequestData( vtkInformationVector** );
  void Reset( vtkInformationVector** );
  int FillInputPortInformation(int port, vtkInformation *info);

private:
  vtkDeformableMesh(const vtkDeformableMesh&);  // Not implemented.
  void operator=(const vtkDeformableMesh&);  // Not implemented.


  vtkWarpVector* WarpFilter; //!< deformation filter
  vtkProbeFilter* ProbeFilter; //!< get the deformation from the image
};

#endif
