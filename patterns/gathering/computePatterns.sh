#!/bin/bash

filenames=`mktemp filenames.XXXXXX`
result=`mktemp result.XXXXXX`

for filename in `ls games | grep sgf`
do
    echo games/$filename
done > $filenames

./hsgf2gtp.sh `cat $filenames` | ../main/gather_patterns | grep : | sed 's= /==' | sed 's=:==' > $result

echo `cat $result | wc -l`
cat $result

#rm $filenames
#rm $result