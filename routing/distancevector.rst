.. index:: RIP: Routing Information Protocol

|Routing|.4 Distance Vector Routing
---------------------------------------------

The idea behind the distance-vector algorithm is suggested by its
name.  (The other common name for this class of algorithm is
Bellman-Ford, after its inventors.) Each node constructs a
one-dimensional array (a vector) containing the “distances” (costs) to
all other nodes and distributes that vector to its immediate
neighbors. The starting assumption for distance-vector routing, as
with link-state, is that each node knows the cost of the link to each
of its directly connected neighbors. These costs may be provided when
the node is configured by a network manager. A link that is down is
assigned an infinite cost.

.. _fig-dvroute:
.. figure:: routing/figures/f03-29-9780123850591.png
   :width: 400px
   :align: center

   Distance-vector routing: an example network.

.. _tab-dvtab1:
.. table:: Initial Distances Stored at Each Node (Global View).
   :align: center
   :widths: auto

   +---+---+---+---+---+---+---+---+
   |   | A | B | C | D | E | F | G |
   +===+===+===+===+===+===+===+===+
   | A | 0 | 1 | 1 | ∞ | 1 | 1 | ∞ |
   +---+---+---+---+---+---+---+---+
   | B | 1 | 0 | 1 | ∞ | ∞ | ∞ | ∞ |
   +---+---+---+---+---+---+---+---+
   | C | 1 | 1 | 0 | 1 | ∞ | ∞ | ∞ |
   +---+---+---+---+---+---+---+---+
   | D | ∞ | ∞ | 1 | 0 | ∞ | ∞ | 1 |
   +---+---+---+---+---+---+---+---+
   | E | 1 | ∞ | ∞ | ∞ | 0 | ∞ | ∞ |
   +---+---+---+---+---+---+---+---+
   | F | 1 | ∞ | ∞ | ∞ | ∞ | 0 | 1 |
   +---+---+---+---+---+---+---+---+
   | G | ∞ | ∞ | ∞ | 1 | ∞ | 1 | 0 |
   +---+---+---+---+---+---+---+---+

To see how a distance-vector routing algorithm works, it is easiest to
consider an example like the one depicted in :numref:`Figure %s
<fig-dvroute>`. In this simple example, the cost of each link is set
to 1, so that a least-cost path is simply the one with the fewest
hops. (Since all edges have the same cost, we do not show the costs in
the graph.)  We can represent each node’s knowledge about the
distances to all other nodes as a table like :numref:`Table %s
<tab-dvtab1>`. Note that each node knows only the information in one
row of the table (the one that bears its name in the left column). In
contrast to link-state algorithms, distance-vector protocols never
build a global view of the network in a single node.

We may consider each row in :numref:`Table %s <tab-dvtab1>` as a list
of distances from one node to all other nodes, representing the
current beliefs of that node. Initially, each node sets a cost of 1 to
its directly connected neighbors and ∞ to all other nodes. Thus, A
initially believes that it can reach B in one hop and that D is
unreachable. The routing table stored at A reflects this set of
beliefs and includes the name of the next hop that A would use to
reach any reachable node. Initially, then, A’s routing table would
look like :numref:`Table %s <tab-dvtab2>`.

.. _tab-dvtab2:
.. table::  Initial Routing Table at Node A.
   :align: center
   :widths: auto

   +-------------+------+---------+
   | Destination | Cost | NextHop |
   +=============+======+=========+
   | B           | 1    | B       |
   +-------------+------+---------+
   | C           | 1    | C       |
   +-------------+------+---------+
   | D           | ∞    | —       |
   +-------------+------+---------+
   | E           | 1    | E       |
   +-------------+------+---------+
   | F           | 1    | F       |
   +-------------+------+---------+
   | G           | ∞    | —       |
   +-------------+------+---------+

