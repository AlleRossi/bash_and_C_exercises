#!/bin/sh

case $# in
0|1|2)	echo non abbastanza parametri, usage is C dir dir ...
	exit 1;;
*)	echo da qui $# parametri;;
esac

case $1 in
?)	echo $1 carattere singolo;;
*)	echo $1 non singolo carattere
	exit 2;;
esac

#salvo il char
C=$1

shift

for i in $*
do
	case $i in
	/*)	if test ! -d $i -o ! -x $i
		then
			echo $i non dir o non attraversabile
		exit 3
		fi;;
	*)	echo  $i non nome assoluto
		exit 4;;
	esac
done

#creo il file temporaneo ed esporto il path
> /tmp/nomiAssoluti
PATH=`pwd`:$PATH
export PATH

for i in $*
do
	echo fase per $i
	FCR.sh $i $C /tmp/nomiAssoluti
done

echo il numero di file trovati e\' `wc -l < /tmp/nomiAssoluti`
#variabile per conoscere la risposta di Elena
answer=0
for i in `cat /tmp/nomiAssoluti`
do
	echo Elena vuoi visualizzare il contenuto di $i\?
	read answer
	case $answer in
	y*|s*|Y*|S*)	cat  $i;;
	*);;
	esac
done

#rimuovo il file tmp
rm /tmp/nomiAssoluti
