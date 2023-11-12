.. _submissions_bathymetry_boundary_conditions:

.. role:: raw-html(raw)
    :format: html

3. Bathymetry & Boundary Conditions
===================================

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

The velocity can be calculated with momentum and height as follows.

.. math::

    \begin{aligned}

        u(x) &= \frac{hu(x)}{h(x)}\\
        &=
        \begin{cases}
            \frac{1.8 + 0.05 (x-10)^2}{4.42} \quad   &\text{if } x \in (8,12) \\
            \frac{2}{4.42} \quad &\text{else}
        \end{cases}\\

    \end{aligned}

Using the conditional velocity the Froude number can be calculated in the range of (8, 12) and outside that range.

.. math::

    \begin{aligned}

        \text{For } x\in (8, 12): &\\
        F(x) &= \frac{1.8 + 0.05 (x - 10)^2}{4.42 \sqrt{9.80665 \cdot 4.42}}\\
        &= \frac{1.8 + 0.05 (x^2 - 20x + 100)}{4.42 \sqrt{9.80665 \cdot 4.42}}\\

    \end{aligned}

The Froude number within the range (8, 12) becomes a quadratic equation with a positive gradient, so that the maximum can be found at the boundary i.e. :math:`x_1 = 8` and :math:`x = 12`.

.. math::

    \begin{aligned}

        F(8) &= \frac{1.8 + 0.05 (8 - 10)^2}{4.42 \sqrt{9.80665 \cdot 4.42}}\\
        &\approx 0.068728403\\
        F(12) &= \frac{1.8 + 0.05 (12 - 10)^2}{4.42 \sqrt{9.80665 \cdot 4.42}}\\
        &\approx 0.068728403

    \end{aligned}

The Froude number outside the range (8, 12) is calculated by:

.. math::

    \begin{aligned}

        \text{For } x\notin (8, 12): &\\
        F(x) &= \frac{2}{4.42\sqrt{9.80665 \cdot 4.42}} \approx 0.068728403

    \end{aligned}


Therefore, the maximum Froude number of the subcritical flow in both cases it 0.068728403.


**Supercritical flow**

The velocity can again be calculated with momentum and height as follows.

.. math::

    \begin{aligned}

        u(x) &= \frac{hu(x)}{h(x)}\\
        &=
        \begin{cases}
            \frac{0.13 + 0.05 (x-10)^2}{0.18} \quad   &\text{if } x \in (8,12) \\
            \frac{0.33}{0.18} \quad &\text{else}
        \end{cases}\\

    \end{aligned}

Using the conditional velocity the Froude number can be calculated in the range of (8, 12) and outside that range.

.. math::

    \begin{aligned}

        \text{For } x\in (8, 12): &\\
        F(x) &= \frac{0.13 + 0.05 (x - 10)^2}{0.18 \sqrt{9.80665 \cdot 0.18}}\\
        &= \frac{0.13 + 0.05 (x^2 - 20x + 100)}{0.18 \sqrt{9.80665 \cdot 0.18}}\\

    \end{aligned}

Again the Froude number within the range (8, 12) becomes a quadratic equation with a positive gradient, so that the maximum can be found at the boundary i.e. :math:`x_1 = 8` and :math:`x = 12`.

.. math::

    \begin{aligned}

        F(x) &= \frac{0.13 + 0.05 (8 - 10)^2}{0.18 \sqrt{9.80665 \cdot 0.18}}\\
        &\approx 1.3798912\\
        F(x) &= \frac{0.13 + 0.05 (12 - 10)^2}{0.18 \sqrt{9.80665 \cdot 0.18}}\\
        &\approx 1.3798912

    \end{aligned}

The Froude number outside the range (8, 12) is calculated by:

.. math::

    \begin{aligned}

        \text{For } x\notin (8, 12): &\\
        F(x) &= \frac{0.33}{0.18\sqrt{9.80665 \cdot 0.18}} \approx 1.3798912

    \end{aligned}


Therefore, the maximum Froude number of the subcritical flow in both cases it 1.3798912.

Implementation into Setups
^^^^^^^^^^^^^^^^^^^^^^^^^^