The next step in distance-vector routing is that every node sends a
message to its directly connected neighbors containing its personal list
of distances. For example, node F tells node A that it can reach node G
at a cost of 1; A also knows it can reach F at a cost of 1, so it adds
these costs to get the cost of reaching G by means of F. This total cost
of 2 is less than the current cost of infinity, so A records that it can
reach G at a cost of 2 by going through F. Similarly, A learns from C
that D can be reached from C at a cost of 1; it adds this to the cost of
reaching C (1) and decides that D can be reached via C at a cost of 2,
which is better than the old cost of infinity. At the same time, A
learns from C that B can be reached from C at a cost of 1, so it
concludes that the cost of reaching B via C is 2. Since this is worse
than the current cost of reaching B (1), this new information is
ignored. At this point, A can update its routing table with costs and
next hops for all nodes in the network. The result is shown in
:numref:`Table %s <tab-dvtab3>`.

.. _tab-dvtab3:
.. table:: Final Routing Table at Node A.
   :align: center
   :widths: auto

   +-------------+------+---------+
   | Destination | Cost | NextHop |
   +=============+======+=========+
   | B           | 1    | B       |
   +-------------+------+---------+
   | C           | 1    | C       |
   +-------------+------+---------+
   | D           | 2    | C       |
   +-------------+------+---------+
   | E           | 1    | E       |
   +-------------+------+---------+
   | F           | 1    | F       |
   +-------------+------+---------+
   | G           | 2    | F       |
   +-------------+------+---------+

In the absence of any topology changes, it takes only a few exchanges
of information between neighbors before each node has a complete
routing table.  :numref:`Table %s
<tab-dvtab4>` shows the final set of costs from each node to all other
nodes when routing has converged.  We must stress that there is no one
node in the network that has all the information in this table—each
node only knows about the contents of its own routing table. The
beauty of a distributed algorithm like this is that it enables all
nodes to achieve a consistent view of the network in the absence of
any centralized authority. So we achieve convergence—a consistent view
among all nodes—without any node having a full picture of the
network.

.. _tab-dvtab4:
.. table:: Final Distances Stored at Each Node (Global View).
   :align: center
   :widths: auto

   +---+---+---+---+---+---+---+---+
   |   | A | B | C | D | E | F | G |
   +===+===+===+===+===+===+===+===+
   | A | 0 | 1 | 1 | 2 | 1 | 1 | 2 |
   +---+---+---+---+---+---+---+---+
   | B | 1 | 0 | 1 | 2 | 2 | 2 | 3 |
   +---+---+---+---+---+---+---+---+
   | C | 1 | 1 | 0 | 1 | 2 | 2 | 2 |
   +---+---+---+---+---+---+---+---+
   | D | 2 | 2 | 1 | 0 | 3 | 2 | 1 |
   +---+---+---+---+---+---+---+---+
   | E | 1 | 2 | 2 | 3 | 0 | 2 | 3 |
   +---+---+---+---+---+---+---+---+
   | F | 1 | 2 | 2 | 2 | 2 | 0 | 1 |
   +---+---+---+---+---+---+---+---+
   | G | 2 | 3 | 2 | 1 | 3 | 1 | 0 |
   +---+---+---+---+---+---+---+---+

There are a few details to fill in before our discussion of
distance-vector routing is complete. First, we note that nodes send
updates to their neighbors periodically based on a timer (from seconds
to minutes) but also send them immediately if they detect a change in
the topology. That change may be the result of a local link failure or
restoration, or a change in the local routing table due to an update
received from another node. Thus changes to the state of a link will
ripple through the network until routing converges again.

To understand what happens when a node detects a link failure, consider
what happens when F detects that its link to G has failed. First, F sets
its new distance to G to infinity and passes that information along
to A. Since A knows that its 2-hop path to G is through F, A would also
set its distance to G to infinity. However, with the next update from C,
A would learn that C has a 2-hop path to G. Thus, A would know that it
could reach G in 3 hops through C, which is less than infinity, and so A
would update its table accordingly. When it advertises this to F, node F
would learn that it can reach G at a cost of 4 through A, which is less
than infinity, and the system would again become stable.

