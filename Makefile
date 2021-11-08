ifndef MLIR_BIN_DIR
$(error Please set the MLIR binary directory (MLIR_BIN_DIR))
endif

ifndef ONNXMLIR_DIR
$(error Please set the ONNX-MLIR directory (ONNXMLIR_DIR))
endif

ifndef ONNXMLIR_BIN_DIR
$(error Please set the ONNX-MLIR binary directory (ONNXMLIR_BIN_DIR))
endif

CXX = clang++
CXX_FLAGS = -std=c++14 -g
LIBPNG = -lpng

SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin
MODELS_DIR := models

SRC := $(wildcard $(SRC_DIR)/*.cxx)
OBJS := $(patsubst $(SRC_DIR)/%.cxx, $(OBJ_DIR)/%.o, $(SRC))
MLIR_FILES := $(filter-out %.opt.mlir, $(wildcard $(MODELS_DIR)/*.mlir))
MLIR_OBJS := $(patsubst %.mlir, %.o, $(MLIR_FILES))
MODELS_OBJS := $(patsubst $(MODELS_DIR)/%.o, $(OBJ_DIR)/%.o, $(MLIR_OBJS))

# ONNX-MLIR Runtime
ONNX_SRC := $(filter-out $(SRC_DIR)/main.cxx, $(wildcard $(SRC_DIR)/*.cxx))
ONNX_OBJS := $(filter-out $(OBJ_DIR)/main.o, $(patsubst $(SRC_DIR)/%.cxx, $(OBJ_DIR)/%.o, $(SRC)))
ONNX_MODELS := $(wildcard $(MODELS_DIR)/*.onnx)
ONNX_SO := $(patsubst %.onnx, %.so, $(ONNX_MODELS))

onnx-mlir: onnx-mlir-models bench-onnx-mlir

onnx-mlir-models: BASH:=/bin/bash
onnx-mlir-models:
	$(BASH) scripts/compile-onnx-models.sh $(ONNXMLIR_BIN_DIR) $(MODELS_DIR) $(OBJ_DIR)

bench-onnx-mlir: $(ONNX_OBJS)
	${CXX} $(CXX_FLAGS) -o $(BIN_DIR)/bench-onnx-mlir $^ $(LIBPNG) $(ONNX_SO)

# AOT
all: models main

models: BASH:=/bin/bash
models:
	$(BASH) scripts/compile-models.sh $(MLIR_BIN_DIR) $(MODELS_DIR) $(OBJ_DIR)

main: $(OBJS)
	${CXX} $(CXX_FLAGS) -o $(BIN_DIR)/main $^ $(LIBPNG) $(MODELS_OBJS)

# Compile source files to object
$(OBJ_DIR)/%.o:$(SRC_DIR)/%.cxx
	$(CXX) $(CXX_FLAGS) -I ./ -I $(ONNXMLIR_DIR)/include -c -o $@ $<

clean:
	rm obj/*
	rm $(MODELS_DIR)/*.opt.mlir
