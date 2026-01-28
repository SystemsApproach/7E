5.2 Managing Spectrum
----------------------------------

Wireless transmission is full of challenges that don't arise in wired
networks. Interference can arise from many sources ranging from
microwave ovens to baby monitors, while radio signals reflect off
objects such as buildings and vehicles. Some objects absorb wireless
signals. The properties of the wireless channel vary over time, and
depending on the frequency in use. Much of the design of wireless
radio systems is motivated by the need to deal with these challenges.

.. sidebar:: Spectrum Allocation

   Before networks can allocate spectrum to users, governments have to
   allocate (license) spectrum to network operators.These allocations
   are typically determined by government agencies, such as the
   Federal Communications Commission (FCC) in the United States.
   Specific bands (frequency ranges) are allocated to certain
   uses. Some bands are reserved for government use. Other bands are
   reserved for uses such as AM radio, FM radio, television, satellite
   communication, and cellular phones. Specific frequencies within
   these bands are then licensed to individual organizations for use
   within certain geographical areas. Finally, several frequency bands
   are set aside for license-exempt usage—bands in which a license is
   not needed.

  Devices that use license-exempt frequencies are still subject to
  certain restrictions to make that otherwise unconstrained sharing
  work. Most important of these is a limit on transmission power. This
  limits the range of a signal, making it less likely to interfere
  with another signal. For example, a cordless phone (a common
  unlicensed device) might have a range of about 100 feet.

As we will see in the next two sections, Wi-Fi is contention-based,
whereas 5G uses a reservation-based strategy. But despite this
fundamental difference, Wi-Fi and the 5G both deal with the underlying
radio spectrum using the same transmission technology.  This has not
always been the case, but is now with the latest generation of Wi-Fi,
known as Wi-Fi 6.0. The rest of this section introduces that technology.

5.2.1 Transmission Challenges
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

We start with a little background on the challenge radio-based data
transmission face dealing with several impairments, including noise,
attenuation, distortion, fading, and interference. To see this,
consider the scenario depicted in :numref:`Figure %s <fig-multipath>`,
where the signal bounces off various stationary and moving objects,
following multiple paths from the transmitter to the receiver, who may
also be moving.

.. _fig-multipath:
.. figure:: shared/figures/multipath.png
    :width: 600px
    :align: center

    Signals propagate along multiple paths from transmitter to
    receiver.

As a consequence of these multiple paths, the original signal arrives
at the receiver spread over time, as illustrated in :numref:`Figure %s
<fig-coherence>`. Empirical evidence shows that the Multipath
Spread—the time between the first and last signals of one transmission
arriving at the receiver—is 1-10μs in urban environments and 10-30μs
in suburban environments. These multipath signals can interfere with
each other constructively or destructively, and this will vary over
time. Theoretical bounds for the time duration for which the channel
may be assumed to be invariant, known as the *Coherence Time* and
denoted :math:`T_c`, is given by

.. math::
   T_c =c/v \times 1/f

where :math:`c` is the velocity of the signal, :math:`v` is the
velocity of the receiver (e.g., moving car or train), and :math:`f` is
the frequency of the carrier signal that is being modulated. This
says the coherence time is inversely proportional to the frequency of
the signal and the speed of movement, which makes intuitive sense: The
higher the frequency (narrower the wave) the shorter the coherence time,
and likewise, the faster the receiver is moving the shorter the coherence
time. Based on the target parameters to this model (selected according
to the target physical environment), it is possible to calculate
:math:`T_c`, which in turn bounds the rate at which symbols can be
transmitted without undue risk of interference. The dynamic nature of
the wireless channel is a central challenge to address in the wireless
network.

.. _fig-coherence:
.. figure:: shared/figures/coherence.png
    :width: 500px
    :align: center

    Received data spread over time due to multipath
    variation.

To complicate matters further, :numref:`Figure %s <fig-multipath>` and
:numref:`%s <fig-coherence>` imply the transmission originates from a
single antenna, but base stations are sometimes equipped with an array
of antennas, each transmitting in a different (but overlapping)
direction. This technology, called *Multiple-Input-Multiple-Output
(MIMO)*, opens the door to purposely transmitting data from multiple
antennas in an effort to reach the receiver, adding even more paths to
the environment-imposed multipath propagation.

One of the most important consequences of these factors is that the
transmitter must receive feedback from every receiver to judge how to
best utilize the wireless medium on their behalf. 3GPP specifies a
*Channel Quality Indicator (CQI)* for this purpose. In practice,
the receiver sends a CQI status report to the base station periodically
(e.g., every millisecond). These CQI messages report the observed
signal-to-noise ratio, which impacts the receiver's ability to recover
the data bits. The base station then uses this information to adapt how
it allocates the available radio spectrum to the subscribers it is
serving, as well as which coding and modulation scheme to employ.
All of these decisions are made by the scheduler.

5.2.2  Multiplexing Technique
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

