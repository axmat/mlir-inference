CXX = g++
CXX_FLAGS = -std=c++14
LIBPNG = -lpng

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

SRC = ${wildcard $(SRC_DIR)/*.cxx}
OBJS = $(patsubst $(SRC_DIR)/%.cxx, $(OBJ_DIR)/%.o, $(SRC))

ONNXMLIR_DIR = 
MLIR_DIR = 

main: $(OBJS)
	${CXX} $(CXX_FLAGS) -o $(BIN_DIR)/main $^ $(LIBPNG)

$(OBJ_DIR)/%.o:$(SRC_DIR)/%.cxx
	$(CXX) $(CXX_FLAGS) -I ./ -c -o $@ $<


clean:
	rm obj/*
