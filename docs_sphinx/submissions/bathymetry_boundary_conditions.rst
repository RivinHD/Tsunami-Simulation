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

Contribution
------------

All team members contributed equally to the tasks.
