#!/bin/sh

case $# in
0|1|2) echo numero di parametri insufficente
	exit 1;;
*)	echo OK;;
esac

expr $1 + 0 > /dev/null 2>&1
ctr=$?
if test $ctr -ne 2 -a $ctr -ne 3
then if test $1 -le 0
	then echo $1 non positivo
	exit 2
	fi
else
  echo $1 non numerico
  exit 3
fi
#salvo il primo paramentro e faccio shift
x=$1
shift

#ora verifico che gli latri parametri siano dir attraversabili

for i
do
	case $i in
	/*)if test ! -d $i -o ! -x $i
		then echo $i non dir o non attraversabile
		exit 4
	   fi;;
	*)echo non nome assoluto
	  exit 5;;
	esac
done

#export del path
PATH=`pwd`:$PATH
export PATH

#creo file temporane
> /tmp/tmp$$

for i
do
	echo fase per $i
	FCR17_04_15.sh $i $x /tmp/tmp$$
done

#stampo il numero di file trovati
echo i file trovati sono `wc -l < /tmp/tmp$$`

#per ogni file chiedo quale linea si vuole stampare
k=
for i in `cat /tmp/tmp$$`
do
	echo file trovato: $i
	echo inserire il numero minore di $x di linea da visionare
	read k
	expr $k + 0 > /dev/null 2>&1
	ctr=$?
	if test $ctr -ne 2 -a $ctr -ne 3
	then
		if test $k -le 0 -o $k -ge $x
		then
			echo $k non positivo o non minore di $x
			rm /tmp/tmp$$
			exit 6
		fi
	else
		echo non numerico
		rm /tmp/tmp$$
		exit 7
	fi
	head -$k $i | tail -1
done 

rm /tmp/tmp$$


