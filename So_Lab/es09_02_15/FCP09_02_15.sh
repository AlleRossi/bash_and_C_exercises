#!/bin/sh

#controllo so i parametri passati sono due
case $# in
2);;
*) echo numero parametri non corretto
   exit 1;;
esac

#controllo se il primo parametro è dir attraversabile
case $1 in
/*)if test ! -d $1 -o ! -x $1
   then echo " $1 non dir o non attraversabile" > /dev/tty
   exit 2
   fi;;
*)echo non nome assoluto
  exit 3;;
esac

#controllo se il secondo parametro è un intero ed è positivo
case $2 in
	*[!0-9]*) echo $2 non numerico o non positivo
		exit 4;;
	*)if test $2 -eq 0
	  then echo $2 è zero
	  exit 5
	  fi;;
esac


#export del PATH
PATH=`pwd`:$PATH
export PATH
#creazione file temporaneo
> /tmp/tmp$$

#invoco la funzione ricorsiva
FCR09_02_15.sh $1 $2 /tmp/tmp$$

#ritorno il controllo a questo script e chiedo a user un numero per ogni file trovato
num=
for i in `cat  /tmp/tmp$$`
do
	echo inserire un numero minore di $2
	read num
	while test $num -gt $2 -o $num -lt 0
	do 
		echo inserire un numero minore di $2 valido:
		read num
	done
	echo  File.c $i $num 
done
#rimuovo il file temporaneo
rm /tmp/tmp$$
