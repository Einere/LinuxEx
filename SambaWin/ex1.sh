#!/bin/bash

declare -i cx=0
declare -i cy=0
cb=0
while [ 1 ]
do
  read -sn 1 cb
  if [ "$cb" = "A" ] 
  then
    cy=`expr $cy - 1`
  elif [ "$cb" = "B" ] 
  then
    cy=`expr $cy + 1`
  elif [ "$cb" = "C" ] 
  then
    cx=`expr $cx + 1`
  elif [ "$cb" = "D" ] 
  then
    cx=`expr $cx - 1`
  elif [ "$cb" == "" ]
  then
    echo "space bar~~"
  fi
  
  tput cup $cy $cx

done


