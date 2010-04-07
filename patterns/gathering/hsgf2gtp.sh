#!/bin/bash

convert() 
{
	file=$1
	{ cat $file | sed "s/;/\n/g"  | grep -v "^($" | grep -v "FF\|SO" | grep -v "resign" \
   		| sed "s/W/play white/" | sed "s/B/play black/" | grep "swap" > /dev/null && { 
			cat $file | sed "s/;/\n/g"  | grep -v "^($" | grep -v "FF\|SO" | grep -v "resign" \
		   		| sed "s/W/play white/" | sed "s/B/play black/" |  sed "2s/.*swap.*/swap/" \
#                | sed "4~2s/black/white/" | sed "3~2s/white/black/"\
 #               ; \
		} || {
			cat $file | sed "s/;/\n/g"  | grep -v "^($" | grep -v "FF\|SO" | grep -v "resign" \
		   		| sed "s/W/play white/" | sed "s/B/play black/"; } ;} | sed "s/\[\(.\)\(.\)\]/ \1 \2/" \
                | gawk '{\
                if ($1 != "swap") {\
                    c=$4;
                    ascval = b = 128; \
                    while ((tchar = sprintf("%c", ascval)) != c) \
                        ascval += (b /= 2) * (tchar < c) ? 1 : -1; \
                        printf ("%s %s %s%d\n", $1, $2, $3, ascval-96) \
                    } else {print "swap"}\

    }'

}

#cat games/1022780.sgf | sed "s/;/\n/g"  | grep -v "^($" | grep -v "FF\|SO" | grep -v "resign" \
#	| sed "s/W/play white/" | sed "s/B/play black/" | sed "s/\[\(.\)\(.\)\]/ \1 \2/" | sed "1s/white/black/"  | grep -v swap 

convert $1
