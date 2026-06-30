.. index:: RoCE: RDMA over Converged Ethernet
.. index:: IPU: Infrastructure Processing Unit
.. index:: DCQCN: Datacenter Quantized Congestion Notification
.. index:: CNP: Congestion Notification Packet

|Message|.5 Optimizing Ethernet for RDMA
------------------------------------------

The adoption of RDMA as the preferred transport protocol for HPC
workloads initially happened on InfiniBand, which is a self-contained
networking technology, developed in parallel with the Internet
architecture.  InfiniBand achieved the desired performance because
it (1) off-loaded the transport protocol logic to the NIC (HCA),
thereby bypassing the overhead of the OS-hosted TCP/IP protocol stack;
and (2) augmented packet switches with the queue management logic
needed to avoid the buffering delays and packet loss associated with
best-effort forwarding.

But the Internet did not stop evolving in 1999 (even though you will
still see both arguments in favor of InfiniBand being made even
today). For example, it is now commonplace for Ethernet adaptors to
support per-application transmit/receive packet queues, making it
possible to get messages into and out of user space without any OS
involvement. There have also been many "SmartNIC" products over the
years that offload various aspects of TCP/IP to the NIC, with today's
*Infrastructure Processing Units (IPUs)* replacing traditional NICs as the
preferred technology for connecting servers to datacenter networks.
The first InfiniBand advantage no longer holds, although it is
perfectly legitimate to argue that TCP is not the right abstraction
for RDMA (similar to the argument that QUIC is better suited to RPC
traffic than TCP).

The main advantage InfiniBand continues to enjoy is its ability to
avoid the performance hit of congestion in an Ethernet-based
packet-switched network. It does this with two mechanisms.  First, it
implements per-hop flow control: an upstream node (either an end-host
or a switch) is not allowed to transmit a packet to a downstream node
(host or switch) unless the downstream node has issued a "credit"
saying it has buffer space to hold that packet. Credits are logically
equivalent to advertising an open flow control window in TCP, the main
difference being TCP does flow control on an end-to-end basis, whereas
InfiniBand does it on a per-hop basis.

The second mechanism is support for multiple "virtual lanes", with
each switch supporting a separate queue for each lane. (You can think
of a lane as similar to a virtual circuit.) Flow control credits are
issued on a per-lane basis (ensuring each queue avoids having to drop
a packet), and the set of queues are serviced in priority order.
InfiniBand defines 16 lanes, and hence, 16 priority queues. Each
end-to-end connection is assigned to one of the lanes according to the
QoS parameters associated with that connection.  This is an indirect
way of making a per-connection reservation (the weighted fair queuing
algorithm described in Chapter |Capacity| would be more direct), but it does
provide more isolation between user flows than the single FIFO queue
in a standard Ethernet switch.

There are two related limitations to this approach. The first is that
InfiniBand networks are limited in scale. You cannot make the kinds
of guarantees InfiniBand makes when you are trying to connect billions
of edge devices. They do scale to support modest-sized datacenters,
but at increased cost. Cost is InfiniBand's second limitation, which
is related to the fact that, unlike Ethernet, InfiniBand is not ubiquitous;
it is purpose-built for the RDMA use case.

This is where the continued evolution of Internet technology again
provides an answer. It is possible to augment an Ethernet switch with
Internet-based alternatives to InfiniBand's flow control and packet
scheduling algorithm. They are not identical mechanisms, but do result
in rough equivalency. This realization has led to an effort called
*Converged Ethernet (CE)*, which has resulted in two versions of a
standard known as *RoCE: RDMA over Converged Ethernet*.

The first version, *RoCE.v1* encapsulates InfiniBand packets in
Ethernet packets. In this case, the network is limited to an L2
broadcast domain. The second version, *RoCE.v2*, encapsulates
InfiniBand packets in UDP datagrams, meaning the full routing
capability of IP can be leveraged to interconnect nodes. Datacenters
typically interconnect racks of servers at the IP layer, so we focus
on v2 in the following. :numref:`Figure %s <fig-ib-encapsulate>` shows
the RoCE.v2 packet format. Note that native InfiniBand packets also
have network and link layer headers, but they are not needed in RoCE.v2
since IP subsumes the packet-delivery function; only InfiniBand's
transport header, plus the message payload, needs to be encapsulated.

