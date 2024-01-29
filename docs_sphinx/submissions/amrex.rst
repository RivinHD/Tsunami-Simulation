.. role:: raw-html(raw)
    :format: html

.. _submissions_amrex:

11. Adaptive Mesh Refinement
============================

11.1 Essential data infrastructures
-----------------------------------

To avoid misunderstandings and to provide a basic overview of AMReX data structures, let us introduce the concept of
the data types we use. We will begin by providing a list of classes, allowing you to refer back to them as needed.
Afterwards, we will use a simple diagram to help you understand the construction and relationships between the different
data types used.

Basics
^^^^^^

**Box**

"``Box`` in ``AMReX_Box.H`` is the data structure for representing a rectangular domain in indexing space. ``Box`` is a
dimension-dependent class. It has lower and upper corners (represented by `IntVect <https://amrex-codes.github.io/amrex/docs_html/Basics.html#intvect>`_)
and an index type (represented by `IndexType <https://amrex-codes.github.io/amrex/docs_html/Basics.html#indextype>`_).
A ``Box`` contains no floating-point data."[6]_

**BoxArray**

"``BoxArray`` is a class in ``AMReX_BoxArray.H`` for storing a collection of Boxes on a single AMR level. One can make a
``BoxArray`` out of a single ``Box`` and then chop it into multiple Boxes. In AMReX, ``BoxArray`` is a global data structure.
It holds all the Boxes in a collection, even though a single process in a parallel run only owns someof the Boxes via
domain decomposition."[4]_

**DistributionMapping**

"``DistributionMapping`` is a class in ``AMReX_DistributionMapping.H`` that describes which process owns the data living on the
domains specified by the Boxes in a ``BoxArray``. Like ``BoxArray``, there is an element for each ``Box`` in ``DistributionMapping``,
including the ones owned by other parallel processes. One can construct a ``DistributionMapping`` object given a ``BoxArray``,
or by simply making a copy."[5]_

**BaseFab**

"``BaseFab`` is a class template for multi-dimensional array-like data structure on a ``Box``. The template parameter
is typically basic types such as Real, int or char. The dimensionality of the array is ``AMREX_SPACEDIM`` (here 2) plus
one. The additional dimension is for the number of components."[7]_

**Array4**

"Array4 is a class template for accessing BaseFab data in a more array like manner."[7]_

**FabArray**

"``FabArray<FAB>`` is a class template in AMReX_FabArray.H for a collection of FABs on the same AMR level associated
with a ``BoxArray``. The template parameter ``FAB`` is usually ``BaseFab<T>``".[8]_

**MultiFab**

"In AMReX, there are some specialized classes derived from ``FabArray``. The most commonly used ``FabArray`` kind class
is ``MultiFab`` in AMReX_MultiFab.H derived from ``FabArray <FArrayBox>``
(`FArrayBox <https://amrex-codes.github.io/amrex/docs_html/Basics.html#basefab-farraybox-iarraybox-and-array4>`_)."[8]_

Structure
^^^^^^^^^

**Basic structure**

.. raw:: html

    <center>
        <img src="../_static/photos/AMR_Explanation_01.png" alt="AMReX structure overall">
    </center>

**Neighbouring cells in detail**

.. raw:: html

    <center>
        <img src="../_static/photos/AMR_Explanation_02.png" alt="AMReX structure box">
    </center>

Flowchart
^^^^^^^^^

The program's flowchart is presented here. The following chapter will provide an explanation of the process, giving an
overview of the steps that the program must execute.

.. error::

    Insert flowchart!

11.2 Code-Walkthrough
---------------------

In this section, we want to explain the process of our programme step by step, so that you can become familiar with the
code. We are going to start the course in our ``main.cpp`` file and follow along with the code.

Initialize & Finalize
^^^^^^^^^^^^^^^^^^^^^

