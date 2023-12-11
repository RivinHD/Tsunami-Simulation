#!/bin/bash

#SBATCH --job-name=launch_simulation
#SBATCH --output=launch_simulation.out

BuildDirectory="/home/$USER/tsunami/Tsunami-Simulation/build"
ScriptDirectory="/home/$USER/tsunami"

# Loading cmake to launch this task
echo "Loading needed modules"
module load tools/cmake/3.22.2
module load libs/netcdf/4.6.1-gcc-7.3.0
module load compiler/gcc/11.2.0
module load compiler/intel/2020-Update2
module load libs/netcdf/4.6.1-intel-2018


# Setting up cmake
echo "Setting up cmake"
cd "$BuildDirectory"
cmake ..

# Compiling c++
# Options:
#   --config: Release, Debug
#   --target: simulation, sanitize, test, sanitize_test, test_middle_states
echo "Building the project"
cmake --build . --config Release --target simulation

#creating ouput directory
directory=/beegfs/$USER/$(date +"%F_%H-%M")
mkdir $directory

# Coping requiered resources for this job
echo "Copying files to $directory"
cp simulation $directory/simulation
mkdir $directory/resources
cp -R resources/* $directory/resources/

echo "Launching the job"
sbatch -D "$directory" "$ScriptDirectory"/simulation.sh
