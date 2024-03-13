#!/bin/bash

executable=$1

if [ $# = 1 ]; then
  ./build/src/$executable
  exit 0
fi

sol=$2

if [ $# = 2 ]; then
  ./build/src/$executable "$sol"
  exit 0
fi

test_type=$3

if [ $# = 3 ]; then
  ./build/src/$executable "$sol" "$test_type"
  exit 0
fi

first_test=$4

if [ $# = 4 ]; then
  ./build/src/$executable "$sol" "$test_type" "$first_test"
  exit 0
fi

last_test=$5

if [ $# = 5 ]; then
  ./build/src/$executable "$sol" "$test_type" "$first_test" "$last_test"
  exit 0
fi