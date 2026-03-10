.. _artifact-quic:

13.3 Optimizing RPC
-----------------------

RPC has long been a popular communicataion primative for building
distributed systems, with gRPC expanding on that history to support
scalable cloud services. But the designers of gRPC make a calculated
tradeoff, favoring the convenience of RPC over the known performance
limitations of running HTTP over TCP. Their experience building and
supporting gRPC, coupled with the broader Internet's experience
building RESTful applications directly on top of HTTP/TCP, led to
insights about how both HTTP and the the underlying transport protocol
could be improved.  These insights, in turn, led to the development of
new version of HTTP, known as HTTP/3, and a new transport protocol,
called QUIC.  The two protocols are co-designed to better support both
REST-based or RPC-based distributed/cloud systems. This section
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
Chapter 12).

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

Finally, as noted above, the reliable byte stream model for TCP is a
poor match to a web page request, when many objects need to be fetched
and page rendering could begin before they have all arrived. While one
workaround for this would be to open multiple TCP connections in
parallel, this approach (which was used in the early days of the web)
has its own set of drawbacks, notably on congestion
control. Specifically, each connection runs its own congestion control
loop, so the experience of congestion on one connection is not
apparent to the other connections, and each connection tries to figure
out the appropriate amount of bandwidth to consume on its own. QUIC
introduced the idea of streams within a connection, so that objects
could be delivered out-of-order while maintaining a holistic view of
congestion across all the streams.

By the time QUIC emerged, many design decisions had
been made that presented challenges for the deployment of a new
transport protocol. Notably, many "middleboxes'' such as NATs and
firewalls have enough understanding of the existing
widespread transport protocols (TCP and UDP) that they can't be relied
upon to pass a new transport protocol. As a result, QUIC actually
rides on top of UDP. In other words, it is a transport protocol
running on top of a transport protocol. As we noted in the previous
section, such "violations" of strict layering are both a fact of life
and a sign that layering is not the rigid set of rules it might have
first appeared to be. 

QUIC implements fast connection establishment with encryption and
authentication in the first RTT. It provides a connection identifier
that persists across changes in the underlying network. It supports
the multiplexing of several streams onto a single transport
connection, to avoid the head-of-line blocking that may arise when a
single packet is dropped while other useful data continues to
arrive. And it preserves (and in some ways improves on) the congestion
avoidance properties of TCP.

HTTP went through a number of versions in an effort to map its requirements more cleanly onto
the capabilities of TCP as we noted in Chapter 2. With the arrival of QUIC, HTTP/3 is now able
to leverage a transport layer that was explicitly designed to meet the
application requirements of the Web.

QUIC is a most interesting development in the world of transport
protocols. Many of the limitations of TCP have been known for decades,
but QUIC represents one of the most successful efforts to date to
stake out a different point in the design space. Because QUIC was
inspired by experience with HTTP and the Web—which arose long after
TCP was well established in the Internet—it presents a fascinating
case study in the unforeseen consequences of layered designs and in
the evolution of the Internet.



HTTP/3 is implemented in the majority of browsers and is incrementally
being deployed on servers across the Internet. There remain plenty of
servers running HTTP/2 and even some HTTP/1.1 as well, so version
negotiation is likely to be part of HTTP implementations for the
foreseeable future.
