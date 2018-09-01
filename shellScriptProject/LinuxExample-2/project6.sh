#!/bin/bash

declare -i width=120                                                  #set frame width
declare -i length=30                                                  #set frame length
declare -i px                                                         #set present x
declare -i py                                                         #set present y
declare -i ax=20                                                      #set addition for x
declare -i ay=6                                                       #set addition for y
declare -i cx                                                         #declare cursor x
declare -i cy                                                         #declare cursor y
a_c=("$null" "$null" "$null" "$null")                                 #array for copy
a_m=("$null" "$null" "$null" "$null")                                 #array for move

set_base(){
  p_f_list=`ls -a | grep '\.\.'; ls -1F | grep '/$'; ls -1F | grep '*$'; ls -1F | grep -v '[/*|]$'`  #present file list < directory
  for AB in *
  do
    if ! [ -d $AB ] && ! [ -f $AB ]
    then
      p_f_list="$p_f_list $AB"                                        #to deal with special file
    fi
  done
  a_list=($p_f_list)
}

print_equal(){                                                        #print =====...
  printf " "
  for ((line=1 ; line<$width ; line++))
  do
    printf "="
  done
  printf "\n"
}
 
make_frame(){                                                         #print total frame
  clear                                                               #first, clear terminal
 
  print_equal                                                       
  for ((i=0 ; i<$length ; i++))
  do
    echo "|                   |"                                      #print 1st left right frame
    tput cup `expr $i + 1` $width                                     #print 2nd right frame
    echo "|"
  done
  tput cup 0 20                                                       #no connection with length, so x represent 20
  printf " "
  tput cup $i $width
  printf " "
  tput cup $i 0
 
  print_equal                                                       
  for ((i=`expr $length + 1` ; i<`expr $length + 7` ; i++))           #print 3rd left right frame
  do
    echo "|"
    tput cup $i $width
    echo "|"
  done
  tput cup $length 20
  printf " "
  tput cup $i 0

  print_equal                                                       
  printf "|"                                                          #print 4th left right frame
  tput cup `expr $i + 1` $width
  echo "|"
  print_equal
}

print_4th_inform(){                                                   #print 4th information
  pt_num=`ls | wc -l`                                                 #present total number
  pd_num=`ls -lF | grep '/$' | wc -l`                                 #present directory number
  ps_num=`ls -lF | grep '|$' | wc -l`                                 #present special file number
  pf_num=`expr $pt_num - $pd_num - $ps_num`                           #present file number
  pt_size=`du -h . | tail -n 1 | head -c 4`                              #present total size
  tput cup 38 30
  echo "$pt_num total   $pd_num directory   $pf_num file   $ps_num s-file   $pt_size total size"
#  tput cup 31
}

print_1st_inform(){                                                   #print 1st information
  declare -i i=2
  p_dir=`pwd`
  cd ..
  tput cup `expr $i - 1` 1
  tput setaf 1
  echo ".."
  
  for up_file in *                                                    #upper directory files
  do
    tput cup $i 1
    if [ -d $up_file ]
    then
      tput setaf 4
      echo "$up_file" | cut -b -10                                        
    elif [ -f $up_file ]
    then
      if [ -x $up_file ]
      then
        tput setaf 1
        echo "$up_file" | cut -b -10                                       
      else
        tput setaf 7
        echo "$up_file" | cut -b -10                                    
      fi
    else
      tput setaf 2
      echo "$up_file" | cut -b -10                                         
    fi
    tput setaf 7
    i=`expr $i + 1`
    
    if [ $i -eq 22 ]                                                  #if over 20 line, break
    then
      break
    fi
  done
  cd $p_dir
}

