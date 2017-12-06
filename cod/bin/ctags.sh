#!/bin/bash
# make lara tags file
usage="$0: [Files]
defaults to ant/* boy/*, run from top of git tree"
if [ ! -d ./.git ]; then echo $usage; exit; fi
dir=$(dirname $0)
if [ $# -eq 0 ]; then
#  Files="$(cat $dir/filelist.lara)"
  Files="ant/*.? boy/*.?"
else
  Files="$*"
fi
echo "Files:"
echo "$Files" | fmt

ctags -o .tags $Files


# Headers="$(find ~/cf2/headers/*/Headers -name '*.h')"


#cd ~/NewportAcoustic
#ctags -L sh/filelist.lara -o .tags


#files="MPC/ADS/ADS.c LARA/LARA.c MPC/CTD/CTD.c MPC/GPSIRID/GPSIRID.c MPC/WISPR/WISPR.c MPC/WINCH/Winch.c MPC/MPC_SETTINGS/Settings.c MPC/MPC_Global/MPC.c MPC/CTD/CTD.h MPC/GPSIRID/GPSIRID.h MPC/WISPR/WISPR.h MPC/WINCH/Winch.h MPC/MPC_SETTINGS/Settings.h LARA/PLATFORM.h MPC/MPC_Global/MPC_Global.h MPC/ADS/ADS.h"
#ctags -f .tags $files
