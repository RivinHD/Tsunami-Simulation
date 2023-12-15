.. role:: raw-html(raw)
    :format: html

.. _submissions_optimization:

8. Optimization
===============

8.1. ARA
--------

1. Setup on cluster
^^^^^^^^^^^^^^^^^^^

Follow the instructions off :ref:`getting_started_building_project`.

To simplify matters, we have created two Slurm scripts for running our simulations. The first one ``launchSimulation.sh``
compiles our code and is a wrapper to start the actually script ``simulation.sh`` in the right direction.

.. warning::

    Change to final scrips!

.. code-block:: bash

    ///File: launchSimulation.sh
    #!/bin/bash
    #SBATCH --job-name=launch_simulation
    #SBATCH --output=launch_simulation.out
    #SBATCH --partition=s_standard
    #SBATCH --nodes=1
    #SBATCH --ntasks=1
    #SBATCH --time=00:10:00
    #SBATCH --cpus-per-task=2
    #SBATCH --mem=4G
    
    set -e

    BuildDirectory="/home/$USER/tsunami/Tsunami-Simulation/build"
    ScriptDirectory="/home/$USER/tsunami"

    # Loading cmake to launch this task
    echo "Loading needed modules"
    module load tools/cmake/3.22.2
    module load libs/netcdf/4.6.1-gcc-7.3.0
    module load compiler/gcc/11.2.0
    module load compiler/intel/2020-Update2


    # Cleaning up Build Directory
    echo "Cleaning up Build Directory"
    cd "$BuildDirectory"
    rm -rf "$BuildDirectory"
    mkdir "$BuildDirectory"

    # Setting up cmake
    echo "Setting up cmake"
    # intel compiler can only be used without io
    CC="/cluster/intel/parallel_studio_xe_2020.2.108/compilers_and_libraries_2020/linux/bin/intel64/icc" \
    CXX="/cluster/intel/parallel_studio_xe_2020.2.108/compilers_and_libraries_2020/linux/bin/intel64/icpc" \
    cmake .. -DCMAKE_BUILD_TYPE=Release -D DISABLE_IO=ON

    # Compiling c++
    # Options:
    #   --config: Release, Debug
    #   --target: simulation, sanitize, test, sanitize_test, test_middle_states
    echo "Building the project"
    cmake --build . --target simulation

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


``simulation.sh`` then runs the actual simulation on a long term node with lots of resources.

.. code-block:: bash

    ///File: simulation.sh
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

    ./simulation 2700 1500 -B -w 60 -t 13000 -c 5


2. Verification
^^^^^^^^^^^^^^^

Scale in x-dimension predetermined with :math:`x: 2700000` :raw-html:`<br>`
Scale in y-dimension predetermined with :math:`y: 1500000`

Cell size: **2000m**

Required cells in x-direction: :math:`\frac{2700000}{2000}=1350` :raw-html:`<br>`
Required cells in y-direction: :math:`\frac{2700000}{2000}=750`

.. raw:: html

    <center>
        <video width="700" controls>
            <source src="../_static/videos/tohoku_2000_ara.mp4" type="video/mp4">
        </video>
    </center>

Cell size: **1000m**

Required cells in x-direction: :math:`\frac{2700000}{1000}=2700` :raw-html:`<br>`
Required cells in y-direction: :math:`\frac{2700000}{1000}=1500`

.. raw:: html

    <center>
        <video width="700" controls>
            <source src="../_static/videos/tohoku_1000_ara.mp4" type="video/mp4">
        </video>
    </center>

As we can see, the results of both simulations match those in :ref:`submissions_tsunami_simulation_tohoku`.

3. Comparison
^^^^^^^^^^^^^

