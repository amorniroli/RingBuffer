#!/bin/bash
for filename in `find ./example | egrep '\.c'`; 
do 
  gcov -n -o ./obj $filename > /dev/null; 
done
