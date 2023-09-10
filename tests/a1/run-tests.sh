#!/bin/bash
# >>> ./run-tests.sh public ../../reference/mitscript-parser

ROOT=".."
TESTS="$1"
PARSER="$2"

TOTAL=0
COUNT=0

for filename in "$ROOT/a1/$TESTS"/bad*.mit; do
    OUT=$($PARSER $filename)
    CODE=$?
    if [[ $CODE -eq 0 ]] && [[ $OUT != *"Error"* ]]; then
        echo "Fail: $(basename $filename) (exit code $CODE)"
    else
        COUNT=$((COUNT + 1))
    fi
    TOTAL=$((TOTAL + 1))
done

for filename in "$ROOT/a1/$TESTS"/good*.mit; do
    OUT=$($PARSER $filename)
    CODE=$?
    if [[ $CODE -ne 0 ]] || [[ $OUT = *"Error"* ]]; then
        echo "Fail: $(basename $filename) (exit code $CODE)"
    else
        COUNT=$((COUNT + 1))
    fi
    TOTAL=$((TOTAL + 1))
done

echo "$PARSER Passed $COUNT out of $TOTAL tests"
