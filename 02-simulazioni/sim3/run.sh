#!/usr/bin/env bash

set -e

usage() {
    cat >&2 << EOF
Usage: $0 <mode> <logfile>
    - mode must be either "server" or "client"
EOF
    exit 1
}

if [ $# -ne 2 ]; then
    usage
fi
name="./app"

make NAME="$name"
"$name" "$1" "$2"
