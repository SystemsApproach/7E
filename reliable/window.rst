11.4 Sliding Window
------------------------------

We are now ready to discuss TCP’s variant of the sliding window
algorithm, which serves several purposes: (1) it guarantees the reliable
delivery of data, (2) it ensures that data is delivered in order, and
(3) it enforces flow control between the sender and the
receiver. TCP’s use of the sliding window algorithm is similar to the
simple example in Section 11.1 in the case of the first two of these
three functions.  Where TCP differs from the simple example is that it
folds the flow-control function in as well. In particular, rather than
having a fixed-size sliding window, the receiver *advertises* a window
size to the sender. This is done using the ``AdvertisedWindow`` field
in the TCP header. The sender is then limited to having no more than
``AdvertisedWindow`` bytes of unacknowledged data at any given
time. The receiver selects a suitable value for ``AdvertisedWindow``
based on the amount of memory allocated to the connection for the
purpose of buffering data. The idea is to keep the sender from
over-running the receiver’s buffer. We discuss this at greater length
below.

11.4.1 Reliable and Ordered Delivery
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To see how the sending and receiving sides of TCP interact with each
other to implement reliable and ordered delivery, consider the
situation illustrated in :numref:`Figure %s <fig-tcp-fc>`. TCP on the
sending side maintains a send buffer. This buffer is used to store
data that has been sent but not yet acknowledged, as well as data that
has been written by the sending application but not transmitted. On
the receiving side, TCP maintains a receive buffer. This buffer holds
data that arrives out of order, as well as data that is in the correct
order (i.e., there are no missing bytes earlier in the stream) but
that the application process has not yet had the chance to read.

.. _fig-tcp-fc:
.. figure:: reliable/figures/f05-08-9780123850591.png
   :width: 500px
   :align: center

   Relationship between TCP send buffer (a) and receive
   buffer (b).

To make the following discussion simpler to follow, we initially ignore
the fact that both the buffers and the sequence numbers are of some
finite size and hence will eventually wrap around. Also, we do not
distinguish between a pointer into a buffer where a particular byte of
data is stored and the sequence number for that byte.

Looking first at the sending side, three pointers are maintained into
the send buffer, each with an obvious meaning: ``LastByteAcked``,
``LastByteSent``, and ``LastByteWritten``. Clearly,

::

   LastByteAcked <= LastByteSent

since the receiver cannot have acknowledged a byte that has not yet been
sent, and

::

   LastByteSent <= LastByteWritten

since TCP cannot send a byte that the application process has not yet
written. Also note that none of the bytes to the left of
``LastByteAcked`` need to be saved in the buffer because they have
already been acknowledged, and none of the bytes to the right of
``LastByteWritten`` need to be buffered because they have not yet been
generated.

A similar set of pointers (sequence numbers) are maintained on the
receiving side: ``LastByteRead``, ``NextByteExpected``, and
``LastByteRcvd``. The inequalities are a little less intuitive, however,
because of the problem of out-of-order delivery. The first relationship

::

   LastByteRead < NextByteExpected

is true because a byte cannot be read by the application until it is
received *and* all preceding bytes have also been received.
``NextByteExpected`` points to the byte immediately after the latest
byte to meet this criterion. Second,

::

   NextByteExpected <= LastByteRcvd + 1

since, if data has arrived in order, ``NextByteExpected`` points to the
byte after ``LastByteRcvd``, whereas if data has arrived out of order,
then ``NextByteExpected`` points to the start of the first gap in the
data, as in :numref:`Figure %s <fig-tcp-fc>`. Note that bytes to the left of
``LastByteRead`` need not be buffered because they have already been
read by the local application process, and bytes to the right of
``LastByteRcvd`` need not be buffered because they have not yet arrived.

11.4.2 Flow Control
~~~~~~~~~~~~~~~~~~~~~~~~

Most of the above discussion is similar to that found in the standard
sliding window algorithm.  In what follows, we focus on the fact that
both buffers are of some finite size, denoted ``MaxSendBuffer`` and
``MaxRcvBuffer``, although we don’t worry about the details of how
they are implemented. In other words, we are only interested in the
number of bytes being buffered, not in where those bytes are actually
stored.

Recall that in a sliding window protocol, the size of the window sets
the amount of data that can be sent without waiting for acknowledgment
from the receiver. Thus, the receiver throttles the sender by
advertising a window that is no larger than the amount of data that it
can buffer. Observe that TCP on the receive side must keep

::

   LastByteRcvd - LastByteRead <= MaxRcvBuffer

to avoid overflowing its buffer. It therefore advertises a window size
of

