# .bashrc v3

# Source global definitions
if [ -f /etc/bashrc ]; then . /etc/bashrc; fi

export LC_COLLATE=C

# User specific aliases and functions

alias l='ls --color=auto'
alias la='l -a'
alias ll='ls -l'
alias lk='ll --block-size=K'
alias lm='ll --block-size=M'
alias lr='ls -R'
alias lf='ls -F'
alias lt='ls -rt'
alias df='df -B1G'
alias du='du -B1G'
alias vi=vim
alias hd='od -A x -t x1z'
alias df='/bin/df -B1G'
alias g='grep -in'

# transient alias
if [ -f ~/.alias ]; then . ~/.alias; fi

if [[ $PATH != *sbin* ]]; then PATH=/sbin:/usr/sbin:$PATH ; fi
if [ `whoami` != "root" ]; then 
  PATH=~/bin:$PATH
  if [[ $PATH != *local* ]]; then PATH=$PATH:/usr/local/bin ; fi
fi
export PATH

# Foreground: 30 Black 31 Red 32 Green 33 Yellow 34 Blue 35 Magenta 36 Cyan 37 White 
# Background: 40 Black 41 Red 42 Green 43 Yellow 44 Blue 45 Magenta 46 Cyan 47 White
# 0 Reset all 1 Bold
# prompt color pcolor 32=green (31=red for root)
pcolor=33
if [ `whoami` = "kahn" ]; then pcolor=32;  fi
if [ `whoami` = "root" ]; then pcolor=31;  fi

export PS1="[\u@\h \T \w]\[\e[0m\]\n:; "
##export PS1="[\u@\h \T \w]\[\e[0m\]\n \\\$ "
if [[ "$TERM" == xterm* ]]; then
  if [ `hostname` = "BRUTE" ]; then
    export PS1="\e[${pcolor}m: \u@\h \T \w :\e[0m\n "
  else
    export PS1="\e[${pcolor}m\\\$ \u@\h \T \w \\\$\e[0m\n "
  fi
  export PROMPT_COMMAND='printf "\033]0;%s@%s:%s\007" "${USER}" "${HOSTNAME%%.*}" "${PWD/#$HOME/\~}"'
fi

export PRINTER="//hatfield/xerox-npt PCL 6"
