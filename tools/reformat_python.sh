#!/bin/bash

## dependence
# python -m pip install git+https://github.com/google/yapf.git@main

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
MAGICMIND_ROOT=$SCRIPT_DIR/../
cd $MAGICMIND_ROOT

DEFAULT_ARGS="-i -p -r"
if [ "$#" == 0 ]; then
  echo "Reformatting magicmind/python recursively."
  yapf ${DEFAULT_ARGS} magicmind/python
  echo "All file formatted"
else
  yapf ${DEFAULT_ARGS} ${@:1}
fi

