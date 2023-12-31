#!/bin/bash

sol=$1

if ! [[ $2 =~ ^[0-9]+$ ]]; then
    first_test=1
else
    first_test=$2
fi
if ! [[ $3 =~ ^[0-9]+$ ]]; then
    last_test=14
else
    last_test=$3
fi

./build/src/main "$sol" "$first_test" "$last_test"