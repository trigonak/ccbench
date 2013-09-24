#!/bin/bash

conf=$1;
shift;
run=$@;

tmp="conf.run.tmp";


if [ $# -lt 1 ];
then
    echo "Usage: $0 TARGET_CONFIDENCE ./ccbench [PARAMETERS]";
    echo "       runs ccbench until there is a run with a clustering";
    echo "       around the avg that has >= TARGET_CONFIDENCE percentage";
    echo "       of the total samples. The TARGET_CONFIDENCE is decreased";
    echo "       by D after F failed attempts (D, F defined in the script)";
    exit;
fi;

tries_fail=3;
reduce_on_fail=1;

echo " ** Confidence lvl: $conf";

tries=1;
while :
do
    ./$run > $tmp;
    res=$(cut -d'(' -f2 $tmp | gawk -v c=$conf '/%  \|/ { if ($1+0 > c) print $1" --("$0 }');

    if [ "$res" ];
    then
	cat $tmp;
	echo " ** in # tries: $tries";
	break;
    fi;

    tries=$((tries+1));
    if [ $tries -gt $tries_fail ];
    then
	conf=$((conf-reduce_on_fail));
	tries=1;
	echo " ** Failed after $tries_fail tries. New confidence lvl: $conf"
    fi;
done;

if [ -f $tmp ];
then
    rm $tmp;
fi;
