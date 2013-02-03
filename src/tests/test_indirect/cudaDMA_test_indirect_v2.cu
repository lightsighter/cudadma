/*
 *  Copyright 2010 NVIDIA Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

/* Software DMA project
*
* Host code.
*/

// includes, system
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <set>

#include "cuda.h"
#include "cuda_runtime.h"

#include "cudaDMAv2.h"

#include "params_directed.h"

#define WARP_SIZE 32

#define MAX_TRIES 8192

// includes, project

// includes, kernels

#define CUDA_SAFE_CALL(x)					\
	{							\
		cudaError_t err = (x);				\
		if (err != cudaSuccess)				\
		{						\
			printf("Cuda error: %s\n", cudaGetErrorString(err));	\
			exit(false);				\
		}						\
	}

template<int ALIGNMENT, int ALIGN_OFFSET, int BYTES_PER_THREAD, int BYTES_PER_ELMT, int DMA_THREADS, int NUM_ELMTS>
__global__ void __launch_bounds__(1024,1)
special_xfer_four( float *idata, float *odata, int *offsets, 
                   int num_compute_threads, int buffer_size, const bool single, const bool qualified)
{
  extern __shared__ float buffer[];

  CudaDMAIndirect<true,true,ALIGNMENT,BYTES_PER_THREAD,BYTES_PER_ELMT,DMA_THREADS,NUM_ELMTS>
    dma0 (1, num_compute_threads,
             num_compute_threads);

  if (dma0.owns_this_thread())
  {
    float *base_ptr = &(idata[ALIGN_OFFSET]);
    if (single)
    {
      if (qualified)
        dma0.template execute_dma<true,LOAD_CACHE_GLOBAL,STORE_CACHE_GLOBAL>(offsets, base_ptr, &(buffer[ALIGN_OFFSET])); 
      else
        dma0.execute_dma(offsets, base_ptr, &(buffer[ALIGN_OFFSET]));
    }
    else
    {
      if (qualified)
      {
        dma0.template start_xfer_async<true,LOAD_CACHE_GLOBAL,STORE_CACHE_STREAMING>(offsets, base_ptr);
        dma0.template wait_xfer_finish<true,LOAD_CACHE_GLOBAL,STORE_CACHE_STREAMING>(&(buffer[ALIGN_OFFSET]));
      }
      else
      {
        dma0.start_xfer_async(offsets, base_ptr);
        dma0.wait_xfer_finish(&(buffer[ALIGN_OFFSET]));
      }
    }
  }
  else
  {
    // Zero out the buffer 
    int iters = buffer_size/num_compute_threads;	
    int index = threadIdx.x;
    for (int i=0; i<iters; i++)
    {
            buffer[index] = 0.0f;
            index += num_compute_threads;
    }
    if (index < buffer_size)
            buffer[index] = 0.0f;
    dma0.start_async_dma();
    dma0.wait_for_dma_finish();
    // Now read the buffer out of shared and write the results back
    index = threadIdx.x;
    for (int i=0; i<iters; i++)
    {
            float res = buffer[index];
            odata[index] = res;
            index += num_compute_threads;
    }
    if (index < buffer_size)
    {
            float res = buffer[index];
            odata[index] = res;
    }
  }
}


template<int ALIGNMENT, int ALIGN_OFFSET, int BYTES_PER_THREAD, int BYTES_PER_ELMT, int DMA_THREADS>
__global__ void __launch_bounds__(1024,1)
special_xfer_three( float *idata, float *odata, int *offsets, 
                    int num_compute_threads, int num_elmts, int buffer_size, const bool single, const bool qualified)
{
  extern __shared__ float buffer[];

  CudaDMAIndirect<true,true,ALIGNMENT,BYTES_PER_THREAD,BYTES_PER_ELMT,DMA_THREADS>
    dma0 (1, num_compute_threads,
             num_compute_threads,
             num_elmts);

  if (dma0.owns_this_thread())
  {
    float *base_ptr = &(idata[ALIGN_OFFSET]);
    if (single)
    {
      if (qualified)
        dma0.template execute_dma<true,LOAD_CACHE_GLOBAL,STORE_CACHE_GLOBAL>(offsets, base_ptr, &(buffer[ALIGN_OFFSET])); 
      else
        dma0.execute_dma(offsets, base_ptr, &(buffer[ALIGN_OFFSET]));
    }
    else
    {
      if (qualified)
      {
        dma0.template start_xfer_async<true,LOAD_CACHE_GLOBAL,STORE_CACHE_STREAMING>(offsets, base_ptr);
        dma0.template wait_xfer_finish<true,LOAD_CACHE_GLOBAL,STORE_CACHE_STREAMING>(&(buffer[ALIGN_OFFSET]));
      }
      else
      {
        dma0.start_xfer_async(offsets, base_ptr);
        dma0.wait_xfer_finish(&(buffer[ALIGN_OFFSET]));
      }
    }
  }
  else
  {
    // Zero out the buffer 
    int iters = buffer_size/num_compute_threads;	
    int index = threadIdx.x;
    for (int i=0; i<iters; i++)
    {
            buffer[index] = 0.0f;
            index += num_compute_threads;
    }
    if (index < buffer_size)
            buffer[index] = 0.0f;
    dma0.start_async_dma();
    dma0.wait_for_dma_finish();
    // Now read the buffer out of shared and write the results back
    index = threadIdx.x;
    for (int i=0; i<iters; i++)
    {
            float res = buffer[index];
            odata[index] = res;
            index += num_compute_threads;
    }
    if (index < buffer_size)
    {
            float res = buffer[index];
            odata[index] = res;
    }
  }
}

