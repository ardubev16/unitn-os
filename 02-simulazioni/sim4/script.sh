#!/usr/bin/env bash

set -e

usage() {
    cat >&2 << EOF
Usage: $0 <logfile>
EOF
    exit 1
}

if [ $# -ne 1 ]; then
    usage
fi

if [ ! -f "$1" ]; then
    echo "File $1 does not exist."
    exit 1
fi

cnt=0
while IFS= read -r line; do
    if [[ $((cnt % 2)) == 0 ]]; then
        echo "$line"
    else
        echo "$line" >&2
    fi
    ((cnt += 1))
done < "$1"
