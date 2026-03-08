.. _artifact-roce:

13.5 Optimizing Ethernet for RDMA
------------------------------------------

The adoption of RDMA as the preferred transport protocol for HPC
workloads initially happened on Infiniband, which is a self-contained
networking technology, developed in parallel with the Internet
architecture.  Infiniband achieved the desired performance because
it (1) off-loaded the transport protocol logic to the NIC (HCA),
thereby bypassing the overhead of the OS-hosted TCP/IP protocol stack;
and (2) augmented packet switches with the queue management logic
needed to avoid the buffering delays and packet loss associated with
best-effort forwarding.

But the Internet did not stop evolving in 1999 (even though you will
still see both arguments in favor of Infiniband still being made even
today). For example, it is now common place for Ethernet adaptors to
support per-application transmit/receive packet queues, making it
possible to get messages into and out of user space without any OS
involvement. There have also been many "SmartNIC" products over the
years that off-load various aspects of TCP/IP to the NIC, with today's
*Infrastructure Packet Units (IPUs)* replacing traditional NICs as the
preferred technology for connecting servers to datacenter networks.
The first Infiniband advantage no longer holds, although it is
perfectly legitimate to argue that TCP is not the right abstraction
for RDMA (similar to QUIC being preferred to TCP for RPC traffic).

The main advantage Infiniband continues to enjoy is its ability to
avoid the performance hit of congestion in an Ethernet-based
packet-switched network. It does this with two mechanisms.  First, it
implements per-hop flow control: an upstream node (either an end-host
or a switch) is not allowed to transmit a packet to a downstream node
(host or switch) unless the downstream node has issued a "credit"
saying it has buffer space to hold that packet. Credits are logically
equivalent to advertizing an open flow control window in TCP, the main
difference being TCP does flow control on an end-to-end basis, whereas
Infiniband does it on a per-hop basis.

The second mechanism is support for multiple "virtual lanes", with
each switch supporting a separate queue for each lane. (You can think
of a lane as similar to a virtual circuit.) Flow control credits are
issued on a per-lane basis (ensuring each queue avoids having to drop
a packet), and the set of queues are serviced in priority order.
Infiniband defines 16 lanes, and hence, 16 priority queues. Each
end-to-end connection is assigned to one of the lanes according to the
QoS parameters associated with that connection.  This is an indirect
way of making a per-connection reservation (the weighted fair queuing
algorithm described in Chapter X would be more direct), but is does
provide more isolation between user flows than the single FIFO queue
in a standard Ethernet switch.

There are two related limitations to this approach. The first is that
Infiniband networks are limited in scale. You cannot make the kinds
of guarantees Infiniband makes when you are trying to connect billions
of edge devices. They do scale to support modest-sized datacenters,
but at increased cost. Cost is Infiniband's second limitation, which
is related to the fact that, unlike Ethernet, Infiniband is not ubiquitous;
it is purpose-built for the RDMA use case.

This is where the continued evolution of Internet technology again
provides an answer. It is possible to augment an Ethernet switch with
Internet-based alternatives to Infiniband's flow control and packet
scheduling algorithm. They are not identical mechanisms, but do result
in rough equivalency. This realization has led to an effort called
*Converged Ethernet (CE)*, which has resulted in two versions of a
standard known as *RoCE: RDMA over Converged Ethernet*.

The first version, *RoCE.v1* encapsulates Infiniband packets in an
Ethernet packet. In this case, the network is limited to an L2
broadcast domain. The second version, *RoCE.v2*, encapsulates
Infiniband packets in a UDP datagram, meaning the full routing
capability of IP can be leveraged to interconnect nodes. Datacenters
typically interconnect racks of servers at the IP layer, so we focus
on v2 in the following. :numref:`Figure %s <fig-ib-encapsulate>` shows
the RoCE.v2 packet format. Note that native Infiniband packets also
have network and link layer headers, but they are not needed in RoCE.v2
since IP subsumes the packet-delivery function; only Infiniband's
transport header, plus the message payload, needs to be encapsulated.

.. _fig-ib-encapsulate:
.. figure:: message/figures/ib-encapsulate.png
   :width: 500px
   :align: center

   The Infiniband transport header and payload are encapsulated in a
   UDP datagram.

There are two main options for how Ethernet switches emulate
Infiniband behavior, although other approaches continue to be under
consideration. The first is to simply replicate the Infiniband
mechanism by adding support for Priority Flow Control to Ethernet.
This approach has been standardized as IEEE 802.1Qbb.  The second is
to take advantage of two existing mechanisms: the Explicit Congestion
Notification (ECN) described in Chapter X and the Differentiated
Services Code Point (DSCP) described in Chapter Y. Both are encoded in
IP's TOS field, and so require no change to standards. The approach
is also appealing to cloud providers because their datacenters already
leverage ECN in their approach to TCP congestion control.

.. _fig-soft-roce:
.. figure:: message/figures/soft-roce.png
   :width: 200px
   :align: center

   Software configuration of a RoCE-capable edge server, with the
   Infiniband transport layer implemented in software and running in
   user space (as part of the application).

Finally, there is the question of where the RDMA transport function is
implemented for RoCE: (1) in hardware on the NIC, or (2) in software
running on the host. While hardware offloading is possible (see the
sidebar on IPUs), a software implementation is a standard part of the
Linux kernel. The latter consumes host resources, but that does not
necessarily translate to worse performance. This configuration is
shown in :numref:`Figure %s <fig-soft-roce>`.


..  Infiband (role of fabric)
    https://network.nvidia.com/pdf/whitepapers/IB_Intro_WP_190.pdf
        https://cloudswit.ch/blogs/roce-or-infiniband-technical-comparison/#1-6-transport-layer

    Converged Ethernet
    https://dl.acm.org/doi/pdf/10.1145/3718958.3754353

    RDMA over Heterogeneous NICS
    https://www.usenix.org/system/files/osdi23-li-qiang.pdf

    RoCEv2 Header Encapsulation https://en.wikipedia.org/wiki/RDMA_over_Converged_Ethernet#/media/File:RoCE_Header_format.png

    A good overview of RoCE
    https://www.fs.com/blog/rdma-over-converged-ethernet-roce-guide-2208.html

    For a good overview of RoCE, see
    https://www.fs.com/blog/rdma-over-converged-ethernet-roce-guide-2208.html


.. sidebar:: Information Processing Units

   Tell the IPU/DPU story.
