#!/bin/bash
if [ ! -d ~/Winch/cod/dot ]; then 
  echo cannot find ~/Winch/cod/dot
  exit -1
fi
echo run the following commands:
echo 'cd ~'
cd ~/Winch/cod/dot
for i in $(find .??*); do
  if [ -f ~/$i ]; then
    echo mv $i $i.orig
  fi
done
echo 'cp -r ~/Winch/cod/dot/.??* ~'
echo

cyg=/cygdrive/c/cygwin64/setup-x86_64.exe
if [ ! -x $cyg ]; then cyg='((location of cyg setup))'; fi

echo "$cyg -qP bind-utils,binutils,ctags,cygcheck-dep,cygutils-extra,cygwin-devel,enscript,git,indent,openssh,perl,python2,python2-setuptools,rsync,vim,vim-common,vim-doc,wget"
