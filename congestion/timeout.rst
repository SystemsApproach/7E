|CC|.2 Timeout Calculation
--------------------------

Timeouts and retransmissions are a central part TCP's approach to
implementing a reliable byte-stream, but timeouts also play a key role
in congestion control because they signal packet loss, which in turn
indicates the likelihood of congestion. In other words, TCP's timeout
mechanism is a building block for its overall approach to congestion
control.

Note that a timeout can happen because a packet was lost, or
because the corresponding acknowledgment was lost, or because nothing
was lost but the ACK took longer to arrive than we were
expecting. Hence it is important to know how long it might take an ACK
to arrive, because otherwise we risk responding as if there was
congestion when there was not.

TCP has an adaptive approach to setting a timeout, computed as a
function of the measured RTT. As simple as this sounds, the full
implementation is more involved than you might expect, and has
been through multiple refinements over the years. This section
revisits that experience.

|CC|.2.1 Original Algorithm
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

We begin with the simple algorithm that was originally described in
the TCP specification.  The idea is to keep a running average of the
RTT and then to compute the timeout as a function of this RTT.
Specifically, every time TCP sends a data segment, it records the
time. When an ACK for that segment arrives, TCP reads the time again,
and then takes the difference between these two times as a
``SampleRTT``. TCP then computes an ``EstimatedRTT`` as a weighted
average between the previous estimate and this new sample. That is,

.. math::

   \mathsf{EstimatedRTT} = \alpha \times \mathsf{EstimatedRTT} + (1 - \alpha{}) \times \mathsf{SampleRTT}

The parameter :math:`\alpha` is selected to *smooth* the
``EstimatedRTT``. A small :math:`\alpha` tracks changes in the RTT but is
perhaps too heavily influenced by temporary fluctuations. On the other
hand, a large :math:`\alpha` is more stable but perhaps not quick enough to
adapt to real changes. The original TCP specification recommended a
setting of :math:`\alpha` between 0.8 and 0.9. TCP then uses
``EstimatedRTT`` to compute the timeout in a rather conservative way:

.. math::

   \mathsf{TimeOut = 2} \times \mathsf{EstimatedRTT}

|CC|.2.2 Karn/Partridge Algorithm
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

After several years, a rather obvious flaw was discovered in this
simple approach: An ACK does not really acknowledge a transmission,
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
.. figure:: congestion/figures/f05-10-9780123850591.png
   :width: 500px
   :align: center

   Associating the ACK with (a) original transmission
   versus (b) retransmission.

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
aggressively timing out and then retransmitting.  We will see this
idea of exponential backoff again, embodied in a much
more sophisticated mechanism, in a later section.

|CC|.2.3 Jacobson/Karels Algorithm
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The Karn/Partridge algorithm was an improvement to RTT estimation, but it did not
eliminate congestion. The 1988 congestion-control mechanism proposed
by Jacobson and Karels includes (along with several other components) a new way to decide when to time out
and retransmit a segment.

The main problem with the original computation is that it does not
take the variance of the sample RTTs into account. Intuitively, if the
variation among samples is small, then the ``EstimatedRTT`` can be
better trusted and there is no reason for multiplying this estimate by
2 to compute the timeout. On the other hand, a large variance in the
samples suggests that the timeout value should not be too tightly
coupled to the ``EstimatedRTT``.

In the new approach, the sender measures a new ``SampleRTT`` as before.
It then folds this new sample into the timeout calculation as follows:

.. math:: \mathsf{Difference = SampleRTT - EstimatedRTT}

.. math:: \mathsf{EstimatedRTT = EstimatedRTT} + ( \delta \times \mathsf{Difference)}

.. math:: \mathsf{Deviation = Deviation} + \delta \mathsf{(| Difference | - Deviation)}

where :math:`\delta` is between 0 and 1. That is, we calculate both
the weighted moving average of the RTT and the weighted moving average of its
variation. TCP then computes the
timeout value as a function of both ``EstimatedRTT`` and ``Deviation``
as follows:

