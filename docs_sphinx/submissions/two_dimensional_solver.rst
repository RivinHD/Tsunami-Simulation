.. role:: raw-html(raw)
    :format: html
    
.. _submissions_two_dimensional_solver:

4. Two-Dimensional Solver
=========================


4.1. Dimensional Splitting
--------------------------

1. Two-Dimensional Splitting
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

There are changes needed to be done to the ``F-Wave solver``, because two-dimensional splitting is used i.e, first calculating the x-direction than the y-direction.
Therefore only changes will be done in ``WavePropagation2d``.

The constructor and destructor of ``WavePropagation2d`` is very similar to ``WavePropagation1d`` except the new variables:
    - ``m_hv`` for y-momentum
    - ``m_xCells`` & ``m_yCells`` for the number of x and y cells
    - ``stride`` the size in x-direction including ghost cells
    - ``totalCells`` total number of cells including ghost cells

The functions that needed to be changed compared to ``WavePropagation2d`` are the ``get-functions``, ``timeStep`` and ``setGhostOutflow``.

**get-functions**

The get-functions now need to take the stride into account i.e. the first row need to be completely skip because ghost cells are not returned. 

**setGhostOutflow**

In the first for loop, the ghost cells are set in the y direction, except for the first and last row (ghost rows), by taking their value from the value of their left/right neighbor.
In the second loop, the first and last rows (ghost rows) are set by taking the value from the lower and upper neighbor.
In both loops the activated reflection is taken into account.

.. code-block:: cpp

    void tsunami_lab::patches::WavePropagation2d::setGhostOutflow()
    {
        t_real* l_h = m_h[m_step];
        t_real* l_hu = m_hu[m_step];
        t_real* l_hv = m_hv[m_step];

        for( t_idx i = 1; i < m_yCells + 1; i++ )
        {
            t_idx y = stride * i;

            // set left boundary
            l_h[y] = l_h[y + 1] * !hasReflection[Side::LEFT];
            l_hu[y] = l_hu[y + 1];
            l_hv[y] = l_hv[y + 1];
            m_bathymetry[y] = m_bathymetry[y + 1];

            // set right boundary
            l_h[y + m_xCells + 1] = l_h[y + m_xCells] * !hasReflection[Side::RIGHT];
            l_hu[y + m_xCells + 1] = l_hu[y + m_xCells];
            l_hv[y + m_xCells + 1] = l_hv[y + m_xCells];
            m_bathymetry[y + m_xCells + 1] = m_bathymetry[y + m_xCells];
        }

        for( size_t i = 0; i < stride; i++ )
        {
            // set top complete row of ghost cells
            t_idx waterRowT = stride + i;
            l_h[i] = l_h[waterRowT] * !hasReflection[Side::TOP];
            l_hu[i] = l_hu[waterRowT];
            l_hv[i] = l_hv[waterRowT];
            m_bathymetry[i] = m_bathymetry[waterRowT];

            // set bottom complete row of ghost cells
            t_idx ghostRowB = ( m_yCells + 1 ) * stride + i;
            t_idx waterRowB = m_yCells * stride + i;
            l_h[ghostRowB] = l_h[waterRowB] * !hasReflection[Side::BOTTOM];
            l_hu[ghostRowB] = l_hu[waterRowB];
            l_hv[ghostRowB] = l_hv[waterRowB];
            m_bathymetry[ghostRowB] = m_bathymetry[waterRowB];
        }
    }


**timeStep**

First the update is done in x-direction.
Therefore the height and x-momentum arrays are initialized.
The first for loop counts in y-direction and the second for loop counts in x-direction.
Then the index of the left and right cells are calculated.
The next steps are the same as performed in ``WavePropagation1d``.

