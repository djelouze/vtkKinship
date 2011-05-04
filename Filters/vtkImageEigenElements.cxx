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


#include "vtkImageEigenElements.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkDoubleArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkMath.h"

vtkCxxRevisionMacro(vtkImageEigenElements, "$Revision: 0.1 $");
vtkStandardNewMacro(vtkImageEigenElements);

//----------------------------------------------------------------------------
// Construct an instance of vtkImageEigenElements filter.
vtkImageEigenElements::vtkImageEigenElements()
{
  this->UseMask = 0;
  this->InMask = 0;
  this->inM11 = 0;
  this->inM12 = 0;
  this->inM13 = 0;
  this->inM22 = 0;
  this->inM23 = 0;
  this->inM33 = 0;
  this->SetNumberOfInputPorts( 1 );
}

//----------------------------------------------------------------------------
// Destructor
vtkImageEigenElements::~vtkImageEigenElements()
{
}

//----------------------------------------------------------------------------
void vtkImageEigenElements::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
int vtkImageEigenElements::RequestUpdateExtent(vtkInformation*,
                                         vtkInformationVector** inputVector,
                                         vtkInformationVector* outputVector)
{
  // Get input and output pipeline information.
  vtkInformation* outInfo = outputVector->GetInformationObject(0);
  vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);

  return( 1 );
}

//----------------------------------------------------------------------------
int vtkImageEigenElements::RequestInformation(vtkInformation*,
                                         vtkInformationVector** inputVector,
                                         vtkInformationVector* outputVector)
{
  // Get input and output pipeline information.
  vtkInformation* outInfo = outputVector->GetInformationObject(0);
  vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);

  vtkImageData *inImage = vtkImageData::SafeDownCast( inInfo->Get(vtkDataObject::DATA_OBJECT()));
  int inNumComps = inImage->GetNumberOfScalarComponents( );

  vtkDataObject::SetPointDataActiveScalarInfo(outInfo, 
                                              VTK_DOUBLE,
                                              3);

  return 1;
}

void vtkImageEigenElements::MapInputComponentsToTensor(int M11, int M12, int M13, int M22, int M23, int M33)
{
   this->inM11 = M11;
   this->inM12 = M12;
   this->inM13 = M13;
   this->inM22 = M22;
   this->inM23 = M23;
   this->inM33 = M33;
   this->Modified( );  
}

void vtkImageEigenElements::GetInputComponentsToTensorMap(int& M11, int& M12, int& M13, int& M22, int& M23, int& M33)
{
  M11 = this->inM11;
  M12 = this->inM12;
  M13 = this->inM13;
  M22 = this->inM22;
  M23 = this->inM23;
  M33 = this->inM33;
}

