.. _submissions_finite_volume_discretization:

.. role:: raw-html(raw)
    :format: html

2. Finite Volume Discretization
===============================

Switch Solver
-------------

The default solver used in ``WavePropagation1d.h``, ``WavePropagation1d.cpp`` and
``WavePropagation1d.test.cpp`` is the f-wave solver. To change the solver, use the ``-s``
flag and choose between ``roe`` and ``fwave`` as argument to specify the solver used.
Internally, a function pointer is used to select between the solvers.

Logic is in the ``main.cpp`` file:

.. code-block:: cpp

    // error: wrong number of arguments.
    if( i_argc < 2 || i_argc == 3 || i_argc > 4) {
        std::cerr << "invalid number of arguments, usage:" << std::endl
                  << "  ./build/simulation N_CELLS_X [-s <fwave|roe>]" << std::endl
                  << "where N_CELLS_X is the number of cells in x-direction." << std::endl
                  << "optional flag: '-s' set used solvers requires 'fwave' or 'roe' as inputs" << std::endl;
        return EXIT_FAILURE;
    }
    // flag: set solver.
    else if ( i_argc == 4)
    {
        // unknown flag.
        if ( ARG_SOLVER != std::string(i_argv[2]))
        {
          std::cerr << "unknown flag: " << i_argv[2] << std::endl;
          return EXIT_FAILURE;
        }
        // set solver: roe
        if ( "roe" == std::string(i_argv[3]))
        {
          std::cout << "Set Solver: Roe" << std::endl;
          solver = tsunami_lab::patches::Solver::Roe;
        }
        // set solver: fwave
        else if ( "fwave" == std::string(i_argv[3]))
        {
          std::cout << "Set Solver: FWave" << std::endl;
        }
        else
        {
          std::cerr << "unknown argument for flag -s" << std::endl
                    << "valid arguments are 'fwave', 'roe'" << std::endl;
          return EXIT_FAILURE;
        }
    }

    // number of arguments == 2
    l_nx = atoi( i_argv[1] );
    if( l_nx < 1 ) {
      std::cerr << "invalid number of cells" << std::endl;
      return EXIT_FAILURE;
    }
    // choose default solver: fwave

Sanity Check
------------

Use these middle states as a sanity check.

Continuous Integration
----------------------

**Using GitHub Actions** file: ``.github/workflows/main.yml``

.. code-block:: bash

    ##
    # @author Alexander Breuer (alex.breuer AT uni-jena.de)
    # @section DESCRIPTION
    # Continuous integration using GitHub Actions.
    ##
    name: Tsunami Lab

    on:
      push:
        branches: [ main ]
      pull_request:
        branches: [ main ]
      schedule:
        - cron: 0 0 * * *

    jobs:
      CI:
        runs-on: ubuntu-latest

        steps:
          - uses: actions/checkout@v4

          - name: Dependencies
            run: |
              sudo apt-get update
              sudo apt-get install cmake
              sudo apt-get install valgrind
              sudo apt-get install cppcheck
              git submodule init
              git submodule update
          - name: Configure the project
            uses: threeal/cmake-action@v1.3.0

          - name: Static Code Analysis
            run:
              cppcheck src/ --template=gcc --force --error-exitcode=1

          - name: Sanitize
            run: |
              cmake --build build --config Debug --target sanitize
              ./build/sanitize 25
              cmake --build build --config Debug --target sanitize_test
              ./build/sanitize_test 25
              cmake --build build --config Release --target sanitize
              ./build/sanitize 500
              cmake --build build --config Release --target sanitize_test
              ./build/sanitize_test 500
          - name: Valgrind
            run: |
              cmake --build build --config Debug --target test
              valgrind build/test
              cmake --build build --config Debug --target build
              valgrind build/build 25
          - name: Release
            run: |
              cmake --build build --config Release --target test
              ./build/test
              cmake --build build --config Release --target build
              ./build/build 500

???? Ensure to run at least the **solver’s** unit tests after every commit to your git repository. ????

2.1. Shock and Rarefaction Waves
--------------------------------

Implementation of shock-shock setup
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

"**Shock-Shock Problem**: Let’s use our solver to solve shock-shock Riemann problems. Imagine two streams
of water which move in opposite directions and smash into each other at some position :math:`x_\text{dis}`.
The scenario is given by the following setup"[1]_:

.. math::

    \begin{split}\begin{cases}
        Q_i = q_{l} \quad &\text{if } x_i \le x_\text{dis} \\
        Q_i = q_{r} \quad &\text{if }   x_i > x_\text{dis}
    \end{cases} \qquad q_l \in \mathbb{R}^+ \times \mathbb{R}^+, \; q_r \in \mathbb{R}^+ \times \mathbb{R}^-,\end{split}

with initial conditions:

:raw-html:`<center>(2.1.1)</center>`