Unfortunately, slightly different circumstances can prevent the network
from stabilizing. Suppose, for example, that the link from A to E goes
down. In the next round of updates, A advertises a distance of infinity
to E, but B and C advertise a distance of 2 to E. Depending on the exact
timing of events, the following might happen: Node B, upon hearing that
E can be reached in 2 hops from C, concludes that it can reach E in
3 hops and advertises this to A; node A concludes that it can reach E in
4 hops and advertises this to C; node C concludes that it can reach E in
5 hops; and so on. This cycle stops only when the distances reach some
number that is large enough to be considered infinite. In the meantime,
none of the nodes actually knows that E is unreachable, and the routing
tables for the network do not stabilize. This situation is known as the
*count to infinity* problem.

There are several partial solutions to this problem. The first one is to
use some relatively small number as an approximation of infinity. For
example, we might decide that the maximum number of hops to get across a
certain network is never going to be more than 15, and so we could pick
16 as the value that represents infinity. This at least bounds the
amount of time that it takes to count to infinity. Of course, it could
also present a problem if our network grew to a point where some nodes
were separated by more than 15 hops.

One technique to improve the time to stabilize routing is called *split
horizon*. The idea is that when a node sends a routing update to its
neighbors, it does not send those routes it learned from each neighbor
back to that neighbor. For example, if B has the route (E, 2, A) in its
table, then it knows it must have learned this route from A, and so
whenever B sends a routing update to A, it does not include the route
(E, 2) in that update. In a stronger variation of split horizon, called
*split horizon with poison reverse*, B actually sends that route back to
A, but it puts negative information in the route to ensure that A will
not eventually use B to get to E. For example, B sends the route (E, ∞)
to A. The problem with both of these techniques is that they only work
for routing loops that involve two nodes. For larger routing loops, more
drastic measures are called for. Continuing the above example, if B and
C had waited for a while after hearing of the link failure from A before
advertising routes to E, they would have found that neither of them
really had a route to E. Unfortunately, this approach delays the
convergence of the protocol; speed of convergence is one of the key
advantages of link-state routing.