//----------------------------------------------------------------------------
// This templated function executes the filter on any region,
// whether it needs boundary checking or not.
// If the filter needs to be faster, the function could be duplicated
// for strictly center (no boundary) processing.
template <class T>
void vtkImageEigenElementsExecute(vtkImageEigenElements *self,
                             vtkImageData *inData, T *inPtr, 
                             vtkImageData *outData, double *outPtr,
                             int outExt[6], int id,
                             vtkInformation *inInfo)
{
  // For looping though output (and input) pixels.
  int outMin0, outMax0, outMin1, outMax1, outMin2, outMax2;
  int outIdx0, outIdx1, outIdx2;
  vtkIdType inInc0, inInc1, inInc2;
  vtkIdType outInc0, outInc1, outInc2;
  T *inPtr0, *inPtr1, *inPtr2;
  double *outPtr0, *outPtr1, *outPtr2;
  int numComps, outIdxC;


  // The extent of the whole input image
  int inImageExt[6];

  // to compute the range
  unsigned long count = 0;
  unsigned long target;

  // Get information to march through data
  inData->GetIncrements(inInc0, inInc1, inInc2);
  inInfo->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), inImageExt);
  outData->GetIncrements(outInc0, outInc1, outInc2); 
  outMin0 = outExt[0];   outMax0 = outExt[1];
  outMin1 = outExt[2];   outMax1 = outExt[3];
  outMin2 = outExt[4];   outMax2 = outExt[5];
  numComps = outData->GetNumberOfScalarComponents();

  outData->GetPointData()->GetScalars()->SetName("MaxEigenVector");
    
  int inM11, inM12, inM13, inM22, inM23, inM33;
  self->GetInputComponentsToTensorMap(inM11, inM12, inM13, inM22, inM23, inM33);
  

  
    vtkDoubleArray* maxEval = vtkDoubleArray::New( );
    maxEval->SetName( "MaxEigenValue" );
    maxEval->SetNumberOfComponents( 1 );
    maxEval->SetNumberOfTuples( outData->GetNumberOfPoints());
    outData->GetPointData( )->AddArray( maxEval );
    maxEval->Delete( );  

    vtkDoubleArray* medEvec = vtkDoubleArray::New( );
    medEvec->SetName( "MedEigenVector" );
    medEvec->SetNumberOfComponents( 3 );
    medEvec->SetNumberOfTuples( outData->GetNumberOfPoints());
    outData->GetPointData( )->AddArray( medEvec );
    medEvec->Delete( );    
    
    vtkDoubleArray* medEval = vtkDoubleArray::New( );
    medEval->SetName( "MedEigenValue" );
    medEval->SetNumberOfComponents( 1 );
    medEval->SetNumberOfTuples( outData->GetNumberOfPoints());
    outData->GetPointData( )->AddArray( medEval );
    medEval->Delete( );
    
    vtkDoubleArray* minEvec = vtkDoubleArray::New( );
    minEvec->SetName( "MinEigenVector" );
    minEvec->SetNumberOfComponents( 3 );
    minEvec->SetNumberOfTuples( outData->GetNumberOfPoints() );
    outData->GetPointData( )->AddArray( minEvec );
    minEvec->Delete( );    
    
    vtkDoubleArray* minEval = vtkDoubleArray::New( );
    minEval->SetName( "MinEigenValue" );
    minEval->SetNumberOfComponents( 1 );
    minEval->SetNumberOfTuples( outData->GetNumberOfPoints());
    outData->GetPointData( )->AddArray( minEval );
    minEval->Delete( );
    
    
    double *maxEvecPtr = static_cast<double*>(outData->GetPointData()->GetArray("MaxEigenVector")->GetVoidPointer(0));
    double *maxEvalPtr = static_cast<double*>(outData->GetPointData()->GetArray("MaxEigenValue")->GetVoidPointer(0));
    double *medEvecPtr = static_cast<double*>(outData->GetPointData()->GetArray("MedEigenVector")->GetVoidPointer(0));
    double *medEvalPtr = static_cast<double*>(outData->GetPointData()->GetArray("MedEigenValue")->GetVoidPointer(0));
    double *minEvecPtr = static_cast<double*>(outData->GetPointData()->GetArray("MinEigenVector")->GetVoidPointer(0));
    double *minEvalPtr = static_cast<double*>(outData->GetPointData()->GetArray("MinEigenValue")->GetVoidPointer(0));




  // in and out should be marching through corresponding pixels.
  inPtr = static_cast<T *>(
    inData->GetScalarPointer(outMin0, outMin1, outMin2));

  target = static_cast<unsigned long>(numComps*(outMax2 - outMin2 + 1)*
                                      (outMax1 - outMin1 + 1)/50.0);
  target++;
  
	    double** matrix, **evect;
	    matrix = new double*[3];
	    evect = new double*[3];
	    for( int i = 0; i < 3;i++)
	    {
	      matrix[i] = new double[3];
	      evect[i] = new double[3];
	    }
	    double* eval = new double[3];
    // loop through pixels of output
    outPtr2 = outPtr;
    inPtr2 = inPtr;
    for (outIdx2 = outMin2; outIdx2 <= outMax2; ++outIdx2)
      {
      outPtr1 = outPtr2;
      inPtr1 = inPtr2;
      for (outIdx1 = outMin1; 
           outIdx1 <= outMax1 && !self->AbortExecute; 
           ++outIdx1)
        {
        if (!id)
          {
          if (!(count%target))
            {
            self->UpdateProgress(count/(50.0*target));
            }
          count++;
          }

        outPtr0 = outPtr1;
        inPtr0 = inPtr1;

        for (outIdx0 = outMin0; outIdx0 <= outMax0; ++outIdx0)
          {
	    
	    matrix[0][0] = *(inPtr0+inM11);
	    matrix[0][1] = *(inPtr0+inM12);
	    matrix[0][2] = *(inPtr0+inM13);
	    matrix[1][0] = *(inPtr0+inM12);
	    matrix[1][1] = *(inPtr0+inM22);
	    matrix[1][2] = *(inPtr0+inM23);
	    matrix[2][0] = *(inPtr0+inM13);
	    matrix[2][1] = *(inPtr0+inM23);
	    matrix[2][2] = *(inPtr0+inM33);

  	    T multiplier = 1;
  if( self->GetUseMask() )
    multiplier = *(inPtr0+self->GetInMask());
	    
          vtkMath::Jacobi(matrix, eval,evect );
          // Set the output pixel to the correct value
	  for( int comp = 0; comp < 3;comp ++)
	  {
	    
          maxEvecPtr[comp] = evect[comp][0]*multiplier;
          medEvecPtr[comp] = evect[comp][1]*multiplier;
          minEvecPtr[comp] = evect[comp][2]*multiplier;
	  }
	  maxEvecPtr += 3;
	  medEvecPtr += 3;
	  minEvecPtr += 3;
	  
	  *maxEvalPtr = eval[0]*multiplier;
	  maxEvalPtr++;
	  *medEvalPtr = eval[1]*multiplier;
	  medEvalPtr++;
	  *minEvalPtr = eval[2]*multiplier;
	  minEvalPtr++;
 
          inPtr0 += inInc0;
          outPtr0 += outInc0;
          }

        inPtr1 += inInc1;
        outPtr1 += outInc1;
        }

      inPtr2 += inInc2;
      outPtr2 += outInc2;
      }
      for( int i = 0; i< 3;i++)
      {
	delete matrix[i];
	delete evect[i];
      }
      delete eval;
}

//----------------------------------------------------------------------------
// This method contains the first switch statement that calls the correct
// templated function for the input and output Data types.
// It hanldes image boundaries, so the image does not shrink.
void vtkImageEigenElements::ThreadedRequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *vtkNotUsed(outputVector),
  vtkImageData ***inData,
  vtkImageData **outData,
  int outExt[6], int id)
{
  void *inPtr = inData[0][0]->GetScalarPointerForExtent(outExt);
  void *outPtr = outData[0]->GetScalarPointerForExtent(outExt);

  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);

 
  switch (inData[0][0]->GetScalarType())
    {
    vtkTemplateMacro(
      vtkImageEigenElementsExecute(this, 
	                          inData[0][0], static_cast<VTK_TT *>(inPtr), 
                              outData[0], static_cast<double *>(outPtr),
                              outExt, id, inInfo));

    default:
      vtkErrorMacro(<< "Execute: Unknown ScalarType");
      return;
    }
}
