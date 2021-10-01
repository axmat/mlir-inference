ifndef MLIR_BIN_DIR
$(error Please set the MLIR binary directory (MLIR_BIN_DIR))
endif

CXX = clang++
CXX_FLAGS = -std=c++14
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

.PHONY: all

all: models main

models: BASH:=/bin/bash
models:
	$(BASH) compile-models.sh $(MLIR_BIN_DIR) $(MODELS_DIR) $(OBJ_DIR)

main: $(OBJS)
	${CXX} $(CXX_FLAGS) -o $(BIN_DIR)/main $^ $(LIBPNG) $(MODELS_OBJS)

$(OBJ_DIR)/%.o:$(SRC_DIR)/%.cxx
	$(CXX) $(CXX_FLAGS) -I ./ -c -o $@ $<

clean:
	rm obj/*
	rm models/*.opt.mlir
