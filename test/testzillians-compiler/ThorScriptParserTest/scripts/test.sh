#!/bin/bash

TEMP_FILE_A=`mktemp`
TEMP_FILE_B=`mktemp`

EXEC=$1
POST_PROCESSOR=$2

COUNTER=1
for ARG in "$@"; do
    if [ $ARG == $EXEC -o $ARG == $POST_PROCESSOR ]; then
        continue
    fi
    echo "file #$COUNTER.."
    if [ ! -f "$ARG" ]; then
        echo "ERROR: file not found!"
        exit 1
    fi
    echo "filename: $ARG"
    $EXEC $ARG >& $TEMP_FILE_A
    ERROR_MESSAGE=`cat $TEMP_FILE_A | grep "error"`
    if [ -n "$ERROR_MESSAGE" ]; then
        cat $TEMP_FILE_A # NOTE: for convenience
        echo "filename: $ARG"
        echo $ERROR_MESSAGE
        echo "using temp file: $TEMP_FILE_A (contents shown above)" # NOTE: useful for diagnosing error
        exit 1 # NOTE: bail on first error
    fi

    # NOTE: it is unfortunate that '<' and '>' interfere with xml

    # =========================================================================================================
    # replace recognized tags to use curly-brace
    cat $TEMP_FILE_A | sed "s#<try>#{try}#g"               | sed "s#<\/try>#{\/try}#g"               > $TEMP_FILE_B
    cat $TEMP_FILE_B | sed "s#<success>#{success}#g"       | sed "s#<\/success>#{\/success}#g"       > $TEMP_FILE_A
    cat $TEMP_FILE_A | sed "s#<attributes>#{attributes}#g" | sed "s#<\/attributes>#{\/attributes}#g" > $TEMP_FILE_B
    cat $TEMP_FILE_B | sed "s#<fail\/>#{fail\/}#g"                                                   > $TEMP_FILE_A
    # =========================================================================================================

    # handle interfering '<' and '>'
    cat $TEMP_FILE_A | sed "s/>>/{GTGT}/g" | sed "s/<</{LTLT}/g" > $TEMP_FILE_B
    cat $TEMP_FILE_B | sed "s/}>/}{_GT}/g" | sed "s/}</}{_LT}/g" > $TEMP_FILE_A
    cat $TEMP_FILE_A | sed "s/>{/}{_GT}/g" | sed "s/<{/{_LT}{/g" > $TEMP_FILE_B

    # replace: '>' --> {GT}
    # replace: '<' --> {LT}
    cat $TEMP_FILE_B | sed "s/>/{GT}/g" | sed "s/</{LT}/g" > $TEMP_FILE_A

    # =========================================================================================================
    # restore recognized xml tags
    cat $TEMP_FILE_A | sed "s#{try}#<try>#g"               | sed "s#{\/try}#<\/try>#g"               > $TEMP_FILE_B
    cat $TEMP_FILE_B | sed "s#{success}#<success>#g"       | sed "s#{\/success}#<\/success>#g"       > $TEMP_FILE_A
    cat $TEMP_FILE_A | sed "s#{attributes}#<attributes>#g" | sed "s#{\/attributes}#<\/attributes>#g" > $TEMP_FILE_B
    cat $TEMP_FILE_B | sed "s#{fail\/}#<fail\/>#g"                                                   > $TEMP_FILE_A
    # =========================================================================================================

    # replace: {GT} --> '>'
    # replace: {LT} --> '<'
    cat $TEMP_FILE_A | sed "s/{GT}/>/g" | sed "s/{LT}/</g" > $TEMP_FILE_B

    $POST_PROCESSOR $TEMP_FILE_B # NOTE: dump result to stdout
    let COUNTER=$COUNTER+1;
    echo
done

exit 0
