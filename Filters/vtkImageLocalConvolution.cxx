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


#include "vtkImageLocalConvolution.h"

#include "vtkImageData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkDoubleArray.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkPolyData.h"
#include "vtkPointSet.h"

vtkStandardNewMacro(vtkImageLocalConvolution);

//----------------------------------------------------------------------------
// Construct an instance of vtkImageLocalConvolution filter.
// Default kernel is : 3 components 3-D. The first and third
// do nothing, the second component doubles the value.
vtkImageLocalConvolution::vtkImageLocalConvolution()
{
  this->OutputDataName = 0;
  this->NormalizedIntensitiesOff( );
  this->SetNumberOfInputPorts( 3 );
}

//----------------------------------------------------------------------------
// Destructor
vtkImageLocalConvolution::~vtkImageLocalConvolution()
{
}

//----------------------------------------------------------------------------
void vtkImageLocalConvolution::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
// Get the kernel, this is an internal method
double* vtkImageLocalConvolution::GetKernel()
{
  return( static_cast<double*>(static_cast<vtkImageData*>(this->GetInputDataObject( 2,0 ))
                                                     ->GetScalarPointer( )) );
}

//----------------------------------------------------------------------------
// Specify a source object at a specified table location.
void vtkImageLocalConvolution::SetKernelConnection(int id, vtkAlgorithmOutput* algOutput)
{
  if (id < 0)
    {
    vtkErrorMacro("Bad index " << id << " for source.");
    return;
    }

  int numConnections = this->GetNumberOfInputConnections(2);
  if (id < numConnections)
    {
    this->SetNthInputConnection(2, id, algOutput);
    }
  else if (id == numConnections && algOutput)
    {
    this->AddInputConnection(2, algOutput);
    }
  else if (algOutput)
    {
    vtkWarningMacro("The source id provided is larger than the maximum "
                    "source id, using " << numConnections << " instead.");
    this->AddInputConnection(2, algOutput);
    }
}

//----------------------------------------------------------------------------
// Specify a source object at a specified table location.
void vtkImageLocalConvolution::SetImageConnection(int id, vtkAlgorithmOutput* algOutput)
{
  if (id < 0)
    {
    vtkErrorMacro("Bad index " << id << " for source.");
    return;
    }

  int numConnections = this->GetNumberOfInputConnections(1);
  if (id < numConnections)
    {
    this->SetNthInputConnection(1, id, algOutput);
    }
  else if (id == numConnections && algOutput)
    {
    this->AddInputConnection(1, algOutput);
    }
  else if (algOutput)
    {
    vtkWarningMacro("The source id provided is larger than the maximum "
                    "source id, using " << numConnections << " instead.");
    this->AddInputConnection(1, algOutput);
    }
}


//---------------------------------------------------------------------------
int vtkImageLocalConvolution::FillInputPortInformation(int port, vtkInformation *info)
{
  if( port == 0 ) // point set port
     info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPointSet");
  else if( port == 1 || port == 2) // Kernel or image port
     info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkImageData");
  return 1;
}

//---------------------------------------------------------------------------
int vtkImageLocalConvolution::FillOutputPortInformation(int port, vtkInformation *info)
{
  if( port == 0 )
  {  // point set port : check if this is a polydata
     info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkPolyData");
  }
 return 1;
}

