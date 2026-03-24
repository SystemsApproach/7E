4.5 Routing in Datacenters
-------------------------------

We conclude our discussion of routing by looking at a different
scenario: how routes are managed in datacenter networks. Recall from
Chapter |Tech| that datacenters typically interconnect racks of servers
using a leaf-spine network topology. :ref:`Figure 22 <fig-leaf-spine>`
shows a simple four-rack example using a two-level switching fabric.
Hyperscaler datacenters are much larger—and often use three-level
fabrics—but the same idea applies.

Several things are distinctive about this particular use case. For one,
instead of using one of the distributed algorithms described in the
previous sections, many datacenters adopt a centralized approach to
routing based on the principles of *Software-Defined Networking
(SDN)*. For another, the uniform structure of a leaf-spine fabric
lends itself to other routing techniques, including an approach known as
*segment routing*.  This sections gives an overview of SDN, and
explains the role it plays in implementing segment routing.


4.5.1 Software-Defined Networking
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

As discussed in Chapter |Tech|, switches and routers have a control plane
and a data plane, with some interface between them to allow forwarding
rules to be installed in the data plane as the result of routing
calculations made in the control plane. For most of the history of the
Internet, these two planes ran inside each switch and router in the
Internet, and the interface was a strictly internal one. Furthermore,
because the interface existed inside a single box, there was no
pressing need to standardize it: those boxes were implemented by
equipment vendors and the interface between control and data planes
was proprietary and vendor-specific.

There have been efforts to open up this interface so that the
implementation of the control plane and data plane could be undertaken
by independent teams. The approach that finally started to gain
traction around 2009 is known as SDN. Not only did SDN define a
standard interface between the data and control planes, but it also
advanced the idea of using a centralized control plane to push
forwarding rules to many devices implementing the data plane. A
conceptual picture of an SDN system is shown in :numref:`Figure %s
<fig-sdn>`.

.. _fig-sdn:
.. figure:: routing/figures/sdn.png
    :width: 500px
    :align: center

    Network Operating System (NOS) hosting a set of control
    applications and providing a logically centralized point of
    control for an underlying network data plane.

.. TODO -- We may want to drop the NOC angle, and just focus
   on a single "Control Program".

Centralized control opens up the possibility of rethinking how routing
works in a network.  Rather than a fully distributed algorithm of the
sort described in the preceding sections, we now have the option of
using centralized algorithms. As a simple example, a controller could
gather information from all the switches to which is is connected
regarding the state of their links to other switches. With this
information in hand, it has all the information needed to run a
shortest path calculation from the perspective of any switch. Thus it
could calculate forwarding tables for every switch and push them down
to the forwarding plane as a set of flow rules.

Simply replacing the standard fully distributed version of OSPF or RIP
with a centralized equivalent may not seem much of a step forward.
What is more powerful is the capability to run *new* routing
algorithms centrally, without needing to solve the challenges of
getting a consistent view of the network at every node in a
distributed system.

Some famous examples of novel approaches to routing enabled by SDN
include the work at Google on B4 and Microsoft on SWAN. In both cases,
the aim was to pick paths through their respective backbone networks
that were sufficient in terms of capacity to support the expected
matrix of traffic applied to the network. This problem of mapping a
traffic matrix onto a set of links is hard to solve efficiently in a
fully distributed manner; centralizing it makes the problem much
easier. Thus one of the early successes of SDN was to solve these
*traffic engineering* problems in the large backbones interconnecting
hyperscale datacenters.

In this section we look at a related example—how to route *within* a
single datacenter—and the specific method we describe is call *segment
routing*. In terms of :numref:`Figure %s <fig-sdn>`, you can think of
segment routing as an example SDN control application. B4 and SWAN are
examples of other possible control apps.

4.5.2 Link Aggregation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

We start with a more detailed look at how switches and servers are
interconnected in a leaf-spine fabric. As illustrated in
:numref:`Figure %s <fig-ecmp>`, each server is connected to a pair of
Top-of-Rack (ToR, or leaf) switches. This improves availability in the
face of link or switch failures. It also doubles the bandwidth into
and out of each server when everything is working properly. But this
is true only if the nodes are able to take advantage of that
capacity. The routing algorithms we've seen so far in this chapter
select a single best route, so they are not sufficient, by themselves,
to meet that goal.

The additional feature we need is for both servers and switches to
aggregate equivalent links, which is to say, treat them as a single
logical link. On the servers, this is typically done with an OS
mechanism known as *link bonding*. The OS creates a "logical NIC"
associated with the two physical NICs. The local forwarding table then
points to the logical NIC as the output port for each destination
address, and the OS load balances a sequence of packets sent to that
logical NIC across the two physical NICs.

Switches support a similar mechanism, in this case known as *ECMP
groups*. ECMP stands for *Equal-Cost Multi-Path*, which is a bit more
general than server link bonding in that it makes it possible for
a switch to spread traffic across multiple equal-cost *paths* rather
than just equivalent links. This is necessary for traffic crossing the
spine from one rack (leaf switch) to another; there are multiple spine
switches traffic can follow. For example, there is an ECMP Group in
:numref:`Figure %s <fig-ecmp>` containing the four links Leaf 1
can use to reach either of Spine 1 or Spine 2.