The implementation in setups is done using a subclass of ``setup::Setup.h`` where the default constructor setups the initial **supercritical** and **subcritical** flow examples.

+-------------------------------------------------------------------------------+----------------------------------------------------------------------------+
|:raw-html:`<center>Subcritical Flow</center>`                                  |:raw-html:`<center>Supercritical Flow</center>`                             |
+===============================================================================+============================================================================+
|                                                                               |                                                                            |
|   .. math::                                                                   | .. math::                                                                  |
|       \begin{split}\begin{aligned}                                            |    \begin{split}\begin{aligned}                                            |
|           b(x) &=                                                             |       b(x) &=                                                              |
|               \begin{cases}                                                   |           \begin{cases}                                                    |
|                   -1.8 - 0.05 (x-10)^2 \quad   &\text{if } x \in (8,12) \\    |               -0.13 - 0.05 (x-10)^2 \quad   &\text{if } x \in (8,12) \\    |
|                   -2 \quad &\text{else}                                       |               -0.33 \quad &\text{else}                                     |
|               \end{cases}\\                                                   |           \end{cases}\\                                                    |
|           h(x, 0) &= -b(x) \quad \text{if } x \in [0,25] \\                   |       h(x, 0) &= -b(x) \quad \text{if } x \in [0,25] \\                    |
|           hu(x, 0) &= 4.42 \quad \text{if } x \in [0,25].                     |       hu(x, 0) &= 0.18 \quad \text{if } x \in [0,25].                      |
|       \end{aligned}\end{split}                                                |    \end{aligned}\end{split}                                                |
|                                                                               |                                                                            |
+-------------------------------------------------------------------------------+----------------------------------------------------------------------------+

The implementation is similar in both cases, for the subcritical flow the code is shown below.
To calculate the bathymetry as a function of the x-coordinate, a function pointer is used to which a lambda is assigned.

.. code-block::
    :emphasize-lines: 8-11
    
    /// File:   SubcriticalFlow1d.cpp
    /// Header: SubcriticalFlow1d.h
    /// Test:   SubcriticalFlow1d.test.cpp

    tsunami_lab::setups::SubcriticalFlow1d::SubcriticalFlow1d()
        : momentum( 4.42 ), range{ 8, 12 }, bathymetryOutRange( -2 )
    {
        bathymetryInRange = []( t_real x ) -> t_real
            {
                return -1.8 - 0.05 * ( x - 10 ) * ( x - 10 );
            };
    }

Also an extra constructor is provided with four argument (``t_real momentum``, ``t_real range``, ``t_real bathymetryOutRange``, ``t_real( *bathymetryInRange )( t_real )``) to provide an interface for custom supercritical & subcritical flow setups.


The bathymetry is returned based on the range with its stored bathymetry i.e.

.. code-block::
    :emphasize-lines: 4-8

    tsunami_lab::t_real tsunami_lab::setups::SubcriticalFlow1d::getBathymetry( t_real i_x,
                                                                               t_real ) const
    {
        if( range[0] < i_x && i_x < range[1] )
        {
            return bathymetryInRange( i_x );
        }
        return bathymetryOutRange;
    }


The Output of both are visualized below:

**Subcritical Flow**

At around 2 seconds at 2000 a small dent in the total height can be seen.

    .. raw:: html
        
        <center>
            <video width="700" controls>
                <source src="../_static/videos/subcritical_flow.mp4" type="video/mp4">
            </video>
        </center>

**Supercritical Flow**

Between 1 and 5 seconds at around 2300 a large gap can be seen to form in the total height and the momentum begins to oscillate.

    .. raw:: html
    
        <center>
            <video width="700" controls>
                <source src="../_static/videos/supercritical_flow.mp4" type="video/mp4">
            </video>
        </center>

**Hydraulic Jump**

The hydraulic jump can bee seen at position 2300 but the f-wave solver fails to converge to a constant momentum.
Therefore a spike in the momentum can be seen at around 2300.

3.4. 1D Tsunami Simulation
--------------------------

Contribution
------------

All team members contributed equally to the tasks.
