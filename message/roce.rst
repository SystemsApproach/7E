.. _artifact-roce:

13.5 Generalizing RDMA
--------------------------------

Shooting for a best of both worlds: Performance of RDMA and the
ubiquity of Ethernet networks. (Do we focus on generalizing RDMA or
optimizing Ethernet, or both.)

IB supports RC "natively"; edge logic is in the NIC. Go-Back-N and
Selective Repeat (SR). Notes from the white paper below.

* IB is a complete network, from layer 1 up.

 * At the link layer, you set up "virtual lanes" (think virtual
   circuits). Closest we've seen is SONET, but many differences: up to
   16 lanes each at a different priority (VL15 is highest and carries
   mgmt traffic), VL0 is lowest).  QoS Service Level (SL)is applied to
   traffic; each switch maps SL to VL.  Uses Point-to-Point flow
   control at the link level, on a per VL-basis.  It's credit based:
   receiving end supplies crdits to sending device.  This is about
   buffer space. There's a CRC.

  * Significant aspect is Routing at the Network Layer. Two-layer
    (local and global ids). Elements of IPv6. Switched subnets
    connected by routers.

  * Transport layer is as expected... See Cloudswit.ch below

RoCEv2 runs on top of UDP/IP. v1 assumes on the same L2 broadcast
domain. (A concept we need to explain).

RoCE uses IP at the network layer, and includes support for ECN
(Explicit Congestion Notification) for end-to-end congestion control
and DSCP (Diff Serv) as a replacement for IB’s Traffic Class to
implement QoS.

..  Infiband (role of fabric)
    https://network.nvidia.com/pdf/whitepapers/IB_Intro_WP_190.pdf
        https://cloudswit.ch/blogs/roce-or-infiniband-technical-comparison/#1-6-transport-layer

    Converged Ethernet
    https://dl.acm.org/doi/pdf/10.1145/3718958.3754353

    RDMA over Heterogeneous NICS
    https://www.usenix.org/system/files/osdi23-li-qiang.pdf

    RoCEv2 Header Encapsulation https://en.wikipedia.org/wiki/RDMA_over_Converged_Ethernet#/media/File:RoCE_Header_format.png

    RoCE encapsulate IB payload in and Ethernet frame. v1 was at L2; v2
    is now L3-based.

    A good overview of RoCE  https://www.fs.com/blog/rdma-over-converged-ethernet-roce-guide-2208.html

Some facts... Convergence Enhanced Ethernet was defined (2008-09) by
group of vendors including Broadcom, Brocade Communications Systems,
Cisco Systems, Emulex, HP, IBM, Juniper Networks, QLogic. Led to
proposals to 802.1 working groups:

 * Priority-based Flow Control (PFC) provides a link-level flow
   control mechanism that can be controlled independently for each
   frame priority. The goal of this mechanism is to ensure zero loss
   under congestion.

 * Enhanced Transmission Selection (ETS) provides a common management
   framework for the assignment of bandwidth to frame priorities.

 * Data Center Bridging eXchange (DCBX) is a discovery and capability
   exchange protocol that is used for report capabilities and
   configuration between neighbors to ensure consistent configuration
   across the network.

.. For a good overview of RoCE, see
   https://www.fs.com/blog/rdma-over-converged-ethernet-roce-guide-2208.html

.. sidebar:: On-NIC TCP Processing

   Why not just put TCP/IP on the NIC? Maybe talk about IPUs too.
