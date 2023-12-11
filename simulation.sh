#!/bin/bash

#SBATCH --job-name=tsunami_simulation
#SBATCH --output=simulation.out
#SBATCH --partition=s_hadoop
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --time=120:00:00
#SBATCH --cpus-per-task=72
#SBATCH --mem=128G

# Load any necessary modules
# module load module_name
module load libs/netcdf/4.6.1-gcc-7.3.0
module load compiler/gcc/11.2.0

echo "Start executing 'simulation 2700 1500 -B -w 60 -t 13000 -c 5':"

./simulation 2700 1500 -B -w 60 -t 13000 -c 5
