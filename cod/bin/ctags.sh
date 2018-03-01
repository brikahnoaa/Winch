#!/bin/bash
# make lara tags file
usage="$0: dir [dir ...]
defaults to ant boy, run from top of git tree"
top=$(git rev-parse --show-toplevel) 
if [ ! $? ]; then echo $usage; exit; fi
cd $top
hdrFiles=$(find hdr/ -name '*.h' -print -o -name Examples -prune)
ctags -o .tags.h $hdrFiles

if [ $# -eq 0 ]; then
  set "ant boy"
fi

echo -n "creating .tags: "

for i in $*; do
  if [ -d $top/$i ]; then
    cd $top/$i
    echo -n " $i"
    ctags -o .tags *
  fi
done
echo
