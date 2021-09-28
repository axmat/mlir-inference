#pragma once

#include <cstddef>
#include <cstdint>

// Memref descriptor
struct memref {
   // Constructor
   memref(){};
   memref(float *allocatedPtr, float *alignedPtr, intptr_t alignedOffset,
          intptr_t *alignedSizes, intptr_t *alignedStrides,
          bool copyData = true)
       : allocated(allocatedPtr), aligned(alignedPtr), offset(alignedOffset),
         sizes(alignedSizes), strides(alignedStrides), copy(copyData) {}
   // Destructor
   ~memref() {
      if (copy) {
         delete[] allocated;
         delete[] aligned;
         delete[] sizes;
         delete[] strides;
      }
   }

   // Allocated pointer
   float *allocated = nullptr;
   // Aligned pointer
   float *aligned = nullptr;
   // Offset
   intptr_t offset = 0;
   // Sizes
   intptr_t *sizes = nullptr;
   // Strides
   intptr_t *strides = nullptr;
   // Copy the pointers
   bool copy = true;
};
