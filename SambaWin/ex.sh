#!/bin/bash


for up_list in ../*
do
  if [ -d $up_list ]
  then
    echo [31m"$up_list" 
  elif [ -f $up_list ]
  then
    echo [32m"$up_list" 
  else
  echo [33m"$up_list"
  fi
done
 




