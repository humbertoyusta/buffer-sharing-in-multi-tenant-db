#!/bin/bash

sol="lru_policy_1_solution"

first_test=$1
last_test=$2

for i in $(seq "$first_test" "$last_test")
do
    echo "Running test $i"
    ./build/src/main "$sol" "$i"
done