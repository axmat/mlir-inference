# Usage : compile-onnx-models.sh [MLIR binary directory] [Models directory] [Object directory]

ONNXMLIR_BIN_DIR=$1
MODELS_DIR=$2
OBJ_DIR=$3

echo "ONNX-MLIR binary dir: $ONNXMLIR_BIN_DIR"
echo "Models dir: $MODELS_DIR"
echo "Objects dir: $OBJ_DIR"

# [TODO] Download ONNX models

# Compile mlir models in models directory
echo "Compiling ONNX models"
for filename in $(ls $MODELS_DIR | grep -i '^[a-zA-Z]*[a-zA-Z0-9].\.onnx'); do
	model=$MODELS_DIR/$filename
	library=$MODELS_DIR/$(echo $filename | sed 's/\.onnx//')
	echo "   Compiling $model"
	$ONNXMLIR_BIN_DIR/onnx-mlir --EmitLib $model -o $library
done
