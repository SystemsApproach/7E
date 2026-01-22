3.3 Circuit Switching
------------------------------------

This book is squarely focused on packet-switched networks, in which
complete packets are transmitted from one node to another, and any
switch along an end-to-end path from source to destination
stores-and-forwards packets.  There is another general approach to
building networks, known as *circuit switching*, that pre-dates packet
switching. It has always been an integral part of the telephony
network, and in fact, the original ARPANET experiment to demonstrate
the feasibility of packet switching ran on top of 64Kbs circuits
leased from the phone company.

At a high-level, circuit switching involves passing data through a
switch without breaking it into discrete chunks. In some cases, the
analog signal is itself passed through the switch, from input port to
output port, without first converting it to a digital representation.
In other cases, the incoming analog signal is digitized, but the
resulting values are immediately passed to the output port and
transmitted, without being stored. That's only possible if you are
positive the output port is ready to transmit data the instant it is
received, without the possibility of contention. That sort of strict
resource guarantee is an essential aspect of circuit switching.

Picturing an operator patch panel in a telephone system from the turn
of the 20th century, with a human manually setting up a circuit from
some input port to some output port in order to complete a call, is a
concrete place to start. That humble beginning led to today's circuit
switching technology, which not surprisingly, is significantly more
sophisticated. This section briefly looks at two examples, both of
which play a role in providing the cross-country links used to build
wide-area networks.

3.3.1  All-Optical Networks
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

While it's possible to send and receive Ethernet frames over optical
fiber at bandwidths approaching 1Tbps, doing so implies packet
switches that convert between the optical signals on the fiber and the
digital blocks of data that are then processed by the switch. A
circuit-based alternative, known as *Dense Wavelength Division
Multiplexing (DWDM)*, instead switches the optical signal without
requireing that time-consuming conversion. DWDM equipment is able to
transmit a large numbers of optical wavelengths (colors) down a single
fiber. For example, one might send data on 100 or more different
wavelengths, and each wavelength might carry as much as 100 Gbps of
data.

Connecting these fibers is an optical device called a ROADM
(*Reconfigurable Optical Add/Drop Multiplexers*). A collection of
ROADMs (nodes) and fibers (links) form an optical transport network,
where each ROADM is able to forward individual wavelengths along a
multi-hop path, creating a logical end-to-end circuit. From the
perspective of a packet-switched network that might be constructed on
top of this optical transport, one wavelength, even if it crosses
multiple ROADMs, appears to be a single point-to-point link between
two switches, over which one might elect to run SONET or 100-Gbps
Ethernet as the framing protocol. The reconfigurability feature of
ROADMs means that it is possible to change these underlying end-to-end
wavelengths, effectively creating a new topology at the
packet-switching layer.

3.3.2 SONET
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

*Synchronous Optical Network (SONET)* is a standard for transmiting
digital data over optical fiber. Unlike DWDM, SONET does convert
optical signals into a digital from (and back again for transmission),
but it does preserve the illustion of a circuit that supports
end-to-end bit-streams at some fixed rate.

Much of SONET’s design—and complexity, with its full specification
substantially longer than this book—reflects the fact that phone
companies have historically been concerned with multiplexing large
numbers of 64-kbps channels for telephone calls. SONET then defines a
hierarchy of channels, with multiple "low bandwidth channels" nested
inside "high bandwidth channels". Without getting into how this is
implemented, we instead focus on how these circuits are *used* by
customers, perhaps to build a nation-wide packet switched network.

The lowest speed SONET link, which is known as STS-1, runs at 51.84
Mbps. (STS stands for *Synchronous Transport Signal*.) Multiple
64-kbps voice circuits can be carried over one STS-1 link, or
alternatively, the full curcuit can be used to provide a 51.84 Mbps
link to just one data customer.  Moreover, three STS-1 links can be
logically aggregated to provide a single 155.52 Mbps STS-3 circuit. In
general, an STS-N curcuit can be thought of as consisting of N STS-1
circuits, where the bytes from these circuits are interleaved; that
is, a byte from the first circuit is transmitted, then a byte from the
second circuit is transmitted, and so on. The reason for interleaving
the bytes from each STS-N circuit is to ensure that the bytes in each
STS-1 circuit are evenly paced; that is, bytes show up at the receiver
at a smooth 51 Mbps, rather than all bunched up during one particular
interval. STS-768, at 39,813,120 Mbps, is the fastest circuit defined
by SONET.


