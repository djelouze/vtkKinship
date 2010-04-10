// Copyright (c) 2010, Jérôme Velut
// All rights reserved.
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of the University of California, Berkeley nor the
//       names of its contributors may be used to endorse or promote products
//       derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "vtkImageMomentKernelSource.h"

#include "vtkImageData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkStreamingDemandDrivenPipeline.h"

#include <math.h>

vtkCxxRevisionMacro(vtkImageMomentKernelSource, "$Revision: 0.1 $");
vtkStandardNewMacro(vtkImageMomentKernelSource);

//----------------------------------------------------------------------------
vtkImageMomentKernelSource::vtkImageMomentKernelSource()
{
  this->KernelSize = 5;
  this->SubSampling = 100;
  this->AutoSubSampling = 1;
  
  this->SetNumberOfInputPorts( 0 );
}

void vtkImageMomentKernelSource::SetKernelSize( int kernelSize )
{
   if( kernelSize%2 == 0)
   {
      vtkDebugMacro( << "trying to set an odd kernel size, \
                         that is not handled today. Kernel \
                         size is set to "<<kernelSize+1<<".");
      kernelSize++;
   }
   
   if( kernelSize != this->KernelSize )
   {
      this->KernelSize = kernelSize;
      this->Modified( );
   }
}

//----------------------------------------------------------------------------
int vtkImageMomentKernelSource::RequestInformation (
  vtkInformation * vtkNotUsed(request),
  vtkInformationVector** vtkNotUsed( inputVector ),
  vtkInformationVector *outputVector)
{
  // get the info objects
  vtkInformation* outInfo = outputVector->GetInformationObject(0);
  int extent[6] = {0,this->KernelSize-1,
                   0,this->KernelSize-1,
                   0,this->KernelSize-1};
  
  outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(),
               extent,6);
  outInfo->Set(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT(),
               extent,6);
  vtkDataObject::SetPointDataActiveScalarInfo(outInfo, VTK_DOUBLE, 10);
  return 1;
}