.. math::

    \begin{split}q_l=
        \begin{bmatrix}
          h_l \\ (hu)_l
        \end{bmatrix}, \quad
      q_r =
        \begin{bmatrix}
          h_r \\ (hu)_r
        \end{bmatrix} =
        \begin{bmatrix}
          h_l \\ -(hu)_l
        \end{bmatrix}.
    \end{split}

.. code-block:: cpp

    tsunami_lab::setups::ShockShock1d::ShockShock1d(t_real i_heightLeft,
                                                    t_real i_momentumLeft,
                                                    t_real i_locationShock)
    {
        m_heightLeft = i_heightLeft;
        m_momentumLeft = i_momentumLeft;
        m_locationShock = i_locationShock;
    }

    t_real tsunami_lab::setups::ShockShock1d::getHeight(t_real ,
                                                        t_real ) const
    {
        return m_heightLeft;
    }

    t_real tsunami_lab::setups::ShockShock1d::getMomentumX(t_real i_x,
                                                           t_real ) const
    {
        if (i_x <= m_locationShock)
        {
            return m_momentumLeft;
        }
        else
        {
            return -m_momentumLeft;
        }
    }

    t_real tsunami_lab::setups::ShockShock1d::getMomentumY(t_real,
                                                           t_real) const
    {
        return 0;
    }

Implementation of rare-rare setup
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

"Rare-Rare Problem: We can setup rare-rare Riemann problems by two streams of water, which move away
from each other at some position :math:`x_\text{dis}`. The scenario is defined as"[1]_:

.. math::

    \begin{split}\begin{cases}
        Q_i = q_{r} \quad &\text{if } x_i \le x_\text{dis} \\
        Q_i = q_{l} \quad &\text{if }   x_i > x_\text{dis}
        \end{cases} \qquad q_l \in \mathbb{R}^+ \times \mathbb{R}^+, \; q_r \in \mathbb{R}^+ \times \mathbb{R}^-,\end{split}

.. code-block:: cpp

    tsunami_lab::setups::RareRare1d::RareRare1d(tsunami_lab::t_real i_heightLeft,
                                                tsunami_lab::t_real i_momentumLeft, tsunami_lab::t_real i_locationRare) {
        m_heightLeft = i_heightLeft;
        m_momentumLeft = i_momentumLeft;
        m_locationRare = i_locationRare;
    }

    tsunami_lab::t_real tsunami_lab::setups::RareRare1d::getHeight(tsunami_lab::t_real , tsunami_lab::t_real) const {
        return m_heightLeft;
    }

    tsunami_lab::t_real tsunami_lab::setups::RareRare1d::getMomentumX(tsunami_lab::t_real i_x,
                                                                      tsunami_lab::t_real) const {
        if (i_x <= m_locationRare) {
            return -m_momentumLeft;
        } else {
            return m_momentumLeft;
        }
    }

    tsunami_lab::t_real tsunami_lab::setups::RareRare1d::getMomentumY(tsunami_lab::t_real,
                                                                      tsunami_lab::t_real) const {
        return 0;
}

.. [1] From https://scalable.uni-jena.de/opt/tsunami/chapters/assignment_1.html#f-wave-solver (29.10.2023)

Play around
^^^^^^^^^^^

l_hl...height of left side :raw-html:`<br>`
l_hr...height of right side :raw-html:`<br>`
l_ml...momentum of left side :raw-html:`<br>`
l_location...location :raw-html:`<br>`
:math:`\lambda_1`...wave speed one :raw-html:`<br>`
:math:`\lambda_2`...wave speed two

All results with 3 cells!

