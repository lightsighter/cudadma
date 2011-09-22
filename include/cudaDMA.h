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

#pragma once

__device__ __forceinline__ void ptx_cudaDMA_barrier_blocking (const int name, const int num_barriers)
{
  asm volatile("bar.sync %0, %1;" : : "r"(name), "r"(num_barriers) : "memory" );
}

__device__ __forceinline__ void ptx_cudaDMA_barrier_nonblocking (const int name, const int num_barriers)
{
  asm volatile("bar.arrive %0, %1;" : : "r"(name), "r"(num_barriers) : "memory" );
}

#define CUDADMA_BASE cudaDMA
#define MAX_BYTES_OUTSTANDING_PER_THREAD (4*ALIGNMENT) 
#define MAX_LDS_OUTSTANDING_PER_THREAD 4
#define CUDADMA_DMA_TID (threadIdx.x-dma_threadIdx_start)
#define WARP_SIZE 32

//#define CUDADMA_DEBUG_ON
//////////////////////////////////////////////////////////////////////////////////////////////////
// Base class - define common variables and functions:
//////////////////////////////////////////////////////////////////////////////////////////////////
class cudaDMA {

 protected:

  // Synchronization related variables
  const bool is_dma_thread;
  const int barrierID_empty;
  const int barrierID_full;
  const int barrierSize;

  // DMA-thread Synchronization Functions:
  __device__ __forceinline__ void wait_for_dma_start() const
  {
    ptx_cudaDMA_barrier_blocking(barrierID_empty,barrierSize); 
  }
  __device__ __forceinline__ void finish_async_dma() const
  {
    ptx_cudaDMA_barrier_nonblocking(barrierID_full,barrierSize); 
  }

 public:

  const int dma_tid;
  const unsigned long int dma1_src_iter_offs; 
  const unsigned long int dma2_src_iter_offs; 
  const unsigned long int dma3_src_iter_offs; 
  const unsigned long int dma4_src_iter_offs; 
  const unsigned long int dma1_src_offs; 
  const unsigned long int dma2_src_offs; 
  const unsigned long int dma3_src_offs; 
  const unsigned long int dma4_src_offs;
  const unsigned long int dma1_dst_iter_offs; 
  const unsigned long int dma2_dst_iter_offs; 
  const unsigned long int dma3_dst_iter_offs; 
  const unsigned long int dma4_dst_iter_offs; 
  const unsigned long int dma1_dst_offs; 
  const unsigned long int dma2_dst_offs; 
  const unsigned long int dma3_dst_offs; 
  const unsigned long int dma4_dst_offs;

  // Constructor without destination offsets:
  __device__ cudaDMA (const int dmaID,
		      const int num_dma_threads,
		      const int num_compute_threads,
		      const int dma_threadIdx_start, 
                      const unsigned long int dma1_src_iter_offs, 
                      const unsigned long int dma2_src_iter_offs, 
                      const unsigned long int dma3_src_iter_offs, 
                      const unsigned long int dma4_src_iter_offs,
		      const unsigned long int dma1_src_offs, 
                      const unsigned long int dma2_src_offs, 
                      const unsigned long int dma3_src_offs, 
                      const unsigned long int dma4_src_offs
		      ) : 
    is_dma_thread ((threadIdx.x>=dma_threadIdx_start) && (threadIdx.x<(dma_threadIdx_start+num_dma_threads))),
    dma_tid (CUDADMA_DMA_TID),
    barrierID_empty ((dmaID<<1)+1),
    barrierID_full (dmaID<<1),
    barrierSize (num_dma_threads+num_compute_threads),
    dma1_src_iter_offs (dma1_src_iter_offs),
    dma2_src_iter_offs (dma2_src_iter_offs),
    dma3_src_iter_offs (dma3_src_iter_offs),
    dma4_src_iter_offs (dma4_src_iter_offs),
    dma1_src_offs (dma1_src_offs),
    dma2_src_offs (dma2_src_offs),
    dma3_src_offs (dma3_src_offs),
    dma4_src_offs (dma4_src_offs),
    dma1_dst_iter_offs (0),
    dma2_dst_iter_offs (0),
    dma3_dst_iter_offs (0),
    dma4_dst_iter_offs (0),
    dma1_dst_offs (0),
    dma2_dst_offs (0),
    dma3_dst_offs (0),
    dma4_dst_offs (0)
      {
      }

  // Constructor with destination offsets:
  __device__ cudaDMA (const int dmaID,
		      const int num_dma_threads,
		      const int num_compute_threads,
		      const int dma_threadIdx_start, 
                      const unsigned long int dma1_src_iter_offs, 
                      const unsigned long int dma2_src_iter_offs, 
                      const unsigned long int dma3_src_iter_offs, 
                      const unsigned long int dma4_src_iter_offs,
		      const unsigned long int dma1_src_offs, 
                      const unsigned long int dma2_src_offs, 
                      const unsigned long int dma3_src_offs, 
                      const unsigned long int dma4_src_offs,
                      const unsigned long int dma1_dst_iter_offs, 
                      const unsigned long int dma2_dst_iter_offs, 
                      const unsigned long int dma3_dst_iter_offs, 
                      const unsigned long int dma4_dst_iter_offs,
		      const unsigned long int dma1_dst_offs, 
                      const unsigned long int dma2_dst_offs, 
                      const unsigned long int dma3_dst_offs, 
                      const unsigned long int dma4_dst_offs
		      ) : 
    is_dma_thread ((threadIdx.x>=dma_threadIdx_start) && (threadIdx.x<(dma_threadIdx_start+num_dma_threads))),
    dma_tid (CUDADMA_DMA_TID),
    barrierID_empty ((dmaID<<1)+1),
    barrierID_full (dmaID<<1),
    barrierSize (num_dma_threads+num_compute_threads),
    dma1_src_iter_offs (dma1_src_iter_offs),
    dma2_src_iter_offs (dma2_src_iter_offs),
    dma3_src_iter_offs (dma3_src_iter_offs),
    dma4_src_iter_offs (dma4_src_iter_offs),
    dma1_src_offs (dma1_src_offs),
    dma2_src_offs (dma2_src_offs),
    dma3_src_offs (dma3_src_offs),
    dma4_src_offs (dma4_src_offs),
    dma1_dst_iter_offs (dma1_dst_iter_offs),
    dma2_dst_iter_offs (dma2_dst_iter_offs),
    dma3_dst_iter_offs (dma3_dst_iter_offs),
    dma4_dst_iter_offs (dma4_dst_iter_offs),
    dma1_dst_offs (dma1_dst_offs),
    dma2_dst_offs (dma2_dst_offs),
    dma3_dst_offs (dma3_dst_offs),
    dma4_dst_offs (dma4_dst_offs)
      {
      }
    
  // Compute-thread Synchronization Functions:
  __device__ __forceinline__ void start_async_dma() const
  {
    ptx_cudaDMA_barrier_nonblocking(barrierID_empty,barrierSize); 
  }
  __device__ __forceinline__ void wait_for_dma_finish() const
  {
    ptx_cudaDMA_barrier_blocking(barrierID_full,barrierSize); 
  }
  
  // Intraspective Functions
  __device__ __forceinline__ bool owns_this_thread() const { return is_dma_thread; }

  // Transfer primitives used by more than one subclass
  template<bool DO_SYNC, int ALIGNMENT>
    __device__ __forceinline__ void do_xfer( void * src_ptr, void * dst_ptr, int xfer_size) const
  {
     switch (ALIGNMENT)
       {
          case 4:
	    {
	      do_xfer_alignment_04<DO_SYNC>(src_ptr,dst_ptr,xfer_size);
	      break;
	    }
          case 8:
            {
	      do_xfer_alignment_08<DO_SYNC>(src_ptr,dst_ptr,xfer_size);
	      break;
            }
          case 16:
            {
	      do_xfer_alignment_16<DO_SYNC>(src_ptr,dst_ptr,xfer_size);
	      break;
	    }
#ifdef CUDADMA_DEBUG_ON
          default:
            printf("Illegal alignment size (%d).  Must be one of (4,8,16).\n",ALIGNMENT);
            break;
#endif
       }
  }

  // Manage transfers only aligned to 4 bytes
  template<bool DO_SYNC>
   __device__ __forceinline__ void do_xfer_alignment_04( void * src_ptr, void * dst_ptr, int xfer_size) const
  {
    switch (xfer_size)
      {
        case 16:
          {
	    perform_four_xfers<float,float,DO_SYNC,true> (src_ptr,dst_ptr);
	    break;
          }
	case 12:
          {
            perform_three_xfers<float,float,DO_SYNC> (src_ptr,dst_ptr);
            break;
          }
	case 8:
          {
            perform_two_xfers<float,float,DO_SYNC> (src_ptr,dst_ptr);
	    break;
          }
	case 4:
          {
	    perform_one_xfer<float,DO_SYNC> (src_ptr,dst_ptr);
	    break;
          }
        case 0:
	 {
	   if (DO_SYNC) CUDADMA_BASE::wait_for_dma_start(); 
	   break;
	 }
#ifdef CUDADMA_DEBUG_ON
         default:
           printf("Invalid xfer size (%u) for dma_tid = %d\n",xfer_size, CUDADMA_BASE::dma_tid);
	   break;
#endif
      }
  }

  // Manage transfers aligned to 8 byte boundary
  template<bool DO_SYNC>
   __device__ __forceinline__ void do_xfer_alignment_08( void * src_ptr, void * dst_ptr, int xfer_size) const
  {
    switch (xfer_size)
      {
        case 32:
         {
           perform_four_xfers<float2,float2,DO_SYNC,true> (src_ptr,dst_ptr);  
           break;
         }
         case 28:
         {
           perform_four_xfers<float2,float,DO_SYNC,true> (src_ptr,dst_ptr);   
           break;
         }
         case 24:
         {
           perform_three_xfers<float2,float2,DO_SYNC> (src_ptr,dst_ptr);  
           break;
         }
         case 20:
         {
           perform_three_xfers<float2,float,DO_SYNC> (src_ptr,dst_ptr);  
           break;
         }
         case 16:
         {
           perform_two_xfers<float2,float2,DO_SYNC> (src_ptr,dst_ptr); 
           break;
         }
         case 12:
         {
           perform_two_xfers<float2,float,DO_SYNC> (src_ptr,dst_ptr); 
           break;
         }
         case 8:
         {
           perform_one_xfer<float2,DO_SYNC> (src_ptr,dst_ptr); 
           break;
         }
         case 4:
         {
           perform_one_xfer<float,DO_SYNC> (src_ptr,dst_ptr); 
           break;
         }
         case 0:
	 {
	   if (DO_SYNC) CUDADMA_BASE::wait_for_dma_start(); 
	   break;
	 }
#ifdef CUDADMA_DEBUG_ON
         default:
           printf("Invalid xfer size (%u) for dma_tid = %d\n",xfer_size, CUDADMA_BASE::dma_tid);
	   break;
#endif
      }
  }

  // Manage transfers aligned to 16 byte boundary
  template<bool DO_SYNC>
   __device__ __forceinline__ void do_xfer_alignment_16( void * src_ptr, void * dst_ptr, int xfer_size) const
  {
    switch (xfer_size) 
      {
      case 64: 
	{
	  perform_four_xfers<float4,float4,DO_SYNC,true> (src_ptr,dst_ptr);
	  break;
	}
      case 60: 
	{
	  perform_four_xfers<float4,float3,DO_SYNC,true> (src_ptr,dst_ptr);
	  break;
	}
      case 56: 
	{
	  perform_four_xfers<float4,float2,DO_SYNC,true> (src_ptr,dst_ptr);
	  break;
	}
      case 52:
	{
	  perform_four_xfers<float4,float,DO_SYNC,true> (src_ptr,dst_ptr);
	  break;
	}
      case 48:
	{
	  perform_three_xfers<float4,float4,DO_SYNC> (src_ptr,dst_ptr);
	  break;
	}
      case 44:
	{
	  perform_three_xfers<float4,float3,DO_SYNC> (src_ptr,dst_ptr);
	  break;
	}
      case 40:
	{
	  perform_three_xfers<float4,float2,DO_SYNC> (src_ptr,dst_ptr);
	  break;
	}
      case 36:
	{
	  perform_three_xfers<float4,float,DO_SYNC> (src_ptr,dst_ptr);
	  break;
	}
      case 32:
	{
	  perform_two_xfers<float4,float4,DO_SYNC> (src_ptr,dst_ptr);
	  break;
	}
      case 28:
	{
	  perform_two_xfers<float4,float3,DO_SYNC> (src_ptr,dst_ptr);
	  break;
	}
      case 24:
	{
	  perform_two_xfers<float4,float2,DO_SYNC> (src_ptr,dst_ptr);
	  break;
	}
      case 20:
	{
	  perform_two_xfers<float4,float,DO_SYNC> (src_ptr,dst_ptr);
	  break;
	}
      case 16:
	{
	  perform_one_xfer<float4,DO_SYNC> (src_ptr,dst_ptr);
	  break;
	}
      case 12:
	{
	  perform_one_xfer<float3,DO_SYNC> (src_ptr,dst_ptr);
	  break;
	}
      case 8:
	{
	  perform_one_xfer<float2,DO_SYNC> (src_ptr,dst_ptr);
	  break;
	}
      case 4:
	{
	  perform_one_xfer<float,DO_SYNC> (src_ptr,dst_ptr);
	  break;
	}
      case 0:
	{
	  if (DO_SYNC) CUDADMA_BASE::wait_for_dma_start(); 
	  break;
	}
#ifdef CUDADMA_DEBUG_ON
      default:
	printf("Invalid xfer size (%u) for dma_tid = %d\n",xfer_size, CUDADMA_BASE::dma_tid);
	break;
#endif
      }
  }