"To use AMReX, we need to call ``Initialize`` to initialise the execution environment for AMReX, and ``Finalize`` needs
to be paired with Initialize to free the resources used by AMReX. Because many AMReX classes and functions don't work
properly after amrex::Finalize is called, it's best to put the code between amrex::Initialize and amrex::Finalize in its
scope to make sure that resources are freed properly"[1]_.

.. code-block:: cpp
    :emphasize-lines: 6, 8

    /// File: 'root/src/main.cpp'

    int main( int /*i_argc*/, char* i_argv[] )
    {
        [ ... ]
        amrex::Initialize( argc, argv )
        { ... }
        amrex::Finalize();
        return EXIT_SUCCESS;
    }

ParmParse
^^^^^^^^^

Before starting a simulation, the user must define it's configuration. To simplify this process, you only need to adjust
the parameters in the ``root/resources/inputs.amrex file``.  "We use the AMReX class ``AMReX_ParmParse.H``, which
provides a database for storing and retrieving command line and input file arguments"[2]_. This technique is used
throughout the project to get the correct parameters when they are needed. Here is an example of how to get the
displacement and bathymetry file paths:

.. code-block:: cpp

    /// File: 'root/src/main.cpp'

    [ ... ]
    amrex::ParmParse ppTsunami( "tsunami" );
    std::string bathymetryFile;
    std::string displacementFile;
    ppTsunami.query( "bathymetry_file", bathymetryFile );
    ppTsunami.query( "displacement_file", displacementFile );
    [ ... ]

The setup and preparation process of the simulation starts by initializing our ``AMRCoreWavePropagation2d``.

.. code-block:: cpp

    /// File: 'root/src/main.cpp'

    [ ... ]
    tsunami_lab::amr::AMRCoreWavePropagation2d* waveProp = new tsunami_lab::amr::AMRCoreWavePropagation2d( setup );
    [ ... ]

InitFromScratch
^^^^^^^^^^^^^^^

The **constructor** calls ``ReadParameters`` to retrieve input file parameters and then resizes our characteristics.
***THIS IS NECESSARY TO???**. We set the refinement ratio for each level and established our boundaries with confidence.
To initialize our data, we pass the start time, which is still zero, to ``InitFromScratch``.

.. code-block:: cpp
    :emphasize-lines: 4, 5, 24

    /// File:     'root/src/AMRCoreWavePropagation2d.cpp'
    /// Function: 'AMRCoreWavePropagation2d'

    ReadParameters();
    //resize to the levels
    [ ... ]
    // set the refinement ratio for each level for subcycling
    for( int lev = 1; lev <= max_level; ++lev )
    {
        nSubSteps[lev] = MaxRefRatio( lev - 1 );
    }

    // set the interpolation method
    for( int dim = 0; dim < AMREX_SPACEDIM; ++dim )
    {
        for( int n = 0; n < nComponents; ++n )
        {
            physicalBoundary[n].setLo( dim, BCType::foextrap );
            physicalBoundary[n].setHi( dim, BCType::foextrap );
        }
    }

    // init the domain
    InitFromScratch( 0.0 );

This initializes ``BoxArray``, ``DistributionMapping`` and data from scratch. Calling this function requires the derive class
implement its own ``MakeNewLevelFromScratch`` (root/src/AMRCoreWavePropagation2d.cpp) to allocate and initialize data.
This method makes a new level from scratch using provided ``BoxArray`` and ``DistributionMapping`` and then calls
``InitData`` to initialize our datastructures.

InitData
^^^^^^^^

The data from the setup is read into the grid by ``InitData``. To accomplish this, we will now provide a detailed
explanation of how to loop over the grid and access its cells. Working with AMReX will require this as a prerequisite.

