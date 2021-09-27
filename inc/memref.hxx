#pragma once

#include <cstddef>
#include <cstdint>
#include <iostream>

#include "inc/image.hxx"

// Memref descriptor
template<std::size_t N>
class memref {
public:
   memref(intptr_t batch, intptr_t channels, intptr_t height, intptr_t width,
          intptr_t offset, intptr_t sizes[N],
          intptr_t strides[N]);
   memref(intptr_t batch, intptr_t channels, intptr_t height, intptr_t width,
          float *aligned, intptr_t offset, intptr_t sizes[N],
          intptr_t strides[N]);
   memref(const image& img, 
   intptr_t sizes[N], intptr_t strides[N]);
   ~memref();

private:
   // Allocated pointer
   float *allocated;
   // Aligned pointer
   float *aligned;
   // Offset
   intptr_t offset;
   // Shape
   intptr_t batch;
   intptr_t channels;
   intptr_t height;
   intptr_t width;
   intptr_t sizes[N];
   // Strides
   intptr_t strides[N];
};

template<std::size_t N>
memref<N>::memref(intptr_t batch, intptr_t channels, intptr_t height, intptr_t width,
   intptr_t offset,
   intptr_t sizes[N], intptr_t strides[N]) {
   this->batch = batch;
   this->channels = channels;
   this->height = height;
   this->width = width;
   this->offset = offset;
   for (size_t i = 0; i < N; i++) {
      this->sizes[i] = sizes[i];
      this->strides[i] = strides[i];
   }
   this->allocated = new float[1];
   size_t size = batch * channels * height * width;
   this->aligned = new float[size];
}

template<std::size_t N>
memref<N>::memref(intptr_t batch, intptr_t channels, intptr_t height, intptr_t width, float* aligned,
   intptr_t offset,
   intptr_t sizes[N], intptr_t strides[N]) {
   this->batch = batch;
   this->channels = channels;
   this->height = height;
   this->width = width;
   this->offset = offset;
   for (size_t i = 0; i < N; i++) {
      this->sizes[i] = sizes[i];
      this->strides[i] = strides[i];
   }
   this->allocated = new float[1];
   size_t size = batch * channels * height * width;
   this->aligned = new float[size];
   for (size_t i = 0; i < size; i++) {
      this->aligned[i] = aligned[i];
   }
}

template<std::size_t N>
memref<N>::memref(const image& img, intptr_t sizes[N], intptr_t strides[N]) {
   this->batch = 1;
   this->channels = img.channels;
this->height = img.height;
this->width = img.width;
   size_t size = channels * height * width;
   allocated = new float[1];
   aligned = new float[size];
   for (size_t h = 0; h < height; h++) {
      for(size_t w = 0; w < width * channels; w+=channels) {
         for (size_t c = 0; c < channels; c++) {
            size_t offset = c * strides[1] + h * strides[2] + (w/channels) * strides[3];
            aligned[offset] = (float)img.row_pointers[h][w + c];
         }
      }
   }

   for (size_t i =0 ; i < N; i++) {
      this->sizes[i] = sizes[i];
      this->strides[i] = strides[i];
   }

}

template<std::size_t N>
memref<N>::~memref() {
   if (allocated)
      delete[] allocated;
   if (aligned)
      delete[] aligned;
}

