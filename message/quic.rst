.. _artifact-quic:

|Message|.3 Optimizing RPC
--------------------------

RPC has long been a popular communication primitive for building
distributed systems, with gRPC expanding on that history to support
scalable cloud services. But the designers of gRPC make a calculated
tradeoff, favoring the convenience of RPC over the known performance
limitations of running HTTP over TCP. Their experience building and
supporting gRPC, coupled with the broader Internet's experience
building the World Wide Web and RESTful applications directly on top of HTTP, led to
insights about how both HTTP and the the underlying transport protocol
could be improved.  These insights, in turn, led to the development of
new version of HTTP, known as HTTP/3, and a new transport protocol,
called QUIC.  The two protocols are co-designed to better support the
Web and REST-based or RPC-based distributed systems. This section
describes the resulting design.

.. Cut-and-pasted from TCP chapter in 6E

QUIC originated at Google in 2012 and was subsequently developed as a
proposed standard at the IETF. Unlike many other efforts to add to the
set of transport protocols in the Internet, QUIC has achieved
widespread deployment. QUIC was motivated in large part by the
challenges of matching the request/response semantics of HTTP to the
stream-oriented nature of TCP.  These issues have become more
noticeable over time, due to factors such as the rise of high-latency
wireless networks, the availability of multiple networks for a single
device (e.g., Wi-Fi and cellular), and the increasing use of
encrypted, authenticated connections on the Web (as discussed in
Chapter |TLS|).

If network latency is high—say 100 milliseconds or more—then a few
RTTs can quickly add up to a visible annoyance for an end user. It's
also a performance issue for applications using gRPC. Establishing an
HTTP session over TCP with Transport Layer Security would typically
take at least three round trips (one for TCP session establishment and
two for setting up the encryption parameters) before the first HTTP
message could be sent. The designers of QUIC recognized that this
delay—the direct result of a layered approach to protocol design—could
be dramatically reduced if connection setup and the required security
handshakes were combined and optimized for minimal round trips.

The presence of multiple network interfaces also motivates a
different approach. If your mobile phone loses its Wi-Fi connection and needs
to switch to a cellular connection, that would typically require both
a TCP timeout on one connection and a new series of handshakes on the
other. Making the connection something that can persist over different
network layer connections was another design goal for QUIC.

Finally, the reliable byte stream model for TCP is a
poor match to a web page request, when many objects need to be fetched
and page rendering could begin before they have all arrived. Similar
arguments apply for RPC, where requests that can be handled quickly
should not be delayed waiting for
the responses to prior requests. While one
workaround for this would be to open multiple TCP connections in
parallel, this approach (which was used in the early days of the web)
has its own set of drawbacks, notably on congestion
control. Specifically, each connection runs its own congestion control
loop, so the experience of congestion on one connection is not
apparent to the other connections, and each connection tries to figure
out the appropriate amount of bandwidth to consume on its own.


By the time QUIC emerged, deployment of new
transport protocols had been tried many times with limited success due
to the changing nature of the Internet. Notably,  "middleboxes'' such as NATs and
firewalls are widely deployed, and because their operation depends on
looking past IP into the TCP and UDP headers,they can't be relied
upon to pass a new transport protocol. As a result, QUIC actually
rides on top of UDP. In other words, it is a transport protocol
running on top of a transport protocol. As we noted in the previous
section, such "violations" of strict layering are both a fact of life
and a sign that layering is not the rigid set of rules it might have
first appeared to be.

QUIC implements fast connection establishment with encryption and
authentication in the first RTT. It provides a connection identifier
that persists across changes in the underlying network, so a
connection can migrate from from one network to another. It supports
the multiplexing of several streams onto a single transport
connection, to avoid the head-of-line blocking that may arise when a
single packet is dropped while other useful data continues to
arrive. And it preserves (and in some ways improves on) the congestion
avoidance properties of TCP.

HTTP went through a number of versions in an effort to map its requirements more cleanly onto
the capabilities of TCP as we noted in Chapter |Apps|. With the arrival of QUIC, HTTP/3 is now able
to leverage a transport layer that was explicitly designed to meet the
application requirements of the Web. And in meeting the requirements
for web traffic, QUIC provides a better match for RPC as well.

QUIC is a noteworthy development in the world of transport
protocols. Many of the limitations of TCP have been known for decades,
but QUIC represents one of the most successful efforts to date to
stake out a different point in the design space. Because QUIC was
inspired by experience with HTTP and the Web—which arose long after
TCP was well established in the Internet—it presents a fascinating
case study in the unforeseen consequences of layered designs and in
the evolution of the Internet.