template<int ALIGNMENT, int ALIGN_OFFSET, int BYTES_PER_THREAD, int BYTES_PER_ELMT>
__global__ void __launch_bounds__(1024,1)
special_xfer_two( float *idata, float *odata, int *offsets,  
                  int num_compute_threads, int num_dma_threads, int num_elmts, int buffer_size, const bool single, const bool qualified)
{
  extern __shared__ float buffer[];

  CudaDMAIndirect<true,true,ALIGNMENT,BYTES_PER_THREAD,BYTES_PER_ELMT>
    dma0 (1, num_dma_threads, num_compute_threads,
             num_compute_threads, num_elmts);

  if (dma0.owns_this_thread())
  {
    float *base_ptr = &(idata[ALIGN_OFFSET]);
    if (single)
    {
      if (qualified)
        dma0.template execute_dma<true,LOAD_CACHE_GLOBAL,STORE_CACHE_WRITE_THROUGH>(offsets, base_ptr, &(buffer[ALIGN_OFFSET])); 
      else
        dma0.execute_dma(offsets, base_ptr, &(buffer[ALIGN_OFFSET]));
    }
    else
    {
      if (qualified)
      {
        dma0.template start_xfer_async<true,LOAD_CACHE_STREAMING,STORE_CACHE_GLOBAL>(offsets, base_ptr);
        dma0.template wait_xfer_finish<true,LOAD_CACHE_STREAMING,STORE_CACHE_GLOBAL>(&(buffer[ALIGN_OFFSET]));
      }
      else
      {
        dma0.start_xfer_async(offsets, base_ptr);
        dma0.wait_xfer_finish(&(buffer[ALIGN_OFFSET]));
      }
    }
  }
  else
  {
    // Zero out the buffer 
    int iters = buffer_size/num_compute_threads;	
    int index = threadIdx.x;
    for (int i=0; i<iters; i++)
    {
            buffer[index] = 0.0f;
            index += num_compute_threads;
    }
    if (index < buffer_size)
            buffer[index] = 0.0f;
    dma0.start_async_dma();
    dma0.wait_for_dma_finish();
    // Now read the buffer out of shared and write the results back
    index = threadIdx.x;
    for (int i=0; i<iters; i++)
    {
            float res = buffer[index];
            odata[index] = res;
            index += num_compute_threads;
    }
    if (index < buffer_size)
    {
            float res = buffer[index];
            odata[index] = res;
    }
  }
}

template<int ALIGNMENT, int ALIGN_OFFSET, int BYTES_PER_THREAD>
__global__ void __launch_bounds__(1024,1)
special_xfer_one( float *idata, float *odata, int *offsets,  
                  int num_compute_threads, int num_dma_threads, int bytes_per_elmt, int num_elmts,  
                  int buffer_size, const bool single, const bool qualified)
{
  extern __shared__ float buffer[];

  CudaDMAIndirect<true,true,ALIGNMENT,BYTES_PER_THREAD>
    dma0 (1, num_dma_threads, num_compute_threads,
             num_compute_threads, bytes_per_elmt,
             num_elmts);

  if (dma0.owns_this_thread())
  {
    float *base_ptr = &(idata[ALIGN_OFFSET]);
    if (single)
    {
      if (qualified)
        dma0.template execute_dma<true,LOAD_CACHE_STREAMING,STORE_CACHE_STREAMING>(offsets, base_ptr, &(buffer[ALIGN_OFFSET])); 
      else
        dma0.execute_dma(offsets, base_ptr, &(buffer[ALIGN_OFFSET]));
    }
    else
    {
      if (qualified)
      {
        dma0.template start_xfer_async<true,LOAD_CACHE_STREAMING,STORE_CACHE_WRITE_THROUGH>(offsets, base_ptr);
        dma0.template wait_xfer_finish<true,LOAD_CACHE_STREAMING,STORE_CACHE_WRITE_THROUGH>(&(buffer[ALIGN_OFFSET]));
      }
      else
      {
        dma0.start_xfer_async(offsets, base_ptr);
        dma0.wait_xfer_finish(&(buffer[ALIGN_OFFSET]));
      }
    }
  }
  else
  {
    // Zero out the buffer 
    int iters = buffer_size/num_compute_threads;	
    int index = threadIdx.x;
    for (int i=0; i<iters; i++)
    {
            buffer[index] = 0.0f;
            index += num_compute_threads;
    }
    if (index < buffer_size)
            buffer[index] = 0.0f;
    dma0.start_async_dma();
    dma0.wait_for_dma_finish();
    // Now read the buffer out of shared and write the results back
    index = threadIdx.x;
    for (int i=0; i<iters; i++)
    {
            float res = buffer[index];
            odata[index] = res;
            index += num_compute_threads;
    }
    if (index < buffer_size)
    {
            float res = buffer[index];
            odata[index] = res;
    }
  }
}