::

   AdvertisedWindow = MaxRcvBuffer - ((NextByteExpected - 1) - LastByteRead)

which represents the amount of free space remaining in its buffer. As
data arrives, the receiver acknowledges it as long as all the preceding
bytes have also arrived. In addition, ``LastByteRcvd`` moves to the
right (is incremented), meaning that the advertised window potentially
shrinks. Whether or not it shrinks depends on how fast the local
application process is consuming data. If the local process is reading
data just as fast as it arrives (causing ``LastByteRead`` to be
incremented at the same rate as ``LastByteRcvd``), then the advertised
window stays open (i.e., ``AdvertisedWindow = MaxRcvBuffer``). If,
however, the receiving process falls behind, perhaps because it performs
a very expensive operation on each byte of data that it reads, then the
advertised window grows smaller with every segment that arrives, until
it eventually goes to 0.

TCP on the send side must then adhere to the advertised window it gets
from the receiver. This means that at any given time, it must ensure
that

::

   LastByteSent - LastByteAcked <= AdvertisedWindow

Said another way, the sender computes an *effective* window that limits
how much data it can send:

::

   EffectiveWindow = AdvertisedWindow - (LastByteSent - LastByteAcked)

Clearly, ``EffectiveWindow`` must be greater than 0 before the source
can send more data. It is possible, therefore, that a segment arrives
acknowledging *x* bytes, thereby allowing the sender to increment
``LastByteAcked`` by *x*, but because the receiving process was not
reading any data, the advertised window is now *x* bytes smaller than the
time before. In such a situation, the sender would be able to free
buffer space, but not to send any more data.

All the while this is going on, the send side must also make sure that
the local application process does not overflow the send buffer—that is,

::

   LastByteWritten - LastByteAcked <= MaxSendBuffer

If the sending process tries to write *y* bytes to TCP, but

::

   (LastByteWritten - LastByteAcked) + y > MaxSendBuffer

then TCP blocks the sending process and does not allow it to generate
more data.

It is now possible to understand how a slow receiving process ultimately
stops a fast sending process. First, the receive buffer fills up, which
means the advertised window shrinks to 0. An advertised window of 0
means that the sending side cannot transmit any data, even though data
it has previously sent has been successfully acknowledged. Finally, not
being able to transmit any data means that the send buffer fills up,
which ultimately causes TCP to block the sending process. As soon as the
receiving process starts to read data again, the receive-side TCP is
able to open its window back up, which allows the send-side TCP to
transmit data out of its buffer. When this data is eventually
acknowledged, ``LastByteAcked`` is incremented, the buffer space holding
this acknowledged data becomes free, and the sending process is
unblocked and allowed to proceed.

There is only one remaining detail that must be resolved—how does the
sending side know that the advertised window is no longer 0? As
mentioned above, TCP *always* sends a segment in response to a received
data segment, and this response contains the latest values for the
``Acknowledge`` and ``AdvertisedWindow`` fields, even if these values
have not changed since the last time they were sent. The problem is
this. Once the receive side has advertised a window size of 0, the
sender is not permitted to send any more data, which means it has no way
to discover that the advertised window is no longer 0 at some time in
the future. TCP on the receive side does not spontaneously send nondata
segments; it only sends them in response to an arriving data segment.

TCP deals with this situation as follows. Whenever the other side
advertises a window size of 0, the sending side persists in sending a
segment with 1 byte of data every so often. It knows that this data will
probably not be accepted, but it tries anyway, because each of these
1-byte segments triggers a response that contains the current advertised
window. Eventually, one of these 1-byte probes triggers a response that
reports a nonzero advertised window.

Note that these 1-byte messages are called *Zero Window Probes* and in
practice they are sent every 5 to 60 seconds. As for what single byte of
data to send in the probe: it’s the next byte of actual data just
outside the window. (It has to be real data in case it’s accepted by the
receiver.)

Note that the reason the sending side periodically sends this probe
segment is that TCP is designed to make the receive side as simple as
possible—it simply responds to segments from the sender, and it never
initiates any activity on its own. This is an example of a
well-recognized (although not universally applied) protocol design
rule, which, for lack of a better name, we call the *smart sender/
dumb receiver* rule.


11.4.3 Protecting Against Wraparound
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This subsection and the next consider the size of the ``SequenceNum``
and ``AdvertisedWindow`` fields and the implications of their sizes on
TCP’s correctness and performance. TCP’s ``SequenceNum`` field is
32 bits long, and its ``AdvertisedWindow`` field is 16 bits long,
meaning that TCP has easily satisfied the requirement of the sliding
window algorithm that the sequence number space be twice as big as the
window size: 2\ :sup:`32` >> 2 × 2\ :sup:`16`. However, this
requirement is not the interesting thing about these two fields.
Consider each field in turn.

