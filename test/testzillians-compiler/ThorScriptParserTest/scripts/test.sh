#!/bin/bash

TEMP_FILE=`mktemp`

EXEC=$1
POST_PROCESSOR=$2

COUNTER=-1
for ARG in "$@"; do
    if [ $COUNTER -gt -1 ]; then
        echo "now testing file #$COUNTER.."
        if [ -f "$ARG" ]; then
            echo "$ARG"
            $EXEC $ARG >& $TEMP_FILE
            $POST_PROCESSOR $TEMP_FILE
        else
            echo "ERROR: file not found!"
        fi
        echo
    fi
    let COUNTER=$COUNTER+1;
done

exit 0

