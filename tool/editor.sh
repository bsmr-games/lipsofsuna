#!/bin/sh

echo "Available patterns: "
echo $(ls ./data/lipsofsuna/scripts/content/patterns)

NUM=1

if [ -e ./data/lipsofsuna/scripts/content/patterns/$@.lua ]; then 
  echo "file found, opening"
  else
  echo "Not an existing pattern, making a new one"
  echo "Pattern{ \n	name = \"$@\" ,\n	size = Vector(32,8,32)}" > ./data/lipsofsuna/scripts/content/patterns/$@.lua
  echo "require \"content/patterns/$@\"" >> ./data/lipsofsuna/scripts/content/patterns.lua
fi

while [ -e ./data/lipsofsuna/scripts/content/patterns/$@mod$NUM.lua ] ; do

  echo " $NUM file exists, incrementing "
  NUM=$(($NUM+1)) 

done

./lipsofsuna lipsofsuna -E $@ > ./data/lipsofsuna/scripts/content/patterns/$@mod$NUM.lua
     
