#!/usr/bin/env bash

args=""
for arg in "$@"; do
    args="$arg $args"
done
echo "$args"
