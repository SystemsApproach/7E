.. index:: OFDM: Orthogonal Frequency Division Multiplexing
.. index:: OFDMA: Orthogonal Frequency Division Multiple Access

|Shared|.2 Managing Spectrum
----------------------------------

Wireless transmission is full of challenges that don't arise in wired
networks. Interference can arise from many sources ranging from
microwave ovens to baby monitors, while radio signals reflect off
objects such as buildings and vehicles. Some objects absorb wireless
signals. The properties of the wireless channel vary over time, and
depending on the frequency in use. Much of the design of wireless
radio systems is motivated by the need to deal with these challenges.

.. sidebar:: Spectrum Allocation

  Before networks can allocate spectrum to users, someone has to
  decide how to allocate spectrum to network operators.
  These allocations are typically determined by government agencies,
  such as the Federal Communications Commission (FCC) in the United
  States.  Specific bands (frequency ranges) are allocated to certain
  uses.  Some bands are reserved for government use. Other bands are
  reserved for uses such as AM radio, FM radio, television, satellite
  communication, and mobile cellular phones. Specific frequencies
  within these bands are then licensed to individual organizations for
  use within certain geographical areas. Finally, several frequency
  bands are set aside for license-exempt usage—bands in which a
  license is not needed. Wi-Fi falls in this category.

  Devices that use license-exempt frequencies are still subject to
  certain restrictions to make that otherwise unconstrained sharing
  work. Most important of these is a limit on transmission power. This
  limits the range of a signal, making it less likely to interfere
  with another signal. For example, a wireless microphone might have a
  range of about 100 feet. The upper bound on power for Wi-Fi limits
  its range to roughly 100 meters, although in practice closer to tens
  of meters indoors where walls absorb much of the signal.

As we cover in the next two sections, Wi-Fi and 5G take different
approaches to the allocation of radio spectrum, but despite these
differences, they use basically the same multiplexing technology. This
has not always been the case, but is now with the latest generations
of Wi-Fi, known as Wi-Fi 6 and Wi-Fi 7 (also called 802.11ax and
802.11be respectively). The rest of this section introduces the
inherent challenges of using wireless spectrum and describes the
technology used to address them.

|Shared|.2.1 Transmission Challenges
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Radio-based data transmission has to deal with several impairments,
including noise, attenuation, distortion, fading, and interference. To
see this, consider the scenario depicted in :numref:`Figure %s
<fig-multipath>`, where the signal bounces off various stationary and
moving objects, following multiple paths from the transmitter to the
receiver, who may also be moving.

.. _fig-multipath:
.. figure:: shared/figures/multipath.png
    :width: 600px
    :align: center

    Signals propagate along multiple paths from transmitter to
    receiver.

As a consequence of these multiple paths, the original signal arrives
at the receiver spread over time, as illustrated in :numref:`Figure %s
<fig-coherence>`. Empirical evidence shows that the *multipath
spread*—the time between the first and last signals of one transmission
arriving at the receiver—is 1–10μs in urban environments and 10–30μs
in suburban environments. These multipath signals can interfere with
each other constructively or destructively, and this will vary over
time. The theoretical bound for the time duration for which the channel
may be assumed to be invariant, known as the *coherence time* and
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

The main takeaway from this brief introduction to the challenges of
radio transmission is that the solution needs to be adaptive, so it
can be configured to match the environment (indoor, urban, suburban)
and dynamically reconfigured to match changing conditions (noise,
mobility).

|Shared|.2.2  Multiplexing Technique
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

We saw an overview of multiplexing strategies in Chapter 1, and both
Wi-Fi and the Mobile Cellular Network have a long history using
variants of those approaches. Wi-Fi originally used
*spread spectrum*, a technique developed by the military to combat
intentional attempts to jam radio signals. The idea was to spread the
signal over a wide frequency band, so as to minimize the impact of
interference from other devices. For example, *frequency hopping* is a
spread spectrum technique that involves transmitting the signal over a
random sequence of frequencies; that is, first transmitting at one
frequency, then a second, then a third, and so on. The sequence of
frequencies is not truly random but is instead computed
algorithmically by a pseudorandom number generator. The receiver uses
the same algorithm as the sender and initializes it with the same
seed; hence, it is able to hop frequencies in sync with the
transmitter to correctly receive the frame. This scheme reduces
interference by making it unlikely that two signals would be using the
same frequency for very long.

