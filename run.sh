#!/bin/bash

sol="lru_policy_1_solution"

if ! [[ $1 =~ ^[0-9]+$ ]]; then
    first_test=1
else
    first_test=$1
fi
if ! [[ $2 =~ ^[0-9]+$ ]]; then
    last_test=14
else
    last_test=$2
fi

./build/src/main "$sol" "$first_test" "$last_test"