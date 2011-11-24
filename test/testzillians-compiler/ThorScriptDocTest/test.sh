#!/bin/sh

TEMP_FILE_A=`mktemp`
TEMP_FILE_B=`mktemp`

EXEC=$1
GOLD_KEYWORD=$2
GOLD_OUTPUT=$3

$EXEC | grep $GOLD_KEYWORD > $TEMP_FILE_A
cat $GOLD_OUTPUT > $TEMP_FILE_B

diff $TEMP_FILE_A $TEMP_FILE_B
ERROR_CODE="$?"
if [ $ERROR_CODE -ne 0 ];
then
    echo "fail!"
    exit 1
fi
echo "success!"
exit 0
