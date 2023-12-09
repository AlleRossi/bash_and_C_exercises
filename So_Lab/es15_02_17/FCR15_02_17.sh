#!/bin/sh
#file ricorsivo 15/02/17

cd $1
# variabile per contare le linee
nl=
# varibile per contare le linee che contengono numero
n=
#variabile che contiene i file trovati
files=
for i in *
do
	if test -f $i -a -r $i
	then nl= `wc -l < $i `
		if test nl -eq $2
		then 
			n=`grep `[0-9]` $i | wc -l`
			echo nl è $nl e n è $n
			if test $n -eq $nl
			then  files="$files $i"
			fi
		fi
	fi
done

if test "$files"
then echo trovata directory `pwd`
     echo contiene i seguenti files:
     for file in $files
     do
	echo $i
     done
fi

for i in *
do
if test -d $i -a -x $i
then 
	$0 `pwd`/$i $2
fi
done