|Message|.3.1 Rethinking Layering
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The first important design choice in QUIC from the perspective of
performance is that it doesn't treat the transport and security
handshakes as two distinct layers. Instead, QUIC has built a
cryptographic handshake based on TLS into the transport. This is
illustrated by :numref:`Figure %s <fig-quic-tls>`. As RFC 9001 puts it:


*Rather than a strict layering, these two protocols cooperate: QUIC
uses the TLS handshake; TLS uses the reliability, ordered delivery,
and record layer provided by QUIC.*

.. admonition:: Further Reading

   M. Thomson and S. Turner (Eds.). `Using TLS to Secure QUIC
   <https://www.rfc-editor.org/info/rfc9001>`__. RFC 9001,
   May 2021.

.. _fig-quic-tls:
.. figure:: message/figures/QUIC-TLS.png
   :width: 500px
   :align: center

   Protocol stacks compared. (a) HTTP over TLS over TCP. (b) HTTP and
   TLS Handshake over QUIC.

This rearrangement of layers takes a bit of work to understand. The
central idea is that QUIC has the ability to provide encryption and
authentication to the data it transmits once it has a set of keys to
work with. So the TLS handshake operates pretty much as it did over
TCP, but instead of wrapping up TLS handshake messages in the TLS
record layer before sending them out over TCP, we can send the TLS
handshake messages over QUIC directly. QUIC also provides the
reliability, congestion control, etc. that TCP provides. Once the TLS
handshake is complete, the keying material for the connection is
passed to QUIC, which now is able to encrypt and authenticate the data
that is sent by HTTP.

The most obvious practical impact of this is that the establishment of
a QUIC connection takes place at the same time as the transmission of TLS
handshake messages, rather than taking place prior to the TLS
handshake as with TCP. By the time the TLS handshake completes, the
two ends of the QUIC connection have all the state needed to transmit
data such as HTTP messages. Furthermore, in the cases where 0-RTT data
can be sent (because there are shared secrets cached from a
previous connection), the first HTTP request can actually be sent at
the same time as the client Hello message. In this case, the sending of a
request takes place in parallel with the
establishment of a secure connection, enabling requests to be sent in
the first round trip. This is quite an improvement over the old approach of
TCP connection setup followed by TLS establishment followed by request.

|Message|.3.2 QUIC Packets and Frames
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

QUIC differs from TCP in dozens of ways, but beyond the incorporation
of TLS discussed above, the most important is the addition of streams
to a single connection. The *stream* is an ordered byte-stream
abstraction, but there is no ordering among streams. A single
connection is used for the purposes of packet loss detection, loss
recovery, and congestion control. The different streams within a
connection allow multiple requests to proceed in parallel without
waiting for others to complete. Establishment of a stream is
lightweight, with no handshake, making a stream a good match to a
single request and the response to it.

The QUIC header is visually simpler than that of TCP, but there is a
fair bit of complexity hidden by the fact that the flags indicate
different uses of the header. There are two different forms for the
header, long and short, as shown in
:numref:`Figure %s <fig-quic-header>`.
Note that, unlike many header formats, they do not generally
align on 32 bit boundaries.


.. _fig-quic-header:
.. figure:: message/figures/QUIC-headers.png
   :width: 500px
   :align: center

   QUIC packet headers. Long header (top) is used at connection establishment
   time and the short header (bottom) is used for subsequent packets.

The Source and Destination Connection IDs perform a function similar
to port numbers in TCP and UDP, with some important differences.
Firstly, a Connection ID completely identifies the connection, in
contrast to port numbers, which must be combined with the IP addresses
of the endpoints to uniquely identify a connection. This simplifies the
task of migrating a connection to another network, as described below.
The Connection IDs are variable length, hence the ``CID
length`` fields. In QUIC version 1, they may be up to 20 bytes
long. The client opening a connection picks its Source Connection ID,
which will be used by the server as the Destination Connection ID when
it replies. The server will also pick its own Source Connection ID and
send that to the client in its reply. Once the connection is
established, only Destination Connection ID is used to identify the
connection and the length is no longer required because the recipient
already knows the length of the expected ID. There is a
distinct connection ID for each direction.

Packet numbers are used for loss detection. In that sense they
resemble TCP's sequence numbers, but unlike sequence numbers, they
increase monotonically with every packet sent, even
retransmissions. They also count packets rather than bytes, and can be
as large as :math:`2^{62}-1`. Packet numbers must never be reused in a
connection, which means that if a connection stays open long enough,
the packet number space will be exhausted and the connection needs to
be reset. The number of practical situations where this would matter
are few and far between. Finally, the complete packet number does not
need to be carried in the packet header; it is sufficient to carry the
low order bits. The actual number of bits carried in the packet is
variable from 8 to 32 bits, indicated by some flags in the first byte
of the header. An implementation would choose this length to avoid two
packets with the same low order bits of their packet number (and
different high order bits) being in the network at the same time. This
allows for the non-reuse of packet numbers while keeping the
per-packet overhead low. We will return to the question of how QUIC
uses these numbers to handles packet losses below.

