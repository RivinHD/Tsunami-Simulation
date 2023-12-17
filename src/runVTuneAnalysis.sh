#!/bin/bash

#SBATCH --job-name=run_vtune_analysis
#SBATCH --output=vtune_analysis.out
#SBATCH --partition=b_standard
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --time=04:00:00
#SBATCH --cpus-per-task=36
#SBATCH --mem=64G

set -e

OutputDirectory=/home/$USER/tsunami/analysis_$(date +"%F_%H-%M")
ScriptDirectory=/home/$USER/tsunami

# Loading cmake to launch this task
echo "Loading needed modules"
module load compiler/intel/2020-Update2
module load compiler/gcc/11.2.0

mkdir $OutputDirectory
cd $OutputDirectory
echo $OutputDirectory

echo "Start VTune analysis."
# replace the line below with your configured VTune project command
/cluster/intel/vtune_profiler_2020.2.0.610396/bin64/vtune -collect hotspots -app-working-dir /beegfs/ho62zoq/tsunami/Tsunami-Simulation/build -- /beegfs/ho62zoq/tsunami/Tsunami-Simulation/build/simulation 1350 750 -B -w 60 -t 13000 -c 5
printf "Finished analysis.\nResults in directory '$OutputDirectory'.\n"
