#!/bin/sh
case $# in 
2);;
*)	echo errore: Usage is $0 dirass Dir N
	exit 1;;
esac

case $1 in 
/*) if test ! -d $1 -o ! -x $1
    then echo $1 non dir o non attraversabile
    exit 2
    fi;;
*) echo $1 non nome assoluto
   exit 3;;
esac

case $2 in
*[!0-9]*) echo $2  non numerico o non positivo
	  exit 4;;
*)if test $2 -eq 0
  then echo $2 uguale a 0
       exit 5
  else echo $2 numerico 
  fi;;
esac

#settiamo PATH
PATH=`pwd`:$PATH
export PATH

#invochiamo la funzione ricorsiva
FCR15_02_17.sh $*

