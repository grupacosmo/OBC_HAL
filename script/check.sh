#!/bin/bash

find src lib -name *.cpp -o -name *.hpp \
    | xargs clang-format -Werror --verbose --dry-run

run-clang-tidy -p build src lib -quiet