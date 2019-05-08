#!/bin/bash

parts1=(1 2)
parts2=(2 4 8)

rm time.txt

for part1 in ${parts1[@]}; do
    for part2 in ${parts2[@]}; do      
        echo ${part1} proc x node ${part2} num nodes--------------
        echo ${part2} "proc x node" ${part2} "num nodes //////////////////" >> time.txt
        let procs=${part1}*${part2}
        for i in {1..5}; do
            ( salloc -p pops -N 8 srun -n 1 time mpirun -host pop1,pop2,pop3,pop4,pop5,pop6,pop7,pop8 -c $procs -npernode ${part1} ./parallel.obj >> time.txt )2>>time.txt 
        done
    done
done

parts1=(3)
parts2=(4 8)
for part1 in ${parts1[@]}; do
    for part2 in ${parts2[@]}; do      
        echo ${part1} proc x node ${part2} num nodes--------------
        echo ${part2} "proc x node" ${part2} "num nodes //////////////////" >> time.txt
        let procs=${part1}*${part2}
        for i in {1..5}; do
            ( salloc -p pops -N 8 srun -n 1 time mpirun -host pop1,pop2,pop3,pop4,pop5,pop6,pop7,pop8 -c $procs -npernode ${part1} ./parallel.obj >> time.txt )2>>time.txt 
        done
    done
done

parts1=(4)
parts2=(8)
for part1 in ${parts1[@]}; do
    for part2 in ${parts2[@]}; do      
        echo ${part1} proc x node ${part2} num nodes--------------
        echo ${part2} "proc x node" ${part2} "num nodes //////////////////" >> time.txt
        let procs=${part1}*${part2}
        for i in {1..5}; do
            ( salloc -p pops -N 8 srun -n 1 time mpirun -host pop1,pop2,pop3,pop4,pop5,pop6,pop7,pop8 -c $procs -npernode ${part1} ./parallel.obj >> time.txt )2>>time.txt 
        done
    done
done
exit 0