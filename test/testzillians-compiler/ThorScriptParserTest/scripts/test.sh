#!/bin/bash

TEMP_FILE_A=`mktemp`
TEMP_FILE_B=`mktemp`

EXEC=$1
POST_PROCESSOR=$2
MODE=$3

COUNTER=1
for ARG in "$@"; do
    if [ $ARG == $EXEC -o $ARG == $POST_PROCESSOR -o $ARG == $MODE ]; then
        continue
    fi
    echo "file #$COUNTER.."
    echo "filename: $ARG"
    if [ ! -f "$ARG" ]; then
        echo "ERROR: file not found!"
        exit 1
    fi
    if [ $MODE -eq 1 ]; then
        $EXEC $ARG --dump-ast-and-stop --dump-ast-location
        ERROR_CODE="$?"
        if [ $ERROR_CODE -ne 0 ]; then
            echo "ERROR: construct/dump ast fail!"
            exit 1
        fi
        continue
    fi
    $EXEC $ARG --dump-parse-and-stop |& grep -v "[DEBUG]" > $TEMP_FILE_A
    ERROR_CODE="${PIPESTATUS[0]}" # NOTE: need PIPESTATUS because piped grep always succeeds
    if [ $ERROR_CODE -ne 0 ]; then
        cat $TEMP_FILE_A # NOTE: for convenience of reference
        echo "ERROR: parse fail!"
        echo "using temp file: $TEMP_FILE_A (contents shown above)" # NOTE: useful for diagnosing error
        exit 1
    fi

    # =========================================================================================================
    # replace non-content tags to use curly-brace
    cat $TEMP_FILE_A | sed "s#<try>#{try}#g"               | sed "s#<\/try>#{\/try}#g"               > $TEMP_FILE_B
    cat $TEMP_FILE_B | sed "s#<success>#{success}#g"       | sed "s#<\/success>#{\/success}#g"       > $TEMP_FILE_A
    cat $TEMP_FILE_A | sed "s#<attributes>#{attributes}#g" | sed "s#<\/attributes>#{\/attributes}#g" > $TEMP_FILE_B
    cat $TEMP_FILE_B | sed "s#<fail\/>#{fail\/}#g"                                                   > $TEMP_FILE_A
    # =========================================================================================================

    # replace: '<' --> {LT}
    # replace: '>' --> {GT}
    cat $TEMP_FILE_B | sed "s/</{LT}/g" | sed "s/>/{GT}/g" > $TEMP_FILE_A

    # =========================================================================================================
    # restore non-content xml tags
    cat $TEMP_FILE_A | sed "s#{try}#<try>#g"               | sed "s#{\/try}#<\/try>#g"               > $TEMP_FILE_B
    cat $TEMP_FILE_B | sed "s#{success}#<success>#g"       | sed "s#{\/success}#<\/success>#g"       > $TEMP_FILE_A
    cat $TEMP_FILE_A | sed "s#{attributes}#<attributes>#g" | sed "s#{\/attributes}#<\/attributes>#g" > $TEMP_FILE_B
    cat $TEMP_FILE_B | sed "s#{fail\/}#<fail\/>#g"                                                   > $TEMP_FILE_A
    # =========================================================================================================

    # replace: leading  {LT} --> '<'
    # replace: trailing {GT} --> '>'
    cat $TEMP_FILE_A | sed 's/^[ \t]*//' | sed "s/^{LT}/</g" | sed "s/{GT}$/>/g" > $TEMP_FILE_B

    $POST_PROCESSOR $TEMP_FILE_B # NOTE: dump result to stdout
    echo "using temp file: $TEMP_FILE_B" # NOTE: useful for diagnosing error
    let COUNTER=$COUNTER+1;
    echo
done

echo "success!"
exit 0

