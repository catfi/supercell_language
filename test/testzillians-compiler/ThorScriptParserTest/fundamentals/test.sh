#!/bin/bash

EXEC=$1

COUNTER=-1
for ARG in "$@"; do
    if [ $COUNTER -gt -1 ]; then
        echo "now testing file #$COUNTER.."
        echo "$ARG"
        $EXEC $COUNTER
        echo
    fi
    let COUNTER=$COUNTER+1;
done

exit 0