We saw an overview of multiplexing strategies in Chapter 1, and both
Wi-Fi and the mobile cellular network have a rich history using
different approaches over time. For example, Wi-Fi originally used
*spread spectrum* technology developed by the miliary. (SOME SUMMARY).
On the mobile network side, each generation used a different approach;
e.g., 2G used *Time Division Multiple Access (TDMA)* and 3G used *Code
Division Multiple Access (CDMA)*. Today, both 4G and 5G, as well as
the latest versions of Wi-Fi, are based on *Orthogonal
Frequency-Division Multiplexing (OFDM)*, an approach that multiplexes
data over multiple orthogonal subcarrier frequencies, each of which is
modulated independently. One attraction of OFDM is that, by splitting
the frequency band into subcarriers, it can send symbols on each
subcarrier at a relatively low rate. This makes it easier to correctly
decode symbols in the presence of multipath interference. The
efficiency of OFDM depends on the selection of subcarrier frequencies
so as to avoid interference, that is, how it achieves
orthogonality. That topic is beyond the scope of this book.

As long as you understand that OFDM uses a set of subcarriers, with
symbols (each of which contain a few bits of data) being sent at some
rate on each subcarrier, that you can appreciate that there are
discrete schedulable units of the radio spectrum. The fundamental unit
is the time to transmit one symbol on one subcarrier. With that
building block in mind, we are now in a position to examine how
multiplexing works.

The specific multiplexing technology is called *Orthogonal
Frequency-Division Multiple Access (OFDMA)*. You can think of OFDMA as
a specific application of OFDM. The idea of OFDMA is to multiplex data
over a set of orthogonal (non-interfering) subcarrier frequencies,
each of which is modulated independently. The “Multiple Access” in
OFDMA implies that data can simultaneously be sent on behalf of
multiple users, each on a different subcarrier frequency and for a
different duration of time. In its simplest form, the subbands are
narrow (e.g., 15 kHz), and the coding of user data into OFDMA symbols
is designed to minimize the risk of data loss due to interference.

The use of OFDMA naturally leads to conceptualizing the radio spectrum
as a 2-D resource, as shown in :numref:`Figure %s <fig-sched-grid>`,
with the subcarriers represented in the vertical dimension and the time to
transmit symbols on each subcarrier represented in the horizontal dimension.
The basic unit of transmission, called a *Resource Element (RE)*,
corresponds to a 15-kHz band around one subcarrier frequency and the
time it takes to transmit one OFDMA symbol. The number of bits that
can be encoded in each symbol depends on the modulation scheme in use.
For example, using *Quadrature Amplitude Modulation (QAM)*, 16-QAM
yields 4 bits per symbol and 64-QAM yields 6 bits per symbol. The
details of the modulation need not concern us here; the key point is
that there is a degree of freedom to choose the modulation scheme
based on the measured channel quality, sending more bits per symbol
(and thus more bits per second) when the quality is high.

.. _fig-sched-grid:
.. figure:: shared/figures/sched-grid.png
    :width: 600px
    :align: center

    Spectrum abstractly represented by a 2-D grid of
    schedulable Resource Elements.

A scheduler allocates some number of REs to each user that has data to
transmit during each 1 ms *Transmission Time Interval (TTI)*, where users
are depicted by different colored blocks in :numref:`Figure %s <fig-sched-grid>`.
The only constraint on the scheduler is that it must make its allocation
decisions on blocks of 7x12=84 resource elements, called a *Physical
Resource Block (PRB)*. :numref:`Figure %s <fig-sched-grid>` shows two
back-to-back PRBs. Of course time continues to flow along one axis, and
depending on the size of the available frequency band (e.g., it might be
100 MHz wide), there may be many more subcarrier slots (and hence PRBs)
available along the other axis, so the scheduler is essentially
preparing and transmitting a sequence of PRBs.

Note that OFDMA is not a coding/modulation algorithm, but instead
provides a framework for selecting a specific coding and modulation for
each subcarrier frequency. QAM is one common example modulation. It is
the scheduler's responsibility to select the modulation to use for each
PRB, based on the CQI feedback it has received. The scheduler also
selects the coding on a per-PRB basis, for example, by how it sets the
parameters to the turbo code algorithm.

The 1-ms TTI corresponds to the time frame in which the scheduler
receives feedback from users about the quality of the signal they are
experiencing. This is the role of CQI: once every millisecond, each
user sends a set of metrics, which the scheduler uses to make its
decision as to how to allocate PRBs during the subsequent TTI.

Another input to the scheduling decision is the *QoS Class Identifier
(QCI)*, which indicates the quality-of-service each class of traffic
is to receive. In 4G, the QCI value assigned to each class (there are
twenty six such classes, in total) indicates whether the traffic has a
*Guaranteed Bit Rate (GBR)* or not *(non-GBR)*, plus the class's
relative priority within those two categories. (Note that the 5QI
parameter introduced in Chapter 2 serves the same purpose as the QCI
parameter in 4G.)

Finally, keep in mind that :numref:`Figure %s <fig-sched-grid>` focuses on
scheduling transmissions from a single antenna, but the MIMO technology
described above means the scheduler also has to determine which antenna
(or more generally, what subset of antennas) will most effectively reach
each receiver. But again, in the abstract, the scheduler is charged with
allocating a sequence of Resource Elements.

Note that the scheduler has many degrees of freedom: it has to decide which
set of users to service during a given time interval, how many resource
elements to allocate to each such user, how to select the coding and
modulation levels, and which antenna to transmit their data on. This is
an optimization problem that, fortunately, we are not trying to solve
here. Exactly how this scheduler works is where Wi-Fi and 5G differ; we
get to those answers in the next two sections.
