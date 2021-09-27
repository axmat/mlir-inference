
#include <iostream>
#include <string>

#include "inc/image.hxx"
#include "inc/memref.hxx"

// MLIR function
extern "C" memref<2>* __mlir_ciface_main_graph(memref<4>* input);

//void* __mlir_ciface_main_graph(void* input);

// Run inference on mlir
template<std::size_t N, std::size_t M>
void run_inference(memref<N>* input, memref<M>* output) {
   output = __mlir_ciface_main_graph(input);
}


int main(int argc, char** argv) {
   if (argc != 2) {
      std::cout << "Usage: main <path to input image>" << std::endl;
      return 1;
   }
   std::string filepath(argv[1]);
   image img(filepath);

   intptr_t sizes_in[4] = {1, 3, 224, 224};
   intptr_t strides_in[4] = {3 * 224 * 224, 224 * 224, 224, 1};
   memref<4> input(img, sizes_in, strides_in);

   intptr_t sizes_out[2] = {1, 1000};
   intptr_t strides_out[2] = {1000, 1};
   memref<2> output(1, 1, 1, 1000, 0, sizes_out, strides_out);

   run_inference<4,2>(&input, &output);

   return 0;
}
