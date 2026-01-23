3.3 Circuit Switching
------------------------------------

This book is squarely focused on packet-switched networks, in which
complete packets are transmitted from one node to another, and any
switch along an end-to-end path from source to destination
stores-and-forwards packets.  There is another general approach to
building networks, known as *circuit switching*, that pre-dates packet
switching. It has always been an integral part of the telephony
network, and in fact, the original ARPANET experiment to demonstrate
the feasibility of packet switching ran on top of 64 Kbps circuits
leased from the phone company.

At a high-level, circuit switching involves passing data through a
switch without breaking it into discrete chunks. In some cases, the
analog signal is itself passed through the switch, from input port to
output port, without first converting it to a digital representation.
(Sometimes it is amplified to correct for attenuation.)  In other
cases, the incoming analog signal is digitized, but the resulting
values are immediately passed to the output port and transmitted,
without being queued. That's only possible if you are positive the
output port is ready to transmit data the instant it is received,
without the possibility of contention. That sort of strict guarantee
implies circuits resources are first "reserved" before used. Indeed,
reservations are an essential aspect of circuit switching.

Picturing an operator patch panel in a telephone system from the turn
of the 20th century, with a human manually setting up a circuit from
some input port to some output port in order to complete a call, is a
concrete place to start. That humble beginning led to today's circuit
switching technology, which not surprisingly, is significantly more
sophisticated. This section briefly looks at two examples, both of
which play a role in providing the cross-country links used to build
wide-area networks. Note that the two examples illustrate the
alternative multiplexing strategies mentioned in Section 1.4:
frequency-division multiplexing (FDM) and synchronous time-division
multiplexing (STDM).

3.3.1  All-Optical Networks
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

While it's possible to send and receive Ethernet frames over optical
fiber at bandwidths approaching 1 Tbps, doing so implies packet
switches that convert between the optical signals on the fiber and the
digital blocks of data that are then processed by the switch. A
circuit-based alternative, known as *Dense Wavelength Division
Multiplexing (DWDM)*, instead switches the optical signal without
requiring that time-consuming conversion. DWDM equipment is able to
transmit a large numbers of optical wavelengths (colors) down a single
fiber. For example, one might send data on 100 or more different
wavelengths, and each wavelength might carry as much as 100 Gbps of
data.

Connecting these fibers is an optical device called a ROADM
(*Reconfigurable Optical Add/Drop Multiplexer*). A collection of
ROADMs (nodes) and fibers (links) form an optical transport network,
where each ROADM is able to forward individual wavelengths along a
multi-hop path, creating a logical end-to-end circuit. From the
perspective of a packet-switched network that might be constructed on
top of this optical transport, one wavelength, even if it crosses
multiple ROADMs, appears to be a single point-to-point link between
two packet switches. The reconfigurability feature of ROADMs means
that it is possible to change these underlying end-to-end wavelengths,
effectively creating a new topology at the packet-switching layer.

3.3.2 SONET
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

*Synchronous Optical Network (SONET)* is a standard for transmitting
digital data over optical fiber. Unlike DWDM, SONET converts optical
signals into a digital form (and back again for transmission), but it
preserves the illusion of a circuit that supports end-to-end
bit-streams at some fixed rate.

Much of SONET’s design and complexity—its full specification is
substantially longer than this book—reflects the fact that phone
companies have historically been concerned with multiplexing large
numbers of 64 Kbps channels for telephone calls. To this end, SONET
defines a hierarchy of channels, with multiple "low bandwidth"
channels nested inside one "high-bandwidth" channel, all within
the framework of an STDM-based solution.

The base SONET link speed, which is known as STS-1, runs at
51.84 Mbps. (STS stands for *Synchronous Transport Signal*.)  At this
throughput rate, it takes 125 μs to transmit one 810-byte block of
data. This block is sometimes called a "SONET frame", but unlike
frames in a packet-switched scenario, it is not necessarily just one
self-contained block of data being sent to a particular destination.
Instead, there are three possibilities. The first is that it carries
810 bytes from some random place in the middle of a long-running
51.84 Mbps circuit. The second is that the block carries data on
behalf of up to 810 64 Kbps voice circuits. The 810 is not an
accident; each of those circuits gets to send 1 byte (in one STS-1
frame) every 125 μs. This ensure that each circuit delivers data at a
constant 64 Kbps, which is essential for a smooth voice call. The
third possibility is that the 810-byte block is just one of *N* blocks
being carried by *N* separate STS-1 circuits, resulting in an STS-N
circuit being delivered to an end customer. For example, an STS-3
circuit delivers data at a constant 155.52 Mbps. The maximum rate
supported by SONET is STS-768, at 39,813.12 Mbps.

In other words, SONET establishes a hierarchy of circuits, with
64 Kbps circuits aggregated in one STS-1 circuit, and N STS-1 circuits
aggregated to yield an STS-N circuit. There are a lot of details
involved in implementing this hierarchy, but it's the promised circuit
behavior that is key for our purposes: customers are able to procure
constant bit-rate, long-haul SONET circuits ranging from 64 Kbps to
39.8 Gbps.



