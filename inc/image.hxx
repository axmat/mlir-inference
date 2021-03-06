#pragma once

#include <cstddef>
#include <string>

#include "png.h"

class image {
 private:
   // Color type of the image
   png_byte color_type;
   // Shape of the image
   size_t channels;
   size_t height;
   size_t width;
   // Row pointers
   png_bytep *row_pointers = nullptr;
   // Read a png image
   bool readpng(const std::string &filepath);

 public:
   // Constructor
   image() = delete;
   image(const std::string &filepath);
   // Destructor
   ~image(){};

   template <typename T, std::size_t> friend class tensor;
};
