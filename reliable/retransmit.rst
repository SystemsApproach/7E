|TCP|.6 Adaptive Retransmission
--------------------------------

Because TCP guarantees the reliable delivery of data, it retransmits
each segment if an ACK is not received in a certain period of time. TCP
sets this timeout as a function of the RTT it expects between the two
ends of the connection. Unfortunately, given the range of possible RTTs
between any pair of hosts in the Internet, as well as the variation in
RTT between the same two hosts over time, choosing an appropriate
timeout value is not that easy. To address this problem, TCP uses an
adaptive retransmission mechanism.

It is important to set the timeout as accurately as possible for
several reasons. If the timeout is too long, then the sender is likely
to sit idly waiting for the timeout, unable to send while awaiting an
ACK that never arrives. This leads to a loss of throughput, as TCP
will fail to keep the pipe full. Conversely, if the timeout is too
short, then it will falsely indicate that a segment has been lost when
in fact the acknowledgment just hasn't arrived yet. This leads to
needless retransmissions, wasting network resources and potentially
leading to congestion. Furthermore, as we discuss in the next chapter,
a false indication of loss can be interpreted as a signal that
congestion has *already* occurred, which undermines the effectiveness
of congestion control algorithms.


Given then importance of getting timeouts right, there has been a
wealth of work to make them accurate. We now describe the timeout mechanism and
how it has evolved over time as the Internet community has gained more
experience using TCP.

|TCP|.6.1 Original Algorithm
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

We begin with a simple algorithm for computing a timeout value between a
pair of hosts. This is the algorithm that was originally described in
the TCP specification—and the following description presents it in those
terms—but it could be used by any end-to-end protocol.

The idea is to keep a running average of the RTT and then to compute
the timeout as a function of this RTT. Specifically, every time TCP
sends a data segment, it records the time. When an ACK for that
segment arrives, TCP reads the time again, and then takes the
difference between these two times as a ``SampleRTT``. TCP then
computes an ``EstimatedRTT`` as a weighted average between the
previous estimate and this new sample. That is,

::

   EstimatedRTT = alpha x EstimatedRTT + (1 - alpha) x SampleRTT

The parameter ``alpha`` is selected to *smooth* the
``EstimatedRTT``. A small ``alpha`` tracks changes in the RTT but is
perhaps too heavily influenced by temporary fluctuations. On the other
hand, a large ``alpha`` is more stable but perhaps not quick enough to
adapt to real changes. The original TCP specification recommended a
setting of ``alpha`` between 0.8 and 0.9. TCP then uses
``EstimatedRTT`` to compute the timeout in a rather conservative way:

::

   TimeOut = 2 x EstimatedRTT

|TCP|.6.2 Karn/Partridge Algorithm
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

After several years, a rather obvious flaw was discovered in this
simple approach: an ACK does not really acknowledge a transmission,
but rather, it acknowledges the receipt of data. In other words,
whenever a segment is retransmitted and then an ACK arrives at the
sender, it is impossible to determine if this ACK should be associated
with the first or the second transmission of the segment for the
purpose of measuring the sample RTT.

It is necessary to know which transmission to associate it with so as
to compute an accurate ``SampleRTT``. As illustrated in
:numref:`Figure %s <fig-tcp-karn>`, if you assume that the ACK is for
the original transmission but it was really for the second, then the
``SampleRTT`` is too large (a); if you assume that the ACK is for the
second transmission but it was actually for the first, then the
``SampleRTT`` is too small (b).

.. _fig-tcp-karn:
.. figure:: reliable/figures/f05-10-9780123850591.png
   :width: 500px
   :align: center

   Associating the ACK with (a) original transmission
   versus (b) retransmission.


The solution at first looks surprisingly simple. It is known as the
Karn/Partridge algorithm, after its inventors. With this algorithm,
whenever TCP retransmits a segment, it stops taking samples of the
RTT; it only measures ``SampleRTT`` for segments that have been sent
only once.  But the algorithm also includes a second change to TCP’s
timeout mechanism. Each time TCP retransmits, it sets the next timeout
to be twice the last timeout, rather than basing it on the last
``EstimatedRTT``. That is, Karn and Partridge proposed that timeout
calculation use exponential backoff. The motivation for using
exponential backoff is that timeouts cause retransmission, and
retransmitted segments are no longer contributing to an update in the
RTT estimate. So the idea is to be more cautious in declaring that a
packet has been lost, rather than getting into a possible cycle of
aggressively timing out and then retransmitting.


