#!/bin/sh

cd $1

nl=
for i in *
do
	if test -f $i -a -r $i
	then
		nl=`grep  [0-9]$ $i | wc -l`
		if test $nl -ge  $2
		then
			echo `pwd`/$i >> $3
		fi
	fi
done
#ricerca in tutte le sotto dir

for i in *
do
	if test -d $i -a -x $i
	then
		$0 `pwd`/$i $2 $3
	fi
done

