.. role:: raw-html(raw)
    :format: html

.. _submissions_parallelization:

9. Parallelization
==================

1. Parallelization using OpenMP
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. warning::

    Add two example code fragments of parallelization

2. Comparison serial and parallelized solver
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Without parallelization**

.. code-block:: bash
    :emphasize-lines: 27-29

    Start executing 'simulation 2700 1500 -B -w 60 -t 13000 -c 5':
    #####################################################
    ###                  Tsunami Lab                  ###
    ###                                               ###
    ### https://scalable.uni-jena.de                  ###
    ### https://rivinhd.github.io/Tsunami-Simulation/ ###
    #####################################################
    Checking for Checkpoints: File IO is disabled!
    Simulation is set to 2D
    Bathymetry is Enabled
    Set Solver: FWave
    Activated Reflection on None side
    Output format is set to netCDF
    Writing the X-/Y-Axis in format meters
    Simulation Time is set to 13000 seconds
    Writing to the disk every 60 seconds of simulation time
    Checkpointing every 5 minutes
    runtime configuration
      number of cells in x-direction:       2700
      number of cells in y-direction:       1500
      cell size:                            1000
      number of cells combined to one cell: 1
    Max speed 307.668
    entering time loop
    finished time loop
    freeing memory
    The Simulation took 1 h 28 min 28 sec to finish.
    Time per iteration: 597 milliseconds.
    Time per cell:      147 nanoseconds.
    finished, exiting

**With parallelization**

.. code-block:: bash
    :emphasize-lines: 3-5

    start executing 'MP_NUM_THREADS=72 ./simulation 2700 1500 -B -w 60 -t 13000 -c 5':
    finished writing to 'solutions/simulation/solution.nc'. Use ncdump to view its contents.
    The Simulation took 0 h 3 min 39 sec to finish.
    Time per iteration: 24 milliseconds.
    Time per cell:      6 nanoseconds.
    finished, exiting

You can see that we have a run time of around 1 h 28 min without parallelization and a run time of just 3 min 39 sec with
parallelization. That's 24 times faster in comparison to the serial solver.

3. Parallelization of the outer vs. the inner loops
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. warning::

    Did we test this?

4. Different scheduling and pinning strategies
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**All threads**

.. code-block:: bash
    :emphasize-lines: 3-5

    start executing 'MP_NUM_THREADS=72 ./simulation 2700 1500 -B -w 60 -t 13000 -c 5':
    finished writing to 'solutions/simulation/solution.nc'. Use ncdump to view its contents.
    The Simulation took 0 h 3 min 39 sec to finish.
    Time per iteration: 24 milliseconds.
    Time per cell:      6 nanoseconds.
    finished, exiting

**Every core without their second thread**

.. code-block:: bash
    :emphasize-lines: 3-5

    Start executing 'OMP_NUM_THREADS=36 OMP_PLACES={0}:36:1 ./simulation 2700 1500 -B -w 60 -t 13000 -c 5':
    finished writing to 'solutions/simulation/solution.nc'. Use ncdump to view its contents.
    The Simulation took 0 h 2 min 45 sec to finish.
    Time per iteration: 18 milliseconds.
    Time per cell:      4 nanoseconds.
    finished, exiting

**Only half of the cores with their second thread**

.. code-block:: bash
    :emphasize-lines: 3-5

    Start executing 'OMP_NUM_THREADS=36 OMP_PLACES={0}:36:2 ./simulation 2700 1500 -B -w 60 -t 13000 -c 5':
    finished writing to 'solutions/simulation/solution.nc'. Use ncdump to view its contents.
    The Simulation took 0 h 3 min 24 sec to finish.
    Time per iteration: 22 milliseconds.
    Time per cell:      5 nanoseconds.
    finished, exiting

**Only half of the cores without their second thread**

.. code-block:: bash
    :emphasize-lines: 3-5

    Start executing 'OMP_NUM_THREADS=18 OMP_PLACES={0}:18:1 ./simulation 2700 1500 -B -w 60 -t 13000 -c 5':
    finished writing to 'solutions/simulation/solution.nc'. Use ncdump to view its contents.
    The Simulation took 0 h 4 min 9 sec to finish.
    Time per iteration: 28 milliseconds.
    Time per cell:      6 nanoseconds.
    finished, exiting

.. warning::

    Just a first approach

You can see that the second strategy is the best one in respect to the run time. It makes also sense that the simulation
time goes up the less cores and threads are used. The reason for a faster run time of the second run in comparison to the
first one is not easy to clarify. Indeed their is a maximum of parallelization which goes down with more and more communication
between the threads. Also the cores can handle more load when not using their second thread if there is to much data to
swap between them.