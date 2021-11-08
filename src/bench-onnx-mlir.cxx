#include <iostream>
#include <string>

#include "inc/tensor.hxx"

#include "OnnxMlirRuntime.h"

extern "C" OMTensorList* run_main_graph(OMTensorList*);
extern "C" OMTensor* omTensorCreate(void*, intptr_t*, intptr_t, OM_DATA_TYPE);
extern "C" OMTensorList* TensorListCreate(OMTensorList**, int);
extern "C" void omTensorListDestroy(OMTensorList *);
extern "C" OMTensor* omTensorListGetOmtByIndex(OMTensorList*, size_t);
extern "C" void* omTensorGetDataPtr(OMTensor*);

static std::string imagesDir = "images/";

void mobilenet();

int main(int argc, char **argv) {

   mobilenet();

   return 0;
}

void mobilenet() {
   std::string filepath(imagesDir + "224x224x3.png");
   // Import input image
   image img(filepath);
   // Create tensor
   intptr_t sizes_in[3] = {3, 224, 224};
   tensor<3> input(img, sizes_in);
   // Normalize
   input.normalize({0.485, 0.456, 0.406}, {0.229, 0.224, 0.225});
   // Run inference using onnx-mlir runtime
   intptr_t shape[4] = {1, 224, 224, 3};
   OMTensor *inputTensor = omTensorCreate(input.getAlignedPtr(), shape, 4, ONNX_TYPE_FLOAT);
   OMTensorList *inputList = omTensorListCreate(&inputTensor, 1);
   OMTensorList *outputList = run_main_graph(inputList);
   OMTensor* outputTensor = omTensorListGetOmtByIndex(outputList, 0);
   float* outputPtr = (float*) omTensorGetDataPtr(outputTensor);
   // Clean
   omTensorListDestroy(outputList);
}

