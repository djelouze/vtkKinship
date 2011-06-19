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

#include "vtkICPPolyDataFilter.h"

#include <vtkUVSphereSource.h>
#include <vtkSmartPointer.h>
int main(int argc, char** argv )
{
    vtkSmartPointer<vtkUVSphereSource> sphere1;
    sphere1 = vtkSmartPointer<vtkUVSphereSource>::New( );
 
    vtkSmartPointer<vtkUVSphereSource> sphere2;
    sphere2 = vtkSmartPointer<vtkUVSphereSource>::New( );
    sphere2->SetCenter( 1,1,1 );
    
    vtkSmartPointer<vtkICPPolyDataFilter> icp;
    icp = vtkSmartPointer<vtkICPPolyDataFilter>::New( );
    icp->SetSourceConnection( sphere1->GetOutputPort() );
    icp->SetTargetConnection( sphere2->GetOutputPort() );
    icp->Update( );
   
    icp->PrintSelf( std::cout, vtkIndent() ); 
}
