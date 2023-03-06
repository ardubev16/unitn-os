#!/usr/bin/env bash

# se e' una path valida stampa "T" altrimenti stampa "F"
([[ -e $DATA ]] && echo "T") || echo "F"

# se e' un file stampa "file" altrimenti se e' una directory stampa "directory" altrimenti stampa "?"
([[ -f $DATA ]] && echo "file") || ([[ -d $DATA ]] && echo "directory" || echo "?")

# se e' un file esegue il contenuto come valore numerico altrimenti stampa "?"
[[ -f $DATA ]] && echo $(( $(cat "$DATA") )) || echo "?"