On the mobile network side, each generation has used a different
approach. For example, 2G used *Time Division Multiple Access (TDMA)*,
3G used *Code Division Multiple Access (CDMA)*, and 4G uses
*Orthogonal Frequency-Division Multiplexing (OFDM)*—an approach that
multiplexes data over multiple orthogonal frequency subcarriers, each
of which is modulated independently.\ [#]_ One attraction of OFDM is
that, by splitting the frequency band into subcarriers, it can send
symbols on each subcarrier at a relatively low rate. This makes it
easier to correctly decode symbols in the presence of multipath
interference. The efficiency of OFDM depends on the selection of
subcarrier frequencies so as to avoid interference, that is, how it
achieves orthogonality. That topic is beyond the scope of this book.

.. [#] In radio communication, the term "carrier" refers to the base
       signal before it has been modulated to carry data.  Because we
       are now focused on dividing a larger frequency band into
       smaller subbands, each of those subbands has its own carrier
       signal, hence "subcarrier".

Today, the radio multiplexing technology used by both 5G and Wi-Fi is
a specific application of OFDM, a technique called *Orthogonal
Frequency-Division Multiple Access (OFDMA)*.  The “Multiple Access” in
OFDMA just means that data can simultaneously be sent on behalf of
multiple users, each on a different subcarrier and for a different
duration of time. Wi-Fi and 5G manage OFDMA in different ways—as we'll
see in their respective sections—but they are starting with the same
building block. It is this building block, and the ways it can be
(re-)configured to address variability, that is the main focus of this section.

Before getting into the details, there is one other bit of context to
set.  At a high level, some range of spectrum bandwidth is set aside
for wireless communication, whether it's licensed or unlicensed. We
often use a short-hand to refer to that band; e.g., "Wi-Fi at 5-GHz"
or "5G at 6-GHz". In actuality, we're talking about a specific range;
for example, Wi-Fi uses 5.150-GHz to 5.850-GHz. Within such a band, it
is common to then subdivide the full range into discrete *channels*,
of say, 20-MHz or 40-MHz each. The exact number varies from country to
country, and there are so many options that it's impossible to keep
track. For our purposes, we focus on a single channel in this section
(let's assume it's 20-MHz wide), and we take up the question of how to
manage a set of channels in later sections.

The subcarriers in OFDMA are narrow (e.g., 15 kHz), and the coding of
user data into OFDMA symbols is designed to minimize the risk of data
loss due to interference.  The number of bits that can be encoded in
each symbol depends on the specifics of the modulation scheme in use,
which is chosen based on the channel quality.
*Quadrature Amplitude Modulation (QAM)* is commonly used, and it
allows a wide range of bits per symbol depending on the specific
QAM parameters: 16-QAM yields 4 bits per symbol, 64-QAM yields 6 bits per
symbol, and 1024-QAM yields 10 bits per symbol (the current max for
Wi-Fi 6 and 5G). The key is that there is a degree of freedom to
choose the modulation scheme based on the measured channel quality,
sending more bits per symbol (and thus more bits per second) when the
quality is high. In other words, OFDMA is not so much a coding/modulation
algorithm as a *framework* for selecting a
specific coding and modulation scheme for each subcarrier frequency.

We can visualize the OFDMA framework as a grid of discrete schedulable
units of the radio spectrum. The fundamental unit is the time to
transmit one symbol on one subcarrier. :numref:`Figure %s
<fig-sched-grid>` shows the radio spectrum as a 2-D resource, where
the subcarriers are represented in the vertical dimension and the time
to transmit symbols on each subcarrier is represented in the
horizontal dimension. The basic unit of transmission, called a
*Resource Unit (RU)*, corresponds to a small frequency band around one
subcarrier frequency and the time it takes to transmit one OFDMA
symbol.

.. _fig-sched-grid:
.. figure:: shared/figures/sched-grid.png
    :width: 600px
    :align: center

    Spectrum abstractly represented by a 2-D grid of schedulable
    Resource Units (RU). The example shows just 12 subcarriers (a full
    channel might be 20 MHz, or more) and a 0.5ms transmission time
    for each Physical Block of RUs.

Any scheduler employed by the network does two things. First, it
specifies the OFDMA parameters to use during the next transmission
interval (e.g., QAM and MIMO settings). It does this based on feedback
it collects about the interference its transmissions experienced
during previous time intervals. Second, it allocates some number of
RUs to each user that has data to transmit during the next interval,
where users are depicted by different colored blocks in
:numref:`Figure %s <fig-sched-grid>`.

There are two other things to note about the 2-D grid shown in the
figure. First, while individual RUs can be allocated to different
users, larger blocks of RUs, denoted *Physical Blocks* in the figure,
are transmitted over the air.\ [#]_ :numref:`Figure %s
<fig-sched-grid>` shows two back-to-back physical blocks, each of
which takes 0.5ms to transmit. In other words, allocation decisions
are made on a per-RU basis, with a sequence of "allocation plans"
executed on a block-by-block basis over time.

.. [#] This section gives a general overview of OFDMA, independent of
       how it is used by Wi-Fi and 5G, but both of those use cases have
       their own terminology for various concepts. For example, what we
       refer to as a *Physical Block*, is called a *Physical Resource
       Block (PRB)* in 5G and a *Physical Packet Data Unit (PPDU)* in
       Wi-Fi. The term "Scheduling Horizon" is a similar generic term.
       We try to avoid technology-specific jargon, and use intuitively
       helpful terminology in its place.

Second, we used 15-kHz as our example subcarrier spacing, with a
Physical Block transmission time of 0.5ms. (This corresponds to a
time-per-symbol of 66.67 μs in the example, taking into account other
overhead associated with transmitting a physical block.) These are
sometimes referred to as the *numerology* of the radio's air
interface, and our example is just that—an example.  Any given
wireless network will be configured with a particular numerology. The
exact settings can be configured for the target environment (e.g.,
urban, suburban, indoors), and in some cases, even dynamically changed
based on observed behavior.  That is to say, selecting a network's
numerology is another degree-of-freedom that a scheduling algorithm
might take into consideration.

All of this brings us to the following observation: making scheduling
decisions requires complex analysis and heavy use of heuristics. (It
is also, arguably, an opportunity to leverage machine learning.)
Exactly how the decisions are made is both network-specific (Wi-Fi and 5G
adopt different strategies) and vendor-specific. The actual
algorithms are not part of the respective standards, but rather, are
proprietary.  What the next two sections focus on is the objectives
Wi-Fi and 5G, respectively, are trying to achieve. Wi-Fi's approach is
consistent with the best-effort philosophy of the Internet; there are
no guarantees. In contrast, 5G tries to deliver the most predictable
performance it can to the largest number of devices.

