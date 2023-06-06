#!/usr/bin/env bash

set -e

usage() {
    cat >&2 << EOF
Usage: $0 <target> <n>
    - n must be between 1 and 10
EOF
    exit 1
}

if [ $# -ne 2 ]; then
    usage
fi
target="$1"
name="./program"

if [ -f "$target" ]; then
    rm "$target"
fi

make NAME="$name"
"$name" "$target" "$2"

cat "$target"