|Routing|.4.1 Implementation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The code that implements this algorithm is very straightforward; we give
only some of the basics here. Structure ``Route`` defines each entry in
the routing table, and constant ``MAX_TTL`` specifies how long an entry
is kept in the table before it is discarded.\ [#]_

.. [#] You may have noticed we have been talking about how to build a
       "routing table", rather than a "forwarding table", throughout
       this chapter. This is because the table we are building is
       structured to support discovering routes, and not for doing
       fast lookup operations. Once we have decided on a route,
       information from the routing table is installed in the
       forwarding table, as outlined in Chapter |Tech|.

.. code-block:: c

   #define MAX_ROUTES      128     /* maximum size of routing table */
   #define MAX_TTL         120     /* time (in seconds) until route expires */

   typedef struct {
       NodeAddr  Destination;    /* address of destination */
       NodeAddr  NextHop;        /* address of next hop */
       int        Cost;          /* distance metric */
       u_short   TTL;            /* time to live */
   } Route;

   int      numRoutes = 0;
   Route    routingTable[MAX_ROUTES];

The routine that updates the local node’s routing table based on a new
route is given by ``mergeRoute``. Although not shown, a timer function
periodically scans the list of routes in the node’s routing table,
decrements the ``TTL`` (time to live) field of each route, and discards
any routes that have a time to live of 0. Notice, however, that the
``TTL`` field is reset to ``MAX_TTL`` any time the route is reconfirmed
by an update message from a neighboring node.

.. code-block:: c

   void
   mergeRoute (Route *new)
   {
       int i;

       for (i = 0; i < numRoutes; ++i)
       {
           if (new->Destination == routingTable[i].Destination)
           {
               if (new->Cost + 1 < routingTable[i].Cost)
               {
                   /* found a better route: */
                   break;
               } else if (new->NextHop == routingTable[i].NextHop) {
                   /* metric for current next-hop may have changed: */
                   break;
               } else {
                   /* route is uninteresting---just ignore it */
                   return;
               }
           }
       }
       if (i == numRoutes)
       {
           /* this is a completely new route; is there room for it? */
           if (numRoutes < MAX_ROUTES)
           {
               ++numRoutes;
           } else {
               /* can't fit this route in table so give up */
               return;
           }
       }
       routingTable[i] = *new;
       /* reset TTL */
       routingTable[i].TTL = MAX_TTL;
       /* account for hop to get to next node */
       ++routingTable[i].Cost;
   }

Finally, the procedure ``updateRoutingTable`` is the main routine that
calls ``mergeRoute`` to incorporate all the routes contained in a
routing update that is received from a neighboring node.

.. code-block:: c

   void
   updateRoutingTable (Route *newRoute, int numNewRoutes)
   {
       int i;

       for (i=0; i < numNewRoutes; ++i)
       {
           mergeRoute(&newRoute[i]);
       }
   }

|Routing|.4.2 Routing Information Protocol
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

One of the more widely used routing protocols in early IP networks was
the Routing Information Protocol (RIP). Its widespread use was due in
no small part to the fact that it was distributed along with the
popular Berkeley Software Distribution (BSD) version of Unix, from
which many commercial versions of Unix were derived. It is also
extremely simple. RIP is the canonical example of a routing protocol
built on the distance-vector algorithm just described. Today it is
less common, although a commercial implementation of distance-vector
known as EIGRP (Enhanced Interior Gateway Routing Protocol) remains
popular in enterprise networks.

Routing protocols in real networks differ very slightly from the
idealized graph model described above. In an internetwork, the goal of
the routers is to learn how to forward packets to various *networks*.
Thus, rather than advertising the cost of reaching other routers, the
routers advertise the cost of reaching networks, which are identified by
prefixes.

.. _fig-rip:
.. figure:: routing/figures/f03-31-9780123850591.png
   :width: 300px
   :align: center

   RIPv2 packet format.

We can see evidence of this in the RIP (version 2) packet format in
:numref:`Figure %s <fig-rip>`. The majority of the packet is taken up
with ``(address, mask, distance)`` triples. However, the principles of
the routing algorithm are just the same. For example, if router A
learns from router B that network X can be reached at a lower cost via
B than via the existing next hop in the routing table, A updates the
cost and next hop information for the network number accordingly.

RIP is a straightforward implementation of
distance-vector routing. Routers running RIP send their advertisements
every 30 seconds; a router also sends an update message whenever an
update from another router causes it to change its routing table.

As we have discussed, it is generally possible to use a range of
different metrics or costs for the links in a routing protocol. RIP
takes the simplest approach, with all link costs being equal to 1,
just as in our example above. (EIGRP, by contrast, has a fairly complex
approach to calculating costs that includes both bandwidth and latency as
inputs.) Thus, RIP always tries to find the
minimum hop route. Valid distances are 1 through 15, with 16
representing infinity. This also limits RIP to running on fairly small
networks—those with no paths longer than 15 hops.



.. _key-routing-alg:
.. admonition:: Systems Thinking Takeaway

   One of the most important tradeoffs to be made in networking systems
   is between distributed versus centralized control. Distance-vector
   and link-state routing are both examples of distributed routing
   algorithms, adopting different strategies to achieve a consistent
   outcome. In distance-vector, each node talks only to its directly
   connected neighbors, but it tells them everything it has learned
   (i.e., distance to all nodes). In link-state, each node talks to
   all other nodes, via flooding, but it tells them only what it knows for sure
   (i.e., only the state of its directly connected links). In recent
   years there has been a resurgence of interest in centralized
   approaches to routing, which we discuss in the following section.

