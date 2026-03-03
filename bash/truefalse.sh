#!/bin/sh
set -ex

find ~/Workspace/ -maxdepth 1 -type d -name "xperf*" -print0 | while read -d '' -r file; do
  if [[ -n $filename ]]; then
    echo "bad name"
    exit 1
  else
    filename=$file
    echo $filename
  fi
done

if ! $(nvidia-smi); then
  echo "gpu"
else
  echo "no gpu"
fi

check() {
  local var=$(echo "$1" |tr '[:upper:]' '[:lower:]')
  if [[ "true" = "$var" || "1" = "$var" ]]; then
    echo true
  else
    echo false
  fi
}


echo $(check $USE_MPS)
if $(check $1); then
  echo true
else
  echo false
fi

compat_env() {
  if [[ -n $1 && -n $2 ]]; then
    echo "Replicated env ${!1} and ${!2}"
    exit 1
  elif [[ -n $1 ]]; then
    echo $1
  else
    echo $2
  fi
}

export TEST_1=123
export TEST_2=456
export TEST_1=$(compat_env $TEST_1 $TEST)
compat_env $TEST $TEST_2
compat_env $TEST_1 $TEST_2
