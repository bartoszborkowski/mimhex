#!/bin/bash

filenames=`mktemp filenames.XXXXXX`
result=`mktemp result.XXXXXX`

for filename in `ls games | grep sgf`
do
    echo games/$filename
done > $filenames

onefile=`cat $filenames | head -n 200`

#echo $onefile
./hsgf2gtp.sh `echo $onefile` | ./gather_patterns # | grep : | sed 's= /==' | sed 's=:==' > $result
#./hsgf2gtp.sh `cat $filenames` | ./gather_patterns | grep : | sed 's= /==' | sed 's=:==' > $result

#echo ""
#echo -n "Liczba lini wyniku: "
#echo `cat $result | wc -l`
#cat $result

rm $filenames
rm $result
