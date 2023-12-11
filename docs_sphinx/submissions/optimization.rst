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

2. Verification
^^^^^^^^^^^^^^^

Scale in x-dimension predetermined with :math:`x: 2700000` :raw-html:`<br>`
Scale in y-dimension predetermined with :math:`y: 1500000`

Cell size: **2000m**

Required cells in x-direction: :math:`\frac{2700000}{2000}=1350` :raw-html:`<br>`
Required cells in y-direction: :math:`\frac{2700000}{2000}=750`

.. warning::

    Change videos!

.. raw:: html

    <center>
        <video width="700" controls>
            <source src="../_static/videos/tohoku_2000.mp4" type="video/mp4">
        </video>
    </center>

Cell size: **1000m**

Required cells in x-direction: :math:`\frac{2700000}{1000}=2700` :raw-html:`<br>`
Required cells in y-direction: :math:`\frac{2700000}{1000}=1500`

.. raw:: html

    <center>
        <video width="700" controls>
            <source src="../_static/videos/tohoku_1000.mp4" type="video/mp4">
        </video>
    </center>

As we can see, the results of both simulations match those in :ref:`submissions_tsunami_simulation`.

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

The data shows that the local machine is more than **twice as fast** as the ARA cluster.

8.2 Compilers
-------------

1. Support for generic compilers
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. note::

    When compiling locally and there is the need to use the INTEL ICC/ICPC compiler specify this by running
    **cmake .. -D CMAKE_C_COMPILER...**

2. INTEL vs GNU compiler
^^^^^^^^^^^^^^^^^^^^^^^^

