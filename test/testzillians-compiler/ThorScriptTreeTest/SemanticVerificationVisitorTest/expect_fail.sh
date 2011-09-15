#!/bin/bash

EXEC=$1
ARG=$2

$EXEC $ARG
ERROR_CODE="$?"
if [ $ERROR_CODE -ne 0 ];
then
    echo "success!"
    exit 0
fi

echo "fail! -- expect fail, but exit code == 0"
exit 1

