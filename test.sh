#!/bin/bash

if [ -z "$FROM_MAKE" ]; then
    echo "Must be called using 'make test'" >&2
    exit 1
fi

if [ $# -ne 2 ]; then
    echo "usage: $(basename $0) <src dir> <out dir>" >&2
    exit 1
fi

SRC_DIR=$1
OUT_DIR=$2

RESET="\033[0m"
RED="\033[1;31m"
GREEN="\033[1;32m"
BLUE="\033[1;34m"

passed=0
failed=0
for file in $(find $SRC_DIR -type f); do
    filename=$(basename $file)
    name=${filename%.*}
    bin="$OUT_DIR/$name"

    input=$(grep '//< ' $file | cut -c 5-)
    # Delete trailing CR to make results match on Windows (CRLF -> LF).
    expected_output=$(grep '//> ' $file | cut -c 5- | tr -d '\r')

    # Mac/BSD xargs does not run command if stdin is empty.
    if [ -z "$input" ]; then
        output=$($bin 2>&1 | tr -d '\r')
    else
        output=$(echo "$input" | xargs $bin 2>&1 | tr -d '\r')
    fi

    if [ $? -eq 0 ] && [ "$output" = "$expected_output" ]; then
        echo -e $GREEN"[PASSED] $name"$RESET
        ((passed += 1))
    else
        echo -e $RED"[FAILED] $name"$RESET
        ((failed += 1))

        echo -e $BLUE"Expected:"$RESET
        echo "$expected_output"
        echo -e $BLUE"Found:"$RESET
        echo "$output"
    fi
done

echo ""
echo "Total:"
echo "    Passed: $passed"
echo "    Failed: $failed"

if [ $failed -gt 0 ]; then
    exit 1
fi