+---------------------------------------------------------------+---------------------------------------------------------------------+
| :raw-html:`<center>Local machine</center>`                    | :raw-html:`<center>ARA cluster</center>`                            |
+===============================================================+=====================================================================+
| :raw-html:`<center>./simulation 1350 750 -B -w 60 -t 13000 -c 5</center>`                                                           |
+---------------------------------------------------------------+---------------------------------------------------------------------+
|                                                               |                                                                     |
|   .. code-block:: bash                                        |   .. code-block:: bash                                              |
|       :emphasize-lines: 27-29                                 |       :emphasize-lines: 27-29                                       |
|                                                               |                                                                     |
|       ./simulation 1350 750 -B -w 60 -t 13000 -c 5            |       Start executing 'simulation 1350 750 -B -w 60 -t 13000 -c 5': |
|       #####################################################   |       #####################################################         |
|       ###                  Tsunami Lab                  ###   |       ###                  Tsunami Lab                  ###         |
|       ###                                               ###   |       ###                                               ###         |
|       ### https://scalable.uni-jena.de                  ###   |       ### https://scalable.uni-jena.de                  ###         |
|       ### https://rivinhd.github.io/Tsunami-Simulation/ ###   |       ### https://rivinhd.github.io/Tsunami-Simulation/ ###         |
|       #####################################################   |       #####################################################         |
|       Checking for Checkpoints: File IO is disabled!          |       Checking for Checkpoints: File IO is disabled!                |
|       Simulation is set to 2D                                 |       Simulation is set to 2D                                       |
|       Bathymetry is Enabled                                   |       Bathymetry is Enabled                                         |
|       Set Solver: FWave                                       |       Set Solver: FWave                                             |
|       Activated Reflection on None side                       |       Activated Reflection on None side                             |
|       Output format is set to netCDF                          |       Output format is set to netCDF                                |
|       Writing the X-/Y-Axis in format meters                  |       Writing the X-/Y-Axis in format meters                        |
|       Simulation Time is set to 13000 seconds                 |       Simulation Time is set to 13000 seconds                       |
|       Writing to the disk every 60 seconds of simulation time |       Writing to the disk every 60 seconds of simulation time       |
|       Checkpointing every 5 minutes                           |       Checkpointing every 5 minutes                                 |
|       runtime configuration                                   |       runtime configuration                                         |
|         number of cells in x-direction:       1350            |         number of cells in x-direction:       1350                  |
|         number of cells in y-direction:       750             |         number of cells in y-direction:       750                   |
|         cell size:                            2000            |         cell size:                            2000                  |
|         number of cells combined to one cell: 1               |         number of cells combined to one cell: 1                     |
|       Max speed 306.636                                       |       Max speed 306.636                                             |
|       entering time loop                                      |       entering time loop                                            |
|       finished time loop                                      |       finished time loop                                            |
|       freeing memory                                          |       freeing memory                                                |
|       The Simulation took 0 h 5 min 0 sec to finish.          |       The Simulation took 0 h 10 min 37 sec to finish.              |
|       Time per iteration: 67 milliseconds.                    |       Time per iteration: 143 milliseconds.                         |
|       Time per cell:      67 nanoseconds.                     |       Time per cell:      142 nanoseconds.                          |
|       finished, exiting                                       |       finished, exiting                                             |
|                                                               |                                                                     |
+---------------------------------------------------------------+---------------------------------------------------------------------+
| :raw-html:`<center>./simulation 2700 1500 -B -w 60 -t 13000 -c 5</center>`                                                          |
+---------------------------------------------------------------+---------------------------------------------------------------------+
|                                                               |                                                                     |
|   .. code-block:: bash                                        |   .. code-block:: bash                                              |
|       :emphasize-lines: 27-29                                 |       :emphasize-lines: 27-29                                       |
|                                                               |                                                                     |
|       ./simulation 2700 1500 -B -w 60 -t 13000 -c 5           |       Start executing 'simulation 2700 1500 -B -w 60 -t 13000 -c 5':|
|       #####################################################   |       #####################################################         |
|       ###                  Tsunami Lab                  ###   |       ###                  Tsunami Lab                  ###         |
|       ###                                               ###   |       ###                                               ###         |
|       ### https://scalable.uni-jena.de                  ###   |       ### https://scalable.uni-jena.de                  ###         |
|       ### https://rivinhd.github.io/Tsunami-Simulation/ ###   |       ### https://rivinhd.github.io/Tsunami-Simulation/ ###         |
|       #####################################################   |       #####################################################         |
|       Checking for Checkpoints: File IO is disabled!          |       Checking for Checkpoints: File IO is disabled!                |
|       Simulation is set to 2D                                 |       Simulation is set to 2D                                       |
|       Bathymetry is Enabled                                   |       Bathymetry is Enabled                                         |
|       Set Solver: FWave                                       |       Set Solver: FWave                                             |
|       Activated Reflection on None side                       |       Activated Reflection on None side                             |
|       Output format is set to netCDF                          |       Output format is set to netCDF                                |
|       Writing the X-/Y-Axis in format meters                  |       Writing the X-/Y-Axis in format meters                        |
|       Simulation Time is set to 13000 seconds                 |       Simulation Time is set to 13000 seconds                       |
|       Writing to the disk every 60 seconds of simulation time |       Writing to the disk every 60 seconds of simulation time       |
|       Checkpointing every 5 minutes                           |       Checkpointing every 5 minutes                                 |
|       runtime configuration                                   |       runtime configuration                                         |
|         number of cells in x-direction:       2700            |         number of cells in x-direction:       2700                  |
|         number of cells in y-direction:       1500            |         number of cells in y-direction:       1500                  |
|         cell size:                            1000            |         cell size:                            1000                  |
|         number of cells combined to one cell: 1               |         number of cells combined to one cell: 1                     |
|       Max speed 307.668                                       |       Max speed 307.668                                             |
|       entering time loop                                      |       entering time loop                                            |
|       finished time loop                                      |       finished time loop                                            |
|       freeing memory                                          |       freeing memory                                                |
|       The Simulation took 0 h 40 min 24 sec to finish.        |       The Simulation took 1 h 28 min 28 sec to finish.              |
|       Time per iteration: 272 milliseconds.                   |       Time per iteration: 597 milliseconds.                         |
|       Time per cell:      67 nanoseconds.                     |       Time per cell:      147 nanoseconds.                          |
|       finished, exiting                                       |       finished, exiting                                             |
|                                                               |                                                                     |
+---------------------------------------------------------------+---------------------------------------------------------------------+