template<int ALIGNMENT, int ALIGN_OFFSET, int BYTES_PER_THREAD, int BYTES_PER_ELMT, int DMA_THREADS, int NUM_ELMTS>
__global__ void __launch_bounds__(1024,1)
nonspec_xfer_four( float *idata, float *odata, int *offsets, 
                   int buffer_size, const bool single, const bool qualified)
{
  extern __shared__ float buffer[];

  CudaDMAIndirect<true,false,ALIGNMENT,BYTES_PER_THREAD,BYTES_PER_ELMT,DMA_THREADS,NUM_ELMTS>
    dma0;

  // Zero out the buffer
  int iters = buffer_size/blockDim.x;
  int index = threadIdx.x;
  for (int i = 0; i<iters; i++)
  {
    buffer[index] = 0.0f;
    index += blockDim.x;
  }
  if (index < buffer_size)
    buffer[index] = 0.0f;
  __syncthreads();
  // Perform the transfer
  float *base_ptr = &(idata[ALIGN_OFFSET]);
  if (single)
  {
    if (qualified)
      dma0.template execute_dma<true,LOAD_CACHE_LAST_USE,STORE_CACHE_GLOBAL>(offsets, base_ptr,&(buffer[ALIGN_OFFSET])); 
    else
      dma0.execute_dma(offsets, base_ptr,&(buffer[ALIGN_OFFSET]));
  }
  else
  {
    if (qualified)
    {
      dma0.template start_xfer_async<true,LOAD_CACHE_LAST_USE,STORE_CACHE_STREAMING>(offsets, base_ptr);
      dma0.template wait_xfer_finish<true,LOAD_CACHE_LAST_USE,STORE_CACHE_STREAMING>(&(buffer[ALIGN_OFFSET]));
    }
    else
    {
      dma0.start_xfer_async(offsets, base_ptr);
      dma0.wait_xfer_finish(&(buffer[ALIGN_OFFSET]));
    }
  }
  __syncthreads();
  // Now read the buffer out of shared and write the results back
  index = threadIdx.x;
  for (int i = 0; i<iters; i++)
  {
    odata[index] = buffer[index];
    index += blockDim.x;
  }
  if (index < buffer_size)
    odata[index] = buffer[index];
}


template<int ALIGNMENT, int ALIGN_OFFSET, int BYTES_PER_THREAD, int BYTES_PER_ELMT, int DMA_THREADS>
__global__ void __launch_bounds__(1024,1)
nonspec_xfer_three( float *idata, float *odata, int *offsets, int num_elmts,
                    int buffer_size, const bool single, const bool qualified)
{
  extern __shared__ float buffer[];

  CudaDMAIndirect<true,false,ALIGNMENT,BYTES_PER_THREAD,BYTES_PER_ELMT,DMA_THREADS>
    dma0(num_elmts);

  // Zero out the buffer
  int iters = buffer_size/blockDim.x;
  int index = threadIdx.x;
  for (int i = 0; i<iters; i++)
  {
    buffer[index] = 0.0f;
    index += blockDim.x;
  }
  if (index < buffer_size)
    buffer[index] = 0.0f;
  __syncthreads();
  // Perform the transfer
  float *base_ptr = &(idata[ALIGN_OFFSET]);
  if (single)
  {
    if (qualified)
      dma0.template execute_dma<true,LOAD_CACHE_LAST_USE,STORE_CACHE_GLOBAL>(offsets, base_ptr,&(buffer[ALIGN_OFFSET])); 
    else
      dma0.execute_dma(offsets, base_ptr,&(buffer[ALIGN_OFFSET]));
  }
  else
  {
    if (qualified)
    {
      dma0.template start_xfer_async<true,LOAD_CACHE_LAST_USE,STORE_CACHE_STREAMING>(offsets, base_ptr);
      dma0.template wait_xfer_finish<true,LOAD_CACHE_LAST_USE,STORE_CACHE_STREAMING>(&(buffer[ALIGN_OFFSET]));
    }
    else
    {
      dma0.start_xfer_async(offsets, base_ptr);
      dma0.wait_xfer_finish(&(buffer[ALIGN_OFFSET]));
    }
  }
  __syncthreads();
  // Now read the buffer out of shared and write the results back
  index = threadIdx.x;
  for (int i = 0; i<iters; i++)
  {
    odata[index] = buffer[index];
    index += blockDim.x;
  }
  if (index < buffer_size)
    odata[index] = buffer[index];
}

template<int ALIGNMENT, int ALIGN_OFFSET, int BYTES_PER_THREAD, int BYTES_PER_ELMT>
__global__ void __launch_bounds__(1024,1)
nonspec_xfer_two( float *idata, float *odata, int *offsets, int num_elmts,
                  int buffer_size, const bool single, const bool qualified)
{
  extern __shared__ float buffer[];

  CudaDMAIndirect<true,false,ALIGNMENT,BYTES_PER_THREAD,BYTES_PER_ELMT>
    dma0(num_elmts);

  // Zero out the buffer
  int iters = buffer_size/blockDim.x;
  int index = threadIdx.x;
  for (int i = 0; i<iters; i++)
  {
    buffer[index] = 0.0f;
    index += blockDim.x;
  }
  if (index < buffer_size)
    buffer[index] = 0.0f;
  __syncthreads();
  // Perform the transfer
  float *base_ptr = &(idata[ALIGN_OFFSET]);
  if (single)
  {
    if (qualified)
      dma0.template execute_dma<true,LOAD_CACHE_LAST_USE,STORE_CACHE_WRITE_THROUGH>(offsets, base_ptr,&(buffer[ALIGN_OFFSET])); 
    else
      dma0.execute_dma(offsets, base_ptr,&(buffer[ALIGN_OFFSET]));
  }
  else
  {
    if (qualified)
    {
      dma0.template start_xfer_async<true,LOAD_CACHE_VOLATILE,STORE_CACHE_GLOBAL>(offsets, base_ptr);
      dma0.template wait_xfer_finish<true,LOAD_CACHE_VOLATILE,STORE_CACHE_GLOBAL>(&(buffer[ALIGN_OFFSET]));
    }
    else
    {
      dma0.start_xfer_async(offsets, base_ptr);
      dma0.wait_xfer_finish(&(buffer[ALIGN_OFFSET]));
    }
  }
  __syncthreads();
  // Now read the buffer out of shared and write the results back
  index = threadIdx.x;
  for (int i = 0; i<iters; i++)
  {
    odata[index] = buffer[index];
    index += blockDim.x;
  }
  if (index < buffer_size)
    odata[index] = buffer[index];
}

