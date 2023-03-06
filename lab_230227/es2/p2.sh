#!/usr/bin/env bash

input=($(ls))

args=""
for arg in "${input[@]}"; do
    args="$arg $args"
done
echo "$args"
