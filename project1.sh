#!/bin/bash

# number of element, i
declare -i element_count=`ls | wc -l`
declare -i i=1

#print head information
print_form(){
printf "=============== Total Information =============== \n\n"
printf "Current directory : $PWD \n"
printf "Number of element : $element_count \n\n"
}


#print detail information
print_inform(){
echo "[$i] `stat -c %n $n_file`"
if [ -d $n_file ]
  then
    echo -e [34m "\bfile type : `stat -c %F $n_file`"   
  elif [ -f $n_file ]
  then
    echo -e [0m "\bfile type : `stat -c %F $n_file`"
  else
    echo -e [32m "\bfile type : `stat -c %F $n_file`"
fi
echo -e [0m "\bfile size : `stat -c %s $n_file`"
echo "modification time : `stat -c %y $n_file`" 
echo "permition : `stat -c %a $n_file`"
echo "absolute path : `realpath -e $n_file`"
echo "relavant path : ./$n_file"
echo "  "
i+=1 
}

#print subdirectory detail information
print_subinform(){
echo "  [$i] `stat -c %n $n_subfile`"
if [ -d $n_subfile ]
  then
    echo -e [34m "\b  file type : `stat -c %F $n_subfile`"   
  elif [ -f $n_subfile ]
  then
    echo -e [0m "\b  file type : `stat -c %F $n_subfile`"
  else
    echo -e [32m "\b  file type : `stat -c %F $n_subfile`"
fi
echo -e [0m "\b  file size : `stat -c %s $n_subfile`"
echo "  modification time : `stat -c %y $n_subfile`" 
echo "  permition : `stat -c %a $n_subfile`"
echo "  absolute path : `realpath -e $n_subfile`"
echo "  relavant path : ./$n_subfile"
echo "  "
i+=1 
}


#print special file information
print_d(){
printf "=============== Detail information =============== \n\n"
for n_file in *
  do
    if [ -d $n_file ]
    then
      if [ `ls $n_file | wc -l` -eq 0 ]
      then
        print_inform
      else
        print_inform
        for n_subfile in ./$n_file/*
	  do
            print_subinform
	  done
      fi
    fi
  done
}  

#print file information
print_f(){
printf "=============== Detail information =============== \n"
for n_file in *
  do
    if [ -f $n_file ]
    then
      print_inform
    fi
  done
}  

#print special file information
print_sf(){
printf "=============== Detail information =============== \n"
for n_file in *
  do
    if [ -f $n_file ] || [ -d $n_file ]
    then
      printf "" 
    else
    print_inform
    fi
  done
}  

#main function

print_form

if [ -d $n_file ]
then
  print_d
fi
if [ -f $n_file ]
then
  print_f
fi
if [ -f $n_file ]
then
  print_sf
fi

