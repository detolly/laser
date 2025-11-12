#!/bin/bash

magick $1 $1.pnm \
&& potrace --svg $1.pnm  -o tmp.svg --flat -z majority -a 1 -t 25 -k 0.5 \
&& python scripts/svg_to_points.py tmp.svg > $2 \
&& rm $1.pnm tmp.svg


