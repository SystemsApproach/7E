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
known as Wi-Fi 6.0 (also called 802.11ax). The rest of this section
introduces that technology.

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

.. Following paragraph is 5G-specific. Need to rewrite to generalize
   for Wi-Fi, which also uses feedback (but of a different sort).

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
Wi-Fi and the Mobile Cellular Network have a rich history using
variants of those approaches. For example, Wi-Fi originally used
*spread spectrum*, a technique developed by the miliary to combat
intentional attempts to jam radio signals. The idea behind spread
spectrum is to spread the signal over a wider frequency band, so as to
minimize the impact of interference from other devices. For example,
*frequency hopping* is a spread spectrum technique that involves
transmitting the signal over a random sequence of frequencies; that
is, first transmitting at one frequency, then a second, then a third,
and so on. The sequence of frequencies is not truly random but is
instead computed algorithmically by a pseudorandom number
generator. The receiver uses the same algorithm as the sender and
initializes it with the same seed; hence, it is able to hop
frequencies in sync with the transmitter to correctly receive the
frame. This scheme reduces interference by making it unlikely that two
signals would be using the same frequency for more than the infrequent
isolated bit.

On the mobile network side, each generation has used a different
approach. For example, 2G used *Time Division Multiple Access (TDMA)*
and 3G used *Code Division Multiple Access (CDMA)*. Today, both 4G and
5G are based on *Orthogonal Frequency-Division Multiplexing (OFDM)*,
an approach that multiplexes data over multiple orthogonal frequency
subbands, each of which is modulated independently. One attraction of
OFDM is that, by splitting the frequency band into subbands, it can
send symbols on each subband at a relatively low rate. This makes it
easier to correctly decode symbols in the presence of multipath
interference. The efficiency of OFDM depends on the selection of
subband frequencies so as to avoid interference, that is, how it
achieves orthogonality. That topic is beyond the scope of this book.

Today, the radio multiplexing technology used by both 5G and Wi-Fi is
a specific application of OFDM, a technique called *Orthogonal
Frequency-Division Multiple Access (OFDMA)*.  The “Multiple Access” in
OFDMA just means that data can simultaneously be sent on behalf of
multiple users, each on a different subband and for a different
duration of time. Wi-FI and 5G manage OFDMA in different ways—as we'll
see in their respective sections—but they are starting with the same
building block.

In its simplest form, the subbands are narrow (e.g., 15 kHz), and the
coding of user data into OFDMA symbols is designed to minimize the
risk of data loss due to interference.  The number of bits that can be
encoded in each symbol depends on the modulation scheme in use.  For
example, using *Quadrature Amplitude Modulation (QAM)*, 16-QAM yields
4 bits per symbol and 64-QAM yields 6 bits per symbol. The details of
the modulation need not concern us here; the key point is that there
is a degree of freedom to choose the modulation scheme based on the
measured channel quality, sending more bits per symbol (and thus more
bits per second) when the quality is high. In other words, OFDMA is
not a coding/modulation algorithm, *per se*, but instead provides a
framework for selecting a specific coding and modulation for each
subband frequency.
