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

//! \class vtkChainSource
//! \brief VTK source based on a set of vtkPoints
//!
//! \todo Comments
//!
//! \author Jerome Velut
//! \date 14 dec 2009

#ifndef __VTKPOLYLINESOURCE_H__
#define __VTKPOLYLINESOURCE_H__

#include "vtkPolyDataAlgorithm.h"
#include "vtkPoints.h"

class VTK_EXPORT vtkChainSource : public vtkPolyDataAlgorithm
{
public:
    static vtkChainSource *New();
    vtkTypeRevisionMacro(vtkChainSource,vtkPolyDataAlgorithm);
    void PrintSelf(ostream& os, vtkIndent indent);

    // Description:
    // Set position of first end point.
    vtkSetObjectMacro(Points,vtkPoints);
    vtkGetObjectMacro(Points,vtkPoints);

    vtkSetMacro( CellType, int );
    vtkGetMacro( CellType, int );

    void SetCellTypeToVertices( ) {
        this->SetCellType(0);
    };
    void SetCellTypeToLines( ) {
        this->SetCellType(1);
    };

protected:
    vtkChainSource( );
    ~vtkChainSource() {};

    int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
    int RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
    vtkPoints* Points;

private:
    vtkChainSource(const vtkChainSource&);  // Not implemented.
    void operator=(const vtkChainSource&);  // Not implemented.

    int CellType; //!< spcify if the output topology will be made of vertices or lines
};

#endif //__VTKPOLYLINESOURCE_H__
