.. role:: raw-html(raw)
    :format: html

.. _submissions_tsunami_simulation:

6. Tsunami Simulations
======================

6.1. 2010 M 8.8 Chile Event
---------------------------

1. Visualization of the input data
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. raw:: html

    <center>
        <img src="../_static/photos/task_6_1_1.png" alt="Visualization of the input data">
    </center>

2. Simulation of the tsunami event (Chile)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Scale in x-dimension predetermined with :math:`x: 3500000` :raw-html:`<br>`
Scale in y-dimension predetermined with :math:`y: 3000000`

.. math::

    \text{l_endTime} = \frac{\text{scale in x direction}}{\lambda} = \frac{\text{scale in x direction}}{\sqrt{g\cdot h}}

    \lambda = 284.8 \approx 285\,\frac{m}{s}

    \text{l_endTime} = \frac{3500000\,m}{285\,\frac{m}{s}} = 12280.702\,s

Since lambda is only the maximum value and this is not the corresponding value for the wave over the entire time course,
we use the calculated value only as an order of magnitude and add a generous time span.
The bottom line is that we use 15000 seconds.

Cell size: **1000m**

Required cells in x-direction: :math:`\frac{3500000}{1000}=3500` :raw-html:`<br>`
Required cells in x-direction: :math:`\frac{3000000}{1000}=3000`

.. raw:: html

    <center>
        <video width="700" controls>
            <source src="../_static/videos/chile_1000.mp4" type="video/mp4">
        </video>
    </center>

Cell size: **500m**

Required cells in x-direction: :math:`\frac{3500000}{500}=7000` :raw-html:`<br>`
Required cells in x-direction: :math:`\frac{3000000}{500}=6000`

.. raw:: html

    <center>
        <video width="700" controls>
            <source src="../_static/videos/chile_500.mp4" type="video/mp4">
        </video>
    </center>

Cell size: **250m**

Required cells in x-direction: :math:`\frac{3500000}{250}=14000` :raw-html:`<br>`
Required cells in x-direction: :math:`\frac{3000000}{250}=12000`

TODO ADD LEGGO_250

6.2. 2011 M 9.1 Tohoku Event
----------------------------

1. Simulation of the tsunami event (Tohoku)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Scale in x-dimension predetermined with :math:`x: 2700000` :raw-html:`<br>`
Scale in y-dimension predetermined with :math:`y: 1500000`

.. math::

    \text{l_endTime} = \frac{\text{scale in x direction}}{\lambda} = \frac{\text{scale in x direction}}{\sqrt{g\cdot h}}

    \lambda = 307.3 \approx 308\,\frac{m}{s}

    \text{l_endTime} = \frac{2700000\,m}{308\,\frac{m}{s}} = 8766.234\,s

Since lambda is only the maximum value and this is not the corresponding value for the wave over the entire time course,
we use the calculated value only as an order of magnitude and add a generous time span.
The bottom line is that we use 13000 seconds.

Cell size: **2000m**

Required cells in x-direction: :math:`\frac{2700000}{2000}=1350` :raw-html:`<br>`
Required cells in x-direction: :math:`\frac{2700000}{2000}=750`

.. raw:: html

    <center>
        <video width="700" controls>
            <source src="../_static/videos/tohoku_2000.mp4" type="video/mp4">
        </video>
    </center>

Cell size: **1000m**

Required cells in x-direction: :math:`\frac{2700000}{1000}=2700` :raw-html:`<br>`
Required cells in x-direction: :math:`\frac{2700000}{1000}=1500`

.. raw:: html

    <center>
        <video width="700" controls>
            <source src="../_static/videos/tohoku_1000.mp4" type="video/mp4">
        </video>
    </center>

Cell size: **500m**

Required cells in x-direction: :math:`\frac{2700000}{500}=5400` :raw-html:`<br>`
Required cells in x-direction: :math:`\frac{2700000}{500}=3000`

.. raw:: html

    <center>
        <video width="700" controls>
            <source src="../_static/videos/tohoku_500.mp4" type="video/mp4">
        </video>
    </center>

.. math::
    \text{x: } -127956.17  - -200000 =  72043.83
    \text{y: } -54518.72 - -750000 = 695481.28


2. Sõma
^^^^^^^

"On 11 March 2011, at 14:46 JST (05:46 UTC), an undersea megathrust earthquake of magnitude 9.0-9.1 occurred in the
Pacific Ocean, 72 km east of the Oshika Peninsula in the Tōhoku region. It lasted about six minutes and generated a
tsunami."[1]_

"Sõma is a town in Japan about 54.6 km north and 127.6 km west of the March 11, 2011, M 9.1 Tohoku event’s epicenter.
We are interested in the time between the earthquake rupture and the arrival of the first tsunami waves in Sõma."[2]_

"On 13 March 2011, the Japan Meteorological Agency (JMA) published details of tsunami observations recorded around the
coastline of Japan following the earthquake. These observations included [...] that the water height in Sõma was
:math:`7.3\,m` or even higher at around 15:50 JST (06:50 UTC)."[2]_ 14:46 JST - 15:50 JST is a period of **01:04h** or
**64 minutes**.

Our station with the coordinates (72043.83, 695481.28) w.r.t.

.. code-block:: json
    :emphasize-lines: 7-8

    /// File: ../resources/config.json
    {
      "output_frequency": 60,
      "stations": [
        {
          "name": "soma",
          "x": 72043.83,
          "y": 695481.28
        }
      ]
    }

records that the tsunami wave arrived at the station off the coast of Sõma after 50 minutes (3000 seconds). This can
also be clearly seen in the animation below. The station is marked with a pink dot. Since the 64 minutes date an
approximate maximum of the water height, we can use our 50 minutes as the arrival time of the wave.

TODO ADD VIDEO WITH STATION

Contribution
------------

All team members contributed equally to the tasks.

.. [1] From https://en.wikipedia.org/wiki/2011_T%C5%8Dhoku_earthquake_and_tsunami (30.11.2023)

.. [2] From https://scalable.uni-jena.de/opt/tsunami/chapters/assignment_4.html#stations (30.11.2023)