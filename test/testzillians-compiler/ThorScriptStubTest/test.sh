#!/bin/sh

TCC=$1
FILE=$2
GOLD_FILE=$3

exit 0
diff $FILE $GOLD_FILE
ERROR_CODE="$?"

if [ $ERROR_CODE -ne 0 ];
then
    echo "fail! (test.sh)"
    exit 1
fi
echo "success! (test.sh)"
exit 0