The data shows that the local machine is more than **twice as fast** as the ARA cluster (with ``-O0``).

8.2 Compilers
-------------

1. Support for generic compilers
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To change the compiler on the **ARA cluster** we have to specify the path in the ``launchSimulation.sh``

.. code-block:: bash
    :emphasize-lines: 7-8

    ///File: launchSimulation.sh
    [ ... ]
    # Setting up cmake
    echo "Setting up cmake"
    cd "$BuildDirectory"
    # intel compiler can only be used without io
    CC="/cluster/intel/parallel_studio_xe_2020.2.108/compilers_and_libraries_2020/linux/bin/intel64/icc" \
    CXX="/cluster/intel/parallel_studio_xe_2020.2.108/compilers_and_libraries_2020/linux/bin/intel64/icpc" \
    cmake .. -DCMAKE_BUILD_TYPE=Release -D DISABLE_IO=ON
    [ ... ]

If you are compiling on your local machine or on another server, you can pass the path of your compiler to **cmake** via

.. code-block:: bash

    CC=path/to/c/compiler CXX=path/to/c++/compiler cmake .. -DCMAKE_BUILD_TYPE=Release

or with

.. code-block:: bash

    cmake -D CMAKE_C_COMPILER=path/to/c/compiler -D CMAKE_CXX_COMPILER=path/to/c++/compiler .. -DCMAKE_BUILD_TYPE=Release


2. INTEL vs GNU compiler
^^^^^^^^^^^^^^^^^^^^^^^^

