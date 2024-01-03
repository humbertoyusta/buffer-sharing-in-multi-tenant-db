#!/bin/bash

executable=$1

sol=$2

if ! [[ $3 =~ ^[0-9]+$ ]]; then
    first_test=1
else
    first_test=$3
fi
if ! [[ $4 =~ ^[0-9]+$ ]]; then
    last_test=14
else
    last_test=$4
fi

./build/src/$executable "$sol" "$first_test" "$last_test"