.. math:: \mathsf{TimeOut} = \mu \times \mathsf{EstimatedRTT} + \phi \times \mathsf{Deviation}

where based on experience, :math:`\mu` is typically set to 1 and :math:`\phi` is
set to 4.  Thus, when the variance is small, ``TimeOut`` is close to
``EstimatedRTT``; a large variance causes the ``Deviation`` term to
dominate the calculation.

|CC|.2.4 Implementation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

There are two items of note regarding the implementation of timeouts
in TCP. The first is that it is possible to implement the calculation
for ``EstimatedRTT`` and ``Deviation`` without using floating-point
arithmetic. Instead, the whole calculation is scaled by 2\ :sup:`n`,
with :math:`\delta` selected to be 1/2\ :sup:`n`. This allows us to do
integer arithmetic, implementing multiplication and division using
shifts, thereby achieving higher performance. The resulting
calculation is given by the following code fragment, where n=3 (i.e.,
:math:`\delta` = 1/8). Note that ``EstimatedRTT`` and ``Deviation`` are
stored in their scaled-up forms, while the value of ``SampleRTT`` at
the start of the code and of ``TimeOut`` at the end are real, unscaled
values. If you find the code hard to follow, you might want to try
plugging some real numbers into it and verifying that it gives the
same results as the equations above.

.. literalinclude:: code/timeout.c

The second is that the algorithm is only as good as the clock used to
read the current time. On typical Unix implementations at the time,
the clock granularity was as large as 500 ms, which is significantly
larger than the average cross-country RTT of somewhere between 100 and
200 ms. To make matters worse, the Unix implementation of TCP only
checked to see if a timeout should happen every time this 500-ms clock
ticked and would only take a sample of the round-trip time once per
RTT. The combination of these two factors could mean that a timeout
would happen 1 second after the segment was transmitted. An extension
to TCP, described in the next section, makes this RTT calculation a
bit more precise.

For additional details about the implementation of timeouts in TCP, we
refer the reader to the authoritative RFC:

.. _reading_timeout:
.. admonition::  Further Reading

   `RFC 6298: Computing TCP's Retransmission Timer
   <https://tools.ietf.org/html/rfc6298>`__. June 2011.

|CC|.2.5 TCP Timestamp Extension
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The changes to TCP described up to this point have been adjustments to
how the sender computes timeouts, with no changes to the over-the-wire
protocol. But there are also extensions to the TCP header that help
improve its ability to manage timeouts and retransmissions. We discuss
one that relates to RTT estimation here. Another extension, establishing a scaling factor for
``AdvertisedWindow``, was described in Section 2.3., and a third,
selective acknowledgment or SACK is discussed below.

The TCP timestamp extension helps to improve TCP’s timeout mechanism. Instead of
measuring the RTT using a coarse-grained event, TCP can read the actual
system clock when it is about to send a segment, and put this time—think
of it as a 32-bit *timestamp*\ —in the segment’s header. The receiver then
echoes this timestamp back to the sender in its acknowledgment, and the
sender subtracts this timestamp from the current time to measure the
RTT. In essence, the timestamp option provides a convenient place for
TCP to store the record of when a segment was transmitted; it stores the
time in the segment itself. Note that the endpoints in the connection do
not need synchronized clocks, since the timestamp is written and read at
the same end of the connection. This improves the measurement of RTT
and hence reduces the risk of incorrect timeouts due to poor RTT estimates.

This timestamp extension serves a second purpose, in that it also
provides a way to create a 64-bit sequence number field, addressing
the shortcomings of TCP's 32-bit sequence number outlined in Section 2.3.
Specifically, TCP decides whether to accept or reject a segment based
on a logical 64-bit identifier that has the ``SequenceNum`` field in
the low-order 32 bits and the timestamp in the high-order 32 bits.
Since the timestamp is always increasing, it serves to distinguish
between two different incarnations of the same sequence number. Note
that the timestamp is being used in this setting only to protect
against wraparound; it is not treated as part of the sequence number
for the purpose of ordering or acknowledging data.