print_3rd_inform(){                                                   #print 3rd information
  tput cup `expr $length + 1` 30
  echo "file name : `stat -c %n ${a_list[$I]}`"
  tput cup `expr $length + 2` 30
   if [ -d ${a_list[$I]} ]
     then
       echo -e [34m"file type : `stat -c %F ${a_list[$I]}`"         #]
     elif [ -f ${a_list[$I]} ]
     then
       if [ -x ${a_list[$I]} ]
       then
         echo -e [31m"file type : `stat -c %F ${a_list[$I]}`"       #]
       else
         echo -e [0m"file type : `stat -c %F ${a_list[$I]}`"        #]
       fi
     else
       echo -e [32m"file type : `stat -c %F ${a_list[$I]}`"         #]
   fi
  tput cup `expr $length + 3` 30
  echo -e [0m"file size : `stat -c %s ${a_list[$I]}`"               #]
  tput cup `expr $length + 4` 30
  echo "modification time : `stat -c %y ${a_list[$I]}`"
  tput cup `expr $length + 5` 30
  echo "permition : `stat -c %a ${a_list[$I]}`"
  tput cup `expr $length + 6` 30
  echo "absolute path : `realpath -e ${a_list[$I]}`"
  tput cup $cy $cx
}

print_d_icon(){                                                       #print direcory icon / blue color
  if [ -d ${a_list[$i+$scroll*5]} ]
  then
    if [ "${a_list[$i+$scroll*5]}" = ".." ]
    then
      tput setaf 1
      if [ `expr $py + 4` = $cy ] && [ $px == $cx ]                   #if selected
      then
        tput rev
      fi
      echo '  -----'                                              
      tput cup `expr $py + 1` $px
      echo '--    -'
      tput cup `expr $py + 2` $px
      echo '-  D  -'
      tput cup `expr $py + 3` $px
      echo '-------'
      tput cup `expr $py + 4` $px
      echo `stat -c %n ${a_list[$i+$scroll*5]}` | cut -b -10
      tput setaf 7
      if [ `expr $py + 4` = $cy ] && [ $px == $cx ]                   #if selected
      then
        tput sgr0
      fi
    else
      tput setaf 4 
      if [ `expr $py + 4` = $cy ] && [ $px == $cx ]                   #if selected
      then
        tput rev
      fi
      echo '  -----'  
      tput cup `expr $py + 1` $px
      echo '--    -'
      tput cup `expr $py + 2` $px
      echo '-  D  -'
      tput cup `expr $py + 3` $px
      echo '-------'
      tput cup `expr $py + 4` $px
      echo `stat -c %n ${a_list[$i+$scroll*5]}` | cut -b -10
      tput setaf 7
      if [ `expr $py + 4` = $cy ] && [ $px == $cx ]                   #if selected
      then
        tput sgr0
      fi
    fi
  fi
}

print_o_icon(){
  if [ `expr $py + 4` = $cy ] && [ $px == $cx ]                       #if selected
  then
    tput rev
  fi
  echo '-------'
  tput cup `expr $py + 1` $px
  echo '-     -'
  tput cup `expr $py + 2` $px
  echo '-  F  -'
  tput cup `expr $py + 3` $px
  echo '-------'
  tput cup `expr $py + 4` $px
  echo `stat -c %n ${a_list[$i+$scroll*5]}` | cut -b -10
  if [ `expr $py + 4` = $cy ] && [ $px == $cx ]                       #if selected
  then
    tput sgr0
  fi
}

print_x_icon(){                                                       #print excutive file icon / yellow color
  tput setaf 1
  print_o_icon
  tput setaf 7
}

print_s_icon(){                                                       #print special file icon / green color
  tput setaf 2
  print_o_icon
  tput setaf 7
}

print_icon(){                                                         #print icon
  px=23                                                              
  py=1

  for ((i=0 ; i<25 ; i++))
  do
    if [ "${a_list[$i+$scroll*5]}" = "$NULL" ]                        #if file number < 25, don't print reaminder icon
    then
      break
    fi
    
    tput cup $py $px                                                  #adjust x, y
    if [ -d ${a_list[$i+$scroll*5]} ]                                 #if f_list is directory 
    then
      print_d_icon
    elif [ -f ${a_list[$i+$scroll*5]} ]                               #if f_list is file
    then
      if [ -x ${a_list[$i+$scroll*5]} ]                               #if f_list is excutive file
      then
        print_x_icon
      else                                                            #if f_list is ordinary file
        print_o_icon
      fi
    else
      print_s_icon                                                    #if f_list is special file
    fi
    
    px=`expr $px + $ax`                                               #change x

    if [ $px -ge $width ]                                             #if x over width
    then
      px=23                                                           #reset x
      py=`expr $py + $ay`                                             #next icon line
    elif [ $py -ge $length ]                                          #if y over length
    then
      break
    fi

  done
}

copy(){                                                               #copy 
  a_c[$ic]=`realpath -e ${a_list[$I]}`
  if [ $ic -ge 3 ]
  then
    a_c[0]=${a_c[1]}
    a_c[1]=${a_c[2]}
    a_c[2]=${a_c[3]}
    ic=3
  fi
  ic=$(( $ic + 1 ))
}

move(){                                                               #move
  a_m[$im]=`realpath -e ${a_list[$I]}`
  if [ $im -ge 3 ]
  then
    a_m[0]=${a_m[1]}
    a_m[1]=${a_m[2]}
    a_m[2]=${a_m[3]}
    im=3
  fi
  im=$(( $im + 1 ))
}

cursoring(){                                                          #impement cursor
  set_base
  cx=23                                                               #set cursor x, y
  cy=5
  kb_hit=0                                                            #key board hit 
  declare -i scroll=0                                                 #scroll
  declare -i I=0                                                      #file index
  declare -i ic=0                                                     #index for copy
  declare -i im=0                                                     #index for move
  while [ 1 ]
  do
    make_frame
    print_1st_inform
    print_4th_inform
    print_icon
    print_3rd_inform
    
    read -sn 1 kb_hit
    if [ "$kb_hit" = "A" ]                                            # hit up button
    then
      cy=`expr $cy - $ay`
      I=`expr $I - 5`
    elif [ "$kb_hit" = "B" ]                                          # hit down button
    then
      cy=`expr $cy + $ay`
      I=`expr $I + 5`
    elif [ "$kb_hit" = "C" ]                                          # hit right button
    then
      cx=`expr $cx + $ax`
      I=`expr $I + 1`
    elif [ "$kb_hit" = "D" ]                                          # hit left button
    then
      cx=`expr $cx - $ax`
      I=`expr $I - 1`
    elif [ "$kb_hit" = "" ]                                           #hit space bar
    then
      if [ -d ${a_list[$I]} ]                                         #if directory
      then
        cd `realpath -e ${a_list[$I]}`
        cursoring
      elif [ -x ${a_list[$I]} ]                                       #if excutive file
      then
        clear
        ./${a_list[$I]}
      fi
    elif [ "$kb_hit" = "c" ]                                          #if want to copy
    then
      copy
    elif [ "$kb_hit" = "p" ]                                          #if want to paste
    then
      for((j=0; j<3; j++))
      do
        if [ "${a_c[$j]}" = "$NULL" ]
	then
	  break
	fi
        cp ${a_c[$j]} $PWD
      done
    elif [ "$kb_hit" = "m" ]                                          #if want to move
    then
      move
    elif [ "$kb_hit" = "v" ]                                          #if want to move
    then
      for((j=0; j<3; j++))
      do
        if [ "${a_m[$j]}" = "$NULL" ]
	then
	  break
	fi
        cp ${a_m[$j]} $PWD
      done
    elif [ "$kb_hit" = "q" ]                                          #if hit q button, break
    then
      break
    else
      continue
    fi

    if [ $cx -le 20 ]                                                 #if cursor is out of 2nd frame's left line
    then
      cx=`expr $cx + $ax`
      I=`expr $I + 1`
    elif [ $cx -ge $width ]                                           #if cursor is out of 2nd frame's right line
    then
      cx=`expr $cx - $ax`
      I=`expr $I - 1`
    elif [ $cy -le 0 ]                                                #if cursor is out of 2nd frame's top line
    then
      cy=`expr $cy + $ay`
      I=`expr $I + 5`
      if [ $scroll -ge 1 ]                                            
      then
        scroll=`expr $scroll - 1`
      fi
    elif [ $cy -ge $length ]                                          #if cursor is out of 2nd frame's bottom line
    then
      cy=`expr $cy - $ay`
      I=`expr $I - 5`
      scroll=`expr $scroll + 1`
    fi    
    tput cup $cy $cx                                                  #set cursor next place
  done
}

cursoring
