#!/usr/bin/env bash

# skip first argument
sizes=(10 100 1000)

for arg; do
    echo "============ Testing $arg ============"
    for size in "${sizes[@]}"; do
        time ./$arg $size >/dev/null
    done
    echo "======================================"
done
