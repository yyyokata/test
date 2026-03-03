#!/bin/sh

sed -i "s/$1/$2/g" `grep "$1" -rl ./`
