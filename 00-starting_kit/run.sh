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

# NOTE: read line by line, and print to stdout and stderr alternatively
# cnt=0
# while IFS= read -r line; do
#     if [[ $((cnt % 2)) == 0 ]]; then
#         echo "$line"
#     else
#         echo "$line" >&2
#     fi
#     ((cnt += 1))
# done < "$1"