.. code-block:: cpp
    :linenos:
    :emphasize-lines: 5, 19-20

    /// File:     'root/src/AMRCoreWavePropagation2d.cpp'
    /// Function: 'InitData'

    [ ... ]
    for( MFIter mfi( gridNew[level], true ); mfi.isValid(); ++mfi )
    {
        Box bx = mfi.tilebox();

        // size in x & y direction
        const Real dx = geom[level].CellSize( 0 );
        const Real dy = geom[level].CellSize( 1 );

        Array4<amrex::Real> height = gridNew[level].array( mfi, HEIGHT );
        Array4<amrex::Real> momentumX = gridNew[level].array( mfi, MOMENTUM_X );
        Array4<amrex::Real> momentumY = gridNew[level].array( mfi, MOMENTUM_Y );
        Array4<amrex::Real> bathymetry = gridNew[level].array( mfi, BATHYMERTRY );
        Array4<amrex::Real> error = gridNew[level].array( mfi, ERROR );

        amrex::ParallelFor( bx,
                            [=] AMREX_GPU_DEVICE( int i, int j, int k )
        {
            amrex::Real x = i * dx;
            amrex::Real y = j * dy;
            height( i, j, k ) = setup->getHeight( x, y );
            momentumX( i, j, k ) = setup->getMomentumX( x, y );
            momentumY( i, j, k ) = setup->getMomentumY( x, y );
            bathymetry( i, j, k ) = setup->getBathymetry( x, y );
            error( i, j, k ) = 0;
        } );
    }
    [ ... ]

"Above we see how you can operate on the ``MultiFab`` data with your own functions. AMReX provides an iterator, ``MFIter``
for looping over the ``FArrayBoxes`` in ``MultiFabs``. MFIter only loops over grids owned by this process."[9]_
``Tiling`` is being used in this ``MFiter`` loop because it was set to true in line five. ``Tiling`` improves data
locality. One way to achieve this is by transforming loops into tiling loops that iterate over tiles and element loops
that iterate over the data elements within a tile. We use tiling only where it makes sense. For example, it improves our
``ySweep`` but not our ``xSweep``, which will be introduced later.

1. passing ``true`` when defining ``MFIter`` to indicate tiling

2. calling ``tilebox`` instead of ``validbox`` to obtain the work region for the loop iteration

To simplify data management, we create an ``Array4`` for each component that holds its specific values.
Currently, we are only iterating over the ``Boxes`` of our ``MultiFab``. In order to iterate over the cells,
we will use ``ParallelFor``. "``ParallelFor`` takes two arguments. The first argument is a ``Box`` specifying the
iteration index space, and the second argument is a C++ lambda function that works on cell (i,j,k). Variables a, b and c
in the lambda function are captured by value from the enclosing scope. The code above is performance portable."[10]_

Evolve
^^^^^^

We have completed the setup of our simulation and returned to our ``main.cpp``. Let's jump into
``AMRCoreWavePropagation2d.cpp`` where the real simulation is done. Our starting point is

.. code-block:: cpp

    /// File: 'root/src/main.cpp'

    waveProp->Evolve();

WritePlotFile
^^^^^^^^^^^^^

The ``Evolve`` method progresses through time step by step using a loop. The program confidently determines whether to
generate a plot file at the start. We call ``WritePlotFile`` to write a simulation step.

.. code-block:: cpp

    /// File:     'root/src/AMRCoreWavePropagation2d.cpp'
    /// Function: 'Evolve'

    [ ... ]
    if( currentTime >= writes * writeFrequency )
    {
        writes++;
        WritePlotFile();
    }
    [ ... ]

The tsunami-specific values 'Height', 'MomentumX', 'MomentumY', 'Bathymetry', and 'Error' are defined in WritePlotFile
and are intended to be saved in the plot. We call the provided function ``WriteMultiLevelPlotfile`` to create the actual
plot. "AMReX has its own native plotfile format. Many visualization tools are available for AMReX plotfiles"[3]_.
We used **ParaView** to visualize the plot files with confidence. If you want to get more information about this we
recommend the chapter `Visualization <https://amrex-codes.github.io/amrex/docs_html/Visualization.html#paraview>`_.

timeStepWithSubcycling
^^^^^^^^^^^^^^^^^^^^^^

