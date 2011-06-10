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

//! \class vtkSymmetricRecursivePolyDataFilter
//! \brief Implements a symmetric recursive filtering of points or vectors
//!
//! This is a parent class that implements a causal/anti-causal filtering
//! of points. The impulse response is of second order, and the transfert
//! function is defined by:
//! T(z) = (b2.z^-2 + b1.z^-1 + b0)/(a2.z^-2 + a1.z^-1 + a0)
//! Coefficient an and bn should be overriden by child classes.
//!
//! The initialisation of the filter is performed through IR approximation,
//! the children should also override the Causal and Symmetric Impulse
//! response. It implies to choose for an extrapolation of the signals at
//! initialization task:
//! - Sphere (ExtrapolationMode = 0) : the signal try to find an existing
//! cell that minimizes the angle at poles
//! - Closed (ExtrapolationMode = 1) :
//!
//! An input signal x(k) is evaluated through the abscissae k, which belong to 
//! a cell //! of point ids, either Vertices (SignalContainer = 0) or Lines 
//! (SignalSource = 1). Values f are taken in point geometry (SignalSource = 0)
//! or in vectors (SignalSource = 1).
//! 

#ifndef __vtkSymmetricRecursivePolyDataFilter_h
#define __vtkSymmetricRecursivePolyDataFilter_h

#include <vtkPolyDataAlgorithm.h>

class vtkCellArray;
class SphericSymmetryHelper;

class VTK_EXPORT vtkSymmetricRecursivePolyDataFilter : public vtkPolyDataAlgorithm
{
public:
    vtkTypeRevisionMacro(vtkSymmetricRecursivePolyDataFilter,vtkPolyDataAlgorithm);
    //void PrintSelf(ostream& os, vtkIndent indent);

    static vtkSymmetricRecursivePolyDataFilter *New();

    vtkSetMacro(SignalSource,int);
    vtkGetMacro(SignalSource,int);

    vtkSetMacro(SignalContainer,int);
    vtkGetMacro(SignalContainer,int);    

    vtkSetMacro(ExtrapolationMode,int);
    vtkGetMacro(ExtrapolationMode,int);     
    
    vtkSetMacro(Causality,int);
    vtkGetMacro(Causality,int);

    void SetA( double, double, double );
    void SetB( double, double, double );
    
    double GetPointSampling( vtkPolyData* mesh, int cellLocation, int curveCoord );
    double GetMeanSampling( vtkPolyData* mesh, int cellLocation, int curveCoord, int nbPoint );

protected:
    vtkSymmetricRecursivePolyDataFilter();
    ~vtkSymmetricRecursivePolyDataFilter();

    int RequestData( vtkInformation*,
                     vtkInformationVector**,
                     vtkInformationVector*);
    int FillInputPortInformation(int port, vtkInformation *info);

    virtual double CausalImpulseResponse(int k) {
        return( k==0? 1.0 : 0.0 );
    };
    virtual double SymmetricImpulseResponse(int k) {
        return( k==0? 1.0 : 0.0 );
    };
    virtual bool StopIRApproximation( int k ) {
        return(k<10?false:true);
    };


    //! Recurrence equation coefficients. Child classes should ovverride these functions.
    virtual double b0( int k ); 
    //! Recurrence equation coefficients. Child classes should ovverride these functions.
    virtual double b1( int k );
    //! Recurrence equation coefficients. Child classes should ovverride these functions.
    virtual double b2( int k );
    //! Recurrence equation coefficients. Child classes should ovverride these functions.
    virtual double a0( int k );
    //! Recurrence equation coefficients. Child classes should ovverride these functions.
    virtual double a1( int k );
    //! Recurrence equation coefficients. Child classes should ovverride these functions.
    virtual double a2( int k );

    //! Get the set of signal depending on FilterIsoLines state.
    vtkCellArray* GetSignals( vtkPolyData* );

    //! In spheric symmetry, find the cell that continues the line 
    void SetSymmetricCell( vtkIdType orgCellLocation );

    virtual void ComputeFilterCoefficients( vtkPolyData* mesh, int cellLocation, int curveCoord ) {};

    void GetSignalComponents( vtkPolyData* mesh, int cellId, int curveCoord, double* signalComp );
    void SetData( vtkPolyData* mesh,vtkIdType pointId, double* data);
    int DefinePointRole( vtkIdType );

    vtkPolyData* causalMesh;

    //! Internal accessor to SignalSource that doesn't call Modified()
    void SetSignalSourceInternal( int );
    
private:
    void CausalImpulseResponseApproximation( vtkPolyData* mesh, int cellId, int curveCoord, double* approxPoint );
    void SymmetricImpulseResponseApproximation( vtkPolyData* mesh, int cellId, int curveCoord, double* approxPoint );
  
    double b[3]; //!< b0, b1 and b2 coefficients
    double a[3]; //!< a0, a1 and a2 coefficients
    int Causality; //!< specify the direction of filtering (causal/anticausal)
    int SignalContainer; //!< Where to find the cells? 0 is Vertices, 1 is Lines.
    int SignalSource;//!< Signal may be feed by point geometry (0) or vector data (1)
    int ExtrapolationMode;//!< 0: pole (find the best existing continuing cell), 1: closed spline, 2: mirror (not implemented)

    SphericSymmetryHelper* sphericHelper; //!< contains poles information
    int meanSouthPoleValence;
    int meanNorthPoleValence;
    vtkIdType southPoleId;
    vtkIdType northPoleId;
    double meanSouthPole[3];
    double meanNorthPole[3];

    vtkIdType* symmetricCellPoints; //!< used in the spheric symmetry. Changes at each different polyline filtering
    vtkIdType  nbSymmetricCellPoints;
    vtkIdType  symmetricCellPosition; //!< for causal/anticausal process: one have to reverse the sym cell too.
};

#endif //__vtkSymmetricRecursivePolyDataFilter_h