  template<bool DO_SYNC>
  __device__ __forceinline__ void do_xfer_across(void * src_ptr, void * dst_ptr, int total_lds, int xfer_size) const
  {
	switch (xfer_size)
	  {
	    case 0:
	      {
		if (DO_SYNC) CUDADMA_BASE::wait_for_dma_start();
		break;
	      }	
	    case 4:
	      {
		perform_xfer_across<float,DO_SYNC>(src_ptr,dst_ptr,total_lds);
		break;
	      }
	    case 8:
	      {
		perform_xfer_across<float2,DO_SYNC>(src_ptr,dst_ptr,total_lds);
		break;
	      }
	    case 12:
	      {
		perform_xfer_across<float3,DO_SYNC>(src_ptr,dst_ptr,total_lds);
		break;
	      }
	    case 16:
	      {
		perform_xfer_across<float4,DO_SYNC>(src_ptr,dst_ptr,total_lds);
		break;
	      }
#ifdef CUDADMA_DEBUG_ON
	    default:
		printf("Invalid xfer size (%d) for xfer across.\n",xfer_size);
		break;
#endif
	  }
  }

  template<typename TYPE, bool DO_SYNC>
  __device__ __forceinline__ void perform_xfer_across(void * src_ptr, void * dst_ptr, int total_lds) const
  {
	switch (total_lds)
	  {
	    case 0:
	      {
		if (DO_SYNC) CUDADMA_BASE::wait_for_dma_start();
		break;
	      }
	    case 1:
	      {
		perform_one_xfer<TYPE,DO_SYNC>(src_ptr,dst_ptr);
		break;
	      }
	    case 2:
	      {
		perform_two_xfers<TYPE,TYPE,DO_SYNC>(src_ptr,dst_ptr);
		break;
	      }
	    case 3:
	      {
		perform_three_xfers<TYPE,TYPE,DO_SYNC>(src_ptr,dst_ptr);
		break;
	      }
	    case 4:
	      {
		perform_four_xfers<TYPE,TYPE,DO_SYNC,false>(src_ptr,dst_ptr);
		break;
	      }
#ifdef CUDADMA_DEBUG_ON
	    default:
		printf("Invalid number of total loads for do across final %d.\n",total_lds);
		break;
#endif
	  }
  }

  /*
   * These functions are used to emit vector loads of the appropriate size at
   * the predefined offsets.
   */
  template<typename TYPE1, bool DO_SYNC>
    __device__ __forceinline__ void perform_one_xfer(void *src_ptr, void *dst_ptr) const
  {
    TYPE1 tmp1 = *(TYPE1 *)((char *)src_ptr + dma1_src_offs);
    if (DO_SYNC) CUDADMA_BASE::wait_for_dma_start();
    *(TYPE1 *)((char *)dst_ptr+dma1_dst_offs) = tmp1;
  }
  template<typename TYPE1, typename TYPE2, bool DO_SYNC>
    __device__ __forceinline__ void perform_two_xfers(void *src_ptr, void *dst_ptr) const
  {
    TYPE1 tmp1 = *(TYPE1 *)((char *)src_ptr + dma1_src_offs);
    TYPE2 tmp2 = *(TYPE2 *)((char *)src_ptr + dma2_src_offs);
    if (DO_SYNC) CUDADMA_BASE::wait_for_dma_start();
    *(TYPE1 *)((char *)dst_ptr+dma1_dst_offs) = tmp1;
    *(TYPE2 *)((char *)dst_ptr+dma2_dst_offs) = tmp2;
  }
  template<typename TYPE1, typename TYPE2, bool DO_SYNC>
    __device__ __forceinline__ void perform_three_xfers(void *src_ptr, void *dst_ptr) const
  {
    TYPE1 tmp1 = *(TYPE1 *)((char *)src_ptr + dma1_src_offs);
    TYPE1 tmp2 = *(TYPE1 *)((char *)src_ptr + dma2_src_offs);
    TYPE2 tmp3 = *(TYPE2 *)((char *)src_ptr + dma3_src_offs);
    if (DO_SYNC) CUDADMA_BASE::wait_for_dma_start();
    *(TYPE1 *)((char *)dst_ptr+dma1_dst_offs) = tmp1;
    *(TYPE1 *)((char *)dst_ptr+dma2_dst_offs) = tmp2;
    *(TYPE2 *)((char *)dst_ptr+dma3_dst_offs) = tmp3;
  }
  template <typename TYPE1, typename TYPE2, bool DO_SYNC, bool LAST_XFER>
    __device__ __forceinline__ void perform_four_xfers(void *src_ptr, void *dst_ptr) const
  {
    TYPE1 tmp1 = *(TYPE1 *)((char *)src_ptr + (LAST_XFER ? dma1_src_offs : dma1_src_iter_offs));
    TYPE1 tmp2 = *(TYPE1 *)((char *)src_ptr + (LAST_XFER ? dma2_src_offs : dma2_src_iter_offs));
    TYPE1 tmp3 = *(TYPE1 *)((char *)src_ptr + (LAST_XFER ? dma3_src_offs : dma3_src_iter_offs));
    TYPE2 tmp4 = *(TYPE2 *)((char *)src_ptr + (LAST_XFER ? dma4_src_offs : dma4_src_iter_offs));
    if (DO_SYNC) CUDADMA_BASE::wait_for_dma_start();
    *(TYPE1 *)((char *)dst_ptr+(LAST_XFER ? dma1_dst_offs : dma1_dst_iter_offs)) = tmp1;
    *(TYPE1 *)((char *)dst_ptr+(LAST_XFER ? dma2_dst_offs : dma2_dst_iter_offs)) = tmp2;
    *(TYPE1 *)((char *)dst_ptr+(LAST_XFER ? dma3_dst_offs : dma3_dst_iter_offs)) = tmp3;
    *(TYPE2 *)((char *)dst_ptr+(LAST_XFER ? dma4_dst_offs : dma4_dst_iter_offs)) = tmp4;
  }
};

//////////////////////////////////////////////////////////////////////////////////////////////////
// CudaDMASequential
//////////////////////////////////////////////////////////////////////////////////////////////////

#define CUDADMASEQUENTIAL_DMA_ITERS (BYTES_PER_THREAD-4)/MAX_BYTES_OUTSTANDING_PER_THREAD
// All of these values below will be used as byte address offsets:
#define CUDADMASEQUENTIAL_DMA_ITER_INC MAX_BYTES_OUTSTANDING_PER_THREAD*num_dma_threads
#define CUDADMASEQUENTIAL_DMA1_ITER_OFFS 1*ALIGNMENT*CUDADMA_DMA_TID
#define CUDADMASEQUENTIAL_DMA2_ITER_OFFS 1*ALIGNMENT*num_dma_threads+ALIGNMENT*CUDADMA_DMA_TID
#define CUDADMASEQUENTIAL_DMA3_ITER_OFFS 2*ALIGNMENT*num_dma_threads+ALIGNMENT*CUDADMA_DMA_TID
#define CUDADMASEQUENTIAL_DMA4_ITER_OFFS 3*ALIGNMENT*num_dma_threads+ALIGNMENT*CUDADMA_DMA_TID
#define CUDADMASEQUENTIAL_DMA1_SPECIAL_OFFS \
  (((BYTES_PER_THREAD%MAX_BYTES_OUTSTANDING_PER_THREAD)<(1*ALIGNMENT))&&((BYTES_PER_THREAD%MAX_BYTES_OUTSTANDING_PER_THREAD)!=0)) ? \
  (BYTES_PER_THREAD%ALIGNMENT)*CUDADMA_DMA_TID : \
  ALIGNMENT*CUDADMA_DMA_TID
#define CUDADMASEQUENTIAL_DMA2_SPECIAL_OFFS \
  (((BYTES_PER_THREAD%MAX_BYTES_OUTSTANDING_PER_THREAD)<(2*ALIGNMENT))&&((BYTES_PER_THREAD%MAX_BYTES_OUTSTANDING_PER_THREAD)!=0)) ?		\
  (ALIGNMENT*num_dma_threads+(BYTES_PER_THREAD%ALIGNMENT)*CUDADMA_DMA_TID) :	\
    (ALIGNMENT*num_dma_threads+ALIGNMENT*CUDADMA_DMA_TID)
#define CUDADMASEQUENTIAL_DMA3_SPECIAL_OFFS \
  (((BYTES_PER_THREAD%MAX_BYTES_OUTSTANDING_PER_THREAD)<(3*ALIGNMENT))&&((BYTES_PER_THREAD%MAX_BYTES_OUTSTANDING_PER_THREAD)!=0)) ? \
  (2*ALIGNMENT*num_dma_threads+(BYTES_PER_THREAD%ALIGNMENT)*CUDADMA_DMA_TID) : \
    (2*ALIGNMENT*num_dma_threads+ALIGNMENT*CUDADMA_DMA_TID) 
#define CUDADMASEQUENTIAL_DMA4_SPECIAL_OFFS \
  ((BYTES_PER_THREAD%MAX_BYTES_OUTSTANDING_PER_THREAD)!=0) ? \
  (3*ALIGNMENT*num_dma_threads+(BYTES_PER_THREAD%ALIGNMENT)*CUDADMA_DMA_TID) : \
    (3*ALIGNMENT*num_dma_threads+ALIGNMENT*CUDADMA_DMA_TID)
#define CUDADMASEQUENTIAL_DMA1_OFFS (ALIGNMENT*CUDADMA_DMA_TID)
#define CUDADMASEQUENTIAL_DMA2_OFFS (1*ALIGNMENT*num_dma_threads + ALIGNMENT*CUDADMA_DMA_TID)
#define CUDADMASEQUENTIAL_DMA3_OFFS (2*ALIGNMENT*num_dma_threads + ALIGNMENT*CUDADMA_DMA_TID)
#define CUDADMASEQUENTIAL_DMA4_OFFS (3*ALIGNMENT*num_dma_threads + ALIGNMENT*CUDADMA_DMA_TID)
	      
template <int BYTES_PER_THREAD, int ALIGNMENT>
class cudaDMASequential : public CUDADMA_BASE {

 public:

  // DMA Addressing variables
  const unsigned int dma_iters;
  const unsigned int dma_iter_inc;  // Precomputed offset for next copy iteration
  const bool all_threads_active; // If true, then we know that all threads are guaranteed to be active (needed for sync/divergence functionality guarantee)
  bool is_active_thread;   // If true, then all of BYTES_PER_THREAD will be transferred for this thread
  bool is_partial_thread;  // If true, then only some of BYTES_PER_THREAD will be transferred for this thread
  int partial_thread_bytes;

  // Constructor for when (sz = BYTES_PER_THREAD * num_dma_threads)
  __device__ cudaDMASequential (const int dmaID,
				const int num_dma_threads,
				const int num_compute_threads,
				const int dma_threadIdx_start)
    : CUDADMA_BASE (dmaID,
		    num_dma_threads,
		    num_compute_threads,
		    dma_threadIdx_start, 
                    CUDADMASEQUENTIAL_DMA1_ITER_OFFS,
                    CUDADMASEQUENTIAL_DMA2_ITER_OFFS,
                    CUDADMASEQUENTIAL_DMA3_ITER_OFFS,
                    CUDADMASEQUENTIAL_DMA4_ITER_OFFS,
                    CUDADMASEQUENTIAL_DMA1_SPECIAL_OFFS,
                    CUDADMASEQUENTIAL_DMA2_SPECIAL_OFFS,
                    CUDADMASEQUENTIAL_DMA3_SPECIAL_OFFS,
                    CUDADMASEQUENTIAL_DMA4_SPECIAL_OFFS,
                    CUDADMASEQUENTIAL_DMA1_ITER_OFFS,
                    CUDADMASEQUENTIAL_DMA2_ITER_OFFS,
                    CUDADMASEQUENTIAL_DMA3_ITER_OFFS,
                    CUDADMASEQUENTIAL_DMA4_ITER_OFFS,
                    CUDADMASEQUENTIAL_DMA1_SPECIAL_OFFS,
                    CUDADMASEQUENTIAL_DMA2_SPECIAL_OFFS,
                    CUDADMASEQUENTIAL_DMA3_SPECIAL_OFFS,
                    CUDADMASEQUENTIAL_DMA4_SPECIAL_OFFS
                   ),
    dma_iters (CUDADMASEQUENTIAL_DMA_ITERS),
    dma_iter_inc (CUDADMASEQUENTIAL_DMA_ITER_INC),
    all_threads_active (true)
    {
      is_active_thread = true;
      is_partial_thread = false;
      partial_thread_bytes = 0;
    }

