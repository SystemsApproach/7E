

5.2.9 Performance
-----------------

Recall that Chapter 1 introduced the two quantitative metrics by which
network performance is evaluated: latency and throughput. As mentioned
in that discussion, these metrics are influenced not only by the
underlying hardware (e.g., propagation delay and link bandwidth) but
also by software overheads. Now that we have a complete software-based
protocol graph available to us that includes alternative transport
protocols, we can discuss how to meaningfully measure its performance.
The importance of such measurements is that they represent the
performance seen by application programs.

.. _fig-experiment:
.. figure:: figures/f05-11-9780123850591.png
   :width: 500px
   :align: center

   Measured system: Two Linux workstations and a pair of
   Gbps Ethernet links.

We begin, as any report of experimental results should, by describing
our experimental method. This includes the apparatus used in the
experiments; in this case, each workstation has a pair of dual CPU
2.4-GHz Xeon processors running Linux. In order to enable speeds above
1 Gbps, a pair of Ethernet adaptors (labeled NIC, for network
interface card) are used on each machine. The Ethernet spans a single
machine room so propagation is not an issue, making this a measure of
processor/software overheads. A test program running on top of the
socket interface simply tries to transfer data as quickly as possible
from one machine to the other. :numref:`Figure %s <fig-experiment>`
illustrates the setup.

You may notice that this experimental setup is not especially bleeding
edge in terms of the hardware or link speed. The point of this section
is not to show how fast a particular protocol can run, but to illustrate
the general methodology for measuring and reporting protocol
performance.

The throughput test is performed for a variety of message sizes using
a standard benchmarking tool called TTCP. The results of the
throughput test are given in :numref:`Figure %s <fig-xput>`. The key
thing to notice in this graph is that throughput improves as the
messages get larger. This makes sense—each message involves a certain
amount of overhead, so a larger message means that this overhead is
amortized over more bytes. The throughput curve flattens off above
1 KB, at which point the per-message overhead becomes insignificant
when compared to the large number of bytes that the protocol stack has
to process.

.. _fig-xput:
.. figure:: figures/f05-12-9780123850591.png
   :width: 400px
   :align: center

   Measured throughput using TCP, for various message
   sizes.

It’s worth noting that the maximum throughput is less than 2 Gbps, the
available link speed in this setup. Further testing and analysis of
results would be needed to figure out where the bottleneck is (or if
there is more than one). For example, looking at CPU load might give an
indication of whether the CPU is the bottleneck or whether memory
bandwidth, adaptor performance, or some other issue is to blame.

We also note that the network in this test is basically “perfect.” It
has almost no delay or loss, so the only factors affecting performance
are the TCP implementation and the workstation hardware and software. By
contrast, most of the time we deal with networks that are far from
perfect, notably our bandwidth-constrained, last-mile links and
loss-prone wireless links. Before we can fully appreciate how these
links affect TCP performance, we need to understand how TCP deals with
*congestion*, which is the topic of the next chapter.

At various times in the history of networking, the steadily increasing
speed of network links has threatened to run ahead of what could be
delivered to applications. For example, a large research effort was
begun in the United States in 1989 to build “gigabit networks,” where
the goal was not only to build links and switches that could run at
1Gbps or higher but also to deliver that throughput all the way to a
single application process. There were some real problems (e.g., network
adaptors, workstation architectures, and operating systems all had to be
designed with network-to-application throughput in mind) and also some
perceived problems that turned out to be not so serious. High on the
list of such problems was the concern that existing transport protocols,
TCP in particular, might not be up to the challenge of gigabit
operation.

As it turns out, TCP has done well keeping up with the increasing
demands of high-speed networks and applications. One of the most
important factors was the introduction of window scaling to deal with
larger bandwidth-delay products. However, there is often a big
difference between the theoretical performance of TCP and what is
achieved in practice. Relatively simple problems like copying the data
more times than necessary as it passes from network adaptor to
application can drive down performance, as can insufficient buffer
memory when the bandwidth-delay product is large. And the dynamics of
TCP are complex enough (as will become even more apparent in the next
chapter) that subtle interactions among network behavior, application
behavior, and the TCP protocol itself can dramatically alter
performance.

For our purposes, it’s worth noting that TCP continues to perform very
well as network speeds increase, and when it runs up against some limit
(normally related to congestion, increasing bandwidth-delay products, or
both), researchers rush in to find solutions. We’ve seen some of those
in this chapter, and we’ll see some more in the next.

5.2.10 Alternative Design Choices (SCTP, QUIC)
------------------------------------------------

Although TCP has proven to be a robust protocol that satisfies the needs
of a wide range of applications, the design space for transport
protocols is quite large. TCP is by no means the only valid point in
that design space. We conclude our discussion of TCP by considering
alternative design choices. While we offer an explanation for why TCP’s
designers made the choices they did, we observe that other protocols
exist that have made other choices, and more such protocols may appear
in the future.

First, we have suggested from the very first chapter of this book that
there are at least two interesting classes of transport protocols:
stream-oriented protocols like TCP and request/reply protocols like RPC.
In other words, we have implicitly divided the design space in half and
placed TCP squarely in the stream-oriented half of the world. We could
further divide the stream-oriented protocols into two groups—reliable
and unreliable—with the former containing TCP and the latter being more
suitable for interactive video applications that would rather drop a
frame than incur the delay associated with a retransmission.

