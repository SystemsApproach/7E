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
widespread deployment. As discussed further in Chapter 9, QUIC was
motivated in large part by the challenges of matching the
request/response semantics of HTTP to the stream-oriented nature of
TCP.  These issues have become more noticeable over time, due to
factors such as the rise of high-latency wireless networks, the
availability of multiple networks for a single device (e.g., Wi-Fi and
cellular), and the increasing use of encrypted, authenticated
connections on the Web (as discussed in Chapter 8).  While a full
description of QUIC is beyond our scope, some of the key design
decisions are worth discussing.

If network latency is high—say 100 milliseconds or more—then a few
RTTs can quickly add up to a visible annoyance for an end
user. Establishing an HTTP session over TCP with Transport Layer
Security (Section 8.5) would typically take at least three round trips
(one for TCP session establishment and two for setting up the
encryption parameters) before the first HTTP message could be
sent. The designers of QUIC recognized that this delay—the direct
result of a layered approach to protocol design—could be dramatically
reduced if connection setup and the required security handshakes were
combined and optimized for minimal round trips.

Note also how the presence of multiple network interfaces might affect
the design. If your mobile phone loses its Wi-Fi connection and needs
to switch to a cellular connection, that would typically require both
a TCP timeout on one connection and a new series of handshakes on the
other. Making the connection something that can persist over different
network layer connections was another design goal for QUIC.

Finally, as noted above, the reliable byte stream model for TCP is a
poor match to a Web page request, when many objects need to be fetched
and page rendering could begin before they have all arrived. While one
workaround for this would be to open multiple TCP connections in
parallel, this approach (which was used in the early days of the Web)
has its own set of drawbacks, notably on congestion control (see
Chapter 6). Specifically, each connection runs its own congestion
control loop, so the experience of congestion on one connection is not
apparent to the other connections, and each connection tries to figure
out the appropriate amount of bandwidth to consume on its own. QUIC
introduced the idea of streams within a connection, so that objects
could be delivered out-of-order while maintaining a holistic view of
congestion across all the streams.

Interestingly, by the time QUIC emerged, many design decisions had
been made that presented challenges for the deployment of a new
transport protocol. Notably, many "middleboxes'' such as NATs and
firewalls (see Section 8.5) have enough understanding of the existing
widespread transport protocols (TCP and UDP) that they can't be relied
upon to pass a new transport protocol. As a result, QUIC actually
rides on top of UDP. In other words, it is a transport protocol
running on top of a transport protocol. This is not as uncommon as our
focus on layering might suggest, as the next two subsections also
illustrate. This choice was made in the interests of making QUIC
deployable on the Internet as it exists, and has been quite
successful.

QUIC implements fast connection establishment with encryption and
authentication in the first RTT. It provides a connection identifier
that persists across changes in the underlying network. It supports
the multiplexing of several streams onto a single transport
connection, to avoid the head-of-line blocking that may arise when a
single packet is dropped while other useful data continues to
arrive. And it preserves (and in some ways improves on) the congestion
avoidance properties of TCP, an important aspect of transport
protocols that we return to in Chapter 6.

HTTP went through a number of versions (1.0, 1.1, 2.0, discussed in
Section 9.1) in an effort to map its requirements more cleanly onto
the capabilities of TCP. With the arrival of QUIC, HTTP/3 is now able
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

.. Cut-and-pasted from Applications chapter in 6E

As the preceding discussion illustrates, the history of HTTP has
included a series of incremental changes to make better use of TCP as
the underlying transport. But there is a fundamental issue that can't
be fully resolved: TCP provides a byte-stream abstraction, while HTTP
is a request/response protocol. The natural solution would be to adopt
a more suitable transport, but as we saw in Chapter 5, there is no
standard RPC protocol that enjoys the widespread acceptance of TCP.

Ultimately, the solution to this mismatch was to create a new
transport protocol in QUIC. QUIC was explicitly designed to provide a
good match to the requirements of HTTP, and HTTP/3 takes advantage of
the improved underlying transport. For example, QUIC explicitly
supports stream multiplexing at the transport layer. Thus, a single
packet loss only impacts the delivery of the stream that suffered the
loss, rather than causing a stall in the entire TCP connection while
waiting for that lost packet to be retransmitted. At the same time,
that lost packet provides a congestion signal that is applied to all
streams in the QUIC connection. We cover QUIC in more detail in
Section 5.2.

Another significant advantage of QUIC compared to TCP is the way it
handles the steps required to secure an HTTP connection. Whereas the
exchange of certificates and encryption keys follows the establishment
of a TCP session, QUIC handles these steps as part of session
establishment, leading to a considerable reduction in the number of
round-trips needed to establish a secure connection before the first
content is delivered.

HTTP/3 is implemented in the majority of browsers and is incrementally
being deployed on servers across the Internet. There remain plenty of
servers running HTTP/2 and even some HTTP/1.1 as well, so version
negotiation is likely to be part of HTTP implementations for the
foreseeable future.