  // Constructor for when (sz <= BYTES_PER_THREAD * num_dma_threads)
  __device__ cudaDMASequential (const int dmaID,
				const int num_dma_threads,
				const int num_compute_threads,
				const int dma_threadIdx_start, 
				const int sz)
    : CUDADMA_BASE (dmaID,
		    num_dma_threads,
		    num_compute_threads,
		    dma_threadIdx_start, 
                    CUDADMASEQUENTIAL_DMA1_ITER_OFFS,
                    CUDADMASEQUENTIAL_DMA2_ITER_OFFS,
                    CUDADMASEQUENTIAL_DMA3_ITER_OFFS,
                    CUDADMASEQUENTIAL_DMA4_ITER_OFFS,
                    CUDADMASEQUENTIAL_DMA1_OFFS,
                    CUDADMASEQUENTIAL_DMA2_OFFS,
                    CUDADMASEQUENTIAL_DMA3_OFFS,
                    CUDADMASEQUENTIAL_DMA4_OFFS,
                    CUDADMASEQUENTIAL_DMA1_ITER_OFFS,
                    CUDADMASEQUENTIAL_DMA2_ITER_OFFS,
                    CUDADMASEQUENTIAL_DMA3_ITER_OFFS,
                    CUDADMASEQUENTIAL_DMA4_ITER_OFFS,
                    CUDADMASEQUENTIAL_DMA1_OFFS,
                    CUDADMASEQUENTIAL_DMA2_OFFS,
                    CUDADMASEQUENTIAL_DMA3_OFFS,
                    CUDADMASEQUENTIAL_DMA4_OFFS
                   ),
    dma_iters ( (sz-4)/(MAX_BYTES_OUTSTANDING_PER_THREAD*num_dma_threads) ),
    dma_iter_inc (CUDADMASEQUENTIAL_DMA_ITER_INC),
    //all_threads_active (sz==(BYTES_PER_THREAD*num_dma_threads))
    all_threads_active ((sz % (ALIGNMENT*num_dma_threads))==0)
    {

      // Do a bunch of arithmetic based on total size of the xfer:
      int num_vec4_loads = sz / (ALIGNMENT*num_dma_threads);
      int leftover_bytes = sz % (ALIGNMENT*num_dma_threads);

#ifdef CUDADMA_DEBUG_ON
      if ((CUDADMA_DMA_TID==1)&&(CUDADMA_BASE::barrierID_full==2)&&(CUDADMA_BASE::is_dma_thread)) {
	printf("leftover_bytes = %d\n",leftover_bytes);
	printf("num_vec4_loads = %d\n",num_vec4_loads);
      }
#endif

      // After computing leftover_bytes, figure out the cutoff point in dma_tid:
      // Note, all threads are either active, partial, or inactive
      if (leftover_bytes==0) {
	// Transfer is perfectly divisible by 16 bytes...only have to worry about not using all of BYTES_PER_THREAD
	partial_thread_bytes = num_vec4_loads*ALIGNMENT; 
	is_partial_thread = (partial_thread_bytes!=BYTES_PER_THREAD);
	is_active_thread = (partial_thread_bytes==BYTES_PER_THREAD);
      } else {
	// Threads below partial thread dma_tid will do 16-byte (or BYTES_PER_THREAD leftover) xfers, above should be inactive
	//int max_thread_bytes = min(ALIGNMENT,BYTES_PER_THREAD-(num_vec4_loads*ALIGNMENT));
	int max_thread_bytes = ALIGNMENT;
#ifdef CUDADMA_DEBUG_ON
	if ((CUDADMA_DMA_TID==1)&&(CUDADMA_BASE::barrierID_full==2)&&(CUDADMA_BASE::is_dma_thread)) {
	  printf("max_thread_bytes = %d\n",max_thread_bytes);
	}
#endif
	if (leftover_bytes>=(max_thread_bytes*(CUDADMA_DMA_TID+1))) {
	  // Below:  Do 16-byte xfers
	  partial_thread_bytes = (num_vec4_loads)*ALIGNMENT + max_thread_bytes;
	  is_partial_thread = (partial_thread_bytes!=BYTES_PER_THREAD);
	  is_active_thread = (partial_thread_bytes==BYTES_PER_THREAD);
	} else if (leftover_bytes<(max_thread_bytes*CUDADMA_DMA_TID)) {
	  // Above:  Do 0-byte xfers on last vec_load, do max_bytes_per_thread xfer on all previous loads
	  is_active_thread = false;
	  partial_thread_bytes = (num_vec4_loads-(dma_iters*MAX_LDS_OUTSTANDING_PER_THREAD))*ALIGNMENT;
	  is_partial_thread = (num_vec4_loads!=0);
	} else {
	  // Do less than max_thread_bytes on last vec_load in this thread, do max_bytes_per_thread xfer on all previous loads
	  partial_thread_bytes = (num_vec4_loads-(dma_iters*MAX_LDS_OUTSTANDING_PER_THREAD))*ALIGNMENT + (leftover_bytes%max_thread_bytes);
	  //partial_thread_bytes = (num_vec4_loads*ALIGNMENT) + (leftover_bytes%max_thread_bytes);
	  is_partial_thread = true;
	  is_active_thread = false;
	}
      }
      
#ifdef CUDADMA_DEBUG_ON
      if ((CUDADMA_BASE::barrierID_full==2)&&(CUDADMA_BASE::is_dma_thread)) {
	if (is_partial_thread==true) {
	  printf("PARTIAL THREAD:  tid = %d  dma_tid = %d,\tpartial_thread_bytes=%d\n",threadIdx.x, CUDADMA_DMA_TID,partial_thread_bytes);
	} else if (is_active_thread==false) {
	  printf("INACTIVE THREAD: tid = %d  dma_tid = %d\n",threadIdx.x, CUDADMA_DMA_TID);
	} else {
	  printf("ACTIVE THREAD:  tid = %d  dma_tid = %d,\tactive_thread_bytes=%d\n",threadIdx.x, CUDADMA_DMA_TID,BYTES_PER_THREAD);
	}
      }
#endif
    }
  
  // DMA-thread Data Transfer Functions:
    __device__ __forceinline__ void execute_dma ( void * src_ptr, void * dst_ptr ) const
  {

#ifdef CUDADMA_DEBUG_ON
      /*
      if ((CUDADMA_BASE::dma_tid==0)&&(CUDADMA_BASE::barrierID_full==2)&&(CUDADMA_BASE::is_dma_thread)) {
	printf("dma_tid = %d\nnum_dma_threads = %d\nbytes_per_thread = %d\ndma_iters = %d\ndma_iter_inc = %d\ndma1_src_offs = %lu\ndma2_offs = %lu\ndma3_offs = %lu\ndma4_offs = %lu\ndma1_iter_offs = %lu\ndma2_iter_offs = %lu\ndma3_iter_offs = %lu\ndma4_iter_offs = %lu\n", CUDADMA_BASE::dma_tid, dma_iter_inc/MAX_BYTES_OUTSTANDING_PER_THREAD, BYTES_PER_THREAD, dma_iters, dma1_src_offs, dma1_src_offs, dma2_src_offs, dma3_src_offs, dma4_src_offs, dma1_src_iter_offs, dma2_src_iter_offs, dma3_src_iter_offs, dma4_src_iter_offs);
      }
	*/
#endif

    int this_thread_bytes = is_active_thread ? BYTES_PER_THREAD : is_partial_thread ? partial_thread_bytes : 0;
    if ((dma_iters>0) || (!all_threads_active)) {
      CUDADMA_BASE::wait_for_dma_start(); 
    }
    // Slightly less optimized case
    char * src_temp = (char *)src_ptr;
    char * dst_temp = (char *)dst_ptr;
    switch (ALIGNMENT)
      {
	case 4:
	  {
	    for(int i = 0; i < dma_iters; i++) {
	      CUDADMA_BASE::template perform_four_xfers<float,float,false,false> (src_temp,dst_temp);
	      src_temp += dma_iter_inc;
	      dst_temp += dma_iter_inc;
	    }
	    break;
	  }
        case 8:
          {
            for(int i = 0; i < dma_iters ; i++) {
	      CUDADMA_BASE::template perform_four_xfers<float2,float2,false,false> (src_temp,dst_temp);
	      src_temp += dma_iter_inc;
	      dst_temp += dma_iter_inc;
	    }
	    break;
          }
        case 16:
          {
            for(int i = 0; i < dma_iters ; i++) {
              CUDADMA_BASE::template perform_four_xfers<float4,float4,false,false> (src_temp,dst_temp);
              src_temp += dma_iter_inc;
              dst_temp += dma_iter_inc;
            }
            break;
          }
#ifdef CUDADMA_DEBUG_ON
        default:
          printf("ALIGNMENT must be one of (4,8,16)\n");
          break;
#endif
      }
    // Handle the leftovers
    if (all_threads_active) {
      CUDADMA_BASE::template do_xfer<CUDADMASEQUENTIAL_DMA_ITERS==0, ALIGNMENT> ( src_temp, dst_temp, 
						(this_thread_bytes%MAX_BYTES_OUTSTANDING_PER_THREAD) ? 
						(this_thread_bytes%MAX_BYTES_OUTSTANDING_PER_THREAD) : 
						MAX_BYTES_OUTSTANDING_PER_THREAD );
    } else {
#ifdef CUDADMA_DEBUG_ON
      if ((CUDADMA_BASE::dma_tid==1)&&(CUDADMA_BASE::barrierID_full==4)&&(CUDADMA_BASE::is_dma_thread)) {
	printf("src1 addr = %x\n",((char *)src_temp + dma1_src_offs));
	printf("src2 addr = %x\n",((char *)src_temp + dma2_src_offs));
	printf("src3 addr = %x\n",((char *)src_temp + dma3_src_offs));
	printf("src4 addr = %x\n",((char *)src_temp + dma4_src_offs));
	printf("dst1 addr = %x\n",((char *)dst_temp + dma1_dst_offs));
	printf("dst2 addr = %x\n",((char *)dst_temp + dma2_dst_offs));
	printf("dst3 addr = %x\n",((char *)dst_temp + dma3_dst_offs));
	printf("dst4 addr = %x\n",((char *)dst_temp + dma4_dst_offs));
	printf("bytes = %d\n",
	       (this_thread_bytes%MAX_BYTES_OUTSTANDING_PER_THREAD) ? 
	       (this_thread_bytes%MAX_BYTES_OUTSTANDING_PER_THREAD) : 
	       (this_thread_bytes ? MAX_BYTES_OUTSTANDING_PER_THREAD : 0));
      }
#endif
      CUDADMA_BASE::template do_xfer<false, ALIGNMENT> ( src_temp, dst_temp, 
		       (this_thread_bytes%MAX_BYTES_OUTSTANDING_PER_THREAD) ? 
		       (this_thread_bytes%MAX_BYTES_OUTSTANDING_PER_THREAD) : 
		       (this_thread_bytes ? MAX_BYTES_OUTSTANDING_PER_THREAD : 0));
    }
    CUDADMA_BASE::finish_async_dma();
  }

  // Public DMA-thread Synchronization Functions:
  __device__ __forceinline__ void wait_for_dma_start() const
  {
    CUDADMA_BASE::wait_for_dma_start();
  }
  __device__ __forceinline__ void finish_async_dma() const
  {
    CUDADMA_BASE::finish_async_dma();
  }

};
#undef CUDADMASEQUENTIAL_DMA_ITERS
#undef CUDADMASEQUENTIAL_DMA_ITER_INC
#undef CUDADMASEQUENTIAL_DMA1_ITER_OFFS
#undef CUDADMASEQUENTIAL_DMA2_ITER_OFFS
#undef CUDADMASEQUENTIAL_DMA3_ITER_OFFS
#undef CUDADMASEQUENTIAL_DMA4_ITER_OFFS
#undef CUDADMASEQUENTIAL_DMA1_OFFS
#undef CUDADMASEQUENTIAL_DMA2_OFFS
#undef CUDADMASEQUENTIAL_DMA3_OFFS
#undef CUDADMASEQUENTIAL_DMA4_OFFS
#undef CUDADMASEQUENTIAL_DMA1_SPECIAL_OFFS
#undef CUDADMASEQUENTIAL_DMA2_SPECIAL_OFFS
#undef CUDADMASEQUENTIAL_DMA3_SPECIAL_OFFS
#undef CUDADMASEQUENTIAL_DMA4_SPECIAL_OFFS


//////////////////////////////////////////////////////////////////////////////////////////////////
// CudaDMAStrided
//////////////////////////////////////////////////////////////////////////////////////////////////

// Compute the number of loads required to load an element
#define LDS_PER_ELMT ((BYTES_PER_ELMT+ALIGNMENT-1)/ALIGNMENT)
#define LDS_PER_ELMT_PER_THREAD ((LDS_PER_ELMT+WARP_SIZE-1)/WARP_SIZE)
#define FULL_LDS_PER_ELMT (LDS_PER_ELMT/WARP_SIZE)
#define PARTIAL_LDS_PER_ELMT (((LDS_PER_ELMT%WARP_SIZE)+WARP_SIZE-1)/WARP_SIZE)
// The condition for when we split a warp across many elements
#define SPLIT_ELMT (LDS_PER_ELMT_PER_THREAD <= MAX_LDS_OUTSTANDING_PER_THREAD) 
// The condition for when the warp itself is split 
#define SPLIT_WARP (LDS_PER_ELMT <= (WARP_SIZE))
// In the case where we split a warp, figure out how many threads there are per elmt
#define THREADS_PER_ELMT (LDS_PER_ELMT > (WARP_SIZE/2) ? WARP_SIZE : \
			 LDS_PER_ELMT > (WARP_SIZE/4) ? (WARP_SIZE/2) : \
			 LDS_PER_ELMT > (WARP_SIZE/8) ? (WARP_SIZE/4) : \
			 LDS_PER_ELMT > (WARP_SIZE/16) ? (WARP_SIZE/8) : \
			 LDS_PER_ELMT > (WARP_SIZE/32) ? (WARP_SIZE/16) : WARP_SIZE/32)
#define DMA_COL_ITER_INC_SPLIT (SPLIT_WARP ? THREADS_PER_ELMT*ALIGNMENT : WARP_SIZE*ALIGNMENT)
#define ELMT_PER_STEP_SPLIT (SPLIT_WARP ? (DMA_THREADS/THREADS_PER_ELMT)*MAX_LDS_OUTSTANDING_PER_THREAD : (DMA_THREADS/WARP_SIZE)*MAX_LDS_OUTSTANDING_PER_THREAD)
#define ELMT_ID_SPLIT (SPLIT_WARP ? CUDADMA_DMA_TID/THREADS_PER_ELMT : CUDADMA_DMA_TID/WARP_SIZE)
#define REMAINING_ELMTS ((NUM_ELMTS==ELMT_PER_STEP_SPLIT) ? ELMT_PER_STEP_SPLIT : NUM_ELMTS%ELMT_PER_STEP_SPLIT) // Handle the optimized case special
#define PARTIAL_ELMTS (SPLIT_WARP ? REMAINING_ELMTS/(DMA_THREADS/THREADS_PER_ELMT) + (int(ELMT_ID_SPLIT) < (REMAINING_ELMTS%(DMA_THREADS/THREADS_PER_ELMT)) ? 1 : 0) : \
				REMAINING_ELMTS/(DMA_THREADS/WARP_SIZE) + (int(ELMT_ID_SPLIT) < (REMAINING_ELMTS%(DMA_THREADS/WARP_SIZE)) ? 1 : 0))

