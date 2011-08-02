#!/bin/bash

TEMP_FILE_A=`mktemp`
TEMP_FILE_B=`mktemp`

EXEC=$1
POST_PROCESSOR=$2

COUNTER=-1
for ARG in "$@"; do
    if [ $COUNTER -gt -1 ]; then
        echo "now testing file #$COUNTER.."
        echo "using temp file: $TEMP_FILE_B"
        if [ -f "$ARG" ]; then
            echo "$ARG"
            $EXEC $ARG >& $TEMP_FILE_A

            # NOTE: it is unfortunate that shift ops interfere with spirit xml log

            # =========================================================================================================
            # replace recognized tags to use curly-brace
            cat $TEMP_FILE_A | sed "s#<try>#{try}#g"               | sed "s#<\/try>#{\/try}#g"               > $TEMP_FILE_B
            cat $TEMP_FILE_B | sed "s#<success>#{success}#g"       | sed "s#<\/success>#{\/success}#g"       > $TEMP_FILE_A
            cat $TEMP_FILE_A | sed "s#<attributes>#{attributes}#g" | sed "s#<\/attributes>#{\/attributes}#g" > $TEMP_FILE_B
            cat $TEMP_FILE_B | sed "s#<fail\/>#{fail\/}#g"                                                   > $TEMP_FILE_A
            # =========================================================================================================

            # handle ambiguous case
            cat $TEMP_FILE_A | sed "s/>>/{GTGT}/g" | sed "s/<</{LTLT}/g" > $TEMP_FILE_B
            cat $TEMP_FILE_B | sed "s/}>/}{_GT}/g" | sed "s/}</}{_LT}/g" > $TEMP_FILE_A
            cat $TEMP_FILE_A | sed "s/>{/}{_GT}/g" | sed "s/<{/{_LT}{/g" > $TEMP_FILE_B

            # replace all '>' --> {GT}
            # replace all '<' --> {LT}
            cat $TEMP_FILE_B | sed "s/>/{GT}/g" | sed "s/</{LT}/g" > $TEMP_FILE_A

            # =========================================================================================================
            # restore recognized tags
            cat $TEMP_FILE_A | sed "s#{try}#<try>#g"               | sed "s#{\/try}#<\/try>#g"               > $TEMP_FILE_B
            cat $TEMP_FILE_B | sed "s#{success}#<success>#g"       | sed "s#{\/success}#<\/success>#g"       > $TEMP_FILE_A
            cat $TEMP_FILE_A | sed "s#{attributes}#<attributes>#g" | sed "s#{\/attributes}#<\/attributes>#g" > $TEMP_FILE_B
            cat $TEMP_FILE_B | sed "s#{fail\/}#<fail\/>#g"                                                   > $TEMP_FILE_A
            # =========================================================================================================

            # replace all {GT} --> '>'
            # replace all {LT} --> '<'
            cat $TEMP_FILE_A | sed "s/{GT}/>/g" | sed "s/{LT}/</g" > $TEMP_FILE_B

            $POST_PROCESSOR $TEMP_FILE_B
        else
            echo "ERROR: file not found!"
        fi
        echo
    fi
    let COUNTER=$COUNTER+1;
done

exit 0