+--------+--------+--------+--------+-------------+-----------+---------------------+---------------------+
|  l_hl  |  l_hr  | l_hul  |  l_ul  |  l_location |  Setup    |  :math:`\lambda_1`  |  :math:`\lambda_2`  |
+========+========+========+========+=============+===========+=====================+=====================+
|  10    |   15   |   /    |   /    |      5      | Dam-Break |      -9.90285       |       9.90285       |
+--------+--------+--------+--------+-------------+-----------+---------------------+---------------------+
|  10    |   15   |   8    |  0.8   |      5      | Rare-Rare |      -10.7029       |       9.10285       |
+--------+--------+--------+--------+-------------+-----------+---------------------+---------------------+
|  10    |   15   |   8    |  0.8   |      5      |Shock-Shock|      -9.10285       |       10.7029       |
+--------+--------+--------+--------+-------------+-----------+---------------------+---------------------+
|  10    |   15   |   /    |   /    |      5      | Dam-Break |      -9.90285       |       9.90285       |
+--------+--------+--------+--------+-------------+-----------+---------------------+---------------------+
|  10    |   15   |   0    |   0    |      5      | Rare-Rare |      -9.90285       |       9.90285       |
+--------+--------+--------+--------+-------------+-----------+---------------------+---------------------+
|  10    |   15   |   0    |   0    |      5      |Shock-Shock|      -9.90285       |       9.90285       |
+--------+--------+--------+--------+-------------+-----------+---------------------+---------------------+
|  15    |   15   |   /    |    /   |      5      | Dam-Break |      -12.1285       |       12.1285       |
+--------+--------+--------+--------+-------------+-----------+---------------------+---------------------+
|  15    |   15   |   5    |   1/3  |      5      | Rare-Rare |      -12.4618       |       11.7951       |
+--------+--------+--------+--------+-------------+-----------+---------------------+---------------------+
|  15    |   15   |   5    |   1/3  |      5      |Shock-Shock|      -11.7951       |       12.4618       |
+--------+--------+--------+--------+-------------+-----------+---------------------+---------------------+
|  100   |   15   |   /    |    /   |      5      | Dam-Break |      -31.3156       |       31.3156       |
+--------+--------+--------+--------+-------------+-----------+---------------------+---------------------+
|  100   |   15   |   5    |   0.05 |      5      | Rare-Rare |      -31.3656       |       31.2656       |
+--------+--------+--------+--------+-------------+-----------+---------------------+---------------------+
|  100   |   15   |   5    |   0.05 |      5      |Shock-Shock|      -31.2656       |       31.3656       |
+--------+--------+--------+--------+-------------+-----------+---------------------+---------------------+
|  100   |   15   |   /    |    /   |      5      | Dam-Break |      -31.3156       |       31.3156       |
+--------+--------+--------+--------+-------------+-----------+---------------------+---------------------+
|  100   |   15   |   2000 |   20   |      5      | Rare-Rare |      -51.3156       |       11.3156       |
+--------+--------+--------+--------+-------------+-----------+---------------------+---------------------+
|  100   |   15   |   2000 |   20   |      5      |Shock-Shock|      -11.3156       |       51.3156       |
+--------+--------+--------+--------+-------------+-----------+---------------------+---------------------+

What we observe
^^^^^^^^^^^^^^^

Dam-Break
    :math:`\lambda_1 = -\lambda_2` :raw-html:`<br>`
    The higher the difference between l_hl and l_hr the higher the absolute wave speeds (eigenvalues). :raw-html:`<br>`
    Wave speeds are independent of the velocity which is always zero.

Rare-Rare
    Waves speeds of Rare-Rare and Shock-Shock are swapped. :raw-html:`<br>`
    :math:`\lambda_{1/2} = u \mp \sqrt{gh}` :raw-html:`<br>`
    thus the wave speeds scale with the velocity. The higher the velocity the higher the wave speeds.

Shock-Shock
    Waves speeds of Rare-Rare and Shock-Shock are swapped. :raw-html:`<br>`
    :math:`\lambda_{1/2} = u \mp \sqrt{gh}` :raw-html:`<br>`
    thus the wave speeds scale with the velocity. The higher the velocity the higher the wave speeds.

2.2. Dam-Break
--------------

Play around
^^^^^^^^^^^

l_hl...height of left side :raw-html:`<br>`
l_hr...height of right side :raw-html:`<br>`
l_location...location :raw-html:`<br>`
l_ur...particles velocity of the right side

All results with 100 cells!

+--------+--------+------------+--------+
|  l_hl  |  l_hr  | l_location |  l_ur  |
+========+========+============+========+
|  10    |   10   |      5     |    0   |
+--------+--------+------------+--------+
|  20    |   10   |      5     | 4.1327 |
+--------+--------+------------+--------+
|  10    |   20   |      5     |-4.1356 |
+--------+--------+------------+--------+
|  200   |   10   |      5     |34.2068 |
+--------+--------+------------+--------+
|  200   |   150  |      5     | 5.9400 |
+--------+--------+------------+--------+
|  200   |   190  |      5     | 1.1214 |
+--------+--------+------------+--------+

What we observe
^^^^^^^^^^^^^^^

The larger the gap between the left and right water height the bigger is the velocity.
With higher water columns but same difference between left and right water height the
velocity decreases.

Compute evacuation time
^^^^^^^^^^^^^^^^^^^^^^^

.. math::

    q_l = [14, 0]^T, u_l = \frac{0}{14} =0\\
    q_r = [3.5, 0.7]^T, u_l = \frac{0.7}{3.5} = 0.2\\
    h^{\text{Roe}} = \frac{1}{2}\cdot(14+3.5) = 8.75\\
    u^{\text{Roe}} = \frac{0\cdot\sqrt{14}+0.2\cdot\sqrt{3.5}}{\sqrt{1}+\sqrt{3.5}} = \frac{1}{15}\\
    \sqrt{gh^{\text{Roe}}} = \sqrt{9.80665\cdot8.75} = 9.263270886\\
    \lambda_1 = \frac{1}{15}-9.263270886 = -9.196604219\\
    \lambda_2 = \frac{1}{15}+9.263270886 = 9.329937553

Distance: :math:`25,000\,m` :raw-html:`<br>`
Wave speed: :math:`9.329937553\,m/s`

Time: :math:`\frac{25,000\,m}{9.329937553\,m/s} = 2679.546\,s \approx \text{44:40 min}`