///////////////////////////////////////////////
// Now for the case where SPLIT_ELMT is false
///////////////////////////////////////////////
#define MAX_WARPS_PER_ELMT ((BYTES_PER_ELMT+(WARP_SIZE*MAX_BYTES_OUTSTANDING_PER_THREAD-1))/(WARP_SIZE*MAX_BYTES_OUTSTANDING_PER_THREAD))
// If we can use all the warps on one element, then do it,
// Otherwise check to see if we are wasting DMA warps due to low element count
// If so allocate more warps to a single element than is necessary to improve MLP
#define WARPS_PER_ELMT ((MAX_WARPS_PER_ELMT >= (DMA_THREADS/WARP_SIZE)) ? (DMA_THREADS/WARP_SIZE) : \
	(DMA_THREADS/WARP_SIZE)>(MAX_WARPS_PER_ELMT*NUM_ELMTS) ? (DMA_THREADS/WARP_SIZE)/NUM_ELMTS : MAX_WARPS_PER_ELMT)
#define ELMT_PER_STEP ((DMA_THREADS/WARP_SIZE)/WARPS_PER_ELMT)
#define ELMT_ID ((CUDADMA_DMA_TID/WARP_SIZE)/WARPS_PER_ELMT)
#define CUDADMA_WARP_TID (threadIdx.x - (dma_threadIdx_start + (ELMT_ID*WARPS_PER_ELMT*WARP_SIZE)))

#define CUDADMASTRIDED_DMA_OFF(stride,id) (SPLIT_ELMT ? \
	(id*(SPLIT_WARP ? DMA_THREADS/THREADS_PER_ELMT : DMA_THREADS/WARP_SIZE)*stride + ELMT_ID_SPLIT*stride) : \
	id*ALIGNMENT*WARPS_PER_ELMT*WARP_SIZE + ALIGNMENT*CUDADMA_WARP_TID)

#define ROW_ITERS_FULL ((NUM_ELMTS==ELMT_PER_STEP) ? 0 : NUM_ELMTS/ELMT_PER_STEP)
#define ROW_ITERS_SPLIT ((NUM_ELMTS==ELMT_PER_STEP_SPLIT) ? 0 : NUM_ELMTS/ELMT_PER_STEP_SPLIT) // Handle the optimized case special
#define COL_ITERS_FULL ((BYTES_PER_ELMT-4)/(MAX_BYTES_OUTSTANDING_PER_THREAD*WARPS_PER_ELMT*WARP_SIZE))
#define COL_ITERS_SPLIT (SPLIT_WARP ? 1 : (BYTES_PER_ELMT/(WARP_SIZE*ALIGNMENT)))

