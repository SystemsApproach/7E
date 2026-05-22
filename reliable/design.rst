|TCP|.1  Design Issues
------------------------------

To appreciate the design challenge, we need to first refine our
definition of the abstraction, beyond the reliable, in-order delivery
of a stream of bytes. It is a full-duplex protocol, meaning that each
TCP connection supports a pair of byte streams, one flowing in each
direction. It also includes a flow-control mechanism for each of these
byte streams that allows the receiver to pace how much data the sender
is allowed to transmit at a given time. This mechanism blocks the
sender if it gets too far ahead of the receiver.  Finally, TCP
supports a demultiplexing mechanism that allows multiple application
processes on any given host to simultaneously carry on a conversation
with their peers.

With that background, we might ask why a byte stream is the right
abstraction for building applications (the reliable part is pretty
easy to justify). That answer is that a byte stream is a simple
concept, familiar to any programmer who has written code to read or
write data from a file. That doesn't rule out other possibilities.
Just like picking the right software tool or programming language for
a task, a case can be made other abstractions for various use cases.
We'll see examples in later chapters. But once we settle on a byte
stream as a reasonable abstraction, the next set of design issues
revolve around how to efficiently implement that abstraction on a
best-effort packet delivery network. This reveals a set of challenges,
primarily centered around how to achieve both reliability and high
performance at the same time.

The most straightforward way to achieve reliability works as follows:
after transmitting a packet, the sender waits for an *acknowledgment*
before transmitting the next packet.  If the acknowledgment does not
arrive after a certain period of time, the sender times out and
retransmits the original packet. This strategy is commonly known as
the *stop-and-wait* algorithm.

.. _fig-ack-timeout:
.. figure:: reliable/figures/f02-17-9780123850591.png
   :width: 500px
   :align: center

   Timeline showing four different scenarios for the
   stop-and-wait algorithm. (a) The ACK is received before the timer
   expires; (b) the original packet is lost; (c) the ACK is lost;
   (d) the timeout fires too soon.

:numref:`Figure %s <fig-ack-timeout>` illustrates timelines for four
different scenarios that result from this basic algorithm. The sending
side is represented on the left, the receiving side is depicted on the
right, and time flows from top to bottom. :numref:`Figure %s(a)
<fig-ack-timeout>` shows the situation in which the ACK is received
before the timer expires; (b) and (c) show the situation in which the
original packet and the ACK, respectively, are lost; and (d) shows the
situation in which the timeout fires too soon.

There is one important subtlety in the stop-and-wait
algorithm. Suppose the sender sends a packet and the receiver
acknowledges it, but the acknowledgment is either lost or delayed in
arriving. This situation is illustrated in timelines (c) and (d) of
:numref:`Figure %s <fig-ack-timeout>`. In both cases, the sender times
out and retransmits the original packet, but the receiver will think
that it is the next packet, since it correctly received and
acknowledged the first packet. This has the potential to cause
duplicate copies of a packet to be delivered. To address this problem,
the header for a stop-and-wait protocol usually includes a 1-bit
sequence number—that is, the sequence number can take on the values 0
and 1—and the sequence numbers used for each packet alternate, as
illustrated in :numref:`Figure %s <fig-stop-wait>`. Thus, when the
sender retransmits packet 0, the receiver can determine that it is
seeing a second copy of packet 0 rather than the first copy of packet
1 and therefore can ignore it (the receiver still acknowledges it, in
case the first ACK was lost).

.. _fig-stop-wait:
.. figure:: reliable/figures/stop-wait.png
   :width: 250px
   :align: center

   Timeline for stop-and-wait with 1-bit sequence
   number.

The main shortcoming of the stop-and-wait algorithm is that it allows
the sender to have only one outstanding packet in transit at a time,
and this may be far below the network's capacity. Consider the
following simple example: a 1.5-Mbps end-to-end path with a 45-ms
round-trip time. This path has a delay × bandwidth product of 67.5 Kb,
or approximately 8 KB. Since the sender can send only one packet per
RTT, if we assume a packet size of 1 KB, this implies a maximum
sending rate of

