#!/bin/sh -e

MAX_TESTS=${1:-300}
MAX_SECONDS=${2:-60}

HERE=$(pwd)
DIR=$(mktemp -d)

trap "rm -rf $DIR" EXIT

cd "$DIR" && {
    ln -s "$HERE/kernels" kernels

    cc -O3 -march=native -D NDEBUG -D RUN_NAIVE kernels/c_kernel.c -o naive

    echo "Naive: $(./naive)"

    printf "Searching through approximately %s configurations..." "$($HERE/autotuner.py --print-search-space-size)"

    "$HERE/autotuner.py" --no-dups --stop-after="$MAX_SECONDS" --test-limit="$MAX_TESTS"

    mv mmm_final_config.json "$HERE/mmm_final_config.json"
}