template<int BYTES_PER_ELMT, int NUM_ELMTS, int ALIGNMENT, int DMA_THREADS>
class cudaDMAStrided : public CUDADMA_BASE
{
private:
	const int dma_col_iter_inc;
	const int dma_src_row_iter_inc;
	const int dma_dst_row_iter_inc;
	const int dma_src_offset;
	const int dma_dst_offset;
	const bool warp_active;
	const bool warp_partial;
	const int dma_split_partial_elmts; // Handle the remaining elements at the end of a split
	int thread_bytes;
	int partial_bytes;
	bool all_threads_active;
public:
  // Constructor for when dst_stride == BYTES_PER_ELMT
  __device__ cudaDMAStrided (const int dmaID,
			     const int num_compute_threads,
			     const int dma_threadIdx_start,
			     const int el_stride)
	: CUDADMA_BASE (dmaID,
			DMA_THREADS,
			num_compute_threads,
			dma_threadIdx_start,
			CUDADMASTRIDED_DMA_OFF(el_stride,0),
			CUDADMASTRIDED_DMA_OFF(el_stride,1),
			CUDADMASTRIDED_DMA_OFF(el_stride,2),
			CUDADMASTRIDED_DMA_OFF(el_stride,3),
			CUDADMASTRIDED_DMA_OFF(el_stride,0),
			CUDADMASTRIDED_DMA_OFF(el_stride,1),
			CUDADMASTRIDED_DMA_OFF(el_stride,2),
			CUDADMASTRIDED_DMA_OFF(el_stride,3),
			CUDADMASTRIDED_DMA_OFF(BYTES_PER_ELMT,0),
			CUDADMASTRIDED_DMA_OFF(BYTES_PER_ELMT,1),
			CUDADMASTRIDED_DMA_OFF(BYTES_PER_ELMT,2),
			CUDADMASTRIDED_DMA_OFF(BYTES_PER_ELMT,3),
			CUDADMASTRIDED_DMA_OFF(BYTES_PER_ELMT,0),
			CUDADMASTRIDED_DMA_OFF(BYTES_PER_ELMT,1),
			CUDADMASTRIDED_DMA_OFF(BYTES_PER_ELMT,2),
			CUDADMASTRIDED_DMA_OFF(BYTES_PER_ELMT,3)),
		dma_col_iter_inc (SPLIT_ELMT ? DMA_COL_ITER_INC_SPLIT : MAX_BYTES_OUTSTANDING_PER_THREAD*WARPS_PER_ELMT*WARP_SIZE),  
		dma_src_row_iter_inc (SPLIT_ELMT ? el_stride*ELMT_PER_STEP_SPLIT : el_stride*ELMT_PER_STEP), 
		dma_dst_row_iter_inc (SPLIT_ELMT ? BYTES_PER_ELMT*ELMT_PER_STEP_SPLIT : BYTES_PER_ELMT*ELMT_PER_STEP), 
		dma_src_offset (SPLIT_ELMT ? (SPLIT_WARP ? (threadIdx.x%THREADS_PER_ELMT) : (threadIdx.x%WARP_SIZE))*ALIGNMENT : ELMT_ID*el_stride),
		dma_dst_offset (SPLIT_ELMT ? (SPLIT_WARP ? (threadIdx.x%THREADS_PER_ELMT) : (threadIdx.x%WARP_SIZE))*ALIGNMENT : ELMT_ID*BYTES_PER_ELMT),
		warp_active (int(ELMT_ID) < ELMT_PER_STEP),
		warp_partial (NUM_ELMTS==ELMT_PER_STEP ? int(ELMT_ID) < ELMT_PER_STEP : int(ELMT_ID) < (NUM_ELMTS%ELMT_PER_STEP)),
		dma_split_partial_elmts (PARTIAL_ELMTS)
	{
		if (LDS_PER_ELMT_PER_THREAD == 1) // Stripe the warp's loads across MAX_LDS_OUTSTANDING_PER_WARP elements
		{
			//int num_vec_loads  = BYTES_PER_ELMT / (ALIGNMENT*THREADS_PER_ELMT);
			int leftover_bytes = BYTES_PER_ELMT % (ALIGNMENT*THREADS_PER_ELMT);	

			if (leftover_bytes==0)
			{
				// We also need to make sure that all the threads in a warp have the same number of partial elmts
				if (NUM_ELMTS % (WARP_SIZE/THREADS_PER_ELMT) == 0)
					all_threads_active = true;
				else
					all_threads_active = false;
				partial_bytes = ALIGNMENT;	
			}
			else
			{
				all_threads_active = false;
				int max_thread_bytes = ALIGNMENT;
				if (leftover_bytes>=(max_thread_bytes*((threadIdx.x%THREADS_PER_ELMT)+1)))
				{
					// Below: do alignment-byte xfers
					partial_bytes = max_thread_bytes;
				}
				else if (leftover_bytes<(max_thread_bytes*(threadIdx.x%THREADS_PER_ELMT)))
				{
					// Above: do 0-byte xfers on last load
					partial_bytes = 0;
				}
				else
				{
					// This is the partial thread
					partial_bytes = (leftover_bytes%max_thread_bytes);
				}
			}
#ifdef CUDADMA_DEBUG_ON
			if (CUDADMA_BASE::is_dma_thread)
			{
				printf("WARP SPLIT: DMA id %2d ELMT id %d thread id %d COL_ITERS %d COL_ITERS_INC %d ROW_ITERS %d SRC_ITER_INC %d DST_ITER_INC %d partial elements %d partial bytes %d src_offset %d dst_offset %d\n", CUDADMA_DMA_TID, ELMT_ID_SPLIT, (threadIdx.x%THREADS_PER_ELMT), COL_ITERS_SPLIT, dma_col_iter_inc, ROW_ITERS_SPLIT, dma_src_row_iter_inc, dma_dst_row_iter_inc, dma_split_partial_elmts, partial_bytes, dma_src_offset, dma_dst_offset);
			}
			__syncthreads();
			if (CUDADMA_BASE::is_dma_thread)
			{
				printf("DMA id %2d Threads per element %d split elmt %d split warp %d element per step split %d element id %d\n",CUDADMA_DMA_TID,THREADS_PER_ELMT,SPLIT_ELMT,SPLIT_WARP,ELMT_PER_STEP_SPLIT,ELMT_ID_SPLIT);
			}
			__syncthreads();
			if (CUDADMA_BASE::is_dma_thread)
			{
				printf("DMA id %2d src1 %ld src2 %ld src3 %ld src4 %ld dst1 %ld dst2 %ld dst3 %ld dst4 %ld\n",CUDADMA_DMA_TID,dma1_src_iter_offs,dma2_src_iter_offs,dma3_src_iter_offs,dma4_src_iter_offs,dma1_dst_iter_offs,dma2_dst_iter_offs,dma3_dst_iter_offs,dma4_dst_iter_offs);
			}
#endif
		}
		else if (LDS_PER_ELMT_PER_THREAD <= MAX_LDS_OUTSTANDING_PER_THREAD) // A warp needs to issue multiple loads per element
		{
			//int num_vec_loads  = BYTES_PER_ELMT / (ALIGNMENT*WARP_SIZE);
			int leftover_bytes = BYTES_PER_ELMT % (ALIGNMENT*WARP_SIZE);	

			if (leftover_bytes==0)
			{
				all_threads_active = true;
				partial_bytes = 0;	
			}
			else
			{
				all_threads_active = false;
				int max_thread_bytes = ALIGNMENT;
				if (leftover_bytes>=(max_thread_bytes*((threadIdx.x%WARP_SIZE)+1)))
				{
					// Below: do alignment-byte xfers
					partial_bytes = max_thread_bytes;
				}
				else if (leftover_bytes<(max_thread_bytes*(threadIdx.x%WARP_SIZE)))
				{
					// Above: do 0-byte xfers on last load
					partial_bytes = 0;
				}
				else
				{
					// This is the partial thread
					partial_bytes = (leftover_bytes%max_thread_bytes);
				}
			}
#ifdef CUDADMA_DEBUG_ON
			if (CUDADMA_BASE::is_dma_thread)
			{
				printf("ELMT SPLIT: DMA id %2d ELMT id %d thread id %d COL_ITERS %d COL_ITERS_INC %d ROW_ITERS %d SRC_ITER_INC %d DST_ITER_INC %d partial elements %d partial bytes %d src_offset %d dst_offset %d\n", CUDADMA_DMA_TID, ELMT_ID_SPLIT, (threadIdx.x%THREADS_PER_ELMT), COL_ITERS_SPLIT, dma_col_iter_inc, ROW_ITERS_SPLIT, dma_src_row_iter_inc, dma_dst_row_iter_inc, dma_split_partial_elmts, partial_bytes, dma_src_offset, dma_dst_offset);
			}
			__syncthreads();
			if (CUDADMA_BASE::is_dma_thread)
			{
				printf("DMA id %2d src1 %ld src2 %ld src3 %ld src4 %ld dst1 %ld dst2 %ld dst3 %ld dst4 %ld\n",CUDADMA_DMA_TID,dma1_src_iter_offs,dma2_src_iter_offs,dma3_src_iter_offs,dma4_src_iter_offs,dma1_dst_iter_offs,dma2_dst_iter_offs,dma3_dst_iter_offs,dma4_dst_iter_offs);
			}
#endif
		}
		else // We'll need more than one warp to load this element
		{
			int num_vec_loads  = BYTES_PER_ELMT / (ALIGNMENT*WARPS_PER_ELMT*WARP_SIZE);
			int leftover_bytes = BYTES_PER_ELMT % (ALIGNMENT*WARPS_PER_ELMT*WARP_SIZE);

			if (leftover_bytes==0)
			{
				all_threads_active = true;
				thread_bytes = num_vec_loads*ALIGNMENT;
			}
			else
			{
				all_threads_active = false;
				int max_thread_bytes = ALIGNMENT;
				if (leftover_bytes>=(max_thread_bytes*(CUDADMA_WARP_TID+1)))
				{
					// Below: do alignment-byte xfers
					thread_bytes = (num_vec_loads-(COL_ITERS_FULL*MAX_LDS_OUTSTANDING_PER_THREAD))*ALIGNMENT + max_thread_bytes;
				}
				else if (leftover_bytes<(max_thread_bytes*CUDADMA_WARP_TID))
				{
					// Above: do 0-byte xfers on last load
					thread_bytes = (num_vec_loads-(COL_ITERS_FULL*MAX_LDS_OUTSTANDING_PER_THREAD))*ALIGNMENT;
				}
				else
				{
					// This is the partial thread
					thread_bytes = (num_vec_loads-(COL_ITERS_FULL*MAX_LDS_OUTSTANDING_PER_THREAD))*ALIGNMENT + (leftover_bytes%max_thread_bytes);
				}
			}
#ifdef CUDADMA_DEBUG_ON 
			if (CUDADMA_BASE::is_dma_thread)
			{
				printf("FULL LOAD: DMA id %d: element_id %d  src_offset %d  dst_offset %d  row_iters %d  warps_per_elem %d  thread_bytes %d  col_iters %d  col_iter_inc %d\n",CUDADMA_DMA_TID,ELMT_ID,dma_src_offset,dma_dst_offset,ROW_ITERS_FULL,WARPS_PER_ELMT,thread_bytes, COL_ITERS_FULL, dma_col_iter_inc);
			}
#endif

		}
	}
  // Constructor for explicit destination stride
  __device__ cudaDMAStrided (const int dmaID,
			     const int num_compute_threads,
			     const int dma_threadIdx_start,
			     const int src_stride,
			     const int dst_stride)
	: CUDADMA_BASE (dmaID,
			DMA_THREADS,
			num_compute_threads,
			dma_threadIdx_start,
			CUDADMASTRIDED_DMA_OFF(src_stride,0),
			CUDADMASTRIDED_DMA_OFF(src_stride,1),
			CUDADMASTRIDED_DMA_OFF(src_stride,2),
			CUDADMASTRIDED_DMA_OFF(src_stride,3),
			CUDADMASTRIDED_DMA_OFF(src_stride,0),
			CUDADMASTRIDED_DMA_OFF(src_stride,1),
			CUDADMASTRIDED_DMA_OFF(src_stride,2),
			CUDADMASTRIDED_DMA_OFF(src_stride,3),
			CUDADMASTRIDED_DMA_OFF(dst_stride,0),
			CUDADMASTRIDED_DMA_OFF(dst_stride,1),
			CUDADMASTRIDED_DMA_OFF(dst_stride,2),
			CUDADMASTRIDED_DMA_OFF(dst_stride,3),
			CUDADMASTRIDED_DMA_OFF(dst_stride,0),
			CUDADMASTRIDED_DMA_OFF(dst_stride,1),
			CUDADMASTRIDED_DMA_OFF(dst_stride,2),
			CUDADMASTRIDED_DMA_OFF(dst_stride,3)),
		dma_col_iter_inc (SPLIT_ELMT ? DMA_COL_ITER_INC_SPLIT : MAX_BYTES_OUTSTANDING_PER_THREAD*WARPS_PER_ELMT*WARP_SIZE),  
		dma_src_row_iter_inc (SPLIT_ELMT ? src_stride*ELMT_PER_STEP_SPLIT : src_stride*ELMT_PER_STEP), 
		dma_dst_row_iter_inc (SPLIT_ELMT ? dst_stride*ELMT_PER_STEP_SPLIT : dst_stride*ELMT_PER_STEP), 
		dma_src_offset (SPLIT_ELMT ? (SPLIT_WARP ? (threadIdx.x%THREADS_PER_ELMT) : (threadIdx.x%WARP_SIZE))*ALIGNMENT : ELMT_ID*src_stride),
		dma_dst_offset (SPLIT_ELMT ? (SPLIT_WARP ? (threadIdx.x%THREADS_PER_ELMT) : (threadIdx.x%WARP_SIZE))*ALIGNMENT : ELMT_ID*dst_stride),
		warp_active (int(ELMT_ID) < ELMT_PER_STEP),
		warp_partial (NUM_ELMTS==ELMT_PER_STEP ? int(ELMT_ID) < ELMT_PER_STEP : int(ELMT_ID) < (NUM_ELMTS%ELMT_PER_STEP)),
		dma_split_partial_elmts (PARTIAL_ELMTS)
	{
		if (LDS_PER_ELMT_PER_THREAD == 1) // Stripe the warp's loads across MAX_LDS_OUTSTANDING_PER_WARP elements
		{
			//int num_vec_loads  = BYTES_PER_ELMT / (ALIGNMENT*THREADS_PER_ELMT);
			int leftover_bytes = BYTES_PER_ELMT % (ALIGNMENT*THREADS_PER_ELMT);	

			if (leftover_bytes==0)
			{
				// We also need to make sure that all the threads in a warp have the same number of partial elmts
				if (NUM_ELMTS % (WARP_SIZE/THREADS_PER_ELMT) == 0)
					all_threads_active = true;
				else
					all_threads_active = false;
				partial_bytes = ALIGNMENT;	
			}
			else
			{
				all_threads_active = false;
				int max_thread_bytes = ALIGNMENT;
				if (leftover_bytes>=(max_thread_bytes*((threadIdx.x%THREADS_PER_ELMT)+1)))
				{
					// Below: do alignment-byte xfers
					partial_bytes = max_thread_bytes;
				}
				else if (leftover_bytes<(max_thread_bytes*(threadIdx.x%THREADS_PER_ELMT)))
				{
					// Above: do 0-byte xfers on last load
					partial_bytes = 0;
				}
				else
				{
					// This is the partial thread
					partial_bytes = (leftover_bytes%max_thread_bytes);
				}
			}
#ifdef CUDADMA_DEBUG_ON
			if (CUDADMA_BASE::is_dma_thread)
			{
				printf("WARP SPLIT: DMA id %2d ELMT id %d thread id %d COL_ITERS %d COL_ITERS_INC %d ROW_ITERS %d SRC_ITER_INC %d DST_ITER_INC %d partial elements %d partial bytes %d src_offset %d dst_offset %d\n", CUDADMA_DMA_TID, ELMT_ID_SPLIT, (threadIdx.x%THREADS_PER_ELMT), COL_ITERS_SPLIT, dma_col_iter_inc, ROW_ITERS_SPLIT, dma_src_row_iter_inc, dma_dst_row_iter_inc, dma_split_partial_elmts, partial_bytes, dma_src_offset, dma_dst_offset);
			}
			__syncthreads();
			if (CUDADMA_BASE::is_dma_thread)
			{
				printf("DMA id %2d Threads per element %d split elmt %d split warp %d element per step split %d element id %d\n",CUDADMA_DMA_TID,THREADS_PER_ELMT,SPLIT_ELMT,SPLIT_WARP,ELMT_PER_STEP_SPLIT,ELMT_ID_SPLIT);
			}
			__syncthreads();
			if (CUDADMA_BASE::is_dma_thread)
			{
				printf("DMA id %2d src1 %ld src2 %ld src3 %ld src4 %ld dst1 %ld dst2 %ld dst3 %ld dst4 %ld\n",CUDADMA_DMA_TID,dma1_src_iter_offs,dma2_src_iter_offs,dma3_src_iter_offs,dma4_src_iter_offs,dma1_dst_iter_offs,dma2_dst_iter_offs,dma3_dst_iter_offs,dma4_dst_iter_offs);
			}
#endif
		}
		else if (LDS_PER_ELMT_PER_THREAD <= MAX_LDS_OUTSTANDING_PER_THREAD) // A warp needs to issue multiple loads per element
		{
			//int num_vec_loads  = BYTES_PER_ELMT / (ALIGNMENT*WARP_SIZE);
			int leftover_bytes = BYTES_PER_ELMT % (ALIGNMENT*WARP_SIZE);	

			if (leftover_bytes==0)
			{
				all_threads_active = true;
				partial_bytes = 0;	
			}
			else
			{
				all_threads_active = false;
				int max_thread_bytes = ALIGNMENT;
				if (leftover_bytes>=(max_thread_bytes*((threadIdx.x%WARP_SIZE)+1)))
				{
					// Below: do alignment-byte xfers
					partial_bytes = max_thread_bytes;
				}
				else if (leftover_bytes<(max_thread_bytes*(threadIdx.x%WARP_SIZE)))
				{
					// Above: do 0-byte xfers on last load
					partial_bytes = 0;
				}
				else
				{
					// This is the partial thread
					partial_bytes = (leftover_bytes%max_thread_bytes);
				}
			}
#ifdef CUDADMA_DEBUG_ON
			if (CUDADMA_BASE::is_dma_thread)
			{
				printf("ELMT SPLIT: DMA id %2d ELMT id %d thread id %d COL_ITERS %d COL_ITERS_INC %d ROW_ITERS %d SRC_ITER_INC %d DST_ITER_INC %d partial elements %d partial bytes %d src_offset %d dst_offset %d\n", CUDADMA_DMA_TID, ELMT_ID_SPLIT, (threadIdx.x%THREADS_PER_ELMT), COL_ITERS_SPLIT, dma_col_iter_inc, ROW_ITERS_SPLIT, dma_src_row_iter_inc, dma_dst_row_iter_inc, dma_split_partial_elmts, partial_bytes, dma_src_offset, dma_dst_offset);
			}
			__syncthreads();
			if (CUDADMA_BASE::is_dma_thread)
			{
				printf("DMA id %2d src1 %ld src2 %ld src3 %ld src4 %ld dst1 %ld dst2 %ld dst3 %ld dst4 %ld\n",CUDADMA_DMA_TID,dma1_src_iter_offs,dma2_src_iter_offs,dma3_src_iter_offs,dma4_src_iter_offs,dma1_dst_iter_offs,dma2_dst_iter_offs,dma3_dst_iter_offs,dma4_dst_iter_offs);
			}
#endif
		}
		else // We'll need more than one warp to load this element
		{
			int num_vec_loads  = BYTES_PER_ELMT / (ALIGNMENT*WARPS_PER_ELMT*WARP_SIZE);
			int leftover_bytes = BYTES_PER_ELMT % (ALIGNMENT*WARPS_PER_ELMT*WARP_SIZE);

			if (leftover_bytes==0)
			{
				all_threads_active = true;
				thread_bytes = num_vec_loads*ALIGNMENT;
			}
			else
			{
				all_threads_active = false;
				int max_thread_bytes = ALIGNMENT;
				if (leftover_bytes>=(max_thread_bytes*(CUDADMA_WARP_TID+1)))
				{
					// Below: do alignment-byte xfers
					thread_bytes = (num_vec_loads-(COL_ITERS_FULL*MAX_LDS_OUTSTANDING_PER_THREAD))*ALIGNMENT + max_thread_bytes;
				}
				else if (leftover_bytes<(max_thread_bytes*CUDADMA_WARP_TID))
				{
					// Above: do 0-byte xfers on last load
					thread_bytes = (num_vec_loads-(COL_ITERS_FULL*MAX_LDS_OUTSTANDING_PER_THREAD))*ALIGNMENT;
				}
				else
				{
					// This is the partial thread
					thread_bytes = (num_vec_loads-(COL_ITERS_FULL*MAX_LDS_OUTSTANDING_PER_THREAD))*ALIGNMENT + (leftover_bytes%max_thread_bytes);
				}
			}
#ifdef CUDADMA_DEBUG_ON 
			if (CUDADMA_BASE::is_dma_thread)
			{
				printf("FULL LOAD: DMA id %d: element_id %d  src_offset %d  dst_offset %d  row_iters %d  warps_per_elem %d  thread_bytes %d  col_iters %d  col_iter_inc %d active %d partial %d\n",CUDADMA_DMA_TID,ELMT_ID,dma_src_offset,dma_dst_offset,ROW_ITERS_FULL,WARPS_PER_ELMT,thread_bytes, COL_ITERS_FULL, dma_col_iter_inc, warp_active, warp_partial);
			}
#endif

		}
	}

public:
  __device__ __forceinline__ void execute_dma(void * src_ptr, void * dst_ptr) const
  {
	switch (ALIGNMENT)
	{
	  case 4:
	    {
		execute_internal<float,LDS_PER_ELMT_PER_THREAD,ROW_ITERS_FULL,ROW_ITERS_SPLIT,COL_ITERS_FULL,COL_ITERS_SPLIT>(src_ptr, dst_ptr);
		break;
	    }
	  case 8:
	    {
		execute_internal<float2,LDS_PER_ELMT_PER_THREAD,ROW_ITERS_FULL,ROW_ITERS_SPLIT,COL_ITERS_FULL,COL_ITERS_SPLIT>(src_ptr, dst_ptr);
		break;
	    }
	  case 16:
	    {
		execute_internal<float4,LDS_PER_ELMT_PER_THREAD,ROW_ITERS_FULL,ROW_ITERS_SPLIT,COL_ITERS_FULL,COL_ITERS_SPLIT>(src_ptr, dst_ptr);
		break;
	    }
#ifdef CUDADMA_DEBUG_ON
	  default:
		printf("Invalid ALIGNMENT %d must be one of (4,8,16)\n",ALIGNMENT);
		break;
#endif
	}
  }
private:
  template<typename BULK_TYPE, int ELMT_LDS, int DMA_ROW_ITERS_FULL, int DMA_ROW_ITERS_SPLIT, int DMA_COL_ITERS_FULL, int DMA_COL_ITERS_SPLIT>
  __device__ __forceinline__ void execute_internal(void * src_ptr, void * dst_ptr) const
  {
	if (ELMT_LDS == 1) // Warp's load is striped over all elements
	{
		char * src_row_ptr = ((char*)src_ptr) + dma_src_offset;
		char * dst_row_ptr = ((char*)dst_ptr) + dma_dst_offset;
		if (DMA_ROW_ITERS_SPLIT == 0)
		{
			// The optimized case
			if (all_threads_active)
			{
				CUDADMA_BASE::template do_xfer_across<true>(src_row_ptr, dst_row_ptr, dma_split_partial_elmts, partial_bytes);
			}
			else
			{
				CUDADMA_BASE::wait_for_dma_start();
				if (dma_split_partial_elmts > 0)
					CUDADMA_BASE::template do_xfer_across<false>(src_row_ptr, dst_row_ptr, dma_split_partial_elmts, partial_bytes);
			}
		}	
		else
		{
			CUDADMA_BASE::wait_for_dma_start();
			//#pragma unroll 
			for (int i=0; i<DMA_ROW_ITERS_SPLIT; i++)
			{
				CUDADMA_BASE::template do_xfer_across<false>(src_row_ptr, dst_row_ptr, MAX_LDS_OUTSTANDING_PER_THREAD, partial_bytes);
				src_row_ptr += dma_src_row_iter_inc;
				dst_row_ptr += dma_dst_row_iter_inc;
			}
			if (dma_split_partial_elmts > 0)
				CUDADMA_BASE::template do_xfer_across<false>(src_row_ptr, dst_row_ptr, dma_split_partial_elmts, partial_bytes);
		}
	}
	else if (ELMT_LDS <= MAX_LDS_OUTSTANDING_PER_THREAD) // A warp needs to issue multiple loads per element
	{
		char * src_row_ptr = ((char*)src_ptr) + dma_src_offset;
		char * dst_row_ptr = ((char*)dst_ptr) + dma_dst_offset;
		if (DMA_ROW_ITERS_SPLIT == 0)
		{
			CUDADMA_BASE::wait_for_dma_start();
			copy_across_elmts<BULK_TYPE,DMA_COL_ITERS_SPLIT>(src_row_ptr, dst_row_ptr, dma_split_partial_elmts, partial_bytes);
		}
		else
		{
			CUDADMA_BASE::wait_for_dma_start();
			//#pragma unroll
			for (int i=0; i<DMA_ROW_ITERS_SPLIT; i++)
			{
				copy_across_elmts<BULK_TYPE,DMA_COL_ITERS_SPLIT>(src_row_ptr, dst_row_ptr, MAX_LDS_OUTSTANDING_PER_THREAD, partial_bytes);
				src_row_ptr += dma_src_row_iter_inc;
				dst_row_ptr += dma_dst_row_iter_inc;
			}
			// Handle any partial elements	
			if (dma_split_partial_elmts > 0)
				copy_across_elmts<BULK_TYPE,DMA_COL_ITERS_SPLIT>(src_row_ptr, dst_row_ptr, dma_split_partial_elmts, partial_bytes);
		}
	}
	else // We'll need more than one warp to load an element
	{
		char * src_row_ptr = ((char*)src_ptr) + dma_src_offset;
		char * dst_row_ptr = ((char*)dst_ptr) + dma_dst_offset;
		if (DMA_ROW_ITERS_FULL==0)
		{
			// check to see if there are column iterations to perform, if not might be able to do the optimized case
			if (DMA_COL_ITERS_FULL == 0)
			{
				int opt_xfer = (thread_bytes%MAX_BYTES_OUTSTANDING_PER_THREAD) ?
					(thread_bytes%MAX_BYTES_OUTSTANDING_PER_THREAD) :
					(thread_bytes ? MAX_BYTES_OUTSTANDING_PER_THREAD : 0);
				// The optimized case
				if (all_threads_active)
				{
					if (warp_partial)
						CUDADMA_BASE::do_xfer<true,ALIGNMENT>(src_row_ptr,dst_row_ptr,opt_xfer);
					else
						CUDADMA_BASE::wait_for_dma_start();
				}
				else
				{
					CUDADMA_BASE::wait_for_dma_start();
					if (warp_partial)
						CUDADMA_BASE::do_xfer<false,ALIGNMENT>(src_row_ptr,dst_row_ptr,opt_xfer);
				}
			}
			else // We actually need to load multiple columns
			{
				CUDADMA_BASE::wait_for_dma_start();
				copy_elmt<BULK_TYPE,DMA_COL_ITERS_FULL>(src_row_ptr,dst_row_ptr);
			}
		}
		else
		{
			CUDADMA_BASE::wait_for_dma_start();
			if (warp_active)
			{
				//#pragma unroll
				for (int i=0; i<DMA_ROW_ITERS_FULL; i++)
				{
					copy_elmt<BULK_TYPE, DMA_COL_ITERS_FULL>(src_row_ptr,dst_row_ptr);
					src_row_ptr += dma_src_row_iter_inc;
					dst_row_ptr += dma_dst_row_iter_inc;
				}
			}
			if (warp_partial)
			{
				copy_elmt<BULK_TYPE, DMA_COL_ITERS_FULL>(src_row_ptr,dst_row_ptr);
			}
		}
	}
	CUDADMA_BASE::finish_async_dma();
  }
private:
  template<typename BULK_TYPE, int DMA_COL_ITERS>
  __device__ __forceinline__ void copy_elmt(char * src_ptr, char * dst_ptr) const
  {
	#pragma unroll
	for (int j=0; j<DMA_COL_ITERS; j++)
	{
		CUDADMA_BASE::template perform_four_xfers<BULK_TYPE,BULK_TYPE,false,false> (src_ptr,dst_ptr);
		src_ptr += dma_col_iter_inc;
		dst_ptr += dma_col_iter_inc;
	}
	CUDADMA_BASE::template do_xfer<false,ALIGNMENT> (src_ptr, dst_ptr,
				(thread_bytes%MAX_BYTES_OUTSTANDING_PER_THREAD) ?
				(thread_bytes%MAX_BYTES_OUTSTANDING_PER_THREAD) :
				(thread_bytes ? MAX_BYTES_OUTSTANDING_PER_THREAD : 0));
  } 