//----------------------------------------------------------------------------
int vtkImageMomentKernelSource::RequestData(
  vtkInformation* vtkNotUsed(request),
  vtkInformationVector** vtkNotUsed(inputVector),
  vtkInformationVector* outputVector)
{
  double *outPtr;
  int idxX, idxY, idxZ;
  int maxX, maxY, maxZ;
  int maxApproxX, maxApproxY, maxApproxZ;
  vtkIdType outIncX, outIncY, outIncZ;
  unsigned long count = 0;
  unsigned long target;
  
  // Moment kernel specifics
   double offset = (double)this->KernelSize/2.0;
   double step;
   
   //! User asked to compute automatically the subvoxel approximation rate
   if( this->AutoSubSampling )
   {
      //! Find the closest integer divisor of SubSampling for KernelSize
      //! this ensure that each voxel will be divided entirely, ie no
      //! partial volume effect will appear.
      double dDiv = this->SubSampling / static_cast<double>( this->KernelSize );
      int iDiv = static_cast<int>(dDiv);
      ( dDiv - iDiv ) > 0.5 ? iDiv++:iDiv;
      this->TrueSubSampling = this->KernelSize * iDiv;
      
      step = this->KernelSize / static_cast<double>(this->TrueSubSampling);
   }
   else
      step = 1.0 / static_cast<double>(this->SubSampling);
   
   double dv = pow(2.0 * step / static_cast<double>(this->KernelSize), 3);
   double dx = 2.0 / static_cast<double>(this->KernelSize);
   double sphereRadiusSq = offset * offset;
  
  double cX, cY, cZ; // Centered coordinates
  double sum[10]; // the kernel values of a given moment come from an integral
  double spX, spY, spZ; // Sub-pixel coordinates for the integral approximation
  int idxSpX, idxSpY, idxSpZ; // for-loop variable for the sub-sampling
  
  vtkInformation *outInfo = outputVector->GetInformationObject(0);
  vtkImageData *data = vtkImageData::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

  int extent[6];
  outInfo->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(),extent);
  
  data->SetExtent(extent);
  data->AllocateScalars();
  outPtr = static_cast<double*>(data->GetScalarPointerForExtent(extent));

  if (data->GetScalarType() != VTK_DOUBLE)
  {
    vtkErrorMacro("Execute: This source only outputs doubles");
  }
  
  // find the region to loop over
  maxX = extent[1] - extent[0];
  maxY = extent[3] - extent[2]; 
  maxZ = extent[5] - extent[4];
  
  maxApproxX = this->TrueSubSampling / this->KernelSize - 1;
  maxApproxY = this->TrueSubSampling / this->KernelSize - 1;
  maxApproxZ = this->TrueSubSampling / this->KernelSize - 1;
 
  //! Get increments to march through data 
  data->GetContinuousIncrements(extent, outIncX, outIncY, outIncZ);
  
  target = static_cast<unsigned long>((maxZ+1)*(maxY+1)/50.0);
  target++;

  //! Loop through ouput pixels
  for (idxZ = 0; idxZ <= maxZ; idxZ++)
  {
    cZ = idxZ + step / 2.0 - offset;
    for (idxY = 0; !this->AbortExecute && idxY <= maxY; idxY++)
    {
      if (!(count%target))
        this->UpdateProgress(count/(50.0*target));
      count++;

      cY = idxY + step / 2.0 - offset;
      for (idxX = 0; idxX <= maxX; idxX++)
      {
        cX = idxX + step / 2.0 - offset; 

        // Compute the kernel values for each moment
        for( int i = 0; i < 10; i++)
            sum[i] = 0;

        for( idxSpX = 0; idxSpX <= maxApproxX; idxSpX++ ) 
        {
            spX = cX + idxSpX * step;
            for( idxSpY = 0; idxSpY <= maxApproxY; idxSpY++)
            {
               spY = cY + idxSpY * step;
               for(idxSpZ = 0; idxSpZ <= maxApproxZ; idxSpZ++)
               {
                  spZ = cZ + idxSpZ * step;
                  if( (spX*spX + spY*spY + spZ*spZ) < sphereRadiusSq )
                  {
                     sum[0] += 1.0;
                     sum[1] += spX;
                     sum[2] += spY;
                     sum[3] += spZ;
                     sum[4] += spX*spY;
                     sum[5] += spX*spZ;
                     sum[6] += spY*spZ;
                     sum[7] += spX*spX;
                     sum[8] += spY*spY;
                     sum[9] += spZ*spZ;
                  }
               }
            }
         }

        // Pixel operation
        // M000
        *outPtr = dv * sum[0];
        outPtr++;
        //M100;
        *outPtr = dv * dx * sum[1];
        outPtr++;
        //M010;
        *outPtr = dv * dx * sum[2];
        outPtr++;
        //M001;
        *outPtr = dv * dx * sum[3];
        outPtr++;
        //M110;
        *outPtr = dv * dx * dx * sum[4];
        outPtr++;
        //M101;
        *outPtr = dv * dx * dx * sum[5];
        outPtr++;
        //M011;
        *outPtr = dv * dx * dx * sum[6];
        outPtr++;
        //M200;
        *outPtr = dv * dx * dx * sum[7];
        outPtr++;
        //M020;
        *outPtr = dv * dx * dx * sum[8];
        outPtr++;
        //M002;
        *outPtr = dv * dx * dx * sum[9];
        outPtr++;
      }
      
      outPtr += outIncY;
    }
    
    outPtr += outIncZ;
    }

  data->GetPointData( )->GetScalars( )->SetName( "MomentKernel" );
  data->GetPointData( )->SetActiveScalars( "MomentKernel" );
  return 1;
}

void vtkImageMomentKernelSource::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "KernelSize: " << this->KernelSize << "\n";
  os << indent << "AutoSubSampling: " << this->AutoSubSampling << "\n";
  os << indent << "SubSampling: " << this->SubSampling << "\n";
  os << indent << indent   << "approximation step: " 
                           << this->KernelSize / (double)this->SubSampling << "\n";
  os << indent << indent   << "max approximation step: " 
                           << this->SubSampling / this->KernelSize << "\n";
}



