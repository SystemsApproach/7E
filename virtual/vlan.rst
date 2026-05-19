|Virt|.2 Virtual LANs (VLANs)
-----------------------------------------------

One of the oldest and simplest forms of virtual networking is the
*virtual LAN* (VLAN).  VLANs allow a single extended LAN to be
partitioned into several seemingly separate LANs. Each virtual LAN is
assigned an identifier, and packets can only travel from one segment
to another if both segments have the same identifier. This has the
effect of limiting the number of segments in an extended LAN that will
receive any given broadcast packet. An analogy can be made to virtual
memory: just as every process sees its own private memory address
range, every device on a VLAN sees only the other devices on the same
VLAN, even if physical links are shared with other devices on
different VLANs. VLANs are often used to provide a basic form of
traffic isolation in enterprise networks and datacenters; for example,
they can be used to provide isolation between different departments or
different types of application in a datacenter.

.. _fig-vlan:
.. figure:: virtual/figures/Slide7.png
   :width: 350px
   :align: center

   Two virtual LANs share a common backbone.

We can see how VLANs work with an example. :numref:`Figure %s
<fig-vlan>` shows four hosts and two switches. In the absence of
VLANs, any broadcast packet from any host will reach all the other
hosts. Now let’s suppose that we define the segments connected to
hosts W and X as being in one VLAN, which we’ll call VLAN 100. We also
define the segments that connect to hosts Y and Z as being in
VLAN 200. To do this, we need to configure a VLAN ID on each port of
switches S1 and S2. The link between S1 and S2 is considered to be in
both VLANs. It is sometimes referred to as a *trunk* interface, while
the links facing towards the hosts are *access*
interfaces. Configuration settings determine whether an interface is
in trunk or access mode.

When a packet sent by host X arrives at switch S2, the switch observes
that it came in via an access port that was configured as being in
VLAN 100. Before forwarding the packet on a trunk port, the switch
inserts a VLAN header between the Ethernet header and its payload. The
interesting part of the VLAN header is the VLAN ID; in this case, that
ID is set to 100. The switch now applies its normal rules for forwarding
to the packet, with the extra restriction that the packet may not be
sent out an interface that is not part of VLAN 100. Thus, under no
circumstances will the packet—even a broadcast packet—be sent out the
interface to host Z, which is in VLAN 200. The packet, however, is
forwarded on to switch S1, which follows the same rules and thus may
forward the packet to host W but not to host Y.

An attractive feature of VLANs is that it is possible to change the
logical topology without moving any wires or changing any addresses. For
example, if we wanted to make the link that connects to host Z be part
of VLAN 100 and thus enable X, W, and Z to be on the same virtual LAN,
then we would just need to change one piece of configuration on switch
S2.

An important benefit of VLANs is the reduction in size of broadcast
domains compared to the equivalent physical network if VLANs were not
used. Broadcast storms, caused by misbehaving or malfunctioning hosts,
can be a serious issue in networks that support broadcast such as
Ethernet; VLANs at least limit the number of hosts that will be
affected if a broadcast storm occurs, as it is contained to a single VLAN.

As with any sort of network virtualization, we need a way to identify
the virtual instance to which any given packet belongs. In the case of
VLANs, this is done using a small extension to the
original 802.3 header specification, inserting a 12-bit VLAN ID
(``VID``) field between the ``SrcAddr`` and ``Type`` fields, as shown in
:numref:`Figure %s <fig-vlan-tag>`. (This VID is typically referred to as
a *VLAN Tag*.) There are actually 32-bits inserted in the middle of
the header, but the first 16-bits are used to preserve backwards
compatibility with the original specification (they use ``Type =
0x8100`` to indicate that this frame includes the VLAN extension); the
other four bits hold control information used to prioritize
frames. With 12 bits of VLAN ID, it is possible to map :math:`2^{12} = 4096` virtual
networks onto a single physical LAN.

.. _fig-vlan-tag:
.. figure:: virtual/figures/Slide4.png
   :width: 500px
   :align: center

   802.1Q VLAN tag embedded within an Ethernet (802.3)
   header.

VLANs are, in a sense, the precursor to a more fully fledged form of
network virtualization that we return to in Section |Virt|.4. The use
of a *tag* to identify which particular virtual network a packet
belongs is a fundamental building block of a network virtualization
solution, which we will see used in several different settings in the
coming sections.

