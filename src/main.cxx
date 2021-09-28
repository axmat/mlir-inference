
#include <iostream>
#include <string>

#include "inc/tensor.hxx"

// MLIR function
// extern "C" memref* __mlir_ciface_main_graph(memref* input);

// Run inference on mlir
template<std::size_t N, std::size_t M>
void runInference(tensor<N>& input, tensor<M>& output) {
   memref memrefIn = input.view();
   memref memrefOut = output.view();
   //memrefOut = __mlir_ciface_main_graph(memrefIn);
}

int main(int argc, char **argv) {
   if (argc != 2) {
      std::cout << "Usage: main <path to input image>" << std::endl;
      return 1;
   }
   // Read input image
   std::string filepath(argv[1]);
   image img(filepath);

   intptr_t sizes_in[3] = {3, 224, 224};
   tensor<3> input(img, sizes_in);

   intptr_t sizes_out[2] = {1, 1000};
   tensor<2> output(sizes_out);

   runInference<3,2>(input, output);

   return 0;
}
