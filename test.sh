if [ $1 == "-v" ]
  then
    for f in $3/*.in;
    do
      ERRORTEMP=$(mktemp)
      OUT=$($2 -v <"$f" 2>$ERRORTEMP)
      OUTTEST=$(<${f%in}out)
      ERR=$(<$ERRORTEMP)
      rm $ERRORTEMP
      ERRTEST=$(<${f%in}err)
      if [ "$OUT" != "$OUTTEST" ]
        then echo "NIEPRAWIDŁOWY TEST ${f%in}out"
      else echo "PRAWIDŁOWY TEST ${f%in}out"
      fi
      if [ "$ERR" != "$ERRTEST" ]
        then echo "NIEPRAWIDŁOWY TEST ${f%in}err"
      else echo "PRAWIDŁOWY TEST ${f%in}err"
      fi
    done
else
  for f in $2/*.in;
  do
    OUT=$($1 <"$f")
    OUTTEST=$(<${f%in}out)
    if [ "$OUT" != "$OUTTEST" ]
      then echo "${f%in}out NOPE"
    #else echo "${f%in}out YEAH"
    fi
  done
fi
