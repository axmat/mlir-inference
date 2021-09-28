#pragma once

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <vector>

#include "inc/image.hxx"
#include "inc/memref.hxx"

// tensor
// When the strides are not provided, it assumes that the input is in NCHW, CHW,
// HW or W format
template <std::size_t N> class tensor {
 public:
   // Constructors
   tensor(const float *data, intptr_t offset, intptr_t sizes[N]);
   tensor(const float *data, intptr_t offset, intptr_t sizes[N],
          intptr_t strides[N]);
   tensor(intptr_t sizes[N]);
   tensor(intptr_t sizes[N], intptr_t strides[N]);
   ;
   // Constructor from an image
   tensor(const image &img, intptr_t sizes[N]);
   // Constructor from images
   // Assume that all the images have the same shape
   tensor(const std::vector<image> &imgs, intptr_t sizes[N]);
   // Desctrutor
   ~tensor();
   // Create a memref descriptor
   memref view() {
      return memref(allocated, aligned, offset, sizes, strides, false);
   };

 private:
   // Set the strides from the shape
   void setStrides();
   // Set the shape
   void setShape();

   // Allocated pointer
   float *allocated;
   // Aligned pointer
   float *aligned;
   // Offset
   intptr_t offset;
   // Shape of the image
   intptr_t batch;
   intptr_t channels;
   intptr_t height;
   intptr_t width;
   // Shape of the tensor
   intptr_t sizes[N];
   // Strides
   intptr_t strides[N];
};

template <std::size_t N>
tensor<N>::tensor(const float *data, intptr_t offset, intptr_t sizes[N]) {
   static_assert(N >= 1 && N <= 4, "Tensor size not supported");
   this->offset = offset;
   for (size_t i = 0; i < N; i++) {
      this->sizes[i] = sizes[i];
   }
   setShape();
   setStrides();
   allocated = new float[1];
   size_t size = batch * channels * height * width;
   aligned = new float[size];
   for (size_t i = 0; i < size; i++) {
      aligned[i] = data[i];
   }
}

template <std::size_t N>
tensor<N>::tensor(const float *data, intptr_t offset, intptr_t sizes[N],
                  intptr_t strides[N]) {
   static_assert(N >= 1 && N <= 4, "Tensor size not supported");
   this->offset = offset;
   for (size_t i = 0; i < N; i++) {
      this->sizes[i] = sizes[i];
      this->strides[i] = strides[i];
   }
   setShape();
   allocated = new float[1];
   size_t size = batch * channels * height * width;
   aligned = new float[size];
   for (size_t i = 0; i < size; i++) {
      aligned[i] = data[i];
   }
}

template <std::size_t N> tensor<N>::tensor(intptr_t sizes[N]) {
   static_assert(N >= 1 && N <= 4, "Tensor size not supported");
   offset = 0;
   for (size_t i = 0; i < N; i++) {
      this->sizes[i] = sizes[i];
   }
   setShape();
   setStrides();
   allocated = new float[1];
   size_t size = batch * channels * height * width;
   aligned = new float[size];
}

template <std::size_t N>
tensor<N>::tensor(intptr_t sizes[N], intptr_t strides[N]) {
   static_assert(N >= 1 && N <= 4, "Tensor size not supported");
   offset = 0;
   for (size_t i = 0; i < N; i++) {
      this->sizes[i] = sizes[i];
      this->strides[i] = strides[i];
   }
   setShape();
   allocated = new float[1];
   size_t size = batch * channels * height * width;
   aligned = new float[size];
}

template <std::size_t N>
tensor<N>::tensor(const image &img, intptr_t sizes[N]) {
   static_assert(N == 3, "Image size not supported");
   offset = 0;
   batch = 1;
   channels = img.channels;
   height = img.height;
   width = img.width;
   // Set the shape
   for (size_t i = 0; i < N; i++) {
      this->sizes[i] = sizes[i];
   }
   // Set the strides
   setStrides();
   // Set the data
   size_t size = channels * height * width;
   allocated = new float[1];
   aligned = new float[size];
   for (size_t h = 0; h < height; h++) {
      for (size_t w = 0; w < width * channels; w += channels) {
         for (size_t c = 0; c < channels; c++) {
            size_t alignedOffset =
                c * strides[0] + h * strides[1] + (w / channels) * strides[2];
            aligned[alignedOffset] =
                static_cast<float>(img.row_pointers[h][w + c]);
         }
      }
   }
}

template <std::size_t N>
tensor<N>::tensor(const std::vector<image> &imgs, intptr_t sizes[N]) {
   static_assert(N == 4, "Images size not supported");
   offset = 0;
   batch = imgs.size();
   channels = imgs[0].channels;
   height = imgs[0].height;
   width = imgs[0].width;
   offset = 0;
   // Set the shape
   for (size_t i = 0; i < N; i++) {
      this->sizes[i] = sizes[i];
   }
   // Set the strides
   setStrides();
   // Set the data
   size_t size = batch * channels * height * width;
   allocated = new float[1];
   aligned = new float[size];
   for (size_t b = 0; b < batch; b++) {
      for (size_t h = 0; h < height; h++) {
         for (size_t w = 0; w < width * channels; w += channels) {
            for (size_t c = 0; c < channels; c++) {
               size_t alignedOffset = b * strides[0] + c * strides[1] +
                                      h * strides[2] +
                                      (w / channels) * strides[3];
               aligned[alignedOffset] =
                   static_cast<float>(imgs[b].row_pointers[h][w + c]);
            }
         }
      }
   }
}

template <std::size_t N> tensor<N>::~tensor() {
   if (allocated)
      delete[] allocated;
   if (aligned)
      delete[] aligned;
}

template <std::size_t N> void tensor<N>::setStrides() {
   strides[N - 1] = 1;
   for (long i = N - 2; i >= 0; i--) {
      strides[i] = strides[i + 1] * sizes[i + 1];
   }
}

template <std::size_t N> void tensor<N>::setShape() {
   if (N == 4) { // NCHW
      batch = sizes[0];
      channels = sizes[1];
      height = sizes[2];
      width = sizes[3];
   } else if (N == 3) { // CHW
      batch = 1;
      channels = sizes[0];
      height = sizes[1];
      width = sizes[2];
   } else if (N == 2) { // HW
      batch = 1;
      channels = 1;
      height = sizes[0];
      width = sizes[1];
   } else { // W
      batch = 1;
      channels = 1;
      height = 1;
      width = sizes[0];
   }
}
