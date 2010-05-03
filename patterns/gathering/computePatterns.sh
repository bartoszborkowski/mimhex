#!/bin/bash

#result=`mktemp result.XXXXXX`

for filename in `ls games | grep sgf`
do
    ./hsgf2gtp.sh games/$filename | ./gather_patterns
    #./hsgf2gtp.sh games/$filename | ./gather_patterns | grep -v "=" #| grep : | sed 's= /==' | sed 's=:==' > $result
done

#echo `cat $result | wc -l`
#cat $result

#rm $result