The ``Payload`` is made up of one or more QUIC *frames*. Frames come
in a variety of different types, indicated by the first byte in the
frame. The rest of the frame format varies according to its
type. There may be many frames in a packet, of difference types, allowing for multiple
streams to be multiplexed into the connection and making efficient use
of bandwidth.

Central to the operation of QUIC is the STREAM frame, which is used to
transmit data associated with a stream. The creation of streams is
simple: the sender inserts a stream frame into a
packet with a new Stream ID. A stream frame takes the form shown in :numref:`Figure %s
<fig-quic-frame>`.

.. _fig-quic-frame:
.. figure:: message/figures/QUIC-frame.png
   :width: 500px
   :align: center


   Format of a QUIC STREAM frame. The presence or absence of the
   Length and Offset fields is indicated by low order bits in the Type
   field.

The Frame ``Type`` for a STREAM frame can take one of eight values, from
0x08 to 0x0f. The low order three bits serve as flags to indicate if
an ``Offset`` is present, if a ``Length`` is present, and if this is the last
frame in a stream. ``Offset`` is zero for the first frame in a
stream and can me omitted. If ``Length`` is omitted, then the frame
extends to the end of the packet containing it.

One detail worth noting is that most of the fields here are variable
in length. TCP and IP both have a history of using fixed-length
fields that turned out to be too small as the network grew or got
faster. To allow for fields such as ``Stream ID`` and ``Length`` to be
quite large without forcing excessive per-packet overhead, QUIC has
adopted a form of variable length integer encoding as follows:

.. _tab-varint:
.. table:: Variable length integer encoding.
   :align: center
   :widths: auto

   +------+--------+-------------+-------------------+
   | 2MSB | Length | Usable Bits | Range             |
   +======+========+=============+===================+
   | 00   | 1      | 6           | 0–63              |
   +------+--------+-------------+-------------------+
   | 01   | 2      | 14          | :math:`0–2^{14}-1`|
   +------+--------+-------------+-------------------+
   | 10   | 4      | 30          | :math:`0–2^{30}-1`|
   +------+--------+-------------+-------------------+
   | 01   | 8      | 62          | :math:`0–2^{62}-1`|
   +------+--------+-------------+-------------------+


That is, the top two bits tell you how long the integer is, in a range
from one to 8 bytes. The remaining bits, which may be as few as 6 or
as many as 62, are used to carry the integer value. One thing to take
away from this design decision is that per-packet overhead matters,
especially for small packets, while planning for a long future of a
protocol requires plenty of room for fields to get larger than might
seem necessary at design time.

There are many other types of frames and you can refer to the QUIC RFC
9000 for details.  The ACK frame, as you would expect, allows for the
acknowledgement of received packets. We discuss its role in the next
section.

|Message|.3.3 Loss Detection, Recovery, and Congestion Control
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In many respects, QUIC builds on the best practices learned over
decades of development of TCP congestion control. The default
congestion control algorithm is TCP NewReno, which we covered in
Chapter |CC|. But there are some important differences in the details
which provide some incremental performance benefits over TCP.

As we noted above, QUIC never reuses a packet number, so a
retransmission to recover from loss will be clearly distinguished from
the original transmission. This improves the ability to estimate the RTT by
removing a source of noise in the RTT measurement data. Better RTT
estimation means it is easier to know reliably and quickly when a
packet has failed to arrive.

A second source of noise in RTT estimation is the use of delayed ACKs,
where a recipient waits some time before sending the ACK in response to
a received packet. To remove this uncertainty, QUIC includes an ``ACK
Delay`` in the ACK frame indicates how
much time elapsed between the receipt of the data being ACK'ed and the
ACK being transmitted.

The ACK frame in
QUIC builds on the idea of SACK (selective acknowledgments). A single
ACK frame can acknowledge a set of received packets even if there are
gaps in the sequence, and allows for a larger number of gaps than the
TCP SACK option. This is done using a set of ACK Ranges that describe
a set of packets received in sequence and the gap in packet number between this
sequence and the next set of packets received.

When an ACK frame acknowledges a packet that is sufficiently higher in
number than a packet that has not been acknowledged, that is an
indication that the earlier packet has been lost. This is similar to
the duplicate ACK-based loss detection of TCP, and so the threshold
for "sufficiently higher in number" defaults to 3. It may be set
higher to allow for more reordering of packets in the network.

Similarly, if a packet was sent long enough in the past, and a packet
sent after it has been acknowledged, the packet is declared lost. The
formula to determine when enough time has elapsed to declare packet
lost is as follows:

