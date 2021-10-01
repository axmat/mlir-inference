# Usage : compile-models.sh [MLIR binary directory] [Models directory] [Object directory]

MLIR_BIN_DIR=$1
MODELS_DIR=$2
OBJ_DIR=$3

echo "MLIR binary dir: $MLIR_BIN_DIR"
echo "Models dir: $MODELS_DIR"
echo "Objects dir: $OBJ_DIR"

# [TODO] Lower ONNX models to MLIR

# Compile mlir models in models directory
echo "Compiling MLIR models"
for filename in $(ls $MODELS_DIR | grep -i '^[a-zA-Z]*[a-zA-Z0-9].\.mlir'); do
	model=$MODELS_DIR/$filename
	optimized=$MODELS_DIR/$(echo $filename | sed 's/\./.opt./')
	bitcode=$OBJ_DIR/$(echo $filename | sed 's/\.mlir/.bc/')
	object=$OBJ_DIR/$(echo $filename | sed 's/\.mlir/.o/')
	echo "   Lowering $model to the standard dialect"
	$MLIR_BIN_DIR/mlir-opt --lower-affine --convert-scf-to-std --convert-memref-to-llvm \
		--convert-std-to-llvm='emit-c-wrappers=1' \
		$model > $optimized
	echo "   Compiling $optimized to LLVM bitcode"
	$MLIR_BIN_DIR/mlir-translate --mlir-to-llvmir $optimized -o $bitcode
	echo "   Compiling $bitcode to object file"
	$MLIR_BIN_DIR/llc $bitcode -o $object -filetype=obj --relocation-model=pic
done
