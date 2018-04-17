#!/usr/bin/env bash
FILE=$1
OUTFILE=$(basename $(echo $FILE | sed 's/_raw//') .tex).txt
cat $FILE | grep '^\$' | sed 's/\$(//g; s/)\$//g; s/,//g; s/\\phantom{-}//g'\
    | awk '{print $1, $2, $4, $5, $16, $18, $20}' > $OUTFILE
