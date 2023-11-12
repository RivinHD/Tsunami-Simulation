.. _submissions_bathymetry_boundary_conditions:

.. role:: raw-html(raw)
    :format: html

3. Bathymetry & Boundary Conditions
===================================

1. Extended f-wave solver
^^^^^^^^^^^^^^^^^^^^^^^^^

Bathymetry is the topography of the ocean. We use sea level as a reference which means the bathymetry of a dry cell
(onshore) is zero or positive :math:`b_i \ge 0` and a wet cell (offshore) has a negative value :math:`b_j < 0`.

The term :math:`\Delta x \Psi_{i-1/2}` stands for the effect of the bathymetry:

.. math::

    \begin{split}\Delta x \Psi_{i-1/2} := \begin{bmatrix}
                                0 \\
                                -g (b_r - b_l) \frac{h_l+h_r}{2}
                              \end{bmatrix}.\end{split}

.. code-block:: cpp

    /// File:   FWave.cpp
    /// Header: FWave.h
    /// Test:   FWave.test.cpp
    [ ... ]
    void tsunami_lab::solvers::FWave::computeBathymetryEffects( t_real i_hL, t_real i_hR,
                                                                t_real i_bL, t_real i_bR,
                                                                t_real o_bathymetryEffect[2] )
    {
        o_bathymetryEffect[0] = 0;
        o_bathymetryEffect[1] = -m_g * ( i_bR - i_bL ) * ( t_real( 0.5 ) * ( i_hL + i_hR ) );
    }
    [ ... ]

To add bathymetry into the f-wave solver we need to take these into account in our flux function:

.. math::

    \Delta f - \Delta x \Psi_{i-1/2} =  \sum_{p=1}^2 Z_p.



.. code-block:: cpp
    :emphasize-lines: 10-15

    /// File:   FWave.cpp
    /// Header: FWave.h
    /// Test:   FWave.test.cpp
    [ ... ]
    // compute delta flux
    t_real deltaFlux[2];
    computeDeltaFlux( i_hL, i_hR, l_uL, l_uR, deltaFlux );

    //compute bathymetry
    t_real bathymetry[2];
    computeBathymetryEffects( i_hL, i_hR, i_bL, i_bR, bathymetry );
    t_real bathymetryDeltaFlux[2] = {
        deltaFlux[0] + bathymetry[0],
        deltaFlux[1] + bathymetry[1]
    };

    // compute eigencoefficients
    t_real eigencoefficient1 = 0;
    t_real eigencoefficient2 = 0;
    computeEigencoefficients( eigenvalue1, eigenvalue2, bathymetryDeltaFlux, eigencoefficient1, eigencoefficient2 );
    [ ... ]

2. Illustration of the effect of bathymetry
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

First we need to set the bathymetry to our ``WavePropagation1d``. Because bathymetry is only possible in the f-wave
solver, we have an if-statement in WavePropagation which checks whether bathemetry is present.

