#!/bin/bash

declare -i width=130                                                #set frame width
declare -i length=25                                                #set frame length
declare -i px                                                       #set present x
declare -i py                                                       #set present y
declare -i ax=20                                                    #set addition for x
declare -i ay=6                                                     #set addition for y
declare -i cx                                                       #declare cursor x
declare -i cy                                                       #declare cursor y

p_f_list=`ls -1F | grep '/$'; ls -1F | grep '*$'; ls -1F | grep -v '[/*|]$'`  #present file list < directory

for AB in *
do
  if [ -d $AB ]
  then
    $NULL
  elif [ -f $AB ]
  then
    $NULL
  else
    p_f_list="$p_f_list $AB"                                        #to deal with special file
  fi
done

print_equal(){                                                      #print =====...
  printf " "
  for ((line=1 ; line<$width ; line++))
  do
    printf "="
  done
  printf "\n"
}
 
make_frame(){                                                       #print total frame
  clear                                                             #first, clear terminal
 
  print_equal                                                       
  for ((i=0 ; i<$length ; i++))
  do
    echo "|                   |"                                    #print 1st left right frame
    tput cup `expr $i + 1` $width                                   #print 2nd right frame
    echo "|"
  done
  tput cup 0 20                                                     #no connection with length, so x represent 20
  printf " "
  tput cup $i $width
  printf " "
  tput cup $i 0
 
  print_equal                                                       
  for ((i=`expr $length + 1` ; i<28 ; i++))                         #print 3rd left right frame
  do
    echo "|"
    tput cup $i $width
    echo "|"
  done
  tput cup $length 20
  printf " "
  tput cup $i 0

  print_equal                                                       
  printf "|"                                                        #print 4th left right frame
  tput cup 29 $width
  echo "|"
  print_equal
}

print_4th_inform(){                                                 #print 4th information
  pt_num=`ls | wc -l`                                               #present total number
  pd_num=`ls -lF | grep '/$' | wc -l`                               #present directory number
  ps_num=`ls -lF | grep '|$' | wc -l`                               #present special file number
  pf_num=`expr $pt_num - $pd_num - $ps_num`                         #present file number
  pt_size=`du . | tail -n 1 | head -c 3`                            #present total size
  tput cup 29 30
  echo "$pt_num total   $pd_num directory   $pf_num file   $ps_num s-file   $pt_size total size"
  tput cup 31
}

print_1st_inform(){                                                 #print 1st information
  declare -i i=1
  for up_file in `ls -1 ..*`                                        #upper directory files
  do
    tput cup $i 1
    if [ -d $up_file ]
    then
      echo [34m"$up_file"                                        #]
    elif [ -f $up_file ]
    then
      if [ -x $up_file ]
      then
        echo [32m"$up_file"                                        #]
      else
        echo [33m"$up_file"                                      #]
      fi
    else
      echo [0m "$up_file"                                        #]
    fi
    echo [0m""                                                      #]
    i=`expr $i + 1`
    
    if [ $i -eq 21 ]                                                #if over 20 line, break
    then
      break
    fi
  done
  tput cup 31
}

print_u_icon(){                                                     #print upper(..) icon / red color
  echo [31m'  -----'                                              #]
  tput cup `expr $py + 1` $px
  echo '--    -'
  tput cup `expr $py + 2` $px
  echo '-  D  -'
  tput cup `expr $py + 3` $px
  echo '-------'
  tput cup `expr $py + 4` $px
  echo '..'
  echo -n [0m                                                     #]
}

print_d_icon(){                                                     #print direcory icon / blue color
  if [ -d $f_file ]
  then
    echo [34m'  -----'                                              #]
    tput cup `expr $py + 1` $px
    echo '--    -'
    tput cup `expr $py + 2` $px
    echo '-  D  -'
    tput cup `expr $py + 3` $px
    echo '-------'
    tput cup `expr $py + 4` $px
    echo `stat -c %n $f_list`
    echo -n [0m                                                     #]
  fi
}

print_o_icon(){
  if [ -d $f_file ]
  then
    echo '-------'
    tput cup `expr $py + 1` $px
    echo '-     -'
    tput cup `expr $py + 2` $px
    echo '-  F  -'
    tput cup `expr $py + 3` $px
    echo '-------'
    tput cup `expr $py + 4` $px
    echo `stat -c %n $f_list`
  fi
}

print_x_icon(){                                                     #print excutive file icon / yellow color
  echo -n [33m                                                    #] 
  print_o_icon
  echo -n [0m                                                     #]
}

print_s_icon(){                                                     #print special file icon / green color
  echo -n [32m                                                    #] 
  print_o_icon
  echo -n [0m                                                     #]
  
}

print_icon(){                                                       #print icon
  px=23                                                              
  py=1
  tput cup $py $px                                                  #set x, y
  print_u_icon                                                      #print upper(..) directory icon
  px=`expr $px + $ax`                                               #change x(second icon place)

  for f_list in $p_f_list 
  do
    tput cup $py $px

    if [ -d $f_list ]                                               #if f_list is directory 
    then
      print_d_icon
    elif [ -f $f_list ]                                             #if f_list is file
    then
      if [ -x $f_list ]                                             #if f_list is excutive file
      then
        print_x_icon
      else                                                          #if f_list is ordinary file
        print_o_icon
      fi
    else
      print_s_icon                                                  #if f_list is special file
    fi
    
    px=`expr $px + $ax`                                             #change x

    if [ $px -ge `expr $width - 15` ]                               #if x over width
    then
      px=23                                                         #reset x
      py=`expr $py + $ay`                                           #next icon line
    fi
  done
}

cursoring(){                                                        #impement cursor
  px=23
  py=1
  cs=$px                                                            #set cursor x, y
  cy=$py
  kb_hit=0                                                          #key board hit 
  while [ 1 ]
  do
    kb_hit=`read`

  done

}


make_frame
print_1st_inform
print_4th_inform
print_icon

tput cup 35 0
echo "$kb_git"