template<int ALIGNMENT, int ALIGN_OFFSET, int BYTES_PER_THREAD>
__global__ void __launch_bounds__(1024,1)
nonspec_xfer_one( float *idata, float *odata, int *offsets, int bytes_per_elmt, int num_elmts,
                  int buffer_size, const bool single, const bool qualified)
{
  extern __shared__ float buffer[];

  CudaDMAIndirect<true,false,ALIGNMENT,BYTES_PER_THREAD>
    dma0(bytes_per_elmt, num_elmts);

  // Zero out the buffer
  int iters = buffer_size/blockDim.x;
  int index = threadIdx.x;
  for (int i = 0; i<iters; i++)
  {
    buffer[index] = 0.0f;
    index += blockDim.x;
  }
  if (index < buffer_size)
    buffer[index] = 0.0f;
  __syncthreads();
  // perform the transfer
  float *base_ptr = &(idata[ALIGN_OFFSET]);
  if (single)
  {
    if (qualified)
      dma0.template execute_dma<true,LOAD_CACHE_VOLATILE,STORE_CACHE_STREAMING>(offsets,base_ptr,&(buffer[ALIGN_OFFSET])); 
    else
      dma0.execute_dma(offsets,base_ptr,&(buffer[ALIGN_OFFSET]));
  }
  else
  {
    if (qualified)
    {
      dma0.template start_xfer_async<true,LOAD_CACHE_VOLATILE,STORE_CACHE_WRITE_THROUGH>(offsets,base_ptr);
      dma0.template wait_xfer_finish<true,LOAD_CACHE_VOLATILE,STORE_CACHE_WRITE_THROUGH>(&(buffer[ALIGN_OFFSET]));
    }
    else
    {
      dma0.start_xfer_async(offsets,base_ptr);
      dma0.wait_xfer_finish(&(buffer[ALIGN_OFFSET]));
    }
  }
  __syncthreads();
  index = threadIdx.x;
  for (int i = 0; i<iters; i++)
  {
    odata[index] = buffer[index];
    index += blockDim.x;
  }
  if (index < buffer_size)
    odata[index] = buffer[index];
}

int *h_offsets;
int max_offset = -1;
int max_index;