template <class T>
void vtkImageLocalConvolutionExecute(vtkImageLocalConvolution *self,
                              vtkInformationVector **inputVector, T* inPtr,
                              vtkInformationVector *outputVector)
{
  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *imageInfo = inputVector[1]->GetInformationObject(0);
  vtkInformation *kernelInfo = inputVector[2]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  // get the inputs and ouptut
  vtkImageData *image = vtkImageData::SafeDownCast(
    imageInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkImageData *kernelImage = vtkImageData::SafeDownCast(
    kernelInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkPointSet *input = vtkPointSet::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));

  vtkPointSet *output = vtkPointSet::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

  // For looping though output (and input) pixels.
  vtkIdType inInc0, inInc1, inInc2;
  double *localOutput;
  int numComps;
  numComps = image->GetNumberOfScalarComponents();
  
  // For looping through hood pixels
  int hoodMin0, hoodMax0, hoodMin1, hoodMax1, hoodMin2, hoodMax2;
  int hoodIdx0, hoodIdx1, hoodIdx2;
  T *hoodPtr0, *hoodPtr1, *hoodPtr2, *localInPtr;

  // For looping through the kernel, and compute the kernel result
  int kernelIdx, kernelIdxC, kernelNumComps, *kernelSize, kernelMiddle[3];
  double sum;
  kernelNumComps = kernelImage->GetNumberOfScalarComponents( );
  localOutput = new double[kernelNumComps];
  kernelSize = kernelImage->GetDimensions();
  // Get the kernel. 
  double *kernel = static_cast<double*>( kernelImage->GetScalarPointer( ) );
  
  // Get information to march through data
  image->GetIncrements(inInc0, inInc1, inInc2);
  
  kernelMiddle[0] = kernelSize[0] / 2;
  kernelMiddle[1] = kernelSize[1] / 2;
  kernelMiddle[2] = kernelSize[2] / 2;

  hoodMin0 = -kernelMiddle[0];
  hoodMin1 = -kernelMiddle[1];
  hoodMin2 = -kernelMiddle[2];

  hoodMax0 = hoodMin0 + kernelSize[0] - 1;
  hoodMax1 = hoodMin1 + kernelSize[1] - 1;
  hoodMax2 = hoodMin2 + kernelSize[2] - 1;
  
  // Handle boundaries : change temporarily the image extent for 
  // the FindPoint function : if the convolution place is inside the 
  // boundary (ie 'image border' - kernelSize/2 ), the execution abort
  //! \bug it seems that FindPoint does not handle boundaries...
  int* imageExtent = image->GetExtent( );
  int tempExtent[6];
  for( int comp = 0; comp < 3; comp++ )
  {
     tempExtent[comp*2]= imageExtent[comp*2] + kernelMiddle[comp] + 1;
     tempExtent[comp*2 + 1] = imageExtent[comp*2 + 1] - kernelMiddle[comp] - 1;
  }
  //image->SetUpdateExtent( tempExtent );
  
  // Prepare output
  vtkPoints *outPoints = vtkPoints::New( );
  vtkDoubleArray* outData = vtkDoubleArray::New( );
  outData->SetNumberOfComponents( kernelNumComps );

  // Loop over input points
  for( int ptId = 0; ptId < input->GetPoints( )->GetNumberOfPoints( ); ptId++ )
  {
     double* point = input->GetPoint( ptId );
     vtkIdType imagePtId = image->FindPoint( point );

     int gridLoc[3];
     self->GetGridLocation( image, imagePtId, gridLoc );
     
     if( imagePtId < 0 ) 
     {
        vtkErrorWithObjectMacro(self, <<"Convolution site is outside the image ( "
                                      <<  point[0] << ", "
                                      <<  point[1] << ", "
                                      <<  point[2] << " )" );
        for( int i = 0; i < kernelNumComps ; i++ )
           localOutput[i] = 0;
     }
     else if(    gridLoc[0] < tempExtent[0] || gridLoc[0] > tempExtent[1]
              || gridLoc[1] < tempExtent[2] || gridLoc[1] > tempExtent[3]
              || gridLoc[2] < tempExtent[4] || gridLoc[2] > tempExtent[5] )
     {
        vtkErrorWithObjectMacro(self, <<"Convolution site is outside the image ( "
                                      <<  point[0] << ", "
                                      <<  point[1] << ", "
                                      <<  point[2] << " )" );
        for( int i = 0; i < kernelNumComps ; i++ )
           localOutput[i] = 0;
     }
     else 
     {
      
         // initialize the data pointer to the point position
         localInPtr = inPtr + imagePtId * image->GetNumberOfScalarComponents( );
   
         // loop through kernel components
         for (kernelIdxC = 0; kernelIdxC < kernelNumComps; ++kernelIdxC)
         {
            // Inner loop where we compute the kernel
            // Set the sum to zero
            sum = 0;
            T min = 0, max = 1;


            // If NormizedIntensities is On, pre-loop through neighbors
            // to find min and max values

            if( self->GetNormalizedIntensities( ) )
            {
               hoodPtr2 = localInPtr - kernelMiddle[0] * inInc0 
                                    - kernelMiddle[1] * inInc1 
                                    - kernelMiddle[2] * inInc2;
               min = *hoodPtr2;
               max = *hoodPtr2;

               for (hoodIdx2 = hoodMin2; hoodIdx2 <= hoodMax2; ++hoodIdx2)
               {
                  hoodPtr1 = hoodPtr2;
                     
                  for (hoodIdx1 = hoodMin1; hoodIdx1 <= hoodMax1; ++hoodIdx1)
                  {
                     hoodPtr0 = hoodPtr1;
                  
                     for (hoodIdx0 = hoodMin0; hoodIdx0 <= hoodMax0; ++hoodIdx0)
                     {
                        if( *hoodPtr0 > max )
                           max = *hoodPtr0;
                        if( *hoodPtr0 < min )
                           min = *hoodPtr0;
                  
                        hoodPtr0 += inInc0;
                     }
                  
                     hoodPtr1 += inInc1;
                  }
                  
                     hoodPtr2 += inInc2;
               }
               if( min == max ) // constant image!! don't performed the normalization
               {
                  min = 0;
                  max = 1;
               }
            }
            // loop through neighborhood pixels
            hoodPtr2 = localInPtr - kernelMiddle[0] * inInc0 
                                 - kernelMiddle[1] * inInc1 
                                 - kernelMiddle[2] * inInc2;

            // Set the kernel index to the starting position
            // according to the current component
            kernelIdx = kernelIdxC;

            for (hoodIdx2 = hoodMin2; hoodIdx2 <= hoodMax2; ++hoodIdx2)
            {
               hoodPtr1 = hoodPtr2;
                  
               for (hoodIdx1 = hoodMin1; hoodIdx1 <= hoodMax1; ++hoodIdx1)
               {
                  hoodPtr0 = hoodPtr1;
               
                  for (hoodIdx0 = hoodMin0; hoodIdx0 <= hoodMax0; ++hoodIdx0)
                  {
                     // update the convolution sum.
                     sum += (*hoodPtr0 - min)/(max-min) * kernel[kernelIdx];
               
                     // Take the next position in the kernel
                     kernelIdx += kernelNumComps;
                     hoodPtr0 += inInc0;
                  }
               
                  hoodPtr1 += inInc1;
               }
               
                  hoodPtr2 += inInc2;
            }
            // Set the output to the correct value
            localOutput[kernelIdxC] = sum;
         } // End loop through kernel comp
      }
      outPoints->InsertNextPoint( input->GetPoints( )->GetPoint( ptId ) );
      outData->InsertNextTuple( localOutput );
   }// End loop over input points
   
   // The image has been temporarily shrinked for handling of boundaries.
   // Now get back to the original extent.
   //image->SetUpdateExtent( imageExtent );
   
   // output has new points and data.
   output->SetPoints( outPoints );
   output->GetPointData( )->DeepCopy( input->GetPointData( ) );
   output->GetPointData( )->AddArray( outData );
   outData->SetName( self->GetOutputDataName( ) );

   output->GetCellData( )->DeepCopy( input->GetCellData( ) );
   
   delete localOutput;
   outPoints->Delete( );
   outData->Delete( );
   
   return;
}

