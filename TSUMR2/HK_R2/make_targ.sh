#!/bin/bash

CORE_NUM=${1:-12}
make realclean

time make -j"$CORE_NUM"
time make
if [ $? -eq 0 ]; then
    echo "編譯成功！"
fi

date