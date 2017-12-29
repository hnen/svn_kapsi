#!/bin/bash
FILES=*.jpeg
for f in $FILES
do
  f="${f%.*}"
  echo "Processing $f..."
  convert $f.jpeg -resize 256x256\! $f.jpeg
done
