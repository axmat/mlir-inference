
#include <iostream>
#include <string>

#include "inc/tensor.hxx"

// MLIR functions
// _mlir_ciface_modelname
extern "C" void _mlir_ciface_test(tensor<3>* input, tensor<2>* output);

// Run inference on mlir
template<std::size_t N, std::size_t M>
void runInference(tensor<N>& input, tensor<M>& output) {
   _mlir_ciface_test(&input, &output);
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
