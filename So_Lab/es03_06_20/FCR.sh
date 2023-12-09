#!/bin/sh

#entriamo nell dir in considerazione
cd $1

for i in *
do
	case $i in
	$2?$2)	echo `pwd`/$i >> $3;;
	*);;
	esac
done

for i in *
do
	if test -d $i -a -x $i
	then
		$0 $i $2 $3
	fi
done