  template<typename BULK_TYPE, int DMA_COL_ITERS>
  __device__ __forceinline__ void copy_across_elmts(char * src_ptr, char * dst_ptr, int total_lds, int partial_size) const
  {
	//#pragma unroll
	for (int j=0; j<DMA_COL_ITERS; j++)
	{
		CUDADMA_BASE::template do_xfer_across<false>(src_ptr, dst_ptr, total_lds, sizeof(BULK_TYPE));
		src_ptr += dma_col_iter_inc;
		dst_ptr += dma_col_iter_inc;
	}
	CUDADMA_BASE::template do_xfer_across<false>(src_ptr, dst_ptr, total_lds, partial_size);
  }
public:
  // Public DMA-thread Synchronization Functions
  __device__ __forceinline__ void wait_for_dma_start() const
  {
    CUDADMA_BASE::wait_for_dma_start();
  }
  __device__ __forceinline__ void finish_async_dma() const
  {
    CUDADMA_BASE::finish_async_dma();
  }
};
#undef LDS_PER_ELMT
#undef LDS_PER_ELMT_PER_THREAD
#undef FULL_LDS_PER_ELMT
#undef PARTIAL_LDS_PER_ELMT
#undef SPLIT_ELMT
#undef SPLIT_WARP
#undef THREADS_PER_ELMT
#undef DMA_COL_ITER_INC_SPLIT
#undef ELMT_PER_STEP_SPLIT
#undef ELMT_ID_SPLIT
#undef REMAINING_ELMTS
#undef PARTIAL_ELMTS
#undef MAX_WARPS_PER_ELMT
#undef WARPS_PER_ELMT
#undef ELMT_PER_STEP
#undef ELMT_ID
#undef CUDADMA_WARP_TID
#undef CUDADMASTRIDED_DMA_OFF
#undef ROW_ITERS_FULL
#undef ROW_ITERS_SPLIT
#undef COL_ITERS_FULL
#undef COL_ITERS_SPLIT


//////////////////////////////////////////////////////////////////////////////////////////////////
// CudaDMAHalo
//////////////////////////////////////////////////////////////////////////////////////////////////

//#define MAX_BYTES_PER_ROW  (CORNERS ? (MAX_DIMX+2*RADIUS)*int(sizeof(ELMT_TYPE)) : MAX_DIMX*int(sizeof(ELMT_TYPE)))
#define ROW_BYTES (CORNERS ? (dimx+2*RADIUS)*int(sizeof(ELMT_TYPE)) : dimx*int(sizeof(ELMT_TYPE)))
//#define MAX_WARPS_PER_ROW ((MAX_BYTES_PER_ROW+(WARP_SIZE*MAX_BYTES_OUTSTANDING_PER_THEAD-1))/(WARP_SIZE*MAX_BYTES_OUSTANDING_PER_THREAD))
#define MAX_WARPS_PER_ROW ((ROW_BYTES+(WARP_SIZE*MAX_BYTES_OUTSTANDING_PER_THREAD-1))/(WARP_SIZE*MAX_BYTES_OUTSTANDING_PER_THREAD))
#define ROWS_PER_STEP ((num_dma_threads/WARP_SIZE+MAX_WARPS_PER_ROW-1)/MAX_WARPS_PER_ROW)
#define ROW_ID ((CUDADMA_DMA_TID/WARP_SIZE)/MAX_WARPS_PER_ROW)
// For a given row, figure out how many warps there are loading it
// There will always be at least 2 rows
#define WARPS_PER_ROW (ROWS_PER_STEP == 1 ? (num_dma_threads/WARP_SIZE) : (ROW_ID < (ROWS_PER_STEP-1) ? MAX_WARPS_PER_ROW : (num_dma_threads/WARP_SIZE) - (ROWS_PER_STEP-1)*MAX_WARPS_PER_ROW))
//#define WARPS_PER_ROW ((MAX_WARPS_PER_ROW >= (num_dma_threads/WARP_SIZE)) ? (num_dma_threads/WARP_SIZE) : \
	(num_dma_threads/WARP_SIZE)>(MAX_WARPS_PER_ROW*2*RADIUS) ?  (num_dma_threads/WARP_SIZE)/(2*RADIUS) : MAX_WARPS_PER_ROW)
//#define ROWS_PER_STEP ((num_dma_threads/WARP_SIZE)/WARPS_PER_ROW)
//#define ROW_ID ((CUDADMA_DMA_TID/WARP_SIZE)/WARPS_PER_ROW)
#define CUDADMA_WARP_TID (threadIdx.x - (dma_threadIdx_start + (ROW_ID*MAX_WARPS_PER_ROW*WARP_SIZE)))
#define CUDADMAHALO_DMA1_ITER_OFFS (ALIGNMENT*CUDADMA_WARP_TID)
#define CUDADMAHALO_DMA2_ITER_OFFS (1*ALIGNMENT*WARPS_PER_ROW*WARP_SIZE+ ALIGNMENT*CUDADMA_WARP_TID)
#define CUDADMAHALO_DMA3_ITER_OFFS (2*ALIGNMENT*WARPS_PER_ROW*WARP_SIZE+ ALIGNMENT*CUDADMA_WARP_TID)
#define CUDADMAHALO_DMA4_ITER_OFFS (3*ALIGNMENT*WARPS_PER_ROW*WARP_SIZE+ ALIGNMENT*CUDADMA_WARP_TID)
#define CUDADMAHALO_DMA1_OFFS (ALIGNMENT*CUDADMA_WARP_TID)	
#define CUDADMAHALO_DMA2_OFFS (1*ALIGNMENT*WARPS_PER_ROW*WARP_SIZE+ ALIGNMENT*CUDADMA_WARP_TID)	
#define CUDADMAHALO_DMA3_OFFS (2*ALIGNMENT*WARPS_PER_ROW*WARP_SIZE+ ALIGNMENT*CUDADMA_WARP_TID)
#define CUDADMAHALO_DMA4_OFFS (3*ALIGNMENT*WARPS_PER_ROW*WARP_SIZE+ ALIGNMENT*CUDADMA_WARP_TID)
// We also need to figure out how many loads need to be performed for the sides
// Figure out the biggest size loads we can perform for a given radius and alignment
#define SIDE_BYTES (RADIUS*int(sizeof(ELMT_TYPE)))
// This value should be statically determined 
#define SIDE_XFER_SIZE ((SIDE_BYTES%ALIGNMENT)==0 ? ALIGNMENT : (SIDE_BYTES%(ALIGNMENT/2))==0 ? ALIGNMENT/2 : ALIGNMENT/4)
#define TOTAL_SIDE_LOADS (SIDE_BYTES/SIDE_XFER_SIZE)
// We want threads per side to be a multiple of 2 so it should divide evenly into WARP_SIZE 
#define THREADS_PER_SIDE ((TOTAL_SIDE_LOADS==1) ? 1 : (TOTAL_SIDE_LOADS==2) ? 2 : (TOTAL_SIDE_LOADS<=4) ? 4 : (TOTAL_SIDE_LOADS<=8) ? 8 : (TOTAL_SIDE_LOADS<=16) ? 16 : 32)
// Total number of sides that can be handled by a warp, each thread can handle MAX_LDS_OUTSTANDING sides 
#define SIDES_PER_WARP (WARP_SIZE/THREADS_PER_SIDE)*MAX_LDS_OUTSTANDING_PER_THREAD
// The remaining warps after the warps have been allocated to the rows
#define REMAINING_WARPS ((num_dma_threads/WARP_SIZE) > (2*RADIUS*MAX_WARPS_PER_ROW) ? \
				(num_dma_threads/WARP_SIZE) - (2*RADIUS)*MAX_WARPS_PER_ROW : 0)

// Asserting sizeof(ELMT_TYPE) <= ALIGNMENT
// Asserting (RADIUS*sizeof(ELMT_TYPE))%ALIGNMENT==0
template<typename ELMT_TYPE, int RADIUS, bool CORNERS, int ALIGNMENT>
class cudaDMAHalo : public CUDADMA_BASE
{
private:
	// Values for moving rows
	const int row_sz;
	const int dma_col_iters;
	const int dma_col_iter_inc;
	const int dma_row_id;
	const int dma_row_iters;
	const int dma_src_row_iter_inc;
	const int dma_dst_row_iter_inc;
	const int dma_src_top_offset;
	const int dma_src_bot_offset;
	const int dma_dst_top_offset;
	const int dma_dst_bot_offset;
	int thread_bytes;
	// Values for moving the sides
	int side_id;
	int side_load;
	int side_src_iter_inc;
	int side_dst_iter_inc;
	int side_src_offset;
	int side_dst_offset;
	int side_iters;
	bool side_active;

