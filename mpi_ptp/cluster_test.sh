#!/bin/bash

file=./results/results.txt
rm -f $file
touch $file

printf "iterations\tfieldSize\tthreads\t\ttime\n" >> $file

for ((iters=10;iters<10000;iters*=10))
do
  printf "$iters iterations:\n"
  for ((size=100;size<1001;size+=100))
  do
    printf "size = $size:\n"
    for ((threads=1;threads<9;threads++))
    do
      printf "$threads threads: \n"
      printf "%s\t\t%s\t\t%s\t\t" $iters  $size  $threads >> $file
      mpirun -np $threads+1 ./life $size $size $iters >> $file
      printf "\n" >> $file
    done
  done
done
