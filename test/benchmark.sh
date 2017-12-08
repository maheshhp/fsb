nDirectories(){
  for (( i = 0; i < $1; i++ )); do
    currDir="./try/Dir$i"
    mkdir $currDir
  done

}

nDirectories $1