//----------------------------------------------------------------------------
// This method contains the first switch statement that calls the correct
// templated function for the input and output Data types.
int vtkImageLocalConvolution::RequestData(
                              vtkInformation *request,
                              vtkInformationVector **inputVector,
                              vtkInformationVector *outputVector)
{
  vtkInformation *imageInfo = inputVector[1]->GetInformationObject(0);
  vtkImageData *image = vtkImageData::SafeDownCast(
    imageInfo->Get(vtkDataObject::DATA_OBJECT()));
  void* inPtr = image->GetScalarPointer( );
  switch (image->GetScalarType())
  {
    vtkTemplateMacro(
      vtkImageLocalConvolutionExecute(this, inputVector, 
                                   static_cast<VTK_TT *>(inPtr),
                                   outputVector));

    default:
      vtkErrorMacro(<< "Execute: Unknown ScalarType");
      return 0;
  }

  // Preserve topology and geometry
    vtkPointSet *output = vtkPointSet::SafeDownCast(
                           outputVector->GetInformationObject(0)
		                               ->Get(vtkDataObject::DATA_OBJECT()));
    vtkPointSet *input = vtkPointSet::SafeDownCast(
                           inputVector[0]->GetInformationObject(0)
	                                     ->Get(vtkDataObject::DATA_OBJECT()));
	output->CopyStructure( input );

  return 1;
}

void vtkImageLocalConvolution::GetGridLocation( vtkImageData* img, vtkIdType ptId, int* ijk )
{
   int dims[3],extent[6];
   img->GetDimensions( dims );
   img->GetExtent( extent );
   ijk[0] = ptId % dims[0] + extent[0];
   ijk[1] = (ptId / dims[0]) % dims[1] + extent[2];
   ijk[2] = ptId / (dims[0]*dims[1]) + extent[4];
}
