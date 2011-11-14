#!/bin/sh

TEMP_FILE_A=`mktemp`
TEMP_FILE_B=`mktemp`

TS_DRIVER=$1
BUILD_PATH=$2
INPUT_FILE=$3
PROJECT_NAME=$4
KEYWORD=$5
GOLD_OUTPUT=$6

cd $BUILD_PATH
$TS_DRIVER project create $PROJECT_NAME
cd $PROJECT_NAME
mkdir -p src
cp $INPUT_FILE src
$TS_DRIVER
nm $PROJECT_NAME.so | xargs -I{} sh -c "echo {} | cut -d\" \" -f3 | grep \"${KEYWORD}\"" > $TEMP_FILE_A
cat $GOLD_OUTPUT > $TEMP_FILE_B

diff $TEMP_FILE_A $TEMP_FILE_B
ERROR_CODE="$?"
if [ $ERROR_CODE -ne 0 ];
then
    echo "fail! (test.sh)"
    exit 1
fi
echo "success! (test.sh)"
exit 0

