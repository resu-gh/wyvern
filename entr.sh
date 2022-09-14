#!/bin/sh

[[ "$#" -ne 1 ]] && {
    echo "input grammar file (.g) not found!"
    exit 1
}

e=$(echo "$1" | rev | cut -d'.' -f1)
[[ "$e" != 'g' ]] && {
    echo "invalid grammar file (must be a .g file)"
    exit 1
}

fd -e cpp -e hpp -e g | entr make ARGS=$1 
