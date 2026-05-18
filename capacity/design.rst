|Capacity|.1  Design Issues
-------------------------------

One consequence of a best-effort network design is that a given source
of traffic may have ample capacity to send packets into the network at
a high rate, but somewhere in the middle of the network its packets
encounter a bottleneck link that is being used by many different
traffic sources. :numref:`Figure %s <fig-congestion>` illustrates an
acute example of this situation. The router is able to queue (buffer)
packets for a while, but if the problem persists, the queue grows, and
because it is finite, it will eventually overflow. This situation,
where offered load exceeds link capacity, is the very definition of
congestion.

.. _fig-congestion:
.. figure:: capacity/figures/Slide7.png
   :width: 400px
   :align: center

   Congestion at a bottleneck router.


.. TODO change bottleneck to something realistic like 100 Mbps

Note that avoiding congestion is not a problem that can be fully
addressed by routing.  While it is true that a congested link could be
assigned a large cost by a routing protocol, in an effort to make
traffic avoid that link, this can't solve the overall problem of too
much traffic being offered to a bottleneck link. To see this, we need
look no further than the simple network depicted in :numref:`Figure %s
<fig-congestion>`, where all traffic has to flow through the same
router to reach the destination. Although this is an extreme example,
it is not uncommon to have a bottleneck link that you can't route
around. The access link to an enterprise or residential network is a
common example of such a bottleneck.

So how do we address congestion? As originally deployed, the Internet
didn't do anything. Users (or more precisely, TCP running on their
behalf) were free to send as many packets into the network as they
could generate. A phenomenon known as *tragedy of the commons*,
whereby selfish behavior degrades shared resources, was predictable,
and did eventually catch up with the Internet. The response is for
edge hosts to send less traffic (exactly how much less is the essence
of Congestion Control), but responsibility doesn't fall solely to the
edge hosts. Network nodes have a role to play, as well.  The rest of
this section explores that part of the design space.

|Capacity|.1.1 Centralized vs Distributed
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. TODO -- This subsection may be a little too broad for the purposes
   of this chapter, but it does set up TE as a topic.

In principle, the first design decision is whether a network's
approach to resource allocation is centralized (based on global
information) or distributed (based on local information). In practice,
the Internet's scale—along with the autonomy of the organizations that
connect to it—dictated a distributed approach. Indeed, distributed
management of resources was an explicitly stated goal of the
Internet's design, as articulated by Dave Clark. But acknowledging
this default decision is important for two reasons.

.. _reading_design:
.. admonition:: Further Reading

       D. Clark, `The Design Philosophy of the DARPA Internet
       Protocols <https://dl.acm.org/doi/10.1145/52324.52336>`__.
       ACM SIGCOMM, 1988.

First, while the Internet's approach to congestion control is
distributed across its millions of hosts and routers, it is fair to
think of them as cooperatively trying to achieve a globally optimal
solution.  From this perspective, there is a shared objective
function, and all the elements are implementing a distributed
algorithm to optimize that function. The various mechanisms described
in this Chapter—and their edge counterparts in Chapter |CC| in Part
III—are simply defining some objective function. A persistent
challenge has been how to judge competing objective functions when
multiple mechanisms have been deployed.

Second, while a centralized approach is not practical for the Internet
as a whole, it can be appropriate for limited domains; backbone
networks connecting cloud datacenters are one example.  In such
domains, a logically centralized controller could collect information
about the state of the network's links and switches, compute a
globally optimal (or near-optimal) allocation, and then advise end
hosts as to how much capacity is available to each of them. Such an
approach would certainly be limited in either the geographic scope or
the time-scale in which the centralized controller could be responsive
to changes in the network. There are examples of centralized
approaches being successfully applied at
data center scale and in the longer timescale
allocation decisions made by traffic engineering
mechanisms, such as those described in Section |Capacity|.4. There
have been enough challenges to the distributed model that dominated
the first few decades of the Internet's history to suggest that this
design decision is not yet entirely settled.

|Capacity|.1.2 Router-Centric vs Host-Centric
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Given a distributed approach to resource allocation, the next question
is whether to implement the mechanism inside the network (i.e., at the
routers or switches), at the edges of the network (i.e., in the hosts,
perhaps as part of the transport protocol), or some combination of the
two. Individual routers always take responsibility for deciding which
packets to forward and which packets to drop. However, there is a
range of options in how much the router involves the end hosts in
specifying how this decision is made, or learning how this decision
was made.

At one end of the spectrum, routers can allow hosts to reserve
capacity and then ensure each flow's packets are delivered
accordingly. They might do this, for example, by accepting new flows
only when there is sufficient capacity, and policing hosts to make
sure their flows stay within their reservations. This would correspond
to a reservation-based approach in which the network is able to make
QoS guarantees. We postpone this possibility until Chapter |Stream|,
and keep our focus on the best-effort service model for now.

At the other end of the spectrum is a host-centric approach. The
router makes no guarantees and offers no explicit feedback about the
available capacity (i.e., silently drops packets when its buffers are
full) and it is the host's responsibility to observe the network
conditions (e.g., how many packets they are successfully getting
through the network) and adjust its behavior accordingly.

In the middle, routers can take more proactive action to assist the
end hosts in doing their job, but not by reserving buffer space.  This
involves the router sending *feedback* to the end hosts when its
buffers are full. Section |Capacity|.3 describes how such AQM
mechanisms work.

|Capacity|.1.3 Persistent Queues
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

While the discussion up to this point has focused on the worst-case
scenario of overflowing queues, there is something more subtle
going on. The whole point of buffering is to absorb *packet bursts*,
that is, to accommodate the time-varying nature of data communication.
We need to understand burstiness to know how to best manage queues.

