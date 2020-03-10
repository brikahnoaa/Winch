#!/bin/bash
#set up [4] windows
# usage: . bin/startup
SHAG=$(ssh-agent -s)
#echo $SHAG
eval "$SHAG"
if ssh-add ~/.ssh/github_id_rsa; then
  for (( i=0; i<3; i++)) do mintty& sleep 0.1; done
  sleep 1
else
  echo failed: ssh-add ~/.ssh/github_id_rsa
fi

