.. index:: DCTCP: Data Center TCP

|CC|.4  Domain-Specific Variants
-------------------------------------------

As exploration of the design space for congestion control has
continued, a number of new algorithms and protocols have emerged.
These differ from what we've seen earlier in this chapter mostly in
that they target specific use cases, rather than the arbitrarily
complex and heterogeneous network environments that TCP supports.

This section surveys two specific use cases: tuning TCP performance
for datacenters and accommodating mobile cellular networks with unique
radio-induced behavior. Additional domain-specific solutions are
described in a companion book.

.. _reading_tcp_variants:
.. admonition:: Further Reading

   L. Peterson, L. Brakmo, B. Davie. `TCP Congestion Control: A
   Systems Approach <https://tcpcc.systemsapproach.org>`__.
   May 2022.

|CC|.4.1 Datacenter Networks
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

There have been several efforts to optimize TCP for cloud datacenters,
where *Data Center TCP* was one of the first. There are several
aspects of the datacenter environment that warrant an approach that
differs from more traditional TCP. These include:

* Round trip time for intra-DC traffic are small;

* Buffers in datacenter switches are also typically small;

* All the switches are under common administrative control, and thus
  can be required to meet certain standards;

* A great deal of traffic has low latency requirements;

* That traffic competes with high bandwidth flows.

It should be noted that DCTCP is not just a version of TCP, but
rather, a system design that changes both the switch behavior and the
end host response to congestion information received from switches.

The central insight in DCTCP is that using loss as the main signal of
congestion in the datacenter environment is insufficient. By the time
a queue has built up enough to overflow, low latency traffic is
already failing to meet its deadlines, negatively impacting
performance. Thus DCTCP uses a version of ECN to provide an early
signal of congestion. But whereas the original design of ECN treated
an ECN marking much like a dropped packet, and cut the congestion
window in half, DCTCP takes a more finely-tuned approach. DCTCP tries
to estimate the fraction of bytes that are encountering congestion
rather than making the simple binary decision that congestion is
present. It then scales the congestion window based on this
estimate. The standard TCP algorithm still kicks in should a packet
actually be lost. The approach is designed to keep queues short by
reacting early to congestion while not over-reacting to the point that
they run empty and sacrifice throughput.

The key challenge in this approach is to estimate the fraction of bytes
encountering congestion. Each switch is simple. If a packet arrives and
the switch sees the queue length (K) is above some threshold; e.g.,

.. math:: \mathsf{K} > \mathsf{(RTT} \times \mathsf{C)\ /\ 7}

where C is the link rate in packets per second, then the switch sets the
CE bit in the IP header. The complexity of RED is not required.

The receiver then maintains a boolean variable for every flow, which
we’ll denote ``DCTCP.CE``, and sets it initially to false. When sending
an ACK, the receiver sets the ECE (Echo Congestion Experienced) flag
in the TCP header if and only if ``DCTCP.CE`` is true. It also
implements the following state machine in response to every received
packet:

-  If the CE bit is set and ``DCTCP.CE=False``, set ``DCTCP.CE`` to True and
   send an immediate ACK.

-  If the CE bit is not set and ``DCTCP.CE=True``, set ``DCTCP.CE`` to False
   and send an immediate ACK.

-  Otherwise, ignore the CE bit.

The non-obvious consequence of the “otherwise” case is that the
receiver continues to send delayed ACKs once every *n* packets, as
long as a stream of packets with a constant CE value is
received. Delayed ACKs have proven important to maintaining high
performance.

At the end of each observation window (a period usually chosen to be
approximately the RTT), the sender computes the fraction of bytes that
encountered congestion during that window as the ratio of the bytes
marked with CE to total bytes transmitted. DCTCP grows the congestion
window in exactly the same way as the standard algorithm, but it
reduces the window in proportion to how many bytes encountered
congestion during the last observation window.

Specifically, a new variable called ``DCTCP.Alpha`` is initialized to
1 and updated at the end of the observation window as follows:

.. math:: \mathsf{DCTCP.Alpha} = \mathsf{DCTCP.Alpha} \times
          \mathsf{(1 - g) + g} \times \mathsf{M}

``M`` is the faction of bytes marked, and ``g`` is the estimation
gain, a constant (set by the implementation) that determines how
rapidly ``DCTCP.Alpha`` changes in response to marking of
packets. When there is sustained congestion, ``DCTCP.Alpha``
approaches 1, and when there is sustained lack of congestion,
``DCTCP.Alpha`` decays to zero. This causes gentle reaction to newly
arrived congestion and more severe reaction to sustained congestion,
as the congestion window is calculated as follows:

