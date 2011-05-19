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

#include "vtkPolyDataSurfaceOfRevolution.h"

#include "vtkMath.h"

#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"
#include <vtkMath.h>

vtkCxxRevisionMacro(vtkPolyDataSurfaceOfRevolution, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkPolyDataSurfaceOfRevolution);

vtkPolyDataSurfaceOfRevolution::vtkPolyDataSurfaceOfRevolution()
{
   this->SetNumberOfInputPorts( 2 );
   
   this->Theta = 2*vtkMath::Pi();
   this->StepAngle = this->Theta/10;
}


void vtkPolyDataSurfaceOfRevolution::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "Theta: " << this->GetTheta() << endl;
}

//---------------------------------------------------------------------------
int vtkPolyDataSurfaceOfRevolution::FillInputPortInformation(int port, vtkInformation *info)
{
  if( port == 0 || port == 1 ) // input mesh port
     info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData");
  return 1;
}


void vtkPolyDataSurfaceOfRevolution::Reset( vtkInformationVector** inputVector )
{

}

void vtkPolyDataSurfaceOfRevolution::IterativeRequestData(
  vtkInformationVector **inputVector)
{
   // Coordinates after rotation of a point (x,y,z) about an axis passing through (a,b,c) 
   // directed along (u,v,w) by an angle t are:
   // | (a(v*v+w*w)-u(bv+cw-ux-vy-wz))(1-cos(t))+x.cos(t)+(-cv+bw-wy+vz)sin(t)|
   // | (b(u*u+w*w)-v(au+cw-ux-vy-wz))(1-cos(t))+y.cos(t)+(cu-aw+wx-uz)sin(t) |
   // | (c(u*u+v*v)-w(au+bv-ux-vy-wz))(1-cos(t))+z.cos(t)+(-bu+av-vx+uy)sin(t)|
}

