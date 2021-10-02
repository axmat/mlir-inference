# Usage: gen-images.py -shape [Shape] -batch [Batch size] -color [Color type] \
#           -name [Name of the generated images] -out [Output directory]

import argparse
import numpy as np
from PIL import Image

parser = argparse.ArgumentParser(description='Generate PNG images')
parser.add_argument('-shape', type=str)
parser.add_argument('-batch', type=int, default=1)
parser.add_argument('-color', type=str, default='RGB')
parser.add_argument('-name', type=str)
parser.add_argument('-out', type=str)
args = parser.parse_args()

def main():
   # Get arguments
   shape = [int(dim) for dim in args.shape.split('x')]
   batch_size = args.batch
   color = args.color
   if color != 'RGB':
      raise ValueError(f'{color} color model not supported.')
   name = args.name
   out_dir = args.out
   for batch in range(batch_size):
     # Generate a random image
     if color == 'RGB':
        img = np.random.rand(*shape) * 255
        img = img.astype('uint8')
     img = Image.fromarray(img).convert(color)
     # Save the image
     img.save(f'{out_dir}/{name}-{batch}.png')

if __name__=="__main__":
   main()