``timeStepWithSubcycling`` advances a level by dt, includes a recursive call for finer levels. First of all we check
if we want to regrid. To regrid, three conditions must be met. The current level must be lower than the maximum
refinement level ``max_level``. Than we have to be sure that we dont regrid fine levels again if it was taken care of
during a coarse regird. Additionally, we need to check if it is time to regrid based on the ``regridFrequency`` variable,
which defines the number of time steps between each regrid.

.. code-block:: cpp
    :emphasize-lines: 5-7

    /// File:     'root/src/AMRCoreWavePropagation2d.cpp'
    /// Function: 'timeStepWithSubcycling'

    [ ... ]
    if( level < max_level
        && step[level] > lastRegridStep[level]
        && step[level] % regridFrequency == 0 )
    {
        // regrid could add newly refine levels (if finest_level < max_level)
        // so we save the previous finest level index
        int oldFinest = finest_level;
        regrid( level, time );

        // mark that we have regridded this level already
        for( int k = level; k <= finest_level; ++k )
        {
            lastRegridStep[k] = step[k];
        }

        // if there are newly created levels, set the time step
        for( int k = oldFinest + 1; k <= finest_level; ++k )
        {
            dt[k] = dt[k - 1] / MaxRefRatio( k - 1 );
        }
    }
    [ ... ]

After regridding, we advance one level for one time step by calling ``AdvanceGridAtLevel``.

.. code-block:: cpp

    /// File:     'root/src/AMRCoreWavePropagation2d.cpp'
    /// Function: 'timeStepWithSubcycling'

    [ ... ]
    AdvanceGridAtLevel( level, time, dt[level], iteration, nSubSteps[level] );
    [ ... ]

Before we go into this method, let us look at the last part of ``timeStepWithSubcycling``. As the name suggests, we also
aim to progress through time on a smaller scale. To achieve this, we use the recursive procedure.

.. code-block:: cpp
    :emphasize-lines: 10, 14

    /// File:     'root/src/AMRCoreWavePropagation2d.cpp'
    /// Function: 'timeStepWithSubcycling'

    [ ... ]
    if( level < finest_level )
    {
        // recursive call for next-finer level
        for( int i = 1; i <= nSubSteps[level + 1]; ++i )
        {
            timeStepWithSubcycling( level + 1, time + ( i - 1 ) * dt[level + 1], i );
        }

        // update level based on coarse-fine flux mismatch
        AverageDownTo( level ); // average level+1 down to level
    }

As we can see, we are calling the subroutines and then using ``AverageDownTo`` to average down across multiple levels.
We defined this method ourselves to limit the arguments of the ``average_down`` method provided by ``AMReX``.

AdvanceGridAtLevel
^^^^^^^^^^^^^^^^^^



Contribution
------------

All team members contributed equally to the tasks.

.. [6] From https://amrex-codes.github.io/amrex/docs_html/Basics.html#box-intvect-and-indextype (29.01.2024)
.. [4] From https://amrex-codes.github.io/amrex/docs_html/Basics.html#boxarray (29.01.2024)
.. [5] From https://amrex-codes.github.io/amrex/docs_html/Basics.html#distributionmapping (29.01.2024)
.. [7] From https://amrex-codes.github.io/amrex/docs_html/Basics.html#basefab-farraybox-iarraybox-and-array4 (29.01.2024)
.. [8] From https://amrex-codes.github.io/amrex/docs_html/Basics.html#sec-basics-multifab (29.01.2024)
.. [1] From https://amrex-codes.github.io/amrex/docs_html/Basics.html#initialize-and-finalize (28.01.2024)
.. [2] From https://amrex-codes.github.io/amrex/docs_html/Basics.html#parmparse (28.01.2024)
.. [3] From https://amrex-codes.github.io/amrex/docs_html/IO.html# (28.01.2024)
.. [9] From https://amrex-codes.github.io/amrex/docs_html/Basics.html#mfiter-and-tiling (29.01.2024)
.. [10] From https://amrex-codes.github.io/amrex/docs_html/Basics.html#parallelfor (29.01.2024)