+--------------------------------------------------------------------+---------------------------------------------------------------------+
| :raw-html:`<center>INTEL</center>`                                 | :raw-html:`<center>GNU</center>`                                    |
+====================================================================+=====================================================================+
| :raw-html:`<center>./simulation 1350 750 -B -w 60 -t 13000 -c 5</center>`                                                                |
+--------------------------------------------------------------------+---------------------------------------------------------------------+
|                                                                    |                                                                     |
|   .. code-block:: bash                                             |   .. code-block:: bash                                              |
|       :emphasize-lines: 27-29                                      |       :emphasize-lines: 27-29                                       |
|                                                                    |                                                                     |
|       Start executing 'simulation 1350 750 -B -w 60 -t 13000 -c 5':|       Start executing 'simulation 1350 750 -B -w 60 -t 13000 -c 5': |
|       #####################################################        |       #####################################################         |
|       ###                  Tsunami Lab                  ###        |       ###                  Tsunami Lab                  ###         |
|       ###                                               ###        |       ###                                               ###         |
|       ### https://scalable.uni-jena.de                  ###        |       ### https://scalable.uni-jena.de                  ###         |
|       ### https://rivinhd.github.io/Tsunami-Simulation/ ###        |       ### https://rivinhd.github.io/Tsunami-Simulation/ ###         |
|       #####################################################        |       #####################################################         |
|       Checking for Checkpoints: File IO is disabled!               |       Checking for Checkpoints: File IO is disabled!                |
|       Simulation is set to 2D                                      |       Simulation is set to 2D                                       |
|       Bathymetry is Enabled                                        |       Bathymetry is Enabled                                         |
|       Set Solver: FWave                                            |       Set Solver: FWave                                             |
|       Activated Reflection on None side                            |       Activated Reflection on None side                             |
|       Output format is set to netCDF                               |       Output format is set to netCDF                                |
|       Writing the X-/Y-Axis in format meters                       |       Writing the X-/Y-Axis in format meters                        |
|       Simulation Time is set to 13000 seconds                      |       Simulation Time is set to 13000 seconds                       |
|       Writing to the disk every 60 seconds of simulation time      |       Writing to the disk every 60 seconds of simulation time       |
|       Checkpointing every 5 minutes                                |       Checkpointing every 5 minutes                                 |
|       runtime configuration                                        |       runtime configuration                                         |
|         number of cells in x-direction:       1350                 |         number of cells in x-direction:       1350                  |
|         number of cells in y-direction:       750                  |         number of cells in y-direction:       750                   |
|         cell size:                            2000                 |         cell size:                            2000                  |
|         number of cells combined to one cell: 1                    |         number of cells combined to one cell: 1                     |
|       Max speed 306.636                                            |       Max speed 306.636                                             |
|       entering time loop                                           |       entering time loop                                            |
|       finished time loop                                           |       finished time loop                                            |
|       freeing memory                                               |       freeing memory                                                |
|       The Simulation took 0 h 2 min 58 sec to finish.              |       The Simulation took 0 h 10 min 37 sec to finish.              |
|       Time per iteration: 40 milliseconds.                         |       Time per iteration: 143 milliseconds.                         |
|       Time per cell:      39 nanoseconds.                          |       Time per cell:      142 nanoseconds.                          |
|       finished, exiting                                            |       finished, exiting                                             |
|                                                                    |                                                                     |
+--------------------------------------------------------------------+---------------------------------------------------------------------+
| :raw-html:`<center>./simulation 2700 1500 -B -w 60 -t 13000 -c 5</center>`                                                               |
+--------------------------------------------------------------------+---------------------------------------------------------------------+
|                                                                    |                                                                     |
|   .. code-block:: bash                                             |   .. code-block:: bash                                              |
|       :emphasize-lines: 27-29                                      |       :emphasize-lines: 27-29                                       |
|                                                                    |                                                                     |
|       ./simulation 2700 1500 -B -w 60 -t 13000 -c 5                |       Start executing 'simulation 2700 1500 -B -w 60 -t 13000 -c 5':|
|       #####################################################        |       #####################################################         |
|       ###                  Tsunami Lab                  ###        |       ###                  Tsunami Lab                  ###         |
|       ###                                               ###        |       ###                                               ###         |
|       ### https://scalable.uni-jena.de                  ###        |       ### https://scalable.uni-jena.de                  ###         |
|       ### https://rivinhd.github.io/Tsunami-Simulation/ ###        |       ### https://rivinhd.github.io/Tsunami-Simulation/ ###         |
|       #####################################################        |       #####################################################         |
|       Checking for Checkpoints: File IO is disabled!               |       Checking for Checkpoints: File IO is disabled!                |
|       Simulation is set to 2D                                      |       Simulation is set to 2D                                       |
|       Bathymetry is Enabled                                        |       Bathymetry is Enabled                                         |
|       Set Solver: FWave                                            |       Set Solver: FWave                                             |
|       Activated Reflection on None side                            |       Activated Reflection on None side                             |
|       Output format is set to netCDF                               |       Output format is set to netCDF                                |
|       Writing the X-/Y-Axis in format meters                       |       Writing the X-/Y-Axis in format meters                        |
|       Simulation Time is set to 13000 seconds                      |       Simulation Time is set to 13000 seconds                       |
|       Writing to the disk every 60 seconds of simulation time      |       Writing to the disk every 60 seconds of simulation time       |
|       Checkpointing every 5 minutes                                |       Checkpointing every 5 minutes                                 |
|       runtime configuration                                        |       runtime configuration                                         |
|         number of cells in x-direction:       2700                 |         number of cells in x-direction:       2700                  |
|         number of cells in y-direction:       1500                 |         number of cells in y-direction:       1500                  |
|         cell size:                            1000                 |         cell size:                            1000                  |
|         number of cells combined to one cell: 1                    |         number of cells combined to one cell: 1                     |
|       Max speed 307.668                                            |       Max speed 307.668                                             |
|       entering time loop                                           |       entering time loop                                            |
|       finished time loop                                           |       finished time loop                                            |
|       freeing memory                                               |       freeing memory                                                |
|       The Simulation took 0 h 24 min 32 sec to finish.             |       The Simulation took 1 h 28 min 28 sec to finish.              |
|       Time per iteration: 165 milliseconds.                        |       Time per iteration: 597 milliseconds.                         |
|       Time per cell:      40 nanoseconds.                          |       Time per cell:      147 nanoseconds.                          |
|       finished, exiting                                            |       finished, exiting                                             |
|                                                                    |                                                                     |
+--------------------------------------------------------------------+---------------------------------------------------------------------+


8.3 Instrumentation and Performance Counters
--------------------------------------------


Contribution
------------

All team members contributed equally to the tasks.