template<bool SPECIALIZED, int ALIGNMENT, int ALIGN_OFFSET, int BYTES_PER_THREAD, int BYTES_PER_ELMT, int DMA_THREADS, int NUM_ELMTS>
__host__ bool run_experiment(bool single, bool qualified, int num_templates)
{
        const int shared_buffer_size = (NUM_ELMTS*BYTES_PER_ELMT/sizeof(float) + ALIGN_OFFSET);
	// Check to see if we're using more shared memory than there is, if so return
	if ((shared_buffer_size*sizeof(float)) > 49152)
        {
                // Print pass so we don't get confused
                fprintf(stdout," - PASS!\n");
		fflush(stdout);
		return true;
        }

        // Allocate the inputs
        int input_size = (max_index + ALIGN_OFFSET);
        float *h_idata = (float*)malloc(input_size*sizeof(float));
              for (int i=0; i<input_size; i++)
                      h_idata[i] = float(i);

        // Allocate device memory and copy down
        float *d_idata;
        CUDA_SAFE_CALL( cudaMalloc( (void**)&d_idata, input_size*sizeof(float)));
        CUDA_SAFE_CALL( cudaMemcpy( d_idata, h_idata, input_size*sizeof(float), cudaMemcpyHostToDevice));
        // Allocate the output size
        int output_size = (NUM_ELMTS*BYTES_PER_ELMT/sizeof(float) + ALIGN_OFFSET);
        float *h_odata = (float*)malloc(output_size*sizeof(float));
        for (int i=0; i<output_size; i++)
                h_odata[i] = 0.0f;
        // Allocate device memory and copy down
        float *d_odata;
        CUDA_SAFE_CALL( cudaMalloc( (void**)&d_odata, output_size*sizeof(float)));
        CUDA_SAFE_CALL( cudaMemcpy( d_odata, h_odata, output_size*sizeof(float), cudaMemcpyHostToDevice));
        // Finally copy down the offsets
        int *d_offsets;
        CUDA_SAFE_CALL( cudaMalloc( (void**)&d_offsets, NUM_ELMTS*sizeof(int)));
        CUDA_SAFE_CALL( cudaMemcpy( d_offsets, h_offsets, NUM_ELMTS*sizeof(int), cudaMemcpyHostToDevice));

        int num_compute_warps = 1;
        int total_threads = 0;
        if (SPECIALIZED)
          total_threads = (num_compute_warps)*WARP_SIZE + DMA_THREADS;
        else
          total_threads = DMA_THREADS;
        assert(total_threads > 0);

	switch (num_templates)
	{
	case 1:
                if (SPECIALIZED)
                {
                  special_xfer_one<ALIGNMENT,ALIGN_OFFSET,BYTES_PER_THREAD>
                    <<<1,total_threads,shared_buffer_size*sizeof(float),0>>>
                    (d_idata, d_odata, d_offsets, num_compute_warps*WARP_SIZE, DMA_THREADS,
                      BYTES_PER_ELMT, NUM_ELMTS, shared_buffer_size, single, qualified); 
                }
                else
                {
                  nonspec_xfer_one<ALIGNMENT,ALIGN_OFFSET,BYTES_PER_THREAD>
                    <<<1,total_threads,shared_buffer_size*sizeof(float),0>>>
                    (d_idata, d_odata, d_offsets, BYTES_PER_ELMT, NUM_ELMTS, shared_buffer_size,single,qualified);
                }
		break;	
	case 2:
                if (SPECIALIZED)
                {
                  special_xfer_two<ALIGNMENT,ALIGN_OFFSET,BYTES_PER_THREAD,BYTES_PER_ELMT>
                    <<<1,total_threads,shared_buffer_size*sizeof(float),0>>>
                    (d_idata, d_odata, d_offsets, num_compute_warps*WARP_SIZE, DMA_THREADS,
                     NUM_ELMTS, shared_buffer_size, single, qualified);
                }
                else
                {
                  nonspec_xfer_two<ALIGNMENT,ALIGN_OFFSET,BYTES_PER_THREAD,BYTES_PER_ELMT>
                    <<<1,total_threads,shared_buffer_size*sizeof(float),0>>>
                    (d_idata, d_odata, d_offsets, NUM_ELMTS, shared_buffer_size, single, qualified);
                }
		break;
	case 3:
                if (SPECIALIZED)
                {
                  special_xfer_three<ALIGNMENT,ALIGN_OFFSET,BYTES_PER_THREAD,BYTES_PER_ELMT,DMA_THREADS>
                    <<<1,total_threads,shared_buffer_size*sizeof(float),0>>>
                    (d_idata, d_odata, d_offsets, num_compute_warps*WARP_SIZE,
                     NUM_ELMTS, shared_buffer_size, single, qualified);
                }
                else
                {
                  nonspec_xfer_three<ALIGNMENT,ALIGN_OFFSET,BYTES_PER_THREAD,BYTES_PER_ELMT,DMA_THREADS>
                    <<<1,total_threads,shared_buffer_size*sizeof(float),0>>>
                    (d_idata, d_odata, d_offsets, NUM_ELMTS, shared_buffer_size, single, qualified);
                }
		break;
	case 4:
                if (SPECIALIZED)
                {
                  special_xfer_four<ALIGNMENT,ALIGN_OFFSET,BYTES_PER_THREAD,BYTES_PER_ELMT,DMA_THREADS,NUM_ELMTS>
                    <<<1,total_threads,shared_buffer_size*sizeof(float),0>>>
                    (d_idata, d_odata, d_offsets, num_compute_warps*WARP_SIZE,
                     shared_buffer_size, single, qualified);
                }
                else
                {
                  nonspec_xfer_four<ALIGNMENT,ALIGN_OFFSET,BYTES_PER_THREAD,BYTES_PER_ELMT,DMA_THREADS,NUM_ELMTS>
                    <<<1,total_threads,shared_buffer_size*sizeof(float),0>>>
                    (d_idata, d_odata, d_offsets, shared_buffer_size, single, qualified);
                }
		break;
	default:
		assert(false);
		break;
	}
        CUDA_SAFE_CALL( cudaThreadSynchronize());

        CUDA_SAFE_CALL( cudaMemcpy (h_odata, d_odata, output_size*sizeof(float), cudaMemcpyDeviceToHost));

        // Check the result
        bool pass = true;
        for (int i=0; i<NUM_ELMTS && pass; i++)
        {
          int in_index = ALIGN_OFFSET+(h_offsets[i]*BYTES_PER_ELMT/sizeof(float));
          int out_index = ALIGN_OFFSET+i*BYTES_PER_ELMT/sizeof(float);
#if 0
          printf("elment %d result: ",i);
          for (int j=0; j<(BYTES_PER_ELMT/sizeof(float)); j++)
            printf("%f ", h_odata[out_index+j]);
          printf("\n");
          printf("expected element %d: ",i);
          for (int j=0; j<(BYTES_PER_ELMT/sizeof(float)); j++)
            printf("%f ", h_idata[in_index+j]);
          printf("\n");
#endif
          for (int j=0; j<(BYTES_PER_ELMT/sizeof(float)); j++)
          {
            //printf("%f ",h_odata[out_index+j]);
            if (h_idata[in_index+j] != h_odata[out_index+j])
            {
              //fprintf(stderr,"Experiment: %d element bytes, %d elements, %d DMA warps, %d alignment, %d offset, ",BYTES_PER_ELMT,NUM_ELMTS,DMA_THREADS/WARP_SIZE,ALIGNMENT,ALIGN_OFFSET);
              fprintf(stderr,"\nOffsets: ");
              for (int k=0; k<NUM_ELMTS; k++)
                fprintf(stderr,"%d ", h_offsets[k]);
              fprintf(stderr,"\nIndex %d of element %d was expecting %f but received %f\n", j, i, h_idata[in_index+j], h_odata[out_index+j]);
              pass = false;
              break;
            }
          }
          //printf("\n");
        }

	//if (!pass)
	{
		fprintf(stdout," - %s\n",(pass?"PASS!":"FAIL!"));
		fflush(stdout);
	}

        CUDA_SAFE_CALL( cudaFree(d_idata));
        CUDA_SAFE_CALL( cudaFree(d_odata));
        CUDA_SAFE_CALL( cudaFree(d_offsets));
        free(h_idata);
        free(h_odata);
	
	return pass;
}