.. math:: \mathsf{CongestionWindow} = \mathsf{CongestionWindow} \times \mathsf{(1 - DCTCP.Alpha\ /\ 2)}

To summarize, CE marking to indicate incipient congestion happens
early and often, but the reaction to such marking is more measured
than in standard TCP, to avoid the over-reaction that would lead to
queues running empty.

The paper that lays out all the arguments for DCTCP including a study
of the datacenter traffic characteristics that motivated its design is
a "test of time" award winner from SIGCOMM. A more recent NSDI paper
revisits DCTCP, and reports on operational experience using it in
datacenters.

.. _reading_dctcp:
.. admonition::  Further Reading

   M. Alizadeh et al. `Data
   Center TCP (DCTCP)
   <https://dl.acm.org/doi/10.1145/1851182.1851192>`__.
   ACM SIGCOMM '10 Symposium, August 2010.

   A. Dhamija et al. `A Large-Scale Deployment of DCTCP
   <https://www.usenix.org/system/files/nsdi24-dhamija.pdf>`__.
   Usenix NSDI '24 Symposium, April 2024.

There has been considerable research since DCTCP to optimize TCP for
datacenters, with the general approach being to introduce ever-more
sophisticated signals from the network that the sender can use to
manage congestion. We conclude our discussion of this use case by
elaborating on one of the most recent efforts, On-Ramp, because it
focuses instead on the fundamental tension that all congestion control
algorithms face: The trade-off between reaching equilibrium for
long-lived flows versus dealing with transient bursts. On-Ramp adopts
a modular design that directly addresses this tension, and does so
without depending on additional feedback from the network.

The main insight is that when a congestion control algorithm in
equilibrium encounters severe congestion and drastically cuts its
window (or rate), it must decide whether or not to remember its
previous equilibrium state. This is a difficult choice because it
depends on the duration of congestion, which is hard to predict. If
the congestion is transient, the algorithm should remember its
previous state so it can rapidly restore the old equilibrium without
under-utilizing the network once the burst ends. If the congestion is
sustained, for example due to the arrival of one or more new flows,
the algorithm should forget its previous state so that it can rapidly
find a new equilibrium.

.. _fig-onramp:
.. figure:: congestion/figures/Slide13.png
   :width: 350px
   :align: center

   On-Ramp paces packet transmission to avoid in-network queues due to
   bursty traffic, complementing the traditional congestion control
   algorithm's effort to maintain long-term stability and fairness.

The idea is to break the congestion control mechanism into two parts,
with each focused on just one aspect of the equilibrium/transient
trade-off. Specifically, On-Ramp is implemented as a “shim” that sits
below a conventional TCP congestion control algorithm, as shown in
:numref:`Figure %s <fig-onramp>`. The On-Ramp shim deals with bursts
(which temporarily fill network queues) by trying to quickly reduce
queuing delays whenever the measured *One-Way Delay (OWD)* grows too
large. It does this by temporarily holding packets at the sender
(rather than letting them occupy an in-network buffer) whenever OWD is
greater than some threshold. The On-Ramp shim is then composed with an
existing congestion control algorithm, which continues to work towards
reaching equilibrium for long-term flows.  On-Ramp has been shown to
work with several existing congestion control algorithms, including
DCTCP.

The key is that On-Ramp is designed so the two control decisions run
independently, on their own timescale. But to work, the shim needs to
accurately measure OWD, which in turn depends on synchronized clocks
between the sender and receiver. Since datacenter delays can be less
than a few tens of microseconds, the sender and receiver clocks must
be synchronized to within a few microseconds. Such high-accuracy clock
synchronization has traditionally required hardware-intensive
protocols, but On-Ramp leverages a new approach that takes advantage
of the network effect in a mesh of cooperating nodes to achieve
nanosecond-level clock synchronization. It does so without special
hardware, making On-Ramp easy to deploy.

.. _reading_onramp:
.. admonition::  Further Reading

   S. Liu et al. `Breaking the Transience-Equilibrium Nexus: A New
   Approach to Datacenter Packet Transport
   <https://www.usenix.org/system/files/nsdi21-liu.pdf>`__.
   Usenix NSDI '21 Symposium, April 2021.

   Y. Geng et al. `Exploiting a Natural Network Effect for Scalable,
   Fine-grained Clock Synchronization
   <https://www.usenix.org/system/files/conference/nsdi18/nsdi18-geng.pdf>`__.
   Usenix NSDI '18 Symposium, April 2018.