+---------------------------------------------------------------------+---------------------------------------------------------------------+
| :raw-html:`<center>INTEL</center>`                                  | :raw-html:`<center>GNU</center>`                                    |
+=====================================================================+=====================================================================+
| :raw-html:`<center>./simulation 1350 750 -B -w 60 -t 13000 -c 5</center>`                                                                 |
+---------------------------------------------------------------------+---------------------------------------------------------------------+
|                                                                     |                                                                     |
|   .. code-block:: bash                                              |   .. code-block:: bash                                              |
|       :emphasize-lines: 27-29                                       |       :emphasize-lines: 27-29                                       |
|                                                                     |                                                                     |
|       Start executing 'simulation 1350 750 -B -w 60 -t 13000 -c 5': |       Start executing 'simulation 1350 750 -B -w 60 -t 13000 -c 5': |
|       #####################################################         |       #####################################################         |
|       ###                  Tsunami Lab                  ###         |       ###                  Tsunami Lab                  ###         |
|       ###                                               ###         |       ###                                               ###         |
|       ### https://scalable.uni-jena.de                  ###         |       ### https://scalable.uni-jena.de                  ###         |
|       ### https://rivinhd.github.io/Tsunami-Simulation/ ###         |       ### https://rivinhd.github.io/Tsunami-Simulation/ ###         |
|       #####################################################         |       #####################################################         |
|       Checking for Checkpoints: File IO is disabled!                |       Checking for Checkpoints: File IO is disabled!                |
|       Simulation is set to 2D                                       |       Simulation is set to 2D                                       |
|       Bathymetry is Enabled                                         |       Bathymetry is Enabled                                         |
|       Set Solver: FWave                                             |       Set Solver: FWave                                             |
|       Activated Reflection on None side                             |       Activated Reflection on None side                             |
|       Output format is set to netCDF                                |       Output format is set to netCDF                                |
|       Writing the X-/Y-Axis in format meters                        |       Writing the X-/Y-Axis in format meters                        |
|       Simulation Time is set to 13000 seconds                       |       Simulation Time is set to 13000 seconds                       |
|       Writing to the disk every 60 seconds of simulation time       |       Writing to the disk every 60 seconds of simulation time       |
|       Checkpointing every 5 minutes                                 |       Checkpointing every 5 minutes                                 |
|       runtime configuration                                         |       runtime configuration                                         |
|         number of cells in x-direction:       1350                  |         number of cells in x-direction:       1350                  |
|         number of cells in y-direction:       750                   |         number of cells in y-direction:       750                   |
|         cell size:                            2000                  |         cell size:                            2000                  |
|         number of cells combined to one cell: 1                     |         number of cells combined to one cell: 1                     |
|       Max speed 306.636                                             |       Max speed 306.636                                             |
|       entering time loop                                            |       entering time loop                                            |
|       finished time loop                                            |       finished time loop                                            |
|       freeing memory                                                |       freeing memory                                                |
|       The Simulation took 0 h 2 min 55 sec to finish.               |       The Simulation took 0 h 3 min 33 sec to finish.               |
|       Time per iteration: 39 milliseconds.                          |       Time per iteration: 48 milliseconds.                          |
|       Time per cell:      39 nanoseconds.                           |       Time per cell:      47 nanoseconds.                           |
|       finished, exiting                                             |       finished, exiting                                             |
|                                                                     |                                                                     |
+---------------------------------------------------------------------+---------------------------------------------------------------------+
| :raw-html:`<center>./simulation 2700 1500 -B -w 60 -t 13000 -c 5</center>`                                                                |
+---------------------------------------------------------------------+---------------------------------------------------------------------+
|                                                                     |                                                                     |
|   .. code-block:: bash                                              |   .. code-block:: bash                                              |
|       :emphasize-lines: 27-29                                       |       :emphasize-lines: 27-29                                       |
|                                                                     |                                                                     |
|       Start executing 'simulation 2700 1500 -B -w 60 -t 13000 -c 5':|       Start executing 'simulation 2700 1500 -B -w 60 -t 13000 -c 5':|
|       #####################################################         |       #####################################################         |
|       ###                  Tsunami Lab                  ###         |       ###                  Tsunami Lab                  ###         |
|       ###                                               ###         |       ###                                               ###         |
|       ### https://scalable.uni-jena.de                  ###         |       ### https://scalable.uni-jena.de                  ###         |
|       ### https://rivinhd.github.io/Tsunami-Simulation/ ###         |       ### https://rivinhd.github.io/Tsunami-Simulation/ ###         |
|       #####################################################         |       #####################################################         |
|       Checking for Checkpoints: File IO is disabled!                |       Checking for Checkpoints: File IO is disabled!                |
|       Simulation is set to 2D                                       |       Simulation is set to 2D                                       |
|       Bathymetry is Enabled                                         |       Bathymetry is Enabled                                         |
|       Set Solver: FWave                                             |       Set Solver: FWave                                             |
|       Activated Reflection on None side                             |       Activated Reflection on None side                             |
|       Output format is set to netCDF                                |       Output format is set to netCDF                                |
|       Writing the X-/Y-Axis in format meters                        |       Writing the X-/Y-Axis in format meters                        |
|       Simulation Time is set to 13000 seconds                       |       Simulation Time is set to 13000 seconds                       |
|       Writing to the disk every 60 seconds of simulation time       |       Writing to the disk every 60 seconds of simulation time       |
|       Checkpointing every 5 minutes                                 |       Checkpointing every 5 minutes                                 |
|       runtime configuration                                         |       runtime configuration                                         |
|         number of cells in x-direction:       2700                  |         number of cells in x-direction:       2700                  |
|         number of cells in y-direction:       1500                  |         number of cells in y-direction:       1500                  |
|         cell size:                            1000                  |         cell size:                            1000                  |
|         number of cells combined to one cell: 1                     |         number of cells combined to one cell: 1                     |
|       Max speed 307.668                                             |       Max speed 307.668                                             |
|       entering time loop                                            |       entering time loop                                            |
|       finished time loop                                            |       finished time loop                                            |
|       freeing memory                                                |       freeing memory                                                |
|       The Simulation took 0 h 24 min 30 sec to finish.              |       The Simulation took 0 h 30 min 17 sec to finish.              |
|       Time per iteration: 165 milliseconds.                         |       Time per iteration: 204 milliseconds.                         |
|       Time per cell:      40 nanoseconds.                           |       Time per cell:      50 nanoseconds.                           |
|       finished, exiting                                             |       finished, exiting                                             |
|                                                                     |                                                                     |
+---------------------------------------------------------------------+---------------------------------------------------------------------+