.. code-block:: cpp
    :emphasize-lines: 12-13, 20, 23, 25-29

    // pointers to old and new data
    t_real* l_hOld = m_h[m_step];
    t_real* l_huOld = m_hu[m_step];

    m_step = ( m_step + 1 ) % 2;
    t_real* l_hNew = m_h[m_step];
    t_real* l_huNew = m_hu[m_step];

    // init new cell quantities
    for( t_idx l_ce = 0; l_ce < totalCells; l_ce++ )
    {
        l_hNew[l_ce] = l_hOld[l_ce];
        l_huNew[l_ce] = l_huOld[l_ce];
    }

    // only possible for f-wave solver
    if( hasBathymetry )
    {
        // iterates through the row
        for( t_idx i = 0; i < m_yCells + 1; i++ )
        {
            // iterates along the row
            for( t_idx j = 0; j < m_xCells + 1; j++ )
            {
                t_idx k = stride * i + j;

                // determine left and right cell-id
                t_idx l_ceL = k;
                t_idx l_ceR = k + 1;

                // noting to compute both shore cells
                if( l_hOld[l_ceL] == 0 && l_hOld[l_ceR] == 0 )
                {
                    continue;
                }

                // compute reflection
                t_real heightLeft;
                t_real heightRight;
                t_real momentumLeft;
                t_real momentumRight;
                t_real bathymetryLeft;
                t_real bathymetryRight;

                Reflection reflection = calculateReflection( l_hOld,
                                                            l_huOld,
                                                            l_ceL,
                                                            l_ceR,
                                                            heightLeft,
                                                            heightRight,
                                                            momentumLeft,
                                                            momentumRight,
                                                            bathymetryLeft,
                                                            bathymetryRight );

                // compute net-updates
                t_real l_netUpdates[2][2];

                tsunami_lab::solvers::FWave::netUpdates( heightLeft,
                                                        heightRight,
                                                        momentumLeft,
                                                        momentumRight,
                                                        bathymetryRight,
                                                        bathymetryLeft,
                                                        l_netUpdates[0],
                                                        l_netUpdates[1] );

                // update the cells' quantities
                l_hNew[l_ceL] -= i_scaling * l_netUpdates[0][0] * ( Reflection::RIGHT != reflection );
                l_huNew[l_ceL] -= i_scaling * l_netUpdates[0][1] * ( Reflection::RIGHT != reflection );

                l_hNew[l_ceR] -= i_scaling * l_netUpdates[1][0] * ( Reflection::LEFT != reflection );
                l_huNew[l_ceR] -= i_scaling * l_netUpdates[1][1] * ( Reflection::LEFT != reflection );
            }
        }
    }
    else
    {
        // uses a function pointer to choose between the solvers
        void ( *netUpdates )( t_real, t_real, t_real, t_real, t_real*, t_real* ) = solvers::FWave::netUpdates;
        if( solver == Solver::ROE )
        {
            netUpdates = solvers::Roe::netUpdates;
        }

        [ ... ]
    }

.. code-block:: cpp

    //swapping the h buffer new and old to write new data in previous old
    m_h[m_step] = l_hOld;
    m_step = ( m_step + 1 ) % 2;
    m_h[m_step] = l_hNew;

    // pointers to old and new data
    t_real* l_hvOld = m_hv[m_step];
    l_hOld = m_h[m_step];

    m_step = ( m_step + 1 ) % 2;
    t_real* l_hvNew = m_hv[m_step];
    l_hNew = m_h[m_step];

    // copy the calculated cell quantities
    for( t_idx l_ce = 0; l_ce < totalCells; l_ce++ )
    {
        l_hNew[l_ce] = l_hOld[l_ce];
        l_hvNew[l_ce] = l_hvOld[l_ce];
    }

