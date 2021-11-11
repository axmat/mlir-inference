# Usage : compile-onnx-models.sh [MLIR binary directory] [Models directory] [Object directory]

ONNXMLIR_BIN_DIR=$1
MODELS_DIR=$2
OBJ_DIR=$3

# Download ONNX models
links=("https://media.githubusercontent.com/media/onnx/models/master/vision/classification/mobilenet/model/mobilenetv2-7.onnx")
names=("mobilenetv2-7.onnx")

for link in ${links[@]}; do
	# Download the file if it doesn't exist
	file=$MODELS_DIR/$(echo $link | grep -Eo '[a-zA-Z0-9-]*\.onnx')
	if [ ! -f "$file" ]; then
		echo Downloading $file
		wget -O $file $link
	fi
done

# Compile mlir models in models directory
echo "Compiling ONNX models"
for filename in $(ls $MODELS_DIR | grep -i '[a-zA-Z0-9-]*.\.onnx'); do
	model=$MODELS_DIR/$filename
	library=$MODELS_DIR/$(echo $filename | sed 's/\.onnx//')
	echo "   Compiling $model"
	$ONNXMLIR_BIN_DIR/onnx-mlir --EmitLib $model -o $library
done
