#!/bin/bash

valgrind --log-file=valReport --leak-check=full --show-reachable=yes --leak-resolution=low ${1} >>/dev/null

cat valReport | sed -n '/All heap blocks were freed -- no leaks are possible/p'