__host__
int main()
{
#if 0
  CUDA_SAFE_CALL(cudaSetDevice(0));
  {
    cudaDeviceProp prop;
    CUDA_SAFE_CALL(cudaGetDeviceProperties(&prop, 0));
    printf("Running on %s\n", prop.name);
  }
#endif
  // Pick some random offsets
  h_offsets = (int*)malloc(PARAM_NUM_ELMTS*sizeof(int));
  // Initialize the random number generator
  srand(PARAM_RAND_SEED);
  {
    int tries = 0; 
    std::set<int> off_set;
    int cnt = 0;
    while (cnt < PARAM_NUM_ELMTS)
    {
      if (tries > MAX_TRIES)
      {
        // Give up if we can't find 
        // good offsets
        fprintf(stderr,"Warning: couldn't find good offsets, retry\n");
        fflush(stderr);
        return true;
      }
      tries++;
      int off = (rand() % (16384));
      off *= PARAM_ALIGNMENT;
      // Check if the offset is good
      bool good = true;
      for (std::set<int>::iterator it = off_set.begin();
            it != off_set.end(); it++)
      {
        if (!((off+PARAM_ELMT_SIZE) <= (*it)) && 
            !(((*it)+PARAM_ELMT_SIZE) <= off))
        {
          good = false;
          break;
        }
      }
      if (good)
      {
        h_offsets[cnt++] = off;
        //printf("%d ", off);
        if ((off*PARAM_ELMT_SIZE/int(sizeof(float))) > max_offset)
          max_offset = off*PARAM_ELMT_SIZE/int(sizeof(float));
        off_set.insert(off);
      }
    }
  }
  //printf("\n");
  assert(max_offset != -1);
  max_index = max_offset+(PARAM_ELMT_SIZE/sizeof(float));

  CudaDMAIndirect<>::diagnose(PARAM_ALIGNMENT, PARAM_BYTES_PER_THREAD, PARAM_ELMT_SIZE, PARAM_DMA_THREADS, PARAM_NUM_ELMTS, true);
  bool result = true;
  fprintf(stdout,"Running all experiments for ALIGNMENT-%2d OFFSET-%d BYTES_PER_THREAD-%3d ELMT_SIZE-%5d NUM_ELMTS-%5d DMA_WARPS-%2d\n",PARAM_ALIGNMENT,PARAM_OFFSET,PARAM_BYTES_PER_THREAD,PARAM_ELMT_SIZE,PARAM_NUM_ELMTS,PARAM_DMA_THREADS/WARP_SIZE);
  fprintf(stdout,"  Warp-Specialized Experiments\n");
  fprintf(stdout,"    Single-Phase Experiments\n");
  fprintf(stdout,"      Unqualified experiments\n");
  fprintf(stdout,"        Templates-1");
  result = run_experiment<true,PARAM_ALIGNMENT,PARAM_OFFSET,PARAM_BYTES_PER_THREAD,PARAM_ELMT_SIZE,PARAM_DMA_THREADS,PARAM_NUM_ELMTS>(true/*single*/,false/*qualified*/,1);
  if (!result) return result;
  fprintf(stdout,"        Templates-2");
  result = run_experiment<true,PARAM_ALIGNMENT,PARAM_OFFSET,PARAM_BYTES_PER_THREAD,PARAM_ELMT_SIZE,PARAM_DMA_THREADS,PARAM_NUM_ELMTS>(true/*single*/,false/*qualified*/,2);
  if (!result) return result;
  fprintf(stdout,"        Templates-3");
  result = run_experiment<true,PARAM_ALIGNMENT,PARAM_OFFSET,PARAM_BYTES_PER_THREAD,PARAM_ELMT_SIZE,PARAM_DMA_THREADS,PARAM_NUM_ELMTS>(true/*single*/,false/*qualified*/,3);
  if (!result) return result;
  fprintf(stdout,"        Templates-4");
  result = run_experiment<true,PARAM_ALIGNMENT,PARAM_OFFSET,PARAM_BYTES_PER_THREAD,PARAM_ELMT_SIZE,PARAM_DMA_THREADS,PARAM_NUM_ELMTS>(true/*single*/,false/*qualified*/,4);
  if (!result) return result;
  fprintf(stdout,"      Qualified Experiments\n");
  fprintf(stdout,"        Templates-1");
  result = run_experiment<true,PARAM_ALIGNMENT,PARAM_OFFSET,PARAM_BYTES_PER_THREAD,PARAM_ELMT_SIZE,PARAM_DMA_THREADS,PARAM_NUM_ELMTS>(true/*single*/,true/*qualified*/,1);
  if (!result) return result;
  fprintf(stdout,"        Templates-2");
  result = run_experiment<true,PARAM_ALIGNMENT,PARAM_OFFSET,PARAM_BYTES_PER_THREAD,PARAM_ELMT_SIZE,PARAM_DMA_THREADS,PARAM_NUM_ELMTS>(true/*single*/,true/*qualified*/,2);
  if (!result) return result;
  fprintf(stdout,"        Templates-3");
  result = run_experiment<true,PARAM_ALIGNMENT,PARAM_OFFSET,PARAM_BYTES_PER_THREAD,PARAM_ELMT_SIZE,PARAM_DMA_THREADS,PARAM_NUM_ELMTS>(true/*single*/,true/*qualified*/,3);
  if (!result) return result;
  fprintf(stdout,"        Templates-4");
  result = run_experiment<true,PARAM_ALIGNMENT,PARAM_OFFSET,PARAM_BYTES_PER_THREAD,PARAM_ELMT_SIZE,PARAM_DMA_THREADS,PARAM_NUM_ELMTS>(true/*single*/,true/*qualified*/,4);
  if (!result) return result;
  fprintf(stdout,"    Two-Phase Experiments\n");
  fprintf(stdout,"      Unqualified experiments\n");
  fprintf(stdout,"        Templates-1");
  result = run_experiment<true,PARAM_ALIGNMENT,PARAM_OFFSET,PARAM_BYTES_PER_THREAD,PARAM_ELMT_SIZE,PARAM_DMA_THREADS,PARAM_NUM_ELMTS>(false/*single*/,false/*qualified*/,1);
  if (!result) return result;
  fprintf(stdout,"        Templates-2");
  result = run_experiment<true,PARAM_ALIGNMENT,PARAM_OFFSET,PARAM_BYTES_PER_THREAD,PARAM_ELMT_SIZE,PARAM_DMA_THREADS,PARAM_NUM_ELMTS>(false/*single*/,false/*qualified*/,2);
  if (!result) return result;
  fprintf(stdout,"        Templates-3");
  result = run_experiment<true,PARAM_ALIGNMENT,PARAM_OFFSET,PARAM_BYTES_PER_THREAD,PARAM_ELMT_SIZE,PARAM_DMA_THREADS,PARAM_NUM_ELMTS>(false/*single*/,false/*qualified*/,3);
  if (!result) return result;
  fprintf(stdout,"        Templates-4");
  result = run_experiment<true,PARAM_ALIGNMENT,PARAM_OFFSET,PARAM_BYTES_PER_THREAD,PARAM_ELMT_SIZE,PARAM_DMA_THREADS,PARAM_NUM_ELMTS>(false/*single*/,false/*qualified*/,4);
  if (!result) return result;
  fprintf(stdout,"      Qualified Experiments\n");
  fprintf(stdout,"        Templates-1");
  result = run_experiment<true,PARAM_ALIGNMENT,PARAM_OFFSET,PARAM_BYTES_PER_THREAD,PARAM_ELMT_SIZE,PARAM_DMA_THREADS,PARAM_NUM_ELMTS>(false/*single*/,true/*qualified*/,1);
  if (!result) return result;
  fprintf(stdout,"        Templates-2");
  result = run_experiment<true,PARAM_ALIGNMENT,PARAM_OFFSET,PARAM_BYTES_PER_THREAD,PARAM_ELMT_SIZE,PARAM_DMA_THREADS,PARAM_NUM_ELMTS>(false/*single*/,true/*qualified*/,2);
  if (!result) return result;
  fprintf(stdout,"        Templates-3");
  result = run_experiment<true,PARAM_ALIGNMENT,PARAM_OFFSET,PARAM_BYTES_PER_THREAD,PARAM_ELMT_SIZE,PARAM_DMA_THREADS,PARAM_NUM_ELMTS>(false/*single*/,true/*qualified*/,3);
  if (!result) return result;
  fprintf(stdout,"        Templates-4");
  result = run_experiment<true,PARAM_ALIGNMENT,PARAM_OFFSET,PARAM_BYTES_PER_THREAD,PARAM_ELMT_SIZE,PARAM_DMA_THREADS,PARAM_NUM_ELMTS>(false/*single*/,true/*qualified*/,4);
  if (!result) return result;
  fprintf(stdout,"  Non-Warp-Specialized Experiments\n");
  fprintf(stdout,"    Single-Phase Experiments\n");
  fprintf(stdout,"      Unqualified experiments\n");
  fprintf(stdout,"        Templates-1");
  result = run_experiment<false,PARAM_ALIGNMENT,PARAM_OFFSET,PARAM_BYTES_PER_THREAD,PARAM_ELMT_SIZE,PARAM_DMA_THREADS,PARAM_NUM_ELMTS>(true/*single*/,false/*qualified*/,1);
  if (!result) return result;
  fprintf(stdout,"        Templates-2");
  result = run_experiment<false,PARAM_ALIGNMENT,PARAM_OFFSET,PARAM_BYTES_PER_THREAD,PARAM_ELMT_SIZE,PARAM_DMA_THREADS,PARAM_NUM_ELMTS>(true/*single*/,false/*qualified*/,2);
  if (!result) return result;
  fprintf(stdout,"        Templates-3");
  result = run_experiment<false,PARAM_ALIGNMENT,PARAM_OFFSET,PARAM_BYTES_PER_THREAD,PARAM_ELMT_SIZE,PARAM_DMA_THREADS,PARAM_NUM_ELMTS>(true/*single*/,false/*qualified*/,3);
  if (!result) return result;
  fprintf(stdout,"        Templates-4");
  result = run_experiment<false,PARAM_ALIGNMENT,PARAM_OFFSET,PARAM_BYTES_PER_THREAD,PARAM_ELMT_SIZE,PARAM_DMA_THREADS,PARAM_NUM_ELMTS>(true/*single*/,false/*qualified*/,4);
  if (!result) return result;
  fprintf(stdout,"      Qualified Experiments\n");
  fprintf(stdout,"        Templates-1");
  result = run_experiment<false,PARAM_ALIGNMENT,PARAM_OFFSET,PARAM_BYTES_PER_THREAD,PARAM_ELMT_SIZE,PARAM_DMA_THREADS,PARAM_NUM_ELMTS>(true/*single*/,true/*qualified*/,1);
  if (!result) return result;
  fprintf(stdout,"        Templates-2");
  result = run_experiment<false,PARAM_ALIGNMENT,PARAM_OFFSET,PARAM_BYTES_PER_THREAD,PARAM_ELMT_SIZE,PARAM_DMA_THREADS,PARAM_NUM_ELMTS>(true/*single*/,true/*qualified*/,2);
  if (!result) return result;
  fprintf(stdout,"        Templates-3");
  result = run_experiment<false,PARAM_ALIGNMENT,PARAM_OFFSET,PARAM_BYTES_PER_THREAD,PARAM_ELMT_SIZE,PARAM_DMA_THREADS,PARAM_NUM_ELMTS>(true/*single*/,true/*qualified*/,3);
  if (!result) return result;
  fprintf(stdout,"        Templates-4");
  result = run_experiment<false,PARAM_ALIGNMENT,PARAM_OFFSET,PARAM_BYTES_PER_THREAD,PARAM_ELMT_SIZE,PARAM_DMA_THREADS,PARAM_NUM_ELMTS>(true/*single*/,true/*qualified*/,4);
  if (!result) return result;
  fprintf(stdout,"    Two-Phase Experiments\n");
  fprintf(stdout,"      Unqualified experiments\n");
  fprintf(stdout,"        Templates-1");
  result = run_experiment<false,PARAM_ALIGNMENT,PARAM_OFFSET,PARAM_BYTES_PER_THREAD,PARAM_ELMT_SIZE,PARAM_DMA_THREADS,PARAM_NUM_ELMTS>(false/*single*/,false/*qualified*/,1);
  if (!result) return result;
  fprintf(stdout,"        Templates-2");
  result = run_experiment<false,PARAM_ALIGNMENT,PARAM_OFFSET,PARAM_BYTES_PER_THREAD,PARAM_ELMT_SIZE,PARAM_DMA_THREADS,PARAM_NUM_ELMTS>(false/*single*/,false/*qualified*/,2);
  if (!result) return result;
  fprintf(stdout,"        Templates-3");
  result = run_experiment<false,PARAM_ALIGNMENT,PARAM_OFFSET,PARAM_BYTES_PER_THREAD,PARAM_ELMT_SIZE,PARAM_DMA_THREADS,PARAM_NUM_ELMTS>(false/*single*/,false/*qualified*/,3);
  if (!result) return result;
  fprintf(stdout,"        Templates-4");
  result = run_experiment<false,PARAM_ALIGNMENT,PARAM_OFFSET,PARAM_BYTES_PER_THREAD,PARAM_ELMT_SIZE,PARAM_DMA_THREADS,PARAM_NUM_ELMTS>(false/*single*/,false/*qualified*/,4);
  if (!result) return result;
  fprintf(stdout,"      Qualified Experiments\n");
  fprintf(stdout,"        Templates-1");
  result = run_experiment<false,PARAM_ALIGNMENT,PARAM_OFFSET,PARAM_BYTES_PER_THREAD,PARAM_ELMT_SIZE,PARAM_DMA_THREADS,PARAM_NUM_ELMTS>(false/*single*/,true/*qualified*/,1);
  if (!result) return result;
  fprintf(stdout,"        Templates-2");
  result = run_experiment<false,PARAM_ALIGNMENT,PARAM_OFFSET,PARAM_BYTES_PER_THREAD,PARAM_ELMT_SIZE,PARAM_DMA_THREADS,PARAM_NUM_ELMTS>(false/*single*/,true/*qualified*/,2);
  if (!result) return result;
  fprintf(stdout,"        Templates-3");
  result = run_experiment<false,PARAM_ALIGNMENT,PARAM_OFFSET,PARAM_BYTES_PER_THREAD,PARAM_ELMT_SIZE,PARAM_DMA_THREADS,PARAM_NUM_ELMTS>(false/*single*/,true/*qualified*/,3);
  if (!result) return result;
  fprintf(stdout,"        Templates-4");
  result = run_experiment<false,PARAM_ALIGNMENT,PARAM_OFFSET,PARAM_BYTES_PER_THREAD,PARAM_ELMT_SIZE,PARAM_DMA_THREADS,PARAM_NUM_ELMTS>(false/*single*/,true/*qualified*/,4);
  if (!result) return result;
  fflush(stdout);

  free(h_offsets);

  return result;
}