.. _fig-ib-encapsulate:
.. figure:: message/figures/ib-encapsulate.png
   :width: 500px
   :align: center

   The InfiniBand transport header and payload are encapsulated in a
   UDP datagram.

The approach to augmenting Ethernet so it better emulates InfiniBand
has two main parts. The first is to replicate InfiniBand's flow
control mechanism by adding support for Priority Flow Control to
Ethernet switches. This has been standardized as IEEE 802.1Qbb.  The
second is to take advantage of the existing Explicit Congestion
Notification (ECN) mechanism described in Chapter |Capacity|. This is
appealing for cloud providers because their datacenters already
leverage ECN in support of TCP congestion control.  The two mechanisms
work in concert, with ECN pacing the source host as a first response
to congestion, and PFC serving as a fall-back.

Note that this is all still an active area of research, with the
following papers giving some insight into the issues being studied.
We draw particular attention to the first paper, by Zhu and
colleagues, which spells out an approach that is in wide use today.
The algorithm, known as *DCQCN (Datacenter Quantized Congestion
Notification)*, is similar to the DCTCP algorithm described in Section
|CC|.4.1, but adapted to be rate-based rather than window-based.
(Recall that TCP is window-based, so all congestion-induced
adjustments are to the congestion window, which then indirectly
impacts the sending rate.) DCQCN still uses the same
additive-increase, multiplicative-decrease heuristic as TCP, so that
part of the algorithm is the same. The main difference is in the
details of how the receiver notifies the sender that it needs to slow
down. (Recall that the actual ``CE`` notice is in the IP packet that
arrives at the receiver; we need a transport protocol mechanism to
relay that information back to the sender.)

For DCTCP, the TCP receiver "echos" every notification back to the
sender by setting the ``ECE`` bit in the TCP header. The sender adapts
the window size for each ``ECE`` it sees, and tells the receiver it
has done so by sending a ``CWR`` notice back. For DCQCN, RoCE.v2
defines an analogous transport-level notice, denoted ``CNP``
*(Congestion Notice Packet)*, but the DCQCN receiver limits the rate
at which it sends ``CNP`` notices back to the sender. Instead of
sending a ``CNP`` for every ``CE`` nonfiction that arrives, it sends
no more than one ``CNP`` every 50 μs.\ [#]_ This setting is an
engineering choice designed to reduce overhead, where the important
point is that the sender knows adjusts its sending rate in proportion
to the rate at which ``CNP`` notices arrive (or don't arrive).

.. [#] That the DCQCN receiver thins out the feedback information
         explains the "Quantized" qualifier in the name.

.. admonition:: Further Reading

   Y. Zhu et al. `Congestion Control for Large-Scale RDMA
   Deployments <https://dl.acm.org/doi/10.1145/2829988.2787484>`__.
   ACM SIGCOMM '15 Symposium, August 2015.

   C. Gao et al. `RDMA over Commodity Ethernet at Scale
   <https://dl.acm.org/doi/10.1145/2934872.2934908>`__.
   ACM SIGCOMM '16 Symposium, August 2016.

   Q. Li et al. `RDMA over Heterogeneous NICS
   <https://www.usenix.org/system/files/osdi23-li-qiang.pdf>`__.
   Usenix OSDI '23 Symposium, July 2023.

   A. Singhvi et al. `Falcon: A Reliable, Low Latency Hardware
   Transport <https://dl.acm.org/doi/10.1145/3718958.3754353>`__.
   ACM SIGCOMM '25 Symposium, August 2025.

.. _fig-soft-roce:
.. figure:: message/figures/soft-roce.png
   :width: 200px
   :align: center

   Software configuration of a RoCE-capable edge server, with the
   InfiniBand transport layer implemented in software and running in
   user space (as part of the application).

Finally, there is the question of where the RDMA transport function is
implemented for RoCE: (1) in hardware on the NIC, or (2) in software
running on the host. There are multiple commercial NICs that support
RoCE, with the RDMA functionalilty implemented there. We note,
however, that a software implementation is a standard part of the
Linux kernel, so that is an option for experimenting with RDMA. This
configuration is shown in :numref:`Figure %s <fig-soft-roce>`.

.. sidebar:: Information Processing Units

   Tell the IPU/DPU story. Focus on datacenters wanting to separate
   "sellable" cycles from "infrastructure" cycles. Explain how that is
   not the same as e2e message latency, as cycles are still required
   in both cases. Relate to larger history of SmartNICs.
