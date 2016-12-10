#!/bin/bash

input="$1"
output="$2"
options="if=$input of=$output bs=8 conv=notrunc"

cp "$input" "$output"
dd $options count=32 skip=32
dd $options count=32 seek=32
dd $options count=32 seek=64 skip=256

