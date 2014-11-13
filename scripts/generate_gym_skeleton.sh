#!/bin/sh

if [ $# -ne 1 ]; then
    echo "Error: one input parameter expected, the MODULE_PREFIX"
    exit 1
fi



find . -maxdepth 2 -type f -not -path '*/\.*' -exec sed -i -e "s/_MODULE_PREFIX_/$1/g" {} \;
find . -maxdepth 2 -type f -not -path '*/\.*' -not -name "CMakeLists.txt" -not -name "README.md" -exec bash -c 'dir=$(dirname $0) && file=$(basename $0) && mv $0 "$dir/$1_$file"' {} $1 \;