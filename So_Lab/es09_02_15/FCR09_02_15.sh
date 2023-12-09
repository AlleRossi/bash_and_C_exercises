#!/bin/sh

#entro nella directory indicata
cd $1

#variabile di controllo per stabilire se ho trovato almeno un file
ctr=false
for i in *
do
	if test -f $i -a -r $i
	then
	 	nl=`wc -l < $i`
	 	if test $nl -eq $2
 		then echo `pwd`/$i >> $3
		ctr=true
		fi
	fi
done

if test $ctr = true
then echo la directory contiene almeno un file con le caratteristiche richieste: `pwd`
fi

for i in *
do
	if test -d $i -a -x $i
	then $0 `pwd`/$i $2 $3
	fi
done


