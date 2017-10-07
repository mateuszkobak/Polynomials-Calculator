if [ -x "$1" ] && [ -d "$2" ]
then
  IFS=$'\n'
  file=`grep -r -H -l -m 1 'START' $2`
  tmpfile=$(mktemp)
  out=$(mktemp)
  sed '1d' $file > $tmpfile
  lastline=`echo ok`


  while [ "$lastline" != "STOP" ]
  do
    lastline=`tail -n 1 $tmpfile`
    sed -i '$ d' $tmpfile
    $1 <$tmpfile >$out
    if [ "$lastline" == "STOP" ]
      then
        cat $out
      else
        file_name="$2/${lastline#* }"
        cat $out > $tmpfile
        cat $file_name >> $tmpfile
    fi
  done

  rm $tmpfile
  rm $out
  return 0
else
  if [ -x "$1" ]
  then echo "Plik nie jest wykonywalny"
  else echo "Podano nie folder"
  fi
  return 1
fi