The relevance of the 32-bit sequence number space is that the sequence
number used on a given connection might wrap around—a byte with
sequence number S could be sent at one time, and then at a later time
a second byte with the same sequence number S might be sent. Once
again, we assume that packets cannot survive in the Internet for
longer than the recommended MSL. Thus, we currently need to make sure
that the sequence number does not wrap around within a 120-second
period of time. Whether or not this happens depends on how fast data
can be transmitted over the Internet—that is, how fast the 32-bit
sequence number space can be consumed. (This discussion assumes that
we are trying to consume the sequence number space as fast as
possible, but of course we will be if we are doing our job of keeping
the pipe full.) :numref:`Table %s <tab-eqnum>` shows how long it takes
for the sequence number to wrap around on networks with various
bandwidths.

.. _tab-eqnum:
.. table::  Time Until 32-Bit Sequence Number Space Wraps Around.
   :align: center
   :widths: auto

   +--------------------------+-----------------------+
   | Bandwidth                | Time until Wraparound |
   +==========================+=======================+
   | T1 (1.5 Mbps)            | 6.4 hours             |
   +--------------------------+-----------------------+
   | T3 (45 Mbps)             | 13 minutes            |
   +--------------------------+-----------------------+
   | Fast Ethernet (100 Mbps) | 6 minutes             |
   +--------------------------+-----------------------+
   | OC-3 (155 Mbps)          | 4 minutes             |
   +--------------------------+-----------------------+
   | OC-48 (2.5 Gbps)         | 14 seconds            |
   +--------------------------+-----------------------+
   | OC-192 (10 Gbps)         | 3 seconds             |
   +--------------------------+-----------------------+
   | 10GigE (10 Gbps)         | 3 seconds             |
   +--------------------------+-----------------------+

.. Should probably update this and the next table with at least one
   40Gbps entries.

As you can see, the 32-bit sequence number space is adequate at modest
bandwidths, but given that OC-768 (40 Gbps) links are now common in
the Internet backbone, and that most servers now come with 10Gig
Ethernet (or 10 Gbps) interfaces, we’re now well-past the point where
32 bits is too small. Fortunately, the IETF has worked out an
extension to TCP that effectively extends the sequence number space to
protect against the sequence number wrapping around. This and related
extensions are described in a later section.

11.4.5 Keeping the Pipe Full
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The relevance of the 16-bit ``AdvertisedWindow`` field is that it must
be big enough to allow the sender to keep the pipe full. Clearly, the
receiver is free to not open the window as large as the
``AdvertisedWindow`` field allows; we are interested in the situation in
which the receiver has enough buffer space to handle as much data as the
largest possible ``AdvertisedWindow`` allows.

In this case, it is not just the network bandwidth but the delay x
bandwidth product that dictates how big the ``AdvertisedWindow`` field
needs to be—the window needs to be opened far enough to allow a full
delay × bandwidth product’s worth of data to be transmitted. Assuming an
RTT of 100 ms (a typical number for a cross-country connection in the
United States), :numref:`Table %s <tab-adv-win>` gives the delay × bandwidth
product for several network technologies.

.. _tab-adv-win:
.. table::  Required Window Size for 100-ms RTT
   :align: center
   :widths: auto

   +--------------------------+---------------------------+
   | Bandwidth                | Delay × Bandwidth Product |
   +==========================+===========================+
   | T1 (1.5 Mbps)            | 18 KB                     |
   +--------------------------+---------------------------+
   | T3 (45 Mbps)             | 549 KB                    |
   +--------------------------+---------------------------+
   | Fast Ethernet (100 Mbps) | 1.2 MB                    |
   +--------------------------+---------------------------+
   | OC-3 (155 Mbps)          | 1.8 MB                    |
   +--------------------------+---------------------------+
   | OC-48 (2.5 Gbps)         | 29.6 MB                   |
   +--------------------------+---------------------------+
   | OC-192 (10 Gbps)         | 118.4 MB                  |
   +--------------------------+---------------------------+
   | 10GigE (10 Gbps)         | 118.4 MB                  |
   +--------------------------+---------------------------+

As you can see, TCP’s ``AdvertisedWindow`` field is in even worse shape
than its ``SequenceNum`` field—it is not big enough to handle even a T3
connection across the continental United States, since a 16-bit field
allows us to advertise a window of only 64 KB. The very same TCP
extension mentioned above provides a mechanism for effectively
increasing the size of the advertised window.
