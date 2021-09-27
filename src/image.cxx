#include "inc/image.hxx"

#include <fstream>
#include <iostream>
#include <stdexcept>

#include "png.h"

bool image::readpng(const std::string& filepath) {
   FILE *file = fopen(filepath.c_str(), "rb");
   if (!file) {
      return false;
   }
   png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
   if (!png_ptr) {
      fclose(file);
      return false;
   }
   png_infop info_ptr = png_create_info_struct(png_ptr);
   if (!info_ptr) {
      png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
      fclose(file);
      return false;
   }
   png_infop end_info = png_create_info_struct(png_ptr);
   if (!end_info) {
      png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
      fclose(file);
      return false;
   }
   if (setjmp(png_jmpbuf(png_ptr))) {
      png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
      fclose(file);
      return false;
   }

   png_init_io(png_ptr, file);
   png_read_info(png_ptr, info_ptr);

   // read width
   width = png_get_image_width(png_ptr, info_ptr);
   // read height
   height = png_get_image_height(png_ptr, info_ptr);
   // read channels
   channels = png_get_channels(png_ptr, info_ptr);

   // [TODO] Support other color types (RGBA, GRAY, ...)
   png_byte color_type = png_get_color_type(png_ptr, info_ptr);
   if (color_type != PNG_COLOR_TYPE_RGB) {
      png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
      fclose(file);
      return false;
   }

   row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
   for (size_t h = 0; h < height; h++) {
      row_pointers[h] = (png_byte*)malloc(png_get_rowbytes(png_ptr, info_ptr));
   }
   png_read_image(png_ptr, row_pointers);

   // Clean
   fclose(file);
   png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);

   return true;
}


image::image(const std::string& filepath) {
   bool ok = readpng(filepath);
   if (!ok) {
      throw
         std::runtime_error("Error reading png file.");
   }
}

//image::to_memref() {
//}