|CC|.4.2 Mobile Cellular Networks
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A second use case that continues to attract attention from the
research community is the interplay between congestion control and the
mobile cellular network.  Historically, the TCP/IP Internet and the
mobile cellular network evolved independently, with the latter serving
as the "last mile" for end-to-end TCP connections since the
introduction of broadband service with 3G. Today, the mobile network
plays a significant role in providing Internet connectivity, putting
increased focus on how it impacts congestion control.

While a mobile wireless connection could be viewed as no different
than any other hop along an end-to-end path through the Internet, for
historical reasons it has been treated as a special case, with the
end-to-end path logically divided into the two segments depicted in
:numref:`Figure %s <fig-mobile>`: the wired segment through the
Internet and the wireless last-hop over the Radio Access Network
(RAN). This "special case" perspective is warranted because (1) the
wireless link is typically the bottleneck due to the scarcity of radio
spectrum; (2) the bandwidth available in the RAN can be highly
variable due to a combination of device mobility and radio
interference; and (3) the number of devices being served by a given
base station fluctuates as devices move from one cell to another.

.. _fig-mobile:
.. figure:: congestion/figures/Slide12.png
   :width: 500px
   :align: center

   End-to-end path that includes a last-hop wireless link, where the
   base station buffers packets awaiting transmission over the Radio
   Access Network (RAN).

Although the internals of the RAN are largely closed and proprietary,
researchers have experimentally observed that there is significant
buffering at the edge, presumably to absorb the expected contention
for the radio link, and yet keep sufficient work "close by" for
whenever capacity does open up. As noted by Haiqing Jiang and
colleagues in their 2012 CellNet workshop paper, this large buffer is
problematic for TCP congestion control because it causes the sender to
overshoot the actual bandwidth available on the radio link, and in the
process, introduces significant delay and jitter. This is another
example of the bufferbloat problem identified in Section |Capacity|.1.3.

The Jiang paper suggests possible solutions, and generally observes
that delay-based approaches like Vegas outperform loss-based
approaches like Reno or CUBIC. This result is consistent with research
on a variant of TCP Vegas, known as TCP Westwood (TCPW), dating back
to the early 2000s. TCPW was motivated primarily by the realization
that packet loss is not always a reliable indicator of congestion.
This is particularly noticeable with wireless links, and so TCPW
tries to determine the bottleneck bandwidth by looking at the rate at
which ACKs are coming back for those packets that were delivered
successfully.

The other aspect of cellular networks that makes them a novel
challenge for TCP congestion control is that the bandwidth of a link
is not constant, but instead varies as a function of the
signal-to-noise ratio experienced by each receiver. As noted by the
BBR authors, the (currently opaque) scheduler for this wireless link
can use the number of queued packets for a given client as an input to
its scheduling algorithm, and hence the "reward" for building up a
queue can be an increase in bandwidth provided by the scheduler. BBR
attempts to address this in its design by ensuring that it is
aggressive enough to queue at least some packets in the buffers of
wireless links.

Yet another approach, as exemplified by research by Xie, Yi, and
Jamieson, suggests such that modifying the receiver to provide
end-device feedback may be effective. The idea is to have the receiver
explicitly tell the sender how much bandwidth is available on the last
hop, with the sender then having to judge whether the last-hop or some
other point along the Internet segment is the actual bottleneck.

.. _reading_ran:
.. admonition::  Further Reading

   S. Mascolo et al..
   `TCP Westwood: Bandwidth Estimation for Enhanced Transport over Wireless
   Links <https://dl.acm.org/doi/10.1145/381677.381704>`__,
   ACM Mobicom '01 Symposium, July 2001.

   H. Jiang, Z. Liu, Y. Wang, K. Lee and I. Rhee.
   `Understanding Bufferbloat in Cellular Networks
   <https://dl.acm.org/doi/10.1145/2342468.2342470>`__
   ACM SIGCOMM Workshop on Cellular Networks, August 2012.

   Y. Xie, F. Yi, and K. Jamieson. `PBE-CC: Congestion Control via
   Endpoint-Centric, Physical-Layer Bandwidth Measurements
   <https://dl.acm.org/doi/10.1145/3387514.3405880>`__. ACM SIGCOMM
   '20 Symposium, August 2020.


