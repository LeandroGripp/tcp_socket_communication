#! /bin/bash

gcc client.c -o client

for i in {1..10}
do
  sed -i "s/$(($i - 1))/$i/g" input.txt
  echo "Creating client $i"
  ((cat input.txt | ./client $1 > ./output/output$i.txt 2>&1) & ) &
  sleep 0.5
done

sed -i "s/"10"/"0"/g" input.txt