.. centered:: Bits-Per-Packet / Time-Per-Packet = 1024 x 8 / 0.045 = 182 kbps

or about one-eighth of the network’s capacity. We’d like the sender to
be able to transmit up to eight packets before having to wait for an
acknowledgment. The significance of the delay × bandwidth product is
that it represents the amount of data that could be in transit. We
would like to be able to send this much data without waiting for the
first acknowledgment. The principle at work here is often referred to
as *keeping the pipe full*.

Still assuming an 8 KB delay × bandwidth product and a 1 KB packet
size, we would like the sender to be ready to transmit the ninth
packet at pretty much the same moment that the ACK for the first
packet arrives. The algorithm that allows us to do this is called
*sliding window*, and an illustrative timeline is given in
:numref:`Figure %s <fig-slide-win>`.

.. _fig-slide-win:
.. figure:: reliable/figures/slide-win.png
   :width: 250px
   :align: center

   Timeline for the sliding window algorithm.

In this scenario, packets will need at least a 3-bit sequence number,
(supporting the range 0..7) so that when we're ready to transmit the
ninth packet, we can reuse sequence number 0. Or so it would seem. It
turns out that in the worst case, we need a window size that is twice
the number of packets we hope to have in flight at any given time
(i.e., a 4-bit field supporting 16 sequence numbers in this example).
We leave it as an exercise for the reader to figure out why this is
the case, and do a deeper dive into the specific approach TCP uses in
Section |TCP|.4.

.. Not sure how to deal with additional detail from 6E. (Are there
   other examples?)

   If you are interested in a protocol-agnostic description 
   of the sliding window algorithm, we suggest:
   https://book.systemsapproach.org/direct/reliable.html#sliding-window

Unlike this simplified example, one of the more severe complications
TCP has to address is that it doesn't have a perfect knowledge of the
network RTT, and hence, it does not know how many packets should be in
flight (unacknowledged) in order to keep the pipe full.  For example, a
TCP connection between a host in San Francisco and a host in Boston,
which are separated by several thousand kilometers, might have an RTT
of 100 ms, while a TCP connection between two hosts in the same room,
only a few meters apart, might have an RTT of only 1 ms. The same TCP
protocol must be able to support both of these connections. To make
matters worse, the TCP connection between hosts in San Francisco and
Boston might have an RTT of 100 ms at 3 a.m., but an RTT of 500 ms at
3 p.m. Variations in the RTT are even possible during a single TCP
connection that lasts only a few minutes. What this means to the
sliding window algorithm is that the timeout mechanism that triggers
retransmissions must be adaptive.

A second complication is
that packets may be reordered as they cross the Internet. Packets that
are slightly out of order do not cause a problem; the sliding
window algorithm can reorder packets correctly using the sequence
number. The real issue is how far out of order packets can get or,
said another way, how late a packet can arrive at the destination. In
the worst case, a packet can be delayed in the Internet until the IP
time to live (``TTL``) field expires, at which time the packet is
discarded (and hence there is no danger of it arriving late). Of
course, TTL is a misnomer; it has for many years simply been number
decremented at every router hop. TCP, however, assumes
that each packet has a maximum lifetime. The exact lifetime, known as
the *maximum segment lifetime* (MSL), is an engineering choice. The
current recommended setting in the TCP RFC is 120 seconds (although
many modern implementations use a lower value). IP does
not directly enforce this 120-second value; it is simply a
conservative estimate that TCP makes of how long a packet might live
in the Internet. The implication is significant—TCP has to be prepared
for very old packets to suddenly show up at the receiver, potentially
confusing the sliding window algorithm.

There are other complications, but they are subtle, and come to light
only after you see TCP operating under real world conditions.  This is
exactly what happened: the original version of TCP addressed the
reliable-but-fast issue outlined above, but did not anticipate some of
the other issues we discuss below. In other words, you can read the
rest of this chapter as a case study of how protocols incrementally
evolve. One of those improvements—controlling congestion—is so
consequential that it warrants its own chapter.