Note that the root cause of the issue here, the lack of distinction
between the first and subsequent transmissions, is inherent to the
design of TCP. The TCP Timestamp option, described below, provides one
solution to that problem. In the newer protocol QUIC, which we cover
in Chapter |Message|, the problem is solved by avoiding reuse of
sequence numbers entirely.

|TCP|.6.3 Jacobson/Karels Algorithm
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The Karn/Partridge algorithm was introduced at a time when the Internet
was suffering from high levels of network congestion. Their approach was
designed to fix some of the causes of that congestion, but, although it
was an improvement, the congestion was not eliminated. The following
year (1988), two other researchers—Jacobson and Karels—proposed a more
drastic set of changes to TCP to battle congestion. The bulk of those
changes are described in the next chapter. Here, we focus on the aspect of
that proposal that is related to deciding when to time out and
retransmit a segment.

As noted above, inaccurate timeouts can both induce congestion through
needless retransmissions, and interfere with the operation of
congestion control algorithms of the sort described in the next
chapter. Also note that there is nothing about the Jacobson/Karels
timeout computation that is specific to TCP. It could be used by any
end-to-end protocol.

The main problem with the original computation is that it does not take
the variance of the sample RTTs into account. Intuitively, if the
variation among samples is small, then the ``EstimatedRTT`` can be
better trusted and there is no reason for multiplying this estimate by 2
to compute the timeout. On the other hand, a large variance in the
samples suggests that the timeout value should not be too tightly
coupled to the ``EstimatedRTT``.

In the new approach, the sender measures a new ``SampleRTT`` as before.
It then folds this new sample into the timeout calculation as follows:

::

   Difference = SampleRTT - EstimatedRTT
   EstimatedRTT = EstimatedRTT + ( delta x Difference)
   Deviation = Deviation + delta (|Difference| - Deviation)

where ``delta`` is between 0 and 1. That is, we calculate both the
mean RTT and the variation in that mean.

TCP then computes the timeout value as a function of both
``EstimatedRTT`` and ``Deviation`` as follows:

::

   TimeOut = mu x EstimatedRTT + phi x Deviation

where based on experience, ``mu`` is typically set to 1 and ``phi`` is
set to 4.  Thus, when the variance is small, ``TimeOut`` is close to
``EstimatedRTT``; a large variance causes the ``Deviation`` term to
dominate the calculation.

|TCP|.6.4 Implementation
~~~~~~~~~~~~~~~~~~~~~~~~~

There are two items of note regarding the implementation of timeouts
in TCP. The first is that it is possible to implement the calculation
for ``EstimatedRTT`` and ``Deviation`` without using floating-point
arithmetic. Instead, the whole calculation is scaled by 2\ :sup:`n`,
with delta selected to be 1/2\ :sup:`n`. This allows us to do integer
arithmetic, implementing multiplication and division using shifts,
thereby achieving higher performance. The resulting calculation is
given by the following code fragment, where n=3 (i.e., ``delta =
1/8``). Note that ``EstimatedRTT`` and ``Deviation`` are stored in
their scaled-up forms, while the value of ``SampleRTT`` at the start
of the code and of ``TimeOut`` at the end are real, unscaled
values. If you find the code hard to follow, you might want to try
plugging some real numbers into it and verifying that it gives the
same results as the equations above.

::

   {
       SampleRTT -= (EstimatedRTT >> 3);
       EstimatedRTT += SampleRTT;
       if (SampleRTT < 0)
           SampleRTT = -SampleRTT;
       SampleRTT -= (Deviation >> 3);
       Deviation += SampleRTT;
       TimeOut = (EstimatedRTT >> 3) + (Deviation >> 1);
   }

The second point of note is that the Jacobson/Karels algorithm is only
as good as the clock used to read the current time. On typical Unix
implementations at the time, the clock granularity was as large as 500
ms, which is significantly larger than the average cross-country RTT
of somewhere between 100 and 200 ms. To make matters worse, the Unix
implementation of TCP only checked to see if a timeout should happen
every time this 500-ms clock ticked and would only take a sample of
the round-trip time once per RTT. The combination of these two factors
could mean that a timeout would happen 1 second after the segment was
transmitted.  A *timestamp* extension to TCP, described in Section
|TCP|.8, makes this RTT calculation a bit more precise.  For
additional details about the implementation of timeouts in TCP, we
refer the reader to the authoritative RFC.

.. _reading_timeout:
.. admonition::  Further Reading

   V. Paxson, M. Allman, J. Chu, and M. Sargent. `Computing TCP's
   Retransmission Timer <https://www.rfc-editor.org/info/rfc6298>`__.
   RFC 6298, June 2011.
