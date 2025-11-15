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

    if ! [ -x "$bin" ]; then
        echo -e $RED"[FAILED] $name: no executable"$RESET
        ((failed += 1))
        continue
    fi

    input=$(grep '//< ' $file | cut -c 5-)
    # Delete trailing CR to make results match on Windows (CRLF -> LF).
    expected_output=$(grep '//> ' $file | cut -c 5- | tr -d '\r')

    # Mac/BSD xargs does not run command if stdin is empty.
    if [ -z "$input" ]; then
        output=$($bin 2>&1)
    else
        output=$(echo "$input" | xargs $bin 2>&1)
    fi

    if [ $? -ne 0 ]; then
        echo -e $RED"[FAILED] $name: non-zero exit code"$RESET
        echo -e $BLUE"Output:"$RESET
        echo "$output"
        ((failed += 1))
    elif ! [ "$(echo "$output" | tr -d '\r')" = "$expected_output" ]; then
        echo -e $RED"[FAILED] $name: wrong output"$RESET
        echo -e $BLUE"Expected:"$RESET
        echo "$expected_output"
        echo -e $BLUE"Found:"$RESET
        echo "$output"
        ((failed += 1))
    else
        echo -e $GREEN"[PASSED] $name"$RESET
        ((passed += 1))
    fi
done

echo ""
echo "Total:"
echo "    Passed: $passed"
echo "    Failed: $failed"

if [ $failed -gt 0 ]; then
    exit 1
fi
