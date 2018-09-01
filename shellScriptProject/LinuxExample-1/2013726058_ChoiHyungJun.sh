#!/bin/bash


#declare number of element, i
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
echo "absolute path : $PWD/$n_file"
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
echo "  absolute path : $PWD/$n_subfile"
echo "  relavant path : ./$n_subfile"
echo "  "
i+=1 
}


#print directory, file, spcial file information
print_first(){
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
        print_second
      fi
    fi
  done

for n_file in *
  do
    if [ -f $n_file ]
    then
      print_inform
    fi
  done

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
  
#print sub directory, file, spcial file information
print_second(){
#printf "  =============== Detail information =============== \n\n"
for n_subfile in $n_file/*
  do
    if [ -d $n_subfile ]
    then
      print_subinform
    fi
  done

for n_subfile in $n_file/*
  do
    if [ -f $n_subfile ]
    then
      print_subinform
    fi
  done

for n_subfile in $n_file/*
  do
    if [ -f $n_subfile ] || [ -d $n_subfile ]
    then
      printf "" 
    else
      print_subinform
    fi
  done
}  


#main function

print_form
print_first
