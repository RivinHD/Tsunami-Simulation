#!/bin/bash

#SBATCH --job-name=tsunami_simulation
#SBATCH --output=simulation.out
#SBATCH --partition=s_hadoop
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --time=120:00:00
#SBATCH --cpus-per-task=72
#SBATCH --mem=128G

echo "Start executing 'OMP_NUM_THREADS=36 OMP_PLACES={0}:36:1 ./simulation':"

OMP_NUM_THREADS=36 OMP_PLACES={0}:36:1 ./simulation