	// Optimizations for different loading techniques
	const bool split; // If we split warps into row warps and side warps
	const bool optimized; // If everything can be loaded in one step, pre-load into registers
	const bool all_threads_active; // Check if all row threads are active for the optmized case
	const bool row_thread; // true if responsible for loading rows, otherwise a side thread in split cases
public:
  __device__ cudaDMAHalo (const int dmaID,
			  const int num_dma_threads,
			  const int num_compute_threads,
			  const int dma_threadIdx_start,
			  const int dimx,
			  const int dimy,
			  const int pitch)
	: CUDADMA_BASE (dmaID,
			num_dma_threads,
			num_compute_threads,
			dma_threadIdx_start,
			CUDADMAHALO_DMA1_ITER_OFFS,
			CUDADMAHALO_DMA2_ITER_OFFS,
			CUDADMAHALO_DMA3_ITER_OFFS,
			CUDADMAHALO_DMA4_ITER_OFFS,
			CUDADMAHALO_DMA1_OFFS,
			CUDADMAHALO_DMA2_OFFS,
			CUDADMAHALO_DMA3_OFFS,
			CUDADMAHALO_DMA4_OFFS,	
			CUDADMAHALO_DMA1_ITER_OFFS,
			CUDADMAHALO_DMA2_ITER_OFFS,
			CUDADMAHALO_DMA3_ITER_OFFS,
			CUDADMAHALO_DMA4_ITER_OFFS,
			CUDADMAHALO_DMA1_OFFS,
			CUDADMAHALO_DMA2_OFFS,
			CUDADMAHALO_DMA3_OFFS,
			CUDADMAHALO_DMA4_OFFS),	
		row_sz (ROW_BYTES),
		dma_col_iters ((ROW_BYTES-4)/(MAX_BYTES_OUTSTANDING_PER_THREAD*WARPS_PER_ROW*WARP_SIZE)),
		dma_col_iter_inc (MAX_BYTES_OUTSTANDING_PER_THREAD*WARPS_PER_ROW*WARP_SIZE),
		dma_row_id (ROW_ID),	
		dma_row_iters (ROWS_PER_STEP),
		dma_src_row_iter_inc (pitch*sizeof(ELMT_TYPE)),
		dma_dst_row_iter_inc ((2*RADIUS+dimx)*sizeof(ELMT_TYPE)),
		dma_src_top_offset (CORNERS ? (-(RADIUS*pitch + RADIUS)*sizeof(ELMT_TYPE)) : 
						(-(RADIUS*pitch)*sizeof(ELMT_TYPE))),
		dma_src_bot_offset (CORNERS ? ((dimy*pitch - RADIUS)*sizeof(ELMT_TYPE)) : 
						(dimy*pitch)*sizeof(ELMT_TYPE)),
		dma_dst_top_offset (CORNERS ? (-(RADIUS*(dimx+2*RADIUS)+RADIUS)*sizeof(ELMT_TYPE)) : 
						(-(RADIUS*(dimx+2*RADIUS)*sizeof(ELMT_TYPE)))),
		dma_dst_bot_offset (CORNERS ? ((dimy*(dimx+2*RADIUS)-RADIUS)*sizeof(ELMT_TYPE)) : 
						(dimy*(dimx+2*RADIUS)*sizeof(ELMT_TYPE))),
		split (REMAINING_WARPS>0),	
		optimized ((REMAINING_WARPS>0) && ((REMAINING_WARPS*SIDES_PER_WARP)>=2*dimy)),
		all_threads_active (ROW_BYTES % (ALIGNMENT*WARPS_PER_ROW*WARP_SIZE) == 0),
		row_thread (ROW_ID<(2*RADIUS))
	{
		if (REMAINING_WARPS>0)
		{
			// In this case we will have seperate warps for handling the rows and the sides
			// This encompasses both the split and optimized cases
			if (!(ROW_ID<(2*RADIUS)))
			{
				int local_id = threadIdx.x - (dma_threadIdx_start + num_dma_threads 
									- (REMAINING_WARPS*WARP_SIZE));	
				side_id = local_id/THREADS_PER_SIDE;
				side_load = local_id%THREADS_PER_SIDE;
				side_active = (side_load < TOTAL_SIDE_LOADS) && (side_id < (2*dimy));
				side_src_iter_inc = ((REMAINING_WARPS*WARP_SIZE/THREADS_PER_SIDE)/2)*pitch*sizeof(ELMT_TYPE);
				side_dst_iter_inc = ((REMAINING_WARPS*WARP_SIZE/THREADS_PER_SIDE)/2)*(2*RADIUS+dimx)*sizeof(ELMT_TYPE);
				side_src_offset = (side_id%2 ? dimx*sizeof(ELMT_TYPE)+side_load*SIDE_XFER_SIZE 
							: -(TOTAL_SIDE_LOADS-side_load)*SIDE_XFER_SIZE) + (side_id/2)*pitch*sizeof(ELMT_TYPE);
				side_dst_offset = (side_id%2 ? dimx*sizeof(ELMT_TYPE)+side_load*SIDE_XFER_SIZE 
							: -(TOTAL_SIDE_LOADS-side_load)*SIDE_XFER_SIZE) + (side_id/2)*(2*RADIUS+dimx)*sizeof(ELMT_TYPE);
				int temp_side_iters = 2*dimy - side_id + (REMAINING_WARPS*WARP_SIZE/THREADS_PER_SIDE) - 1;
				int temp_side_iters2 = REMAINING_WARPS*WARP_SIZE/THREADS_PER_SIDE;
				side_iters = temp_side_iters/temp_side_iters2;
				// For some reason the following line causes the compiler to seg-fault
				//side_iters = temp_side_iters/(REMAINING_WARPS*WARP_SIZE/THREADS_PER_SIDE);
			}
			else // These are the row threads, finish computing their bytes
			{
				int num_vec_loads  = ROW_BYTES / (ALIGNMENT*WARPS_PER_ROW*WARP_SIZE);
				int leftover_bytes = ROW_BYTES % (ALIGNMENT*WARPS_PER_ROW*WARP_SIZE);

				if (leftover_bytes==0)
					thread_bytes = num_vec_loads*ALIGNMENT;
				else
				{
					int max_thread_bytes = ALIGNMENT;
					if (leftover_bytes>=(max_thread_bytes*(CUDADMA_WARP_TID+1)))
					{
						// Below: do alignment-byte xfers
						thread_bytes = (num_vec_loads-(dma_col_iters*MAX_LDS_OUTSTANDING_PER_THREAD))*ALIGNMENT + max_thread_bytes;
					}
					else if (leftover_bytes<(max_thread_bytes*CUDADMA_WARP_TID))
					{
						// Above: do 0-byte xfers on last load
						thread_bytes = (num_vec_loads-(dma_col_iters*MAX_LDS_OUTSTANDING_PER_THREAD))*ALIGNMENT;
					}
					else
					{
						// This is the partial thread
						thread_bytes = (num_vec_loads-(dma_col_iters*MAX_LDS_OUTSTANDING_PER_THREAD))*ALIGNMENT + (leftover_bytes%max_thread_bytes);
					}
				}
			}
		}
		else
		{
			// Unoptimized case where all threads will do both rows and sides
			side_id   = CUDADMA_DMA_TID / THREADS_PER_SIDE;
			side_load = CUDADMA_DMA_TID % THREADS_PER_SIDE;
			side_active = (side_load < TOTAL_SIDE_LOADS) && (side_id < (2*dimy));
			// Compute side_stride from the number of rows that can be handled
			side_src_iter_inc = ((num_dma_threads/THREADS_PER_SIDE)/2)*pitch*sizeof(ELMT_TYPE);
			side_dst_iter_inc = ((num_dma_threads/THREADS_PER_SIDE)/2)*(2*RADIUS+dimx)*sizeof(ELMT_TYPE);
			side_iters = (2*dimy - side_id +(num_dma_threads/THREADS_PER_SIDE)-1)/(num_dma_threads/THREADS_PER_SIDE);
			side_src_offset = (side_id%2 ? dimx*sizeof(ELMT_TYPE)+side_load*SIDE_XFER_SIZE 
							: -(TOTAL_SIDE_LOADS-side_load)*SIDE_XFER_SIZE) + (side_id/2)*pitch*sizeof(ELMT_TYPE);
			side_dst_offset = (side_id%2 ? dimx*sizeof(ELMT_TYPE)+side_load*SIDE_XFER_SIZE 
							: -(TOTAL_SIDE_LOADS-side_load)*SIDE_XFER_SIZE) + (side_id/2)*(2*RADIUS+dimx)*sizeof(ELMT_TYPE);

			// Finally set up the row information
			int num_vec_loads  = ROW_BYTES / (ALIGNMENT*WARPS_PER_ROW*WARP_SIZE);
			int leftover_bytes = ROW_BYTES % (ALIGNMENT*WARPS_PER_ROW*WARP_SIZE);

			if (leftover_bytes==0)
				thread_bytes = num_vec_loads*ALIGNMENT;
			else
			{
				int max_thread_bytes = ALIGNMENT;
				if (leftover_bytes>=(max_thread_bytes*(CUDADMA_WARP_TID+1)))
				{
					// Below: do alignment-byte xfers
					thread_bytes = (num_vec_loads-(dma_col_iters*MAX_LDS_OUTSTANDING_PER_THREAD))*ALIGNMENT + max_thread_bytes;
				}
				else if (leftover_bytes<(max_thread_bytes*CUDADMA_WARP_TID))
				{
					// Above: do 0-byte xfers on last load
					thread_bytes = (num_vec_loads-(dma_col_iters*MAX_LDS_OUTSTANDING_PER_THREAD))*ALIGNMENT;
				}
				else
				{
					// This is the partial thread
					thread_bytes = (num_vec_loads-(dma_col_iters*MAX_LDS_OUTSTANDING_PER_THREAD))*ALIGNMENT + (leftover_bytes%max_thread_bytes);
				}
			}	
		}
#ifdef CUDADMA_DEBUG_ON
		if (CUDADMA_BASE::is_dma_thread)
		{
			printf("DMA id %2d: col_iter %d col_iter_inc %d row_id %d row_iters %d src_row_inc %d dst_row_inc %d src_top_offset %d src_bot_offset %d dst_top_offset %d dst_bot_offset %d thread_bytes %d radius %d\n",CUDADMA_DMA_TID,dma_col_iters,dma_col_iter_inc, dma_row_id, dma_row_iters, dma_src_row_iter_inc, dma_dst_row_iter_inc, dma_src_top_offset, dma_src_bot_offset, dma_dst_top_offset, dma_dst_bot_offset, thread_bytes, RADIUS);
		}
		__syncthreads();
		if (CUDADMA_BASE::is_dma_thread)
		{
			printf("DMA id %2d: side_id %d side_load %d side_src_iter_inc %d side_dst_iter_inc %d side_src_offset %d side_dst_offset %d side_iters %d side_active %d\n",CUDADMA_DMA_TID, side_id, side_load, side_src_iter_inc, side_dst_iter_inc, side_src_offset, side_dst_offset, side_iters, side_active);
		}
		__syncthreads();
		if (CUDADMA_BASE::is_dma_thread)
		{
			printf("DMA id %2d: threads_per_side %d side_xfer_size %d total_side_loads %d radius %d side_bytes %d\n", CUDADMA_DMA_TID,THREADS_PER_SIDE, SIDE_XFER_SIZE, TOTAL_SIDE_LOADS, RADIUS, SIDE_BYTES);
		}
		__syncthreads();
		if (CUDADMA_BASE::is_dma_thread)
		{
			printf("DMA id %2d: optimized %d split %d row_thread %d\n",CUDADMA_DMA_TID,(REMAINING_WARPS>0) && ((REMAINING_WARPS*SIDES_PER_WARP)>=2*dimy),(REMAINING_WARPS>0),(ROW_ID<2*RADIUS));
		}
		__syncthreads();
		if (CUDADMA_BASE::is_dma_thread)
		{
			printf("DMA id %2d: max warps per row %d rows per step %d remaining warps %d row bytes %d\n",CUDADMA_DMA_TID,MAX_WARPS_PER_ROW,ROWS_PER_STEP,REMAINING_WARPS,ROW_BYTES);
		}
#endif
	}
public:
  __device__ __forceinline__ void execute_dma(void * src_origin, void * dst_origin) const
  {
	// First check if this is the optimized case
	if (optimized)
	{
		// For the optimized case we know that everything can be loaded in a single pass
		// so we can do some optimizations like pre-loading into registers
		if (row_thread) // These are the row loader threads
		{
			char *src_row_ptr = ((char*)src_origin) + (dma_row_id<RADIUS ? dma_src_top_offset + (dma_row_id*dma_src_row_iter_inc) : dma_src_bot_offset + (dma_row_id-RADIUS)*dma_src_row_iter_inc);
			char *dst_row_ptr = ((char*)dst_origin) + (dma_row_id<RADIUS ? dma_dst_top_offset + (dma_row_id*dma_dst_row_iter_inc) : dma_dst_bot_offset + (dma_row_id-RADIUS)*dma_dst_row_iter_inc);
			if (all_threads_active)
			{
				load_rows_opt<true>(src_row_ptr,dst_row_ptr);
			}
			else
			{
				CUDADMA_BASE::wait_for_dma_start();
				load_rows_opt<false>(src_row_ptr, dst_row_ptr);	
			}
		}
		else // These are the side loader threads
		{
			char *src_side_ptr = ((char*)src_origin) + side_src_offset;
			char *dst_side_ptr = ((char*)dst_origin) + side_dst_offset;
			load_sides_opt<SIDE_XFER_SIZE>(src_side_ptr, dst_side_ptr);
		}
		// Indicate that we finished the load
		CUDADMA_BASE::finish_async_dma();
	}
	else if (split) // Same as above but threads might night to run multiple iterations
	{
		CUDADMA_BASE::wait_for_dma_start();
		if (row_thread) // These are the row loader threads
		{
			int row_id = dma_row_id;
			char *src_row_ptr = ((char*)src_origin) + dma_src_top_offset + row_id*dma_src_row_iter_inc;
			char *dst_row_ptr = ((char*)dst_origin) + dma_dst_top_offset + row_id*dma_dst_row_iter_inc;
			load_rows<RADIUS>(row_id, src_row_ptr, dst_row_ptr);
			src_row_ptr = ((char*)src_origin) + dma_src_bot_offset + (row_id-RADIUS)*dma_src_row_iter_inc;
			dst_row_ptr = ((char*)dst_origin) + dma_dst_bot_offset + (row_id-RADIUS)*dma_dst_row_iter_inc;
			load_rows<2*RADIUS>(row_id, src_row_ptr, dst_row_ptr);
		}
		else // These are the side loader threads
		{
			char *src_side_ptr = ((char*)src_origin) + side_src_offset;
			char *dst_side_ptr = ((char*)dst_origin) + side_dst_offset;
			if (side_active)
				load_sides<SIDE_XFER_SIZE>(src_side_ptr,dst_side_ptr);
		}
		CUDADMA_BASE::finish_async_dma();
	}
	else  // In this case all threads have to load both rows and sides (the general case)
	{
		// Wait for the transfer to begin
		CUDADMA_BASE::wait_for_dma_start();
		// Do the top first
		int row_id = dma_row_id;
		char * src_row_ptr = ((char*)src_origin) + dma_src_top_offset + row_id*dma_src_row_iter_inc;
		char * dst_row_ptr = ((char*)dst_origin) + dma_dst_top_offset + row_id*dma_dst_row_iter_inc;
		load_rows<RADIUS>(row_id, src_row_ptr, dst_row_ptr);
		// Now do the bottom set of rows
		src_row_ptr = ((char*)src_origin) + dma_src_bot_offset + (row_id-RADIUS)*dma_src_row_iter_inc;
		dst_row_ptr = ((char*)dst_origin) + dma_dst_bot_offset + (row_id-RADIUS)*dma_dst_row_iter_inc;
		load_rows<2*RADIUS>(row_id, src_row_ptr, dst_row_ptr);
		// We've finished the rows, now do the sides
		char * src_side_ptr = ((char*)src_origin) + side_src_offset;
		char * dst_side_ptr = ((char*)dst_origin) + side_dst_offset;
		if (side_active)
			load_sides<SIDE_XFER_SIZE>(src_side_ptr, dst_side_ptr);	
		// We're finally finished, indicate we're done
		CUDADMA_BASE::finish_async_dma();
	}
  }
private: // Helper methods
  template<int MAX_ROW>
  __device__ __forceinline__ void load_rows(int &row_id, char *src_row_ptr, char *dst_row_ptr) const
  {
	char * src_temp = src_row_ptr;
	char * dst_temp = dst_row_ptr;
	  switch(ALIGNMENT)
	    {
	      case 4:
		{
		  for ( ; row_id < MAX_ROW; row_id += dma_row_iters)
		  {
		    for (int j=0; j < dma_col_iters; j++)
		    {
		      CUDADMA_BASE::template perform_four_xfers<float,float,false,false> (src_temp,dst_temp);
		      src_temp += dma_col_iter_inc;
		      dst_temp += dma_col_iter_inc;
		    }
		    // Handle the leftovers
		    CUDADMA_BASE::template do_xfer<false,ALIGNMENT> (src_temp, dst_temp,
			(thread_bytes%MAX_BYTES_OUTSTANDING_PER_THREAD) ?
			(thread_bytes%MAX_BYTES_OUTSTANDING_PER_THREAD) :
			(thread_bytes ? MAX_BYTES_OUTSTANDING_PER_THREAD : 0));
		    // Now set up for the next row
	  	    src_row_ptr += (dma_row_iters*dma_src_row_iter_inc);
	  	    src_temp = src_row_ptr;
	  	    dst_row_ptr += (dma_row_iters*dma_dst_row_iter_inc);
	  	    dst_temp = dst_row_ptr;
		  }
		  break;
		}
	      case 8:
		{
		  for ( ; row_id < MAX_ROW; row_id += dma_row_iters)
		  {
		    for (int j=0; j < dma_col_iters; j++)
		    {
		      CUDADMA_BASE::template perform_four_xfers<float2,float2,false,false> (src_temp,dst_temp);	
		      src_temp += dma_col_iter_inc;
		      dst_temp += dma_col_iter_inc;
		    }
		    // Now handle the column leftovers
	  	    CUDADMA_BASE::template do_xfer<false,ALIGNMENT> (src_temp, dst_temp,
			(thread_bytes%MAX_BYTES_OUTSTANDING_PER_THREAD) ?
			(thread_bytes%MAX_BYTES_OUTSTANDING_PER_THREAD) :
			(thread_bytes ? MAX_BYTES_OUTSTANDING_PER_THREAD : 0));
	  	    // Now set up for the next row
	  	    src_row_ptr += (dma_row_iters*dma_src_row_iter_inc);
	  	    src_temp = src_row_ptr;
	  	    dst_row_ptr += (dma_row_iters*dma_dst_row_iter_inc);
	  	    dst_temp = dst_row_ptr;
		  }
		  break;
		}
	      case 16:
		{
		  for ( ; row_id < MAX_ROW; row_id += dma_row_iters)
		  {
		    for (int j=0; j < dma_col_iters; j++)
		    {
		      CUDADMA_BASE::template perform_four_xfers<float4,float4,false,false> (src_temp,dst_temp);
		      src_temp += dma_col_iter_inc;
		      dst_temp += dma_col_iter_inc;
		    }
		    // Now handle the column leftovers
	  	    CUDADMA_BASE::template do_xfer<false,ALIGNMENT> (src_temp, dst_temp,
			(thread_bytes%MAX_BYTES_OUTSTANDING_PER_THREAD) ?
			(thread_bytes%MAX_BYTES_OUTSTANDING_PER_THREAD) :
			(thread_bytes ? MAX_BYTES_OUTSTANDING_PER_THREAD : 0));
	  	    // Now set up for the next row
	  	    src_row_ptr += (dma_row_iters*dma_src_row_iter_inc);
	  	    src_temp = src_row_ptr;
	  	    dst_row_ptr += (dma_row_iters*dma_dst_row_iter_inc);
	  	    dst_temp = dst_row_ptr;
		  }
		  break;
		}
#ifdef CUDADMA_DEBUG_ON
	      default:
		printf("ALIGNMENT must be one of (4,8,16)\n");
		break;
#endif
	    }
  }
  template<bool DO_SYNC>
  __device__ __forceinline__ void load_rows_opt(char *src_row_ptr, char *dst_row_ptr) const
  {
	CUDADMA_BASE::template do_xfer<DO_SYNC,ALIGNMENT> (src_row_ptr,dst_row_ptr,
				(thread_bytes%MAX_BYTES_OUTSTANDING_PER_THREAD) ?
				(thread_bytes%MAX_BYTES_OUTSTANDING_PER_THREAD) :
				(thread_bytes ? MAX_BYTES_OUTSTANDING_PER_THREAD : 0));
  }
  template<int XFER_SIZE>
  __device__ __forceinline__ void load_sides(char * src_side_ptr, char * dst_side_ptr) const
  {
	switch (XFER_SIZE)
	  {
	    case 4:
	      {
		for (int i = 0; i < side_iters; i++)
		{
			float temp = (*((float*)(src_side_ptr)));
			(*((float*)(dst_side_ptr))) = temp;
			src_side_ptr += side_src_iter_inc;
			dst_side_ptr += side_dst_iter_inc;
		}
		break;
	      }
	    case 8:
	      {
		for (int i = 0; i < side_iters; i++)
		{
			float2 temp = (*((float2*)(src_side_ptr)));
			(*((float2*)(dst_side_ptr))) = temp;	
			src_side_ptr += side_src_iter_inc;
			dst_side_ptr += side_dst_iter_inc;
		}
		break;
	      }
	    case 16:
	      {
		for (int i = 0; i < side_iters; i++)
		{
			float4 temp = (*((float4*)(src_side_ptr)));
			(*((float4*)(dst_side_ptr))) = temp;
			src_side_ptr += side_src_iter_inc;
			dst_side_ptr += side_dst_iter_inc;
		}
		break;
	      }
#ifdef CUDADMA_DEBUG_ON
	    default:
	      printf("Warning CUDA_DMA internal error, invalid side xfer size: %d\n",XFER_SIZE);
	      break;
#endif
	  }	
  }
  template<int XFER_SIZE>
  __device__ __forceinline__ void load_sides_opt(char * src_side_ptr, char * dst_side_ptr) const
  {
	switch (XFER_SIZE)
	  {
	    case 4:
	      {
		float tmp[MAX_LDS_OUTSTANDING_PER_THREAD];
		if (side_active) {
		for (int i=0; i<side_iters; i++)
		{
		  tmp[i] = (*((float*)(src_side_ptr)));
		  src_side_ptr += side_src_iter_inc;
		}	
		}
		CUDADMA_BASE::wait_for_dma_start();
		if (side_active) { 
		for (int i=0; i<side_iters; i++)
		{
		  (*((float*)(dst_side_ptr))) = tmp[i];
		  dst_side_ptr += side_dst_iter_inc;
		}
		}
		break;
	      }
	    case 8:
	      {
		float2 tmp[MAX_LDS_OUTSTANDING_PER_THREAD];
		if (side_active) {
		for (int i=0; i<side_iters; i++)
		{
		  tmp[i] = (*((float2*)(src_side_ptr)));
		  src_side_ptr += side_src_iter_inc;
		}	
		}
		CUDADMA_BASE::wait_for_dma_start();
		if (side_active) {
		for (int i=0; i<side_iters; i++)
		{
		  (*((float2*)(dst_side_ptr))) = tmp[i];
		  dst_side_ptr += side_dst_iter_inc;
		}
		}
		break;
	      }
	    case 16:
	      {
		float4 tmp[MAX_LDS_OUTSTANDING_PER_THREAD];
		if (side_active) {
		for (int i=0; i<side_iters; i++)
		{
		  tmp[i] = (*((float4*)(src_side_ptr)));
		  src_side_ptr += side_src_iter_inc;
		}	
		}
		CUDADMA_BASE::wait_for_dma_start();
		if (side_active) {
		for (int i=0; i<side_iters; i++)
		{
		  (*((float4*)(dst_side_ptr))) = tmp[i];
		  dst_side_ptr += side_dst_iter_inc;
		}
		}
		break;
	      }
#ifdef CUDADMA_DEBUG_ON
	    default:
	      printf("Warning CUDA_DMA internal error, invalid side xfer size: %d\n", SIDE_XFER_SIZE);
	      break;
#endif
	  } 
  }
public:
  // Public DMA-thread Synchronization Functions:
  __device__ __forceinline__ void wait_for_dma_start() const
  {
    CUDADMA_BASE::wait_for_dma_start();
  }
  __device__ __forceinline__ void finish_async_dma() const
  {
    CUDADMA_BASE::finish_async_dma();
  }
};
//#undef MAX_BYTES_PER_ROW
#undef ROW_BYTES
#undef MAX_WARPS_PER_ROW
#undef ROWS_PER_STEP
#undef ROW_ID
#undef WARPS_PER_ROW
#undef CUDADMA_WARP_TID
#undef CUDADMAHALO_DMA1_ITER_OFFS
#undef CUDADMAHALO_DMA2_ITER_OFFS
#undef CUDADMAHALO_DMA3_ITER_OFFS
#undef CUDADMAHALO_DMA4_ITER_OFFS
#undef CUDADMAHALO_DMA1_OFFS
#undef CUDADMAHALO_DMA2_OFFS
#undef CUDADMAHALO_DMA3_OFFS
#undef CUDADMAHALO_DMA4_OFFS
#undef SIDE_BYTES
#undef SIDE_XFER_SIZE
#undef TOTAL_SIDE_LOADS
#undef THREADS_PER_SIDE
#undef SIDES_PER_WARP
#undef REMAINING_WARPS
    
//////////////////////////////////////////////////////////////////////////////////////////////////
// CudaDMACustom
//////////////////////////////////////////////////////////////////////////////////////////////////

class cudaDMACustom : public CUDADMA_BASE {

  __device__ cudaDMACustom (const int dmaID,
				const int num_dma_threads,
				const int num_compute_threads,
				const int dma_threadIdx_start)
    : CUDADMA_BASE (dmaID,
		    num_dma_threads,
		    num_compute_threads,
		    dma_threadIdx_start, 
		    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
    {
    }

  public:
  // Public DMA-thread Synchronization Functions:
  __device__ __forceinline__ void wait_for_dma_start() const
  {
    CUDADMA_BASE::wait_for_dma_start();
  }
  __device__ __forceinline__ void finish_async_dma() const
  {
    CUDADMA_BASE::finish_async_dma();
  }
};

#undef CUDADMA_BASE
#undef MAX_BYTES_OUTSTANDING_PER_THREAD
#undef MAX_LDS_OUTSTANDING_PER_THREAD
#undef CUDADMA_DMA_TID
#undef WARP_SIZE

// EOF

