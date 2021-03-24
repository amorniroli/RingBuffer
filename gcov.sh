#!/bin/bash
for filename in `find ./src | egrep '\.c'`; 
do 
  gcov -n -o ./obj $filename > /dev/null; 
done
