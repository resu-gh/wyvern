#!/bin/sh

export CC="clang"
export CXX="clang++"

fd -e cpp -e hpp | \
entr bazelisk run //:wyvcc  -- \
-i $(pwd)/unit/grammars/json.g
