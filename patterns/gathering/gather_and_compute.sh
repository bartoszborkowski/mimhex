#!/bin/bash

time=`date`;
echo "Rozpoczynam zbieranie patternów: $time"
./computePatterns_my.sh 2> ../utils/pattern_teams.txt 1> gather_output.txt
time=`date`;
echo "Zakonczyłem zbieranie patternow: $time"
echo "Rozpoczynam obliczanie."
cd ../utils
./simple_pattern_conv
time=`date`;
echo "Zakończyłem obliczanie: $time"

