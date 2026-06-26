.. index:: DWDM: Dense Wavelength Division Multiplexing
.. index:: ROADM: Reconfigurable Optical Add/Drop Multiplexer
.. index:: SONET: Synchronous Optical Network

|Tech|.3 Circuit Switching
------------------------------------

This book squarely focuses on packet-switched networks, in which
complete packets are transmitted from one node to another, and any
switch along an end-to-end path from source to destination
stores and forwards packets.  There is another general approach to
building networks, known as *circuit switching*, that pre-dates packet
switching. It has always been an integral part of the telephony
network. In fact, as we pointed out in the introduction to this
chapter, the original ARPANET was an experiment to demonstrate the
feasibility of packet switching, and it ran on top of 64 Kbps circuits
leased from the phone company.

At a high level, circuit switching involves passing data through a
switch without breaking it into discrete chunks. In some cases, the
analog signal is itself passed through the switch, from input port to
output port, without first converting it to a digital representation.
(Sometimes it is amplified or regenerated to correct for attenuation.)
In other cases, the incoming analog signal is digitized, but the
resulting values are immediately passed to the output port and
transmitted, without being queued. That's only possible if you are
positive the output port is ready to transmit data the instant it is
received, without the possibility of contention. That sort of strict
guarantee implies circuit resources are first "reserved" before
used. Indeed, reservations are an essential aspect of circuit
switching.

Picturing an operator patch panel in a telephone system from the turn
of the 20th century, with a human manually setting up a circuit from
some input port to some output port in order to complete a call, is a
concrete place to start. That humble beginning led to today's circuit
switching technology, which would sound like science fiction to that
1900 operator. This section briefly looks at two examples, both of
which play a role in providing the cross-country links used to build
wide-area networks. Note that the two examples illustrate the
alternative multiplexing strategies mentioned in Section |Intro|.4:
frequency-division multiplexing (FDM) and synchronous time-division
multiplexing (STDM).

|Tech|.3.1  All-Optical Networks
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

While it's possible to send and receive Ethernet frames over optical
fiber at bandwidths approaching 1 Tbps, forwarding packets at this
speed would entail interfaces on switches that convert between the
optical signals on the fiber and the digital blocks of data that are
then processed by the switch. A circuit-based alternative, known as
*Dense Wavelength Division Multiplexing (DWDM)*, instead switches the
optical signal without requiring that time- and energy-consuming
conversion. DWDM equipment is able to transmit a large number of
optical wavelengths (colors) down a single fiber. Each wavelength is
called a lambda (λ), and corresponds to the length of each wave
(measured in nanometers) in the optical signal. The "Dense" qualifier
in DWDM indicates that the technology is able to distinguish between
~100 different wavelengths; each of these wavelengths might carry as
much as 100 Gbps of data.

.. _fig-roadm:
.. figure:: technology/figures/roadm.png
   :width: 500px
   :align: center

   Packet switches connected by a logical point-to-point circuit, that
   is in turn implemented by a path through a sequence of ROADMs in an
   all-optical network.

Connecting these fibers is an optical device called a ROADM
(*Reconfigurable Optical Add/Drop Multiplexer*). A collection of
ROADMs (nodes) and fibers (links) form an optical transport network,
where each ROADM is able to forward individual wavelengths along a
multi-hop path, creating a logical end-to-end circuit. From the
perspective of a packet-switched network that might be constructed on
top of this optical transport, one wavelength, even if it crosses
multiple ROADMs, appears to be a single point-to-point link between
two packet switches. This scenario is shown in :numref:`Figure %s
<fig-roadm>`. The reconfigurability feature of ROADMs means that it is
possible to change these underlying end-to-end paths, effectively
creating a new topology when viewed from the perspective of the
packet-switching layer.

|Tech|.3.2 SONET
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

*Synchronous Optical Network (SONET)* is a standard for transmitting
digital data over optical fiber.\ [#]_ Unlike DWDM, SONET converts optical
signals into a digital form (and back again for transmission), but it
preserves the illusion of a circuit that supports end-to-end
bit-streams at some constant rate.

.. [#] SONET is primarily used in North America. A similar standard,
       Synchronous Digital Hierarchy (SDH), is used elsewhere in the
       world. SONET and SDH are similar standards, but the term SONET
       is often used to refer to either.

Much of SONET’s complexity—its full specification is substantially
longer than this book—reflects the fact that phone companies have
historically been concerned with multiplexing large numbers of 64 Kbps
channels for voice calls. To this end, SONET defines a hierarchy
of channels, with multiple "low bandwidth" channels multiplexed over
one "high-bandwidth" channel; this can be repeated multiple times to
form a hierarchy of transmission speeds. Finally, all of this is done
within the framework of an STDM-based approach, in which data at each
level of the hierarchy are sent and received during discrete time
intervals.

The base SONET link speed, known as STS-1, runs at 51.84 Mbps. (STS
stands for *Synchronous Transport Signal*.)  At this throughput rate,
it takes 125 μs to transmit one 810-byte block of data. This block is
sometimes called a "SONET frame", but unlike frames in a
packet-switched network, it is not necessarily just one self-contained
block of data being sent to a particular destination.  Instead, there
are three possibilities. The first is that it carries 810 bytes from
somewhere in the middle of a long-running 51.84 Mbps circuit assigned
to some customer. The second is that the block carries data on behalf
of up to 810 64 Kbps voice circuits. The 810 is not an accident; each
of those circuits gets to send 1 byte (in one STS-1 frame) every
125 μs. This ensures that each circuit delivers data to the customer
at a constant 64 Kbps, which is essential for a smooth voice call. The
third possibility is that the 810-byte block is just one of *N* blocks
being carried by *N* separate STS-1 circuits, resulting in an STS-N
circuit being delivered to an end customer. For example, an STS-3
circuit delivers data at a constant 155.52 Mbps. The maximum rate
supported by SONET is STS-768, at 39,813.12 Mbps.

Below SONET is an optical signalling layer, called *Optical Carrier
(OC)*, with a corresponding set of transmission rates; e.g., OC-1 at
51.84 Mbps, OC-3 at 155.52 Mbps, and so on. Those optical signals can,
in turn, be carried by a single wavelength over a DWDM substrate.

In summary, SONET establishes a hierarchy of circuits, with 64 Kbps
circuits aggregated in one STS-1 circuit, and N STS-1 circuits
aggregated (multiplexed together) to yield an STS-N circuit. There are
a lot of details involved in implementing this hierarchy, but it's the
promised circuit behavior that is key for our purposes: Customers are
able to procure constant bit-rate, long-haul SONET circuits ranging
from 64 Kbps to 39.8 Gbps.



