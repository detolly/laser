#!/bin/bash

convert $1 $1.pnm
potrace --svg $1.pnm -o tmp.svg
python scripts/svg_to_points.py tmp.svg > $2

rm $1.pnm tmp.svg
