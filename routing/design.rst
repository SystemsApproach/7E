4.1 Design Issues
------------------

Since this chapter is about routing, we need to be clear about what we
mean by that term. As pointed out in Chapter 3, *routing* refers to
the process of finding a suitable path through a network. Routing is
distinct from *forwarding*, which consists of receiving a packet,
looking up its destination address in a table, and sending the packet
in a direction determined by that table. Forwarding is considered part
of the network's *data plane*, whereas routing is the process by which
forwarding tables are built. Routing often depends on a complex
distributed algorithm, and is considered part of the network's
*control plane.*

.. TODO -- Rethink next paragraph in terms of "switch as platform"

Recall also that there isn't a lot of difference between switches and
routers (no matter how much the vendors of these devices might argue
otherwise). They have a data plane and a control plane, and the
control plane is in charge of putting entries in the forwarding table
so it can be used by the data plane. Historically, switches had a
simpler control plane (based on the spanning tree protocol) and they
only forwarded based on the Ethernet header, but today
there are Ethernet switches that combine traditional switching functions with
those of a router, so it's harder to make a strong distinction.

One question we need to ask anytime we try to build a mechanism for
the Internet is how well it scales. The answer for the algorithms and
protocols described in this chapter is “moderately.”  They are
designed for networks of fairly modest size—up to a few thousand
switches. That's not nearly enough if our ambition is to have a single
routing mechanism work across the millions of switches in today's
Internet, but that's not actually our goal. This is because the
Internet has naturally self-organized into a set of *routing domains*,
where a domain includes only those the devices are under the same
administrative control (e.g., a single university campus, an
enterprise, or the network of one Internet Service Provider). If we
can solve the routing problem within a routing domain, then we have a
building block that can be used—hierarchically—to solve the global
routing problem.

The mechanisms described in this chapter target individual routing
domains. They are collectively known as *intradomain* routing
protocols, or alternatively, as *interior gateway protocols (IGPs)*.
In Chapter 7 we take up the issue of *interdomain* routing,
specifically, BGP. For now, the important thing to keep in mind is
that we are considering the problem of routing in the context of small
to mid-sized networks, not for a network the size of the entire
Internet.

4.1.1 Network as a Graph
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Routing is, in essence, a problem of graph theory. :numref:`Figure %s
<fig-graph-route>` shows a graph representing a network. The nodes of
the graph, labeled A through F, may be hosts, switches, routers, or
networks. The edges of the graph correspond to the network links. Each
edge has an associated *cost*, which gives some indication of the
desirability of sending traffic over that link. A discussion of how
edge costs are assigned is given in a later section.

That we model switches and routers as vertices in a graph is no
surprise, but why hosts and networks? Including hosts in a network
graph might make sense when they are connected to two or more
networks, and need to pick the best link for outgoing traffic.  Such
hosts could technically forward traffic on behalf of others (if
configured to do so), but typically they are said to be *multi-homed*.
Including networks as nodes in a network graph can be used to model
multi-access networks; such a network behaves like a switch that
connect a set of nodes. (In this case, a graph edge to such a vertex
represents the fact that a node is connected to that network.) Even
switched networks are sometimes "abstracted away" for the purpose of
routing, and hence, reduced to a single vertex in a routing graph. The
important point is that the network graph is an abstract
representation (a model) of the actual physical network. We have some
latitude in exactly how we map the conceptual graph onto the physical
reality.

Note that the example networks (graphs) used throughout this chapter
have undirected edges that are assigned a single cost. This is actually
a slight simplification. It is more accurate to make the edges
directed, which typically means that there would be a pair of edges
between each node—one flowing in each direction, and each with its
own edge cost.

.. _fig-graph-route:
.. figure:: routing/figures/f03-28-9780123850591.png
   :width: 400px
   :align: center

   Network represented as a graph.

The basic problem of routing is to find the lowest-cost path between
any two nodes, where the cost of a path equals the sum of the costs of
all the edges that make up the path. For a simple network like the one
in :numref:`Figure %s <fig-graph-route>`, you could imagine just
calculating all the shortest paths and loading them into some
nonvolatile storage on each node. Such a static approach has several
shortcomings:

-  It does not deal with node or link failures.

-  It does not consider the addition of new nodes or links.

-  It implies that edge costs cannot change, even though we might
   reasonably wish to have link costs change over time (e.g., assigning
   high cost to a link that is heavily loaded).

For these reasons, routing is achieved in most practical networks by
running routing protocols among the nodes. These protocols provide a
distributed, dynamic way to solve the problem of finding the
lowest-cost path in the presence of link and node failures and
(potentially) changing edge costs.  Note the word *distributed* in the
previous sentence; it is generally difficult to make centralized
solutions scalable, so the most commonly-used routing protocols depend
on distributed algorithms.  The rise of centralized control planes as
part of software-defined networking (SDN) has somewhat challenged this
picture, as we discuss later in this chapter.

The distributed nature of routing algorithms is one of the main reasons
why this has been such a rich field of research and development—there
are a lot of challenges in making distributed algorithms work well. For
example, distributed algorithms raise the possibility that two nodes
will at one instant have different ideas about the shortest path to some
destination. In fact, each one may think that the other one is closer to
the destination and decide to send packets to the other one. Clearly,
such packets will be stuck in a loop until the discrepancy is resolved,
and it would be good to resolve it as soon as possible. This is just
one example of the type of problem routing protocols must address.

.. TODO -- DO we come back to this? - yes, now noted in both linkstate and DV

To begin our analysis, we assume that the edge costs in the network
are known; they are often set statically when switches are
configured. In a later section, we return to the problem of
calculating edge costs in a meaningful way.

Because routing is so often treated as a graph theory problem, it can
be easy to lose sight of the practical issues. In particular, avoiding
the formation of loops during periods when the routing protocols are
responding to changes in topology turns out
to be harder than it first appears. We start our
discussion of routing with a look at the spanning tree protocol, which
tackles loop prevention as its central issue.