::

   max(kTimeThreshold x max(smoothed_rtt, latest_rtt), kGranularity)

``kGranularity`` is the local timer granularity, recommended to be
1ms; this sets a lower bound for the threshold. kTimeThreshold is an
empirically determined constant, recommended to be 9/8. This means
that a sender will wait slightly more than the estimated RTT before
declaring a packet lost, or 1ms if the RTT is less than that. The idea
of using ``max(smoothed_rtt, latest_rtt)`` is that a sudden drop in
RTT measurement will be ignored, while a sudden rise in RTT will
causes us to wait longer before declaring a packet loss. The idea is
that an anomalously short RTT should not immediately causes us to
aggressively declare packets lost, while a sudden rise in RTT might
actually mean there is congestion and we should wait longer for
packets to arrive before declaring them lost.

There is also a *probe timeout* (PTO) to detect the case where a packet is
lost and there are not enough subsequent ACKs to cause the process
above to trigger. In this case, the timeout tells the sender to
transmit an ACK-eliciting packet, called a Probe packet. This packet may contain data that is
ready to send, data sent previously but not acknowledged, or, if there
is nothing better to send, a PING frame. The sending of additional data
might temporarily cause the sender to exceed its congestion window,
which is considered an acceptable step to get ACKs flowing again.

The probe timeout is calculated as
follows:

::

   PTO = smoothed_rtt + max(4 x rttvar, kGranularity) + max_ack_delay

where ``rttvar`` is the variance in measured RTT and ``max_ack_delay``
is the most amount of time that a receiver can delay its ACK.

Now that we know how losses are detected, what does the sender do when
a loss occurs? It doesn't retransmit the packet that was
lost. Instead, the sender maintains a copy of all the data that has
been transmitted in packets that have not yet been acknowledged. Once
a loss is detected, the sender retransmits anything that was contained
in the lost packet. How it places that data into packets is largely up
to the sender, although it will usually prioritize the retransmission
of data that was lost over sending new data, unless the application
indicates otherwise.

Packet losses also trigger congestion control actions that are very
similar to those in TCP and the NewReno variant specifically. A QUIC
connection begins in slow start, moves into recovery phase upon a
packet loss, then moves into congestion avoidance when a packet sent
in the recovery period is acknowledged. The details are similar to the
description of NewReno in Chapter |CC|.

|Message|.3.4 Connection Migration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The process by which QUIC allows for connections to migrate from one
network path to another warrants a little discussion. Compared to TCP,
which uses source and destination addresses and ports to distinguish
connections, QUIC is much simpler: a connection is uniquely identified
by a Connection ID. There is one connection ID in each direction for a
bidirectional connection, and it is chosen by the destination, so it
can ensure that these IDs are unique across all connections. So, at
first glance, you might think that if a client wants to migrate a
connection from one network to another (such as switching from Wi-Fi
to a cellular network) it would "just work". Keep on using the same
connection ID on the new network, with a new source address and UDP
port, and the sender could reply to the new address.

The problem with the approach just described is that it opens up a
number of security issues. For example, a denial of service attack
could be launched by a malicious client providing the address of a target as the
new source IP address, and a small request from the client might
elicit a large response from the server, thus magnifying the scale of
the attack. Another attack is on-path address spoofing: an attacker
who sees the traffic on a connection can send a packet with a spoofed
source address that is otherwise a copy of a legitimate packet. If
this packet arrives before the original, it will appear to be a
connection migration.

The way QUIC protects against these migration attacks is to require
*path validation* when connections are migrated. The client does
simply start using a new source address with the old
connection ID. The server then sends a
PATH_CHALLENGE frame to the new address. The frame contains a chunk of
random data. The recipient of the challenge responds with the same
set of data. This proves that the new path works in both directions
and that the responding client has the cryptographic material
necessary to communicate with the server, thus ruling out the
possibility of the spoofing attacks above.


This overview of QUIC necessarily leaves out a fair number of
details. The protocol specification spans four RFCs and many hundreds
of pages. For a good overview of the design philosophy and benefits of
QUIC we recommend the SIGCOMM paper from 2017. The main specification
is RFC 9000 which links to the companion RFCs that cover topics such
as the interactions with TLS and congestion control.

.. _reading_quic:
.. admonition::  Further Reading

   A. Langley et al.
   `The QUIC Transport Protocol: Design and Internet-Scale Deployment
   <https://doi.org/10.1145/3098822.3098842>`__.
   ACM SIGCOMM '17 Symposium, August 2017.

   J. Iyengar and M. Thomson (Eds.). `QUIC: A UDP-Based Multiplexed and
      Secure Transport
      <https://www.rfc-editor.org/info/rfc9000>`__. RFC 9000, May 2021.
