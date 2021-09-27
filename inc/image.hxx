#pragma once

#include <cstddef>
#include <string>

#include "png.h"

class image {
 private:
   size_t height;
   size_t width;
   size_t channels;
   png_bytep *row_pointers = nullptr;

   bool readpng(const std::string &filepath);

 public:
   image() = delete;
   image(const std::string &filepath);
   ~image(){};

   template<std::size_t> friend class memref;
};
