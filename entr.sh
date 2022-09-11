#!/bin/sh

fd -e cpp -e hpp -e g | entr make ARGS=$1 