As we can observe, the Intel compiler is a big step ahead of the GNU compiler (with ``-O2``).

3. INTEL vs GNU flags
^^^^^^^^^^^^^^^^^^^^^

**Numerical accuracy**

An increase in numerical inaccuracy in the ``GNU`` compiler begins with the flag -Ofast. It enables all -O3 optimizations
and turns on -ffast-math. This option can result in incorrect output for programs that depend on an exact implementation
of IEEE or ISO rules/specifications for math functions.

Increasing numerical inaccuracy in the **INTEL icpc** compiler also starts with using the -Ofast flag. It sets the
compiler options -O3, -no-prec-div and -fp-model fast=2. -no-prec-div improves the precision of floating-point division.
It has a small impact on speed. -fp-model fast=2 tells the compiler to use more aggressive optimisations when implementing
floating-point calculations.  These optimisations increase speed, but may reduce the accuracy or reproducibility of
floating-point calculations. floating-point calculations.

+---------------------------------------------------------------------+---------------------------------------------------------------------+
| :raw-html:`<center>INTEL</center>`                                  | :raw-html:`<center>GNU</center>`                                    |
+=====================================================================+=====================================================================+
| :raw-html:`<center>./simulation 2700 1500 -B -w 60 -t 13000 -c 5</center>`                                                                |
+---------------------------------------------------------------------+---------------------------------------------------------------------+
| :raw-html:`<center>-O2</center>`                                                                                                          |
+---------------------------------------------------------------------+---------------------------------------------------------------------+
|                                                                     |                                                                     |
|   .. code-block:: bash                                              |   .. code-block:: bash                                              |
|       :emphasize-lines: 27-29                                       |       :emphasize-lines: 27-29                                       |
|                                                                     |                                                                     |
|       Start executing 'simulation 2700 1500 -B -w 60 -t 13000 -c 5':|       Start executing 'simulation 2700 1500 -B -w 60 -t 13000 -c 5':|
|       #####################################################         |       #####################################################         |
|       ###                  Tsunami Lab                  ###         |       ###                  Tsunami Lab                  ###         |
|       ###                                               ###         |       ###                                               ###         |
|       ### https://scalable.uni-jena.de                  ###         |       ### https://scalable.uni-jena.de                  ###         |
|       ### https://rivinhd.github.io/Tsunami-Simulation/ ###         |       ### https://rivinhd.github.io/Tsunami-Simulation/ ###         |
|       #####################################################         |       #####################################################         |
|       Checking for Checkpoints: File IO is disabled!                |       Checking for Checkpoints: File IO is disabled!                |
|       Simulation is set to 2D                                       |       Simulation is set to 2D                                       |
|       Bathymetry is Enabled                                         |       Bathymetry is Enabled                                         |
|       Set Solver: FWave                                             |       Set Solver: FWave                                             |
|       Activated Reflection on None side                             |       Activated Reflection on None side                             |
|       Output format is set to netCDF                                |       Output format is set to netCDF                                |
|       Writing the X-/Y-Axis in format meters                        |       Writing the X-/Y-Axis in format meters                        |
|       Simulation Time is set to 13000 seconds                       |       Simulation Time is set to 13000 seconds                       |
|       Writing to the disk every 60 seconds of simulation time       |       Writing to the disk every 60 seconds of simulation time       |
|       Checkpointing every 5 minutes                                 |       Checkpointing every 5 minutes                                 |
|       runtime configuration                                         |       runtime configuration                                         |
|         number of cells in x-direction:       2700                  |         number of cells in x-direction:       2700                  |
|         number of cells in y-direction:       1500                  |         number of cells in y-direction:       1500                  |
|         cell size:                            1000                  |         cell size:                            1000                  |
|         number of cells combined to one cell: 1                     |         number of cells combined to one cell: 1                     |
|       Max speed 307.668                                             |       Max speed 307.668                                             |
|       entering time loop                                            |       entering time loop                                            |
|       finished time loop                                            |       finished time loop                                            |
|       freeing memory                                                |       freeing memory                                                |
|       The Simulation took 0 h 24 min 30 sec to finish.              |       The Simulation took 0 h 30 min 17 sec to finish.              |
|       Time per iteration: 165 milliseconds.                         |       Time per iteration: 204 milliseconds.                         |
|       Time per cell:      40 nanoseconds.                           |       Time per cell:      50 nanoseconds.                           |
|       finished, exiting                                             |       finished, exiting                                             |
|                                                                     |                                                                     |
+---------------------------------------------------------------------+---------------------------------------------------------------------+
| :raw-html:`<center>-O3</center>`                                                                                                          |
+---------------------------------------------------------------------+---------------------------------------------------------------------+
|                                                                     |                                                                     |
|   .. code-block:: bash                                              |   .. code-block:: bash                                              |
|       :emphasize-lines: 27-29                                       |       :emphasize-lines: 27-29                                       |
|                                                                     |                                                                     |
|       Start executing 'simulation 2700 1500 -B -w 60 -t 13000 -c 5':|       Start executing 'simulation 2700 1500 -B -w 60 -t 13000 -c 5':|
|       #####################################################         |       #####################################################         |
|       ###                  Tsunami Lab                  ###         |       ###                  Tsunami Lab                  ###         |
|       ###                                               ###         |       ###                                               ###         |
|       ### https://scalable.uni-jena.de                  ###         |       ### https://scalable.uni-jena.de                  ###         |
|       ### https://rivinhd.github.io/Tsunami-Simulation/ ###         |       ### https://rivinhd.github.io/Tsunami-Simulation/ ###         |
|       #####################################################         |       #####################################################         |
|       Checking for Checkpoints: File IO is disabled!                |       Checking for Checkpoints: File IO is disabled!                |
|       Simulation is set to 2D                                       |       Simulation is set to 2D                                       |
|       Bathymetry is Enabled                                         |       Bathymetry is Enabled                                         |
|       Set Solver: FWave                                             |       Set Solver: FWave                                             |
|       Activated Reflection on None side                             |       Activated Reflection on None side                             |
|       Output format is set to netCDF                                |       Output format is set to netCDF                                |
|       Writing the X-/Y-Axis in format meters                        |       Writing the X-/Y-Axis in format meters                        |
|       Simulation Time is set to 13000 seconds                       |       Simulation Time is set to 13000 seconds                       |
|       Writing to the disk every 60 seconds of simulation time       |       Writing to the disk every 60 seconds of simulation time       |
|       Checkpointing every 5 minutes                                 |       Checkpointing every 5 minutes                                 |
|       runtime configuration                                         |       runtime configuration                                         |
|         number of cells in x-direction:       2700                  |         number of cells in x-direction:       2700                  |
|         number of cells in y-direction:       1500                  |         number of cells in y-direction:       1500                  |
|         cell size:                            1000                  |         cell size:                            1000                  |
|         number of cells combined to one cell: 1                     |         number of cells combined to one cell: 1                     |
|       Max speed 307.668                                             |       Max speed 307.668                                             |
|       entering time loop                                            |       entering time loop                                            |
|       finished time loop                                            |       finished time loop                                            |
|       freeing memory                                                |       freeing memory                                                |
|       The Simulation took 0 h 24 min 53 sec to finish.              |       The Simulation took 0 h 30 min 20 sec to finish.              |
|       Time per iteration: 168 milliseconds.                         |       Time per iteration: 204 milliseconds.                         |
|       Time per cell:      41 nanoseconds.                           |       Time per cell:      50 nanoseconds.                           |
|       finished, exiting                                             |       finished, exiting                                             |
|                                                                     |                                                                     |
+---------------------------------------------------------------------+---------------------------------------------------------------------+
| :raw-html:`<center>-Ofast</center>`                                                                                                       |
+---------------------------------------------------------------------+---------------------------------------------------------------------+
|                                                                     |                                                                     |
|   .. code-block:: bash                                              |   .. code-block:: bash                                              |
|       :emphasize-lines: 27-29                                       |       :emphasize-lines: 27-29                                       |
|                                                                     |                                                                     |
|       Start executing 'simulation 2700 1500 -B -w 60 -t 13000 -c 5':|       Start executing 'simulation 2700 1500 -B -w 60 -t 13000 -c 5':|
|       #####################################################         |       #####################################################         |
|       ###                  Tsunami Lab                  ###         |       ###                  Tsunami Lab                  ###         |
|       ###                                               ###         |       ###                                               ###         |
|       ### https://scalable.uni-jena.de                  ###         |       ### https://scalable.uni-jena.de                  ###         |
|       ### https://rivinhd.github.io/Tsunami-Simulation/ ###         |       ### https://rivinhd.github.io/Tsunami-Simulation/ ###         |
|       #####################################################         |       #####################################################         |
|       Checking for Checkpoints: File IO is disabled!                |       Checking for Checkpoints: File IO is disabled!                |
|       Simulation is set to 2D                                       |       Simulation is set to 2D                                       |
|       Bathymetry is Enabled                                         |       Bathymetry is Enabled                                         |
|       Set Solver: FWave                                             |       Set Solver: FWave                                             |
|       Activated Reflection on None side                             |       Activated Reflection on None side                             |
|       Output format is set to netCDF                                |       Output format is set to netCDF                                |
|       Writing the X-/Y-Axis in format meters                        |       Writing the X-/Y-Axis in format meters                        |
|       Simulation Time is set to 13000 seconds                       |       Simulation Time is set to 13000 seconds                       |
|       Writing to the disk every 60 seconds of simulation time       |       Writing to the disk every 60 seconds of simulation time       |
|       Checkpointing every 5 minutes                                 |       Checkpointing every 5 minutes                                 |
|       runtime configuration                                         |       runtime configuration                                         |
|         number of cells in x-direction:       2700                  |         number of cells in x-direction:       2700                  |
|         number of cells in y-direction:       1500                  |         number of cells in y-direction:       1500                  |
|         cell size:                            1000                  |         cell size:                            1000                  |
|         number of cells combined to one cell: 1                     |         number of cells combined to one cell: 1                     |
|       Max speed 307.668                                             |       Max speed 307.668                                             |
|       entering time loop                                            |       entering time loop                                            |
|       finished time loop                                            |       finished time loop                                            |
|       freeing memory                                                |       freeing memory                                                |
|       The Simulation took 0 h 24 min 41 sec to finish.              |       The Simulation took 0 h 27 min 39 sec to finish.              |
|       Time per iteration: 166 milliseconds.                         |       Time per iteration: 186 milliseconds.                         |
|       Time per cell:      41 nanoseconds.                           |       Time per cell:      46 nanoseconds.                           |
|       finished, exiting                                             |       finished, exiting                                             |
|                                                                     |                                                                     |
+---------------------------------------------------------------------+---------------------------------------------------------------------+

The Intel compiler is the fastest overall, with the fastest optimization being O2.
With the GNU compiler, the fastest time is Ofast, whereas O2 and O3 are almost the same in terms of speed.

3. Optimization Report
^^^^^^^^^^^^^^^^^^^^^^

The GNU compiler generates an optimization report with the option ``-fopt-info-optimized=opt_gnu.optrpt`` and creates this :download:`Optimization Report <../_static/resources/opt_gnu_O2.optrpt>`.
Mostly it inline ``functions`` and ``constexpr`` inside the same object and from the imported libraries.
It also unrolled small loops and distributed some loops into library calls.
Furthermore it sinks common stores with same value.
Unfortunately the compiler does not vectorize the code, but at least inlined the F-Wave solver.

8.3 Instrumentation and Performance Counters
--------------------------------------------


Contribution
------------

All team members contributed equally to the tasks.
