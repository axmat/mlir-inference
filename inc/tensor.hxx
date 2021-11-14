#pragma once

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <algorithm>

#include "inc/image.hxx"

// Generic tensor/ranked memref data structure
// The storage order is NCHW
template <typename T, std::size_t N> class tensor {
 public:
   // Constructors
   tensor(const T *data, intptr_t offset, intptr_t sizes[N]);
   tensor(intptr_t sizes[N]);
   // Constructor from an image
   tensor(const image &img, intptr_t sizes[N]);
   // Constructor from images
   // Assume that all the images have the same shape
   tensor(const std::vector<image> &imgs, intptr_t sizes[N]);
   // Desctrutor
   ~tensor();
   void normalize(const std::vector<T>&, const std::vector<T>&);

   // Permute the dimensions of the tensor
   tensor<T,N> transpose(const std::vector<size_t>& );
   // Getters
   T* getData() { return allocated;}
   const intptr_t* getSizes() { return sizes;}
   const intptr_t* getStrides() { return strides;}
   // Get the element at index
   T at(size_t index) const {return allocated[index];}
   T& at(size_t index) {return allocated[index];}

protected:
   // Set the strides from the shape
   void setStrides();
   // Set the shape
   void setShape();

private:
   // Allocated pointer
   T *allocated;
   // Aligned pointer
   T *aligned;
   // Offset
   intptr_t offset;
   // Shape of the tensor
   intptr_t sizes[N];
   // Strides
   intptr_t strides[N];
   // Shape of the image
   intptr_t batch;
   intptr_t channels;
   intptr_t height;
   intptr_t width;
};

template <typename T, std::size_t N>
tensor<T, N>::tensor(const T *data, intptr_t offset, intptr_t sizes[N]) {
   static_assert(N >= 1 && N <= 4, "Tensor size not supported");
   this->offset = offset;
   for (size_t i = 0; i < N; i++) {
      this->sizes[i] = sizes[i];
   }
   setShape();
   setStrides();
   size_t size = batch * channels * height * width;
   allocated = new T[size];
   for (size_t i = 0; i < size; i++) {
      allocated[i] = data[i];
   }
   aligned = allocated;
}

template <typename T, std::size_t N> tensor<T, N>::tensor(intptr_t sizes[N]) {
   static_assert(N >= 1 && N <= 4, "Tensor size not supported");
   offset = 0;
   for (size_t i = 0; i < N; i++) {
      this->sizes[i] = sizes[i];
   }
   setShape();
   setStrides();
   size_t size = batch * channels * height * width;
   allocated = new T[size];
   aligned = allocated;
}

template <typename T, std::size_t N>
tensor<T, N>::tensor(const image &img, intptr_t sizes[N]) {
   static_assert(N >= 1 && N <= 4, "Image size not supported");
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
   allocated = new T[size];
   for (size_t h = 0; h < height; h++) {
      for (size_t w = 0; w < width * channels; w += channels) {
         for (size_t c = 0; c < channels; c++) {
            size_t offset = c * strides[0] + h * strides[1] + (w/channels) * strides[2];
            allocated[offset] =
                static_cast<T>(img.row_pointers[h][w + c]);
         }
      }
   }
   aligned = allocated;
   // Normalize image data to [0,1] range
   if (img.color_type == PNG_COLOR_TYPE_RGB) {
      for (size_t i = 0; i < size; i++) {
         allocated[i] /= 255.0f;
      }
   }
}

template <typename T, std::size_t N>
tensor<T, N>::tensor(const std::vector<image> &imgs, intptr_t sizes[N]) {
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
   allocated = new T[size];
   for (size_t b = 0; b < batch; b++) {
      for (size_t h = 0; h < height; h++) {
         for (size_t w = 0; w < width * channels; w += channels) {
            for (size_t c = 0; c < channels; c++) {
               size_t offset = b * strides[0] + c * strides[1] +
                                      h * strides[2] +
                                      (w / channels) * strides[3];
               allocated[offset] =
                   static_cast<T>(imgs[b].row_pointers[h][w + c]);
            }
         }
      }
   }
   aligned = allocated;
}

template <typename T, std::size_t N> tensor<T, N>::~tensor() {
   if (allocated == aligned) {
      if (allocated)
         delete[] allocated;
   } else {
      if (allocated)
         delete[] allocated;
      if (aligned)
         delete[] aligned;
   }
}

template <typename T, std::size_t N> void tensor<T, N>::setStrides() {
   strides[N - 1] = 1;
   for (long i = N - 2; i >= 0; i--) {
      strides[i] = strides[i + 1] * sizes[i + 1];
   }
}

template <typename T, std::size_t N> void tensor<T, N>::setShape() {
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

template<typename T, std::size_t N> void tensor<T, N>::normalize(const std::vector<T>& mean, const std::vector<T>& std) {
   if (mean.size() != N || std.size() != N) {
      throw
         std::runtime_error("Invalid vector size.");
   }
   static_assert(N == 3 || N == 4, "Currently only support 3D and 4D tensor.");
   if (N == 4) {
      for (std::size_t b = 0; b < batch; b++) {
         for (std::size_t c = 0; c < channels; c++) {
            for (std::size_t h = 0; h < height; h++) {
               for (std::size_t w = 0; w < width; w++) {
                  size_t offset = b * strides[0] + c * strides[1] + h * strides[2] + w * strides[3];
                  allocated[offset] = (allocated[offset] - mean[c]) / std[c];
               }
            }
         }
      }
   } else if (N == 3) {
      for (std::size_t c = 0; c < channels; c++) {
         for (std::size_t h = 0; h < height; h++) {
            for (std::size_t w = 0; w < width; w++) {
               size_t offset = c * strides[0] + h * strides[1] + w * strides[2];
               allocated[offset] = (allocated[offset] - mean[c]) / std[c];
            }
         }
      }
   }
}

template<typename T, std::size_t N>
tensor<T, N> tensor<T, N>::transpose(const std::vector<size_t>& dims) {
   if (dims.size() != N) {
      throw
         std::runtime_error("Invalid dims value.");
   }
   auto axes = dims;
   std::sort(axes.begin(), axes.end(), [](size_t i, size_t j){ return i < j;});
   if (axes[0] != 0 || axes[N-1] == N) {
      throw
         std::runtime_error("All dimensions must be in range 0..N-1");
   }

   intptr_t newSizes[N];
   using namespace std;
   for (size_t i = 0; i < N; i++) {
      newSizes[i] = sizes[dims[i]];
   }
   auto t = tensor<T, N>(newSizes);
   auto newStrides = t.getStrides();
   static_assert(N==3, "Currently only support 3D tensor.");
   // Copy the data
   if (N == 3) {
      auto getNewDim = [&dims](size_t index, size_t c, size_t h, size_t w) {
         if (dims[index] == 0)
            return c;
         else if (dims[index] == 1)
            return h;
         else
            return w;
      };
      for (std::size_t c = 0; c < sizes[0]; c++) {
         for (std::size_t h = 0; h < sizes[1]; h++) {
            for (std::size_t w = 0; w < sizes[2]; w++) {
               size_t oldOffset = c * strides[0] + h * strides[1] + w * strides[2];
               size_t newOffset = getNewDim(0, c, h, w) * newStrides[0] + getNewDim(1, c, h, w) * newStrides[1] + getNewDim(2, c, h, w) * newStrides[2];
               t.at(newOffset) = this->at(oldOffset);
            }
         }
      }
   }

   return t;
}