The key observation is that queues are expected to build up from
time to time. For example, a newly opened connection may dump a burst
of packets into the network, and these are likely to form a queue at
the bottleneck link. This is not in itself a problem. There should be
enough buffer capacity to absorb such bursts. Problems arise when
there is not enough buffer capacity to absorb bursts, leading to
excessive loss. This came to be understood in the 1990s as a
requirement that buffers be able to hold at least one bandwidth-delay
product of packets—a requirement that was probably too large and
subsequently questioned by further research. But the fact is that
buffers are necessary, and it is expected that they will be used to
absorb bursts. This is sometimes referred to this as \"good queue\",
as illustrated in :numref:`Figure %s <fig-good-bad>` (a).

.. _fig-good-bad:
.. figure:: capacity/figures/Slide14.png
   :width: 400px
   :align: center

   Good and Bad Queue Scenarios

On the flip side, over-provisioning queues (i.e., making them too
large) can be a problem if they end up being persistently full. A
persistently full queue does nothing except add delay to the
network. To compound the problem, a queue is less able to absorb
future bursts if it never drains fully. The combination of large
buffers and persistent queues within those buffers is a phenomenon
that Jim Gettys has named *Bufferbloat*. It is clear that persistently
full queues are what a well-designed AQM mechanism should seek to
avoid. Queues that stay full for long periods without draining are
referred to, unsurprisingly, as \"bad queue\", as shown in
:numref:`Figure %s <fig-good-bad>` (b).

.. _reading_bloat:
.. admonition::  Further Reading

   J. Gettys. `Bufferbloat: Dark Buffers in the Internet
   <https://ieeexplore.ieee.org/document/5755608>`__. IEEE
   Internet Computing, April 2011.


|Capacity|.1.4 Flows and Soft State
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Because the Internet assumes a connectionless model, any
connection-oriented service is implemented by an end-to-end transport
protocol running on the end hosts (such as TCP). There is no
connection setup phase implemented *within* the network (in contrast
to circuit based networks), and as a consequence, there is no
mechanism for individual routers to pre-allocate buffer space or link
bandwidth to active connections.

The lack of an explicit connection setup phase does not imply that
routers must be completely unaware of end-to-end connections.  Packets
are switched independently, but it is often the case that a given pair
of hosts exchange many packets consecutively, e.g. as a large video
file is downloaded by a client from a server. Furthermore, a given
stream of packets between a pair of hosts often flows through a
consistent set of routers. This idea of a *flow*—a sequence of packets
sent between a source/destination pair and following the same route
through the network—is an important abstraction that we take advantage
of.

.. TODO deal with the forward reference to TCP port numbers?

One of the powers of the flow abstraction is that flows can be defined
at different granularities. For example, a flow can be host-to-host
(i.e., have the same source/destination IP addresses), or
process-to-process (i.e., have the same source/destination host/port
pairs). :numref:`Figure %s <fig-flow>` illustrates several flows
passing through a series of routers.

.. _fig-flow:
.. figure:: capacity/figures/Slide8.png
   :width: 450px
   :align: center

   Multiple flows passing through a set of routers.

Because multiple related packets flow through each router, it
sometimes makes sense to maintain some state information for each
flow, which can be used to make resource allocation decisions about
the packets of that flow. This is called *soft state*, where the main
difference between soft and hard state is that the former is not
explicitly created and removed by signalling. Soft state represents a
middle ground between a purely connectionless network that maintains
*no* state at the routers and a purely connection-oriented network
that maintains hard state at the routers. In general, the correct
operation of the network does not depend on soft state being present
(each packet is still routed correctly without regard to this state),
but when a packet happens to belong to a flow for which the router is
currently maintaining soft state, then the router is better able to
handle the packet.

|Capacity|.1.5 Differentiated Service
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The final design question is one the original IP specification raised,
but didn't fully address: Should routers treat all traffic the same,
or should it be possible to offer different levels of service to
different classes of traffic? The original spec, RFC 791, defines an
8-bit *Type of Service* field (``ToS``), along with abstract settings
that can be summarized as a 3-bit "Priority" subfield and a 3-bit "
"Quality-of-Service" subfield. The former is used to identify
important packets, such as routing updates, and the latter supports a
three-way tradeoff between low-delay, high-reliability, and
high-throughput.  The other two bits were saved for future use.

The existence of the ``ToS`` field indicates a recognition that there
are good reasons to treat packets differently, but the actual purpose
of the field was to provide routers the information they might need to
properly set parameters on the underlying physical networks they
forwarded packets over. It was assumed those physical networks offered
different levels of service to different types of traffic; it was not
intended that the router itself would change its own packet forwarding
based on the setting. Routers remained purely best-effort, without
favoring one class of traffic over another. To further complicate
things, different vendors ended up using the ``ToS`` bit for different
purposes, and so there wasn't universal agreement on what the bits
meant.

All of that changed many years after IP became ubiquitous, in ways
that we explore in this chapter. The general approach is often
referred to as *differentiated services*, indicating that not all
packets are treated exactly the same. We are still squarely in the
best-effort domain in that no promises are made, but short of
guaranteeing a throughput rate or an upper bound on jitter, there are
steps the network can take to differentiate the level of service various
classes of traffic receive.

The following sections show how this is done by repurposing the
``ToS`` field already included in the IP headers. But finding the
header bits to denote how packets are to be treated is the easy
part. The challenge is that, once you decide to differentiate among
different classes of traffic, you need to decide (a) what classes to
support, and (b) how to modify packet forwarding so each class
receives the desired performance. This is a multi-faceted problem,
involving coordination among multiple mechanisms. We address it in the
context of a specific use case—datacenter networks—in Section
|Capacity|.4.
