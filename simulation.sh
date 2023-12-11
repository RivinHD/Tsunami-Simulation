#!/bin/bash

#SBATCH --job-name=tsunami_simulation
#SBATCH --output=simulation.out
#SBATCH --partition=s_hadoop
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --time=120:00:00
#SBATCH --cpus-per-task=72
#SBATCH --mem=128G

echo "Start executing 'simulation 5000 5000 -B -w 100 -t 1600 -c 1':"

./simulation 5000 5000 -B -w 100 -t 1600 -c 1
