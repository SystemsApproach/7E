.. index:: TE: Traffic Engineering

|Capacity|.5  Traffic Engineering
-----------------------------------------


The idea of traffic engineering for packet-switched networks is almost
as old as packet switching itself, with some ideas of traffic-aware
routing having been tried in the ARPANET. However, traffic engineering
only became mainstream for the Internet backbone with the
advent of MPLS, which provides a set of tools to steer traffic to
balance load across different paths. The key idea is that when there
is more than one path between two points in the network, it would be
best to split the traffic among those paths in a way that avoids
overloading any one of them. That simple idea has proven challenging
to implement.

IP has a source routing option, but it is not widely used for several
reasons, including the fact that only a limited number of hops can be
specified and because it is usual processed outside the “fast path” on
most routers. MPLS provides a convenient way to add capabilities similar to
source-routing to IP networks, although the capability is more often
referred to as *explicit routing* rather than *source routing*. One
reason for the distinction is that it usually isn’t the real source of
the packet that picks the route. More often it is one of the routers
inside a service provider’s network. :numref:`Figure %s <fig-fish>`
shows an example of how the explicit routing capability of MPLS might
be applied.  This sort of network is often called a *fish* network
because of its shape (the routers R1 and R2 form the tail; R7 is at
the head).

.. _fig-fish:
.. figure:: capacity/figures/f04-22-9780123850591.png
   :width: 450px
   :align: center

   A network requiring explicit routing.

Suppose that the operator of the network in :numref:`Figure %s
<fig-fish>` has determined that any traffic flowing from R1 to R7
should follow the path R1-R3-R6-R7 and that any traffic going from R2
to R7 should follow the path R2-R3-R4-R5-R7. One reason for such a
choice would be to make good use of the capacity available along the
two distinct paths from R3 to R7. We can think of the R1-to-R7 traffic
as constituting one forwarding equivalence class, and the R2-to-R7
traffic constitutes a second FEC.  Forwarding traffic in these two
classes along different paths is difficult with normal IP routing,
because R3 doesn’t normally look at where traffic came from in making
its forwarding decisions.

Unlike IP, MPLS uses label swapping to forward packets. Rather than
looking at the destination address, an MPLS router looks at a label in
the packet header and makes a forwarding decision based on the value
of that label. Importantly, labels are swapped at every hop (usually)
and have local scope, unlike IP addresses. So the packets from R1 to
R7 might have label *L1* in the header when they arrive at R3, while those from R2 to R7 have
label *L2* in the header, even though both sets of packets have the
same destination. We have created two distinct FECs, and R3 forwards
them differently.

The question that then arises is how do all the routers in the network
agree on what labels to use and how to forward packets with particular
labels? The protocol that was adopted and extended for
this task is the Resource Reservation Protocol (RSVP). For now it
suffices to say that it is possible to send an RSVP message along an
explicitly specified path (e.g., R1-R3-R6-R7) and use it to set up
label forwarding table entries all along that path.  This is very
similar to the process of establishing a virtual circuit.

Once we have the mechanism of explicit routing, we can apply it to the
task of traffic engineering. The most common approaches is
*constrained shortest path first* (CSPF), which is a link-state
algorithm, but which also takes various *constraints* into
account. For example, if it was required to find a path from R1 to R7
that could carry an offered load of 100 Mbps, we could say that the
constraint is that each link must have at least 100 Mbps of available
capacity. CSPF addresses this sort of problem. It works just like the
SPF algorithm described in Section |Routing|.3 except that links which
don't meet the constraints, e.g., because they lack sufficient
capacity for the demand, are excluded from the calculation.


CSPF can work well, but as a distributed algorithm, it has some
weaknesses. Central planning tools are commonly used to supplement
CSPF, but the real-time management of MPLS paths is usually fully
distributed.  CSPF path calculation algorithms are triggered any time
a link changes status, or as traffic loads change, making local
choices about what seems best. This makes it nearly impossible to
achieve any sort of global optimization of resource usage, as the
example below illustrates.

Realizing the limitations of the traffic engineering approaches being
used by ISPs, some of the largest cloud operators started to look at
other ways to engineer the traffic traversing the
wide-area links between their datacenters. For example, Google has publicly
described their private backbone, called B4, which is built entirely
using bare-metal switches and SDN. Similarly, Microsoft has described
an approach to interconnecting their data centers called SWAN. A
central component of both B4 and SWAN is a centralized
Traffic Engineering control program that provisions the network
according to the needs of various classes of applications.

Consider the example in :numref:`Figure %s <fig-te-example>`. Assume
that all links are of unit capacity and we are trying to find paths
for three unit flows of traffic. In the figure on the left, Flow A is
placed first and picks one of the two shortest paths available. Flow B
is placed next and takes the shortest remaining path, as the
single-hop path is already filled by Flow A. When placing Flow C last,
there is no choice but the long path. But a central algorithm that
looked at all three flows at once and tried to place them optimally
would end up with the much less wasteful set of paths shown on the
right hand side of the figure. While this is a contrived example,
sub-optimal outcomes as shown on the left are unavoidable when there
is no central view of traffic.

.. _fig-te-example:
.. figure:: capacity/figures/Slide53.png
    :width: 600px
    :align: center

    Example of non-optimal traffic engineering (left) and optimal
    placement (right).

B4 and SWAN recognize this shortcoming and move the path calculation to a
logically centralized SDN controller. When a link fails, for example,
the controller calculates a new mapping of traffic demands onto
available links, and programs the switches to forward traffic flows in
such a way that no link is overloaded.

Over many years of operation, these approaches have become more sophisticated. For
example, B4 evolved from treating all traffic equally to supporting a
range of traffic classes with different levels of tolerance to delay
and availability requirements. Examples of traffic classes
included: (1) copying user data (e.g., email, documents, audio/video)
to remote datacenters for availability; (2) accessing remote storage
by computations that run over distributed data sources; and (3)
pushing large-scale data to synchronize state across multiple
datacenters. In this example, user-data represents the lowest volume
on B4, is the most latency sensitive, and is of the highest
priority. By breaking traffic up into these classes with different
properties, and running a path calculation algorithm for each one, the
team was able to considerably improve the efficiency of the network,
while still meeting the requirements of the most demanding
applications.

Through a combination of centralizing the decision-making process,
programmatically rate-limiting traffic at the senders, and
differentiating classes of traffic, Google has been able to drive
their link utilizations to nearly 100%. This is two to three times
better than the 30-40% average utilization that WAN links are
typically provisioned for, which is necessary to allow those networks
to deal with both traffic bursts and link/switch failures. Microsoft's
reported experience with SWAN was similar. These hyperscale
experiences with SDN show both the value of being able to customize
the network and the power of centralized control to change networking
abstractions. A conversation with Amin Vahdat, Jennifer Rexford, and
David Clark is especially insightful about the thought process in
adopting SDN in the cloud's backbone.

.. _reading_b4:
.. admonition:: Further Reading

   A. Vahdat, D. Clark, and J. Rexford. `A Purpose-built Global Network:
   Google's Move to SDN
   <https://dl.acm.org/doi/10.1145/2814326>`__.
   Communications of the ACM, February 2016.