.. _fig-ecmp:
.. figure:: routing/figures/ecmp.png
    :width: 450px
    :align: center

    High availability through a combination of link bonding on servers
    and ECMP groups on switches.

.. TODO -- Links from Leaf 1 switch to two servers are misdrawn.
   It's a snap issue.

The one complication for both link bonding and ECMP groups is the
algorithm used to distribute traffic over the available links/paths.
Some end-to-end protocols, most notably TCP, work best when all the
packets belonging to the same stream follow the same path.
Out-of-order packets are handled correctly, but performance may
suffer. To address this, both aggregation techniques keep all packets
belonging to the same end-to-end flow on the same physical path. This
is often done by taking the TCP or UDP port numbers into account when
forwarding packets along equal-cost paths.

There are two takeaways from this overview of link aggregation, First,
with respect to routing, it is important to note that while routing
algorithms are capable of determining which paths are best by some
metric, they are not designed to balance load across equivalent
paths. Such a fine-grain mechanism is left to the data plane after the
routing decision has determined the equal-cost paths.

Second, ECMP is a forwarding strategy that is applies uniformly across
all the switches in a fabric. The SDN control application knows the
topology and pushes the port groups into each of the fabric switches
accordingly.  Each switch then applies these port groups to its
forwarding pipeline, which then forwards packets across the set of
ports in each group without additional control plane involvement.

4.5.3 Segment Routing
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. TODO -- We need to at least mention other possibilities, including
   BGP. This would require a forward reference to the interdomain
   routing chapter. (A sidebar in that chapter makes sense.)

Independent of link aggregation, we still need to discover routes
between all the servers. One approach is a routing algorithm called
*Segment Routing (SR)*. The term comes from the idea that the
end-to-end path between any pair of servers can be constructed from a
sequence of segments. When applied to a leaf-spine fabric, there are
always two segments involved: leaf-to-spine and spine-to-leaf.

There are only so many such segments in a leaf-spine fabric, so we can
simply "label" them; that is, assign a unique identifier to each.
These identifiers (labels) are then used to uniquely identify each
segment, packets carry the labels of the sequence of segments then
need to traverse, and switches are programmed to use these labels to
decided how to forward packets. We need a standard header format for
including labels in packets (which we'll get to in a moment), but
assuming an agreed upon format, this approach is known as
*label-switching*. A centralized fabric control program instructs the
switches to match labeled or unlabeled packets and push or pop labels
as needed.

:numref:`Figure %s <fig-sr>` illustrates how SR works using a simple
configuration that forwards traffic between a pair of hosts: 10.0.1.1
and 10.0.2.1. In this example, the servers connected to Leaf 1 are on
subnet 10.0.1/24, the servers connected to Leaf 2 are on subnet
10.0.2/24, and each of the switches has an assigned id: 101, 103, 102,
and 104. (In our simple example, only one segment label is needed, and
we can think of it as labeling either the segment or the target
switch; they are effectively the same.)

.. _fig-sr:
.. figure:: routing/figures/sr.png
    :width: 550px
    :align: center

    Example of Segment Routing being used to forward traffic between a
    pair of hosts.

When Host 1 sends a packet with destination address 10.0.2.1, it is by
default forwarded to the server’s ToR/leaf switch. Leaf 1 matches the
destination IP address, learns this packet needs to cross the fabric
and emerge at Leaf 2 to reach subnet 10.0.2/24, and so pushes the
label 102 onto the packet. Because of ECMP, Leaf 1 can forward the
resulting packet to either spine, at which point that switch matches
the label 102, pops the label off the header, and forwards it to
Leaf 2.  Finally, Leaf 2 matches the destination IP address and
forwards the packet along to Host 2.

What you should take away from this example is that SR is highly
stylized. For a given combination of leaf and spine switches, the
mechanism first assigns all identifiers, with each rack configured to
share an IP prefix. The fabric control app then pre-computes the
possible paths and installs the corresponding match/action rules in
the underlying switches. The complexity having to do with balancing
load across multiple paths is delegated to ECMP, which is similarly
unaware of any end-to-end paths.

Finally, we answer the question about how labels are added to packets.
There are two standardized approaches. One, called SR-MPLS, takes
advantage of labels already being an integral part of *Multi-Protocol
Label Switching (MPLS)*. We refer you to Chapter |Capacity| for more
information about MPLS. The second, called SRv6, is an SR-specific
extension to IPv6. We discuss in Chapter |Fed|, but for the purposes
of this discussion, SRv6 attaches a list of 128-bit *Segment IDs* to
the end of the IPv6 header, plus a *Segments Left* field that points
to the current active segment.

.. TODO -- Probably ought to give an example layout diagram


For a useful overview of one hyperscale data center design that
leverages SDN, we recommend the paper on Google's Jupiter
architecture.

.. admonition:: Further Reading

   L. Poutievski et al. `Jupiter evolving: transforming Google's
      datacenter network via optical circuit switches and
      software-defined networking
      <https://doi.org/10.1145/3544216.3544265>`__. Proc. SIGCOMM 2022.
