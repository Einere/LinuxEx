#!/bin/bash

declare -i width=120                                                #set frame width
declare -i length=25                                                #set frame length
declare -i px                                                       #set present x
declare -i py                                                       #set present y
declare -i ax=20                                                    #set addition for x
declare -i ay=6                                                     #set addition for y
declare -i cx                                                       #declare cursor x
declare -i cy                                                       #declare cursor y

p_f_list=`ls -a | grep '\.\.'; ls -1F | grep '/$'; ls -1F | grep '*$'; ls -1F | grep -v '[/*|]$'`  #present file list < directory
for AB in *
do
  if ! [ -d $AB ] && ! [ -f $AB ]
  then
    p_f_list="$p_f_list $AB"                                        #to deal with special file
  fi
done
a_list=($p_f_list)

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
  for ((i=`expr $length + 1` ; i<32 ; i++))                         #print 3rd left right frame
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
  tput cup 33 $width
  echo "|"
  print_equal
}

print_4th_inform(){                                                 #print 4th information
  pt_num=`ls | wc -l`                                               #present total number
  pd_num=`ls -lF | grep '/$' | wc -l`                               #present directory number
  ps_num=`ls -lF | grep '|$' | wc -l`                               #present special file number
  pf_num=`expr $pt_num - $pd_num - $ps_num`                         #present file number
  pt_size=`du . | tail -n 1 | head -c 3`                            #present total size
  tput cup 33 30
  echo "$pt_num total   $pd_num directory   $pf_num file   $ps_num s-file   $pt_size total size"
#  tput cup 31
}

print_1st_inform(){                                                 #print 1st information
  declare -i i=1
  for up_file in ../*                                        #upper directory files
  do
    tput cup $i 1
    if [ -d $up_file ]
    then
      echo [34m"$up_file" | cut -b -18                                        #]
    elif [ -f $up_file ]
    then
      if [ -x $up_file ]
      then
        echo [31m"$up_file" | cut -b -18                                       #]
      else
        echo [0m"$up_file" | cut -b -18                                     #]
      fi
    else
      echo [32m"$up_file" | cut -b -18                                         #]
    fi
    echo [0m""                                                    #]
    i=`expr $i + 1`
    
    if [ $i -eq 21 ]                                                #if over 20 line, break
    then
      break
    fi
  done
}

print_3rd_inform(){                                                 #print 3rd information
  tput cup `expr $length + 1` 30
  echo "file name : `stat -c %n ${a_list[$I]}`"
  tput cup `expr $length + 2` 30
   if [ -d ${a_list[$I]} ]
     then
       echo -e [34m"file type : `stat -c %F ${a_list[$I]}`"             #]
     elif [ -f ${a_list[$I]} ]
     then
       if [ -x ${a_list[$I]} ]
       then
         echo -e [31m"file type : `stat -c %F ${a_list[$I]}`"           #]
       else
         echo -e [0m"file type : `stat -c %F ${a_list[$I]}`"            #]
       fi
     else
       echo -e [32m"file type : `stat -c %F ${a_list[$I]}`"             #]
   fi
  tput cup `expr $length + 3` 30
  echo -e [0m"file size : `stat -c %s ${a_list[$I]}`"                    #]
  tput cup `expr $length + 4` 30
  echo "modification time : `stat -c %y ${a_list[$I]}`"
  tput cup `expr $length + 5` 30
  echo "permition : `stat -c %a ${a_list[$I]}`"
  tput cup `expr $length + 6` 30
  echo "absolute path : `realpath -e ${a_list[$I]}`"
  tput cup $cy $cx
}

print_d_icon(){                                                     #print direcory icon / blue color
  if [ -d $f_list ]
  then
    if [ "$f_list" = ".." ]
    then
      echo [31m'  -----'                                              #]
      tput cup `expr $py + 1` $px
      echo '--    -'
      tput cup `expr $py + 2` $px
      echo '-  D  -'
      tput cup `expr $py + 3` $px
      echo '-------'
      tput cup `expr $py + 4` $px
      echo `stat -c %n $f_list` | cut -b -10
      echo -n [0m                                                     #]
    else
      echo [34m'  -----'                                              #]
      tput cup `expr $py + 1` $px
      echo '--    -'
      tput cup `expr $py + 2` $px
      echo '-  D  -'
      tput cup `expr $py + 3` $px
      echo '-------'
      tput cup `expr $py + 4` $px
      echo `stat -c %n $f_list` | cut -b -10
      echo -n [0m                                                     #]
    fi
  fi
}

print_o_icon(){
  echo '-------'
  tput cup `expr $py + 1` $px
  echo '-     -'
  tput cup `expr $py + 2` $px
  echo '-  F  -'
  tput cup `expr $py + 3` $px
  echo '-------'
  tput cup `expr $py + 4` $px
  echo `stat -c %n $f_list` | cut -b -10
}

print_x_icon(){                                                     #print excutive file icon / yellow color
  echo -n [31m                                                    #] 
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

  for f_list in $p_f_list 
  do
    tput cup $py $px                                                #adjust x, y
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

    if [ $px -ge $width ]                                           #if x over width
    then
      px=23                                                         #reset x
      py=`expr $py + $ay`                                           #next icon line
    elif [ $py -ge $length ]
    then
      break
    fi
  done
}

cursoring(){                                                        #impement cursor
  cx=23                                                            #set cursor x, y
  cy=5
  kb_hit=0                                                          #key board hit 
  declare -i scroll=0                                                          #scroll
  declare -i I=0                                                    #file index
  
  while [ 1 ]
  do
    make_frame
    print_1st_inform
    print_4th_inform
    print_icon
    print_3rd_inform

    read -sn 1 kb_hit
    if [ $kb_hit = "A" ]                                            # hit up button
    then
      cy=`expr $cy - $ay`
      I=`expr $I - 5`
    elif [ $kb_hit = "B" ]                                          # hit down button
    then
      cy=`expr $cy + $ay`
      I=`expr $I + 5`
    elif [ $kb_hit = "C" ]                                          # hit right button
    then
      cx=`expr $cx + $ax`
      I=`expr $I + 1`
    elif [ $kb_hit = "D" ]                                          # hit left button
    then
      cx=`expr $cx - $ax`
      I=`expr $I - 1`
    elif [ $kb_hit = "q" ]                                             #if hit q button, break
    then
      break
    else
      continue
    fi

    if [ $cx -le 20 ]                                               #if cursor is out of frame
    then
      cx=`expr $cx + $ax`
      I=`expr $I + 1`
    elif [ $cx -ge $width ]
    then
      cx=`expr $cx - $ax`
      I=`expr $I - 1`
    elif [ $cy -le 0 ]
    then
      cy=`expr $cy + $ay`
      I=`expr $I + 5`
      scroll=$scroll - 1
    elif [ $cy -ge $length ]
    then
      cy=`expr $cy - $ay`
      I=`expr $I - 5`
      scroll=$scroll + 1
    fi
    
    tput cup $cy $cx

  done

}


#make_frame
#print_1st_inform
#print_4th_inform
#print_icon
cursoring