This exercise in building a transport protocol taxonomy is interesting
and could be continued in greater and greater detail, but the world
isn’t as black and white as we might like. Consider the suitability of
TCP as a transport protocol for request/reply applications, for example.
TCP is a full-duplex protocol, so it would be easy to open a TCP
connection between the client and server, send the request message in
one direction, and send the reply message in the other direction. There
are two complications, however. The first is that TCP is a
*byte*-oriented protocol rather than a *message*-oriented protocol, and
request/reply applications always deal with messages. (We explore the
issue of bytes versus messages in greater detail in a moment.) The
second complication is that in those situations where both the request
message and the reply message fit in a single network packet, a
well-designed request/reply protocol needs only two packets to implement
the exchange, whereas TCP would need at least nine: three to establish
the connection, two for the message exchange, and four to tear down the
connection. Of course, if the request or reply messages are large enough
to require multiple network packets (e.g., it might take 100 packets to
send a 100,000-byte reply message), then the overhead of setting up and
tearing down the connection is inconsequential. In other words, it isn’t
always the case that a particular protocol cannot support a certain
functionality; it’s sometimes the case that one design is more efficient
than another under particular circumstances.

Second, as just suggested, you might question why TCP chose to provide a
reliable *byte*-stream service rather than a reliable *message*-stream
service; messages would be the natural choice for a database application
that wants to exchange records. There are two answers to this question.
The first is that a message-oriented protocol must, by definition,
establish an upper bound on message sizes. After all, an infinitely long
message is a byte stream. For any message size that a protocol selects,
there will be applications that want to send larger messages, rendering
the transport protocol useless and forcing the application to implement
its own transport-like services. The second reason is that, while
message-oriented protocols are definitely more appropriate for
applications that want to send records to each other, you can easily
insert record boundaries into a byte stream to implement this
functionality.

A third decision made in the design of TCP is that it delivers bytes
*in order* to the application. This means that it may hold onto bytes
that were received out of order from the network, awaiting some
missing bytes to fill a hole. This is enormously helpful for many
applications but turns out to be quite unhelpful if the application is
capable of processing data out of order. As a simple example, a Web
page containing multiple embedded objects doesn’t need all the objects
to be delivered in order before starting to display the page. In fact,
there is a class of applications that would prefer to handle
out-of-order data at the application layer, in return for getting data
sooner when packets are dropped or misordered within the network.  The
desire to support such applications led to the creation of not one but
two IETF standard transport protocols. The first of these was SCTP,
the *Stream Control Transmission Protocol*. SCTP provides a partially
ordered delivery service, rather than the strictly ordered service of
TCP.  (SCTP also makes some other design decisions that differ from
TCP, including message orientation and support of multiple IP
addresses for a single session.) More recently, the IETF has been
standardizing a protocol optimized for Web traffic, known as
QUIC. More on QUIC in a moment.

Fourth, TCP chose to implement explicit setup/teardown phases, but
this is not required. In the case of connection setup, it would be
possible to send all necessary connection parameters along with the
first data message. TCP elected to take a more conservative approach
that gives the receiver the opportunity to reject the connection
before any data arrives. In the case of teardown, we could quietly
close a connection that has been inactive for a long period of time,
but this would complicate applications like remote login that want to
keep a connection alive for weeks at a time; such applications would
be forced to send out-of-band “keep alive” messages to keep the
connection state at the other end from disappearing.

Finally, TCP is a window-based protocol, but this is not the only
possibility. The alternative is a *rate-based* design, in which the
receiver tells the sender the rate—expressed in either bytes or packets
per second—at which it is willing to accept incoming data. For example,
the receiver might inform the sender that it can accommodate 100 packets
a second. There is an interesting duality between windows and rate,
since the number of packets (bytes) in the window, divided by the RTT,
is exactly the rate. For example, a window size of 10 packets and a
100-ms RTT implies that the sender is allowed to transmit at a rate of
100 packets a second. It is by increasing or decreasing the advertised
window size that the receiver is effectively raising or lowering the
rate at which the sender can transmit. In TCP, this information is fed
back to the sender in the ``AdvertisedWindow`` field of the ACK for
every segment. One of the key issues in a rate-based protocol is how
often the desired rate—which may change over time—is relayed back to the
source: Is it for every packet, once per RTT, or only when the rate
changes? While we have just now considered window versus rate in the
context of flow control, it is an even more hotly contested issue in the
context of congestion control, which we will discuss in the next
chapter.

.. sidebar:: Multipath TCP

          It isn't always necessary to define a new protocol if you
          find an existing protocol does not adequately serve a
          particular use case. Sometimes it's possible to make
          substantial changes in how an existing protocol is
          implemented, yet remain true to the original spec.
          Multipath TCP is an example of such a situation.

          The idea of Multipath TCP is to steer packets over
          multiple paths through the Internet, for example, by
          using two different IP addresses for one of the
          end-points.  This can be especially helpful when
          delivering data to a mobile device that is connected to
          both Wi-Fi and the cellular network (and hence, has two
          unique IP addresses). Being wireless, both networks can
          experience significant packet-loss, so being able to use
          both to carry packets can dramatically improve the user
          experience.  The key is for the receiving side of TCP to
          reconstruct the original, in-order byte stream before
          passing data up to the application, which remains unaware
          it is sitting on top of Multipath TCP. (This is in
          contrast to applications that purposely open two or more
          TCP connections to get better performance.)

          As simple as Multipath TCP sounds, it is incredibly
          difficult to get right because it breaks many assumptions
          about how TCP flow control, in-order segment reassembly,
          and congestion control are implemented. We leave it as an
          exercise for the reader to explore these subtleties. Doing
          so is a great way to make sure your basic understanding
          of TCP is sound.
