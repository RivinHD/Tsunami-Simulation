#!/bin/bash

#SBATCH --job-name=tsunami_simulation
#SBATCH --output=simulation.out
#SBATCH --partition=s_hadoop
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --time=120:00:00
#SBATCH --cpus-per-task=72
#SBATCH --mem=128G

echo "Start executing 'simulation 2700 1500 -B -w 60 -t 13000 -c 5':"

OMP_NUM_THREADS=1 OMP_PLACES={0}:1:1 ./simulation 2700 1500 -B -w 60 -t 13000 -c 5