.. code-block::
    :emphasize-lines: 15, 32, 52-53, 62-63, 72-73

    /// File: WavePropagation1d.cpp
    /// Header: WavePropagation1d.h
    /// Test: WavePropagation1d.test.cpp
    [ ... ]
    tsunami_lab::patches::WavePropagation1d::WavePropagation1d( t_idx i_nCells )
    {
        m_nCells = i_nCells;

        // allocate memory including a single ghost cell on each side
        for( unsigned short l_st = 0; l_st < 2; l_st++ )
        {
            m_h[l_st] = new t_real[m_nCells + 2];
            m_hu[l_st] = new t_real[m_nCells + 2];
        }
        m_bathymetry = new t_real[m_nCells + 2]{ 0 };
        m_totalHeight = new t_real[m_nCells + 2]{ 0 };

        // init to zero
        for( unsigned short l_st = 0; l_st < 2; l_st++ )
        {
            for( t_idx l_ce = 0; l_ce < m_nCells + 2; l_ce++ )
            {
                m_h[l_st][l_ce] = 0;
                m_hu[l_st][l_ce] = 0;
            }
        }
    }
    [ ... ]
    void tsunami_lab::patches::WavePropagation1d::timeStep( t_real i_scaling )
    {
        [ ... ]
        if( hasBathymetry )
        {
            // iterate over edges and update with Riemann solutions
            for( t_idx l_ed = 0; l_ed < m_nCells + 1; l_ed++ )
            {
                // determine left and right cell-id
                t_idx l_ceL = l_ed;
                t_idx l_ceR = l_ed + 1;

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
        else
        {
            // uses a function pointer to choose between the solvers
            void ( *netUpdates )( t_real, t_real, t_real, t_real, t_real*, t_real* ) = solvers::FWave::netUpdates;
            if( solver == Solver::ROE )
            {
                netUpdates = solvers::Roe::netUpdates;
            }
            // iterate over edges and update with Riemann solutions
            for( t_idx l_ed = 0; l_ed < m_nCells + 1; l_ed++ )
            {
                // determine left and right cell-id
                t_idx l_ceL = l_ed;
                t_idx l_ceR = l_ed + 1;

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

                Reflection reflection = calculateReflection( l_hOld,
                                                             l_huOld,
                                                             l_ceL,
                                                             heightLeft,
                                                             heightRight,
                                                             momentumLeft,
                                                             momentumRight );

                // compute net-updates
                t_real l_netUpdates[2][2];

                netUpdates( heightLeft,
                            heightRight,
                            momentumLeft,
                            momentumRight,
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
    [ ... ]

And set our ghost cells :math:`b_0 := b_1` and :math:`b_n+1 := b_n`.

.. code-block:: cpp
    :emphasize-lines: 12, 17

    /// File: WavePropagation1d.cpp
    /// Header: WavePropagation1d.h
    /// Test: WavePropagation1d.test.cpp
    void tsunami_lab::patches::WavePropagation1d::setGhostOutflow()
    {
        t_real* l_h = m_h[m_step];
        t_real* l_hu = m_hu[m_step];

        // set left boundary
        l_h[0] = l_h[1] * !hasReflection[Side::LEFT];
        l_hu[0] = l_hu[1];
        m_bathymetry[0] = m_bathymetry[1];

        // set right boundary
        l_h[m_nCells + 1] = l_h[m_nCells] * !hasReflection[Side::RIGHT];
        l_hu[m_nCells + 1] = l_hu[m_nCells];
        m_bathymetry[m_nCells + 1] = m_bathymetry[m_nCells];
    }
    [ ... ]

.. raw:: html

    <center>
        <video width="700" controls>
            <source src="../_static/videos/task_3_1_2.mp4" type="video/mp4">
        </video>
    </center>

3.1. Non-zero Source Term
-------------------------

3.2 Reflecting Boundary Conditions
----------------------------------

3.3. Hydraulic Jumps
--------------------

Hydraulic jumps behavior in shallow water theory can be characterized by the Froude number.

.. math::

    F := \frac{u}{\sqrt{gh}}


"We call regions with :math:`F < 1` subcritical, :math:`F \approx 1` critical and :math:`F > 1` supercritical."[1]_

.. [1] From https://scalable.uni-jena.de/opt/tsunami/chapters/assignment_3.html#hydraulic-jumps (10.11.2023)

Maximum Froude number
^^^^^^^^^^^^^^^^^^^^^

**Subcritical flow**

.. math::

    \begin{aligned}

        u(x) &= \frac{hu(x)}{h(x)}\\
        &=
        \begin{cases}
            \frac{1.8 + 0.05 (x-10)^2}{4.42} \quad   &\text{if } x \in (8,12) \\
            \frac{2}{4.42} \quad &\text{else}
        \end{cases}\\

    \end{aligned}

.. math::

    \begin{aligned}

        \text{For } x\in (8, 12): &\\
        F(x) &= \frac{1.8 + 0.05 (x - 10)^2}{4.42 \sqrt{9.80665 \cdot 4.42}}\\
        &= \frac{1.8 + 0.05 (x^2 - 20x + 100)}{4.42 \sqrt{9.80665 \cdot 4.42}}\\

    \end{aligned}

.. math::

    \begin{aligned}

        F'(x) &= \frac{0.05 (2x - 20)}{4.42 \sqrt{9.80665 \cdot 4.42}}\\
        &= \frac{0.1x - 1}{4.42 \sqrt{9.80665 \cdot 4.42}}\\
        F'(x_1) &= 0\\
        x_1 &= 10 (4.42 \sqrt{9.80665 \cdot 4.42})\\
        F'(x_1) &\approx 135.73393

    \end{aligned}

.. math::

    \begin{aligned}

        \text{For } x\notin (8, 12): &\\
        F(x) &= \frac{2}{4.42\sqrt{9.80665 \cdot 4.42}} \approx 0.068728403 \lt 135.73393

    \end{aligned}


Therefore, the maximum Froude number of the subcritical flow is **135.73393**.


**Supercritical flow**

.. math::

    \begin{aligned}

        u(x) &= \frac{hu(x)}{h(x)}\\
        &=
        \begin{cases}
            \frac{0.13 + 0.05 (x-10)^2}{0.18} \quad   &\text{if } x \in (8,12) \\
            \frac{0.33}{0.18} \quad &\text{else}
        \end{cases}\\

    \end{aligned}

.. math::

    \begin{aligned}

        \text{For } x\in (8, 12): &\\
        F(x) &= \frac{0.13 + 0.05 (x - 10)^2}{0.18 \sqrt{9.80665 \cdot 0.18}}\\
        &= \frac{0.13 + 0.05 (x^2 - 20x + 100)}{0.18 \sqrt{9.80665 \cdot 0.18}}\\

    \end{aligned}

.. math::

    \begin{aligned}

        F'(x) &= \frac{0.05 (2x - 20)}{0.18 \sqrt{9.80665 \cdot 0.18}}\\
        &= \frac{0.1x - 1}{0.18 \sqrt{9.80665 \cdot 0.18}}\\
        F'(x_1) &= 0\\
        x_1 &= 10 (0.18 \sqrt{9.80665 \cdot 0.18})\\
        F'(x_1) &\approx 12.646782

    \end{aligned}

.. math::

    \begin{aligned}

        \text{For } x\notin (8, 12): &\\
        F(x) &= \frac{0.33}{0.18\sqrt{9.80665 \cdot 0.18}} \approx 0.013798912 \lt 12.646782

    \end{aligned}


Therefore, the maximum Froude number of the supercritical flow is **12.646782**.

3.4. 1D Tsunami Simulation
--------------------------

1. Extracting bathymetry data for the 1D domain
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

1. Download `GEBCO_2021 <https://www.gebco.net/data_and_products/historical_data_sets/>`_ grid.

2. Reduce grid size with :raw-html:`</br>`
   :code:`gmt grdcut -R138/147/35/39 path/to/GEBCO_2021.nc -Gpath/to/GEBCO_2021_cut.nc`

3. Create datapoints with :raw-html:`</br>`
   :code:`gmt grdtrack -Gdpath/to/GEBCO_2021_cut.nc -E141.024949/37.316569/146/37.316569+i250e+d -Ar > bathy_profile.out`

4. Add commas to create comma-separated values file with :raw-html:`</br>`
   :code:`cat bathy_profile.out | tr -s '[:blank:]' ',' > bathy_profile.csv`

The ``bathy_profile.csv`` is located in: ``.../Tsunami-Simulation/resources/bathy_profile.csv``.


2. Extend **tsunami_lab::io::Csv** to read bathy_profile.csv
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp
    :emphasize-lines: 29-32, 35

    /// File:   Csv.cpp
    /// Header: Csv.h
    /// Test:   Csv.test.cpp
    bool tsunami_lab::io::Csv::readBathymetry( std::ifstream& stream,
                                               t_real& o_hBathy)
    {
        std::string line;

        // read next complete line
        while( std::getline( stream, line ) )
        {
            // skip commented lines
            if( line[0] == '#' )
            {
                continue;
            }

            // parse lines divided by ',' to single values
            std::istringstream lineStream( line );
            std::string longitude;
            std::getline( lineStream, longitude, ',' );
            // o_longitude = atof( longitude.c_str() );
            std::string latitude;
            std::getline( lineStream, latitude, ',' );
            // o_latitude = atof( latitude.c_str() );
            std::string location;
            std::getline( lineStream, location, ',' );
            // o_location = atof( location.c_str() );
            std::string h_bathy;
            std::getline( lineStream, h_bathy, ',' );
            o_hBathy = atof( h_bathy.c_str() );
            return true;
        }
        // no lines left to read
        return false;
    }

This implementation offers scope for reading further data from the file in the future.


3. New setup ``setups::TsunamiEvent1d``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

In the first highlighted block we initialize a vector with the bathymetry height entries of the bathy_profile.csv.
In total, we then have the values of :code:`m_bathy.size()` many data points and a maximum index of ``m_csvDataPoint``.

If the x-coordinate of the queried point is not exactly at the same position as a bathymetry value, a more suitable
bathymetry is calculated for this x-coordinate by linear interpolation. This is done in the second and third highlighted
code-block.

The mathematical formulas

.. math::

    \begin{split}\begin{split}
        h  &= \begin{cases}
                \max( -b_\text{in}, \delta), &\text{if } b_\text{in} < 0 \\
                0, &\text{else}
              \end{cases}\\
        hu &= 0\\
        b  &= \begin{cases}
                \min(b_\text{in}, -\delta) + d, & \text{ if } b_\text{in} < 0\\
                \max(b_\text{in}, \delta) + d, & \text{ else}.
              \end{cases}
    \end{split}\end{split}

can be easily transferred to code.

:math:`b_\text{in}(x) \in \mathbb{R}` is the bathymetry axtracted from the DEM and :math:`d(x) \in \mathbb{R}` is the
vertical displacement. The constant :math:`\delta \in \mathbb{R}^+` avoids running into numerical issues due to missing
support for wetting and drying in our solver.

.. code-block:: cpp
    :emphasize-lines: 13-17, 24-29, 54-59

    /// File:   TsunamiEvent1d.cpp
    /// Header: TsunamiEvent1d.h
    /// Test:   TsunamiEvent1d.test.cpp
    tsunami_lab::setups::TsunamiEvent1d::TsunamiEvent1d( std::string filePath,
                                                         tsunami_lab::t_real i_delta,
                                                         tsunami_lab::t_real i_scale )
    {
        m_delta = i_delta;
        m_scale = i_scale;

        t_real o_hBathy = 0;
        std::ifstream bathy_profile( filePath );
        while( tsunami_lab::io::Csv::readBathymetry( bathy_profile, o_hBathy ) )
        {
            m_bathy.push_back( o_hBathy );
        }
        m_csvDataPoint = m_bathy.size() - 1;
    }

    tsunami_lab::t_real tsunami_lab::setups::TsunamiEvent1d::getHeight( tsunami_lab::t_real i_x,
                                                                        tsunami_lab::t_real ) const
    {
        // linear interpolation between two bathymetries
        t_real l_x = ( i_x / m_scale ) * m_csvDataPoint;
        t_idx indexL = std::floor( l_x );
        t_idx indexR = std::ceil( l_x );
        t_real l_bathyL = m_bathy[indexL];
        t_real l_bathyR = m_bathy[indexR];
        t_real l_bathy = ( l_bathyR - l_bathyL ) * ( l_x - indexL ) + l_bathyL;

        if( l_bathy < 0 )
        {
            return -l_bathy < m_delta ? m_delta : -l_bathy;
        }
        return 0;
    }

    tsunami_lab::t_real tsunami_lab::setups::TsunamiEvent1d::getMomentumX( tsunami_lab::t_real,
                                                                           tsunami_lab::t_real ) const
    {
        return m_momentum;
    }

    tsunami_lab::t_real tsunami_lab::setups::TsunamiEvent1d::getMomentumY( tsunami_lab::t_real,
                                                                           tsunami_lab::t_real ) const
    {
        return 0;
    }

    tsunami_lab::t_real tsunami_lab::setups::TsunamiEvent1d::getBathymetry( tsunami_lab::t_real i_x,
                                                                            tsunami_lab::t_real ) const
    {
    // linear interpolation between two bathymetries
        t_real l_x = ( i_x / m_scale ) * m_csvDataPoint;
        t_idx indexL = std::floor( l_x );
        t_idx indexR = std::ceil( l_x );
        t_real l_bathyL = m_bathy[indexL];
        t_real l_bathyR = m_bathy[indexR];
        t_real l_bathy = ( l_bathyR - l_bathyL ) * ( l_x - indexL ) + l_bathyL;
        t_real verticalDisplacement = getVerticalDisplacement( i_x, 0 );

        if( l_bathy < 0 )
        {
            return l_bathy < -m_delta ? l_bathy + verticalDisplacement : -m_delta + verticalDisplacement;
        }
        return l_bathy < m_delta ? m_delta + verticalDisplacement : l_bathy + verticalDisplacement;
    }

    tsunami_lab::t_real tsunami_lab::setups::TsunamiEvent1d::getVerticalDisplacement( tsunami_lab::t_real i_x,
                                                                                      tsunami_lab::t_real ) const
    {
        if( 175000 < i_x && i_x < 250000 )
        {
            return 1000 * std::sin( ( ( i_x - 175000 ) / 37500 * M_PI ) + M_PI );
        }
        return 0;
    }


4. Visualization of the TsunamiEvent1d setup
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

l_endTime...time to simulate = 2000 :raw-html:`</br>`
l_scale...length of the x-axis on which the simulation runs = 440000

Result with 10000 cells. To achieve a better visualisation, the vertical displacement is scaled with 1000 instead of 10.

.. math::

    \begin{split}d(x) = \begin{cases}
         1000\cdot\sin(\frac{x-175000}{37500} \pi + \pi), & \text{ if } 175000 < x < 250000 \\
         0, &\text{else}.
       \end{cases}\end{split}


.. raw:: html

    <center>
        <video width="700" controls>
            <source src="../_static/videos/task_3_4_4.mp4" type="video/mp4">
        </video>
    </center>



Contribution
------------

All team members contributed equally to the tasks.
