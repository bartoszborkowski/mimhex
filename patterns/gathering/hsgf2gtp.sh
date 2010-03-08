#!/bin/bash

convert() 
{
	file=$1
    echo "newgame"
	{ cat $file | sed "s/;/\n/g"  | grep -v "^($" | grep -v "FF\|SO" | grep -v "resign" \
   		| sed "s/W/play white/" | sed "s/B/play black/" | grep "swap" > /dev/null && { 
			cat $file | sed "s/;/\n/g"  | grep -v "^($" | grep -v "FF\|SO" | grep -v "resign" \
		   		| sed "s/W/play white/" | sed "s/B/play black/" | grep -v "swap" | sed "1s/white/black/"; 
		} || {
			cat $file | sed "s/;/\n/g"  | grep -v "^($" | grep -v "FF\|SO" | grep -v "resign" \
		   		| sed "s/W/play white/" | sed "s/B/play black/"; } ;} | sed "s/\[\(.\)\(.\)\]/ \1 \2/" | gawk '{\
		c=$4;
		ascval = b = 128; \
		while ((tchar = sprintf("%c", ascval)) != c) \
		ascval += (b /= 2) * (tchar < c) ? 1 : -1; printf ("%s %s %s%d\n", $1, $2, $3, ascval-96)}'
}

#cat games/1022780.sgf | sed "s/;/\n/g"  | grep -v "^($" | grep -v "FF\|SO" | grep -v "resign" \
#	| sed "s/W/play white/" | sed "s/B/play black/" | sed "s/\[\(.\)\(.\)\]/ \1 \2/" | sed "1s/white/black/"  | grep -v swap 

test -n "$1" || { echo "usage: ./hsgf2gtp.sh sgfPath1 sgfPath2 ... " && exit 1; }

while test -n "$1" ; do
    convert $1
    shift
done

echo "print_verbose"