.. code-block:: cpp

    // calculates xCells dividable by ITERATIONS_CACHE and remaining cells
    t_idx full_xCells = ( m_xCells / ITERATIONS_CACHE ) * ITERATIONS_CACHE;
    t_idx remaining_xCells = m_xCells % ITERATIONS_CACHE;

    // only possible for f-wave solver
    if( hasBathymetry )
    {
        //  iterates over the x direction
        for( t_idx i = 1; i < full_xCells; i += ITERATIONS_CACHE )
        {
            // iterate over the rows i.e. y-coordinates
            for( t_idx j = 0; j < m_yCells + 1; j++ )
            {
                // iterations for more efficient cache usage
                for( t_idx k = 0; k < ITERATIONS_CACHE; k++ )
                {
                    // determine left and right cell-id
                    t_idx l_ceT = stride * j + i + k;
                    t_idx l_ceB = stride * ( j + 1 ) + i + k;

                    // noting to compute both shore cells
                    if( l_hOld[l_ceT] == 0 && l_hOld[l_ceB] == 0 )
                    {
                        continue;
                    }

                    // compute reflection
                    t_real heightLeft;
                    t_real heightRight;
                    t_real momentumLeft;
                    t_real momentumRight;
                    t_real bathymetryLeft;
                    t_real bathymetryRight;

                    Reflection reflection = calculateReflection( l_hOld,
                                                                 l_hvOld,
                                                                 l_ceT,
                                                                 l_ceB,
                                                                 heightLeft,
                                                                 heightRight,
                                                                 momentumLeft,
                                                                 momentumRight,
                                                                 bathymetryLeft,
                                                                 bathymetryRight );

                    // compute net-updates
                    t_real l_netUpdates[2][2];

                    tsunami_lab::solvers::FWave::netUpdates( heightLeft,
                                                             heightRight,
                                                             momentumLeft,
                                                             momentumRight,
                                                             bathymetryRight,
                                                             bathymetryLeft,
                                                             l_netUpdates[0],
                                                             l_netUpdates[1] );

                    // update the cells' quantities
                    l_hNew[l_ceT] -= i_scaling * l_netUpdates[0][0] * ( Reflection::RIGHT != reflection );
                    l_hvNew[l_ceT] -= i_scaling * l_netUpdates[0][1] * ( Reflection::RIGHT != reflection );

                    l_hNew[l_ceB] -= i_scaling * l_netUpdates[1][0] * ( Reflection::LEFT != reflection );
                    l_hvNew[l_ceB] -= i_scaling * l_netUpdates[1][1] * ( Reflection::LEFT != reflection );
                }
            }
        }

        // iterate over the rows i.e. y-coordinates
        for( t_idx j = 0; j < m_yCells + 1; j++ )
        {
            // remaining iterations for more efficient cache usage
            for( t_idx k = 0; k < remaining_xCells; k++ )
            {
                // determine left and right cell-id
                t_idx l_ceT = stride * j + full_xCells + k;
                t_idx l_ceB = stride * ( j + 1 ) + full_xCells + k;

                // noting to compute both shore cells
                if( l_hOld[l_ceT] == 0 && l_hOld[l_ceB] == 0 )
                {
                    continue;
                }

                // compute reflection
                t_real heightLeft;
                t_real heightRight;
                t_real momentumLeft;
                t_real momentumRight;
                t_real bathymetryLeft;
                t_real bathymetryRight;

                Reflection reflection = calculateReflection( l_hOld,
                                                             l_hvOld,
                                                             l_ceT,
                                                             l_ceB,
                                                             heightLeft,
                                                             heightRight,
                                                             momentumLeft,
                                                             momentumRight,
                                                             bathymetryLeft,
                                                             bathymetryRight );

                // compute net-updates
                t_real l_netUpdates[2][2];

                tsunami_lab::solvers::FWave::netUpdates( heightLeft,
                                                         heightRight,
                                                         momentumLeft,
                                                         momentumRight,
                                                         bathymetryRight,
                                                         bathymetryLeft,
                                                         l_netUpdates[0],
                                                         l_netUpdates[1] );

                // update the cells' quantities
                l_hNew[l_ceT] -= i_scaling * l_netUpdates[0][0] * ( Reflection::RIGHT != reflection );
                l_hvNew[l_ceT] -= i_scaling * l_netUpdates[0][1] * ( Reflection::RIGHT != reflection );

                l_hNew[l_ceB] -= i_scaling * l_netUpdates[1][0] * ( Reflection::LEFT != reflection );
                l_hvNew[l_ceB] -= i_scaling * l_netUpdates[1][1] * ( Reflection::LEFT != reflection );
            }
        }
    }
    else
    {
        // uses a function pointer to choose between the solvers
        void ( *netUpdates )( t_real, t_real, t_real, t_real, t_real*, t_real* ) = solvers::FWave::netUpdates;
        if( solver == Solver::ROE )
        {
            netUpdates = solvers::Roe::netUpdates;
        }

        [ ... ]
    }


2. Circular Dam break
^^^^^^^^^^^^^^^^^^^^^

.. raw:: html

    <center>
        <video width="700" controls>
            <source src="../_static/videos/task_4_1_2.mp4" type="video/mp4">
        </video>
    </center>


3. Bathymetry & Obstacle 
^^^^^^^^^^^^^^^^^^^^^^^^

.. raw:: html

    <center>
        <video width="700" controls>
            <source src="../_static/videos/task_4_1_3_alt.mp4" type="video/mp4">
        </video>
    </center>

.. image:: ../images/Task_4_1_3.png
    :align: center

The red lines on the left and right side indicate that the wave in x-direction propagate slower than the wave in y-direction because of the bathymetry dent in the middle in y-direction.


4.2. Stations
-------------


Contribution
------------

All team members contributed equally to the tasks.
