#!/bin/sh

TEMP_FILE_A=`mktemp`
TEMP_FILE_B=`mktemp`

TS_DRIVER=$1
BUILD_PATH=$2
INPUT_FILE=$3
INTERMEDIATE_PATH=$4
PROJECT_NAME=$5
KEYWORD=$6
GOLD_OUTPUT=$7

cd $BUILD_PATH
$TS_DRIVER project create $PROJECT_NAME
cd $PROJECT_NAME
mkdir -p src/$INTERMEDIATE_PATH
cp -f $INPUT_FILE src/$INTERMEDIATE_PATH
$TS_DRIVER
nm build/bin/$PROJECT_NAME.so | xargs -I{} sh -c "echo {} | cut -d\" \" -f3 | grep \"${KEYWORD}\"" > $TEMP_FILE_A
cat $GOLD_OUTPUT > $TEMP_FILE_B

diff $TEMP_FILE_A $TEMP_FILE_B
ERROR_CODE="$?"
if [ $ERROR_CODE -ne 0 ];
then
    #echo `pwd` # for debugging
    #nm $PROJECT_NAME.so | xargs -I{} sh -c "echo {} | cut -d\" \" -f3 | c++filt -t" # for debugging
    echo "fail! (test.sh)"
    exit 1
fi
echo "success! (test.sh)"
exit 0

