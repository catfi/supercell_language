#!/bin/sh

TEMP_FILE_A=`mktemp`
TEMP_FILE_B=`mktemp`

TSCOMPILE=$1
INPUT=$2
GOLD_OUTPUT=$3

$TSCOMPILE --input $INPUT --emit-ast $TEMP_FILE_A

diff $TEMP_FILE_A $TEMP_FILE_B
ERROR_CODE="$?"

if [ $ERROR_CODE -ne 0 ];
then
    echo "fail!"
    exit 1
fi
echo "success!"
exit 0
