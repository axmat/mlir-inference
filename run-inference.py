import argparse
import numpy as np
from PyRuntime import ExecutionSession
from PIL import Image

parser = argparse.ArgumentParser(description='Run inference on onnx models using ONNX Mlir Runtime')
parser.add_argument('-model', type=str)
parser.add_argument('-input', type=str)
args = parser.parse_args()

def main():
  # Get the arguments
  model_path = args.model
  input_path = args.input
  # Create a session
  session = ExecutionSession(model_path, 'run_main_graph')
  # Prepare the input
  input_data = np.asarray(Image.open(input_path)).astype('float32') / 255.0
  input_data = np.transpose(input_data, [2, 0, 1])
  # Run inference
  outputs = session.run(input_data)
  # Get the output
  output = outputs[0]
  # Print the output
  print(output)

if __name__=="__main__":
  main()
