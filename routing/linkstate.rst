4.3 Link-State Routing (OSPF)
-----------------------------

Link-state routing is the most widely used class of intradomain routing
protocol, at least in the service provider networks that make up most
of the
Internet. Distance-vector routing, which we discuss below, continues
to be used in many enterprise networks.

To start with, link-state routing assumes that every node (router) can
determine the state of links to each of its neighbors: the state can be up or
down. Determining the state is usually
achieved by sending periodic probes along the link to the neighbor;
the specifics of these probes depend on the link type and the
particular routing protocol. But somehow, the link state is determined
and changes from up to down or *vice versa* can be detected the same
way.


Each link also has a *cost* or *metric*, which is usually a configured
value. Often it is based on the bandwidth of a link, so that higher
bandwidth links have lower cost than low bandwidth links, making them
preferable when calculating paths. We will return to this topic later;
for now, we can just assume that every link has *some* cost assigned to
it and a router knows the cost of each of its connected links.

Our goal is to enable each node to obtain enough information to enable
it to find the least-cost path to any destination. The basic idea
behind link-state protocols is very simple: every node knows how to
reach its directly connected neighbors, and if we make sure that the
totality of this knowledge is disseminated to every node, then every
node will have enough knowledge of the network to build a complete map
of the network. This is clearly a sufficient condition (although not a
necessary one) for finding the shortest path to any point in the
network. Thus, link-state routing protocols rely on two mechanisms:
reliable dissemination of link-state information, and the calculation
of routes from the sum of all the accumulated link-state knowledge.

4.3.1 Reliable Flooding
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

*Reliable flooding* is the process of making sure that all the nodes
participating in the routing protocol get a copy of the link-state
information from all the other nodes. As the term *flooding* suggests,
the basic idea is for a node to send its link-state information out on
all of its directly connected links; each node that receives this
information then forwards it out on all of *its* links. This process
continues until the information has reached all the nodes in the
network.

More precisely, each node creates an update packet, also called a
*link-state packet* (LSP), which contains the following information:

-  The ID of the node that created the LSP

-  A list of directly connected neighbors of that node, with the cost of
   the link to each one

-  A sequence number

-  A time to live for this packet

The first two items are needed to enable route calculation; the last two
are used to make the process of flooding the packet to all nodes
reliable. Reliability includes making sure that you have the most recent
copy of the information, since there may be multiple, contradictory LSPs
from one node traversing the network. Making the flooding reliable has
proven to be quite difficult. (For example, an early version of
link-state routing used in the ARPANET caused that network to fail in
1981.)

Flooding works in the following way. First, the transmission of LSPs
between adjacent routers is made reliable using acknowledgments and
retransmissions, similar to what is done in reliable transport
protocols like TCP. However, it takes more than just getting the
packets to your immediate neighbor to reliably flood an LSP to all nodes
in a network.

Consider a node X that receives a copy of an LSP that originated at some
other node Y. Note that Y may be any other router in the same routing
domain as X. X checks to see if it has already stored a copy of an LSP
from Y. If not, it stores the LSP. If it already has a copy, it compares
the sequence numbers; if the new LSP has a larger sequence number, it is
assumed to be the more recent, and that LSP is stored, replacing the old
one. A smaller (or equal) sequence number would imply an LSP older (or
not newer) than the one stored, so it would be discarded and no further
action would be needed. If the received LSP was the newer one, X then
sends a copy of that LSP to all of its neighbors except the neighbor
from which the LSP was just received. The fact that the LSP is not sent
back to the node from which it was received helps to bring an end to the
flooding of an LSP. Since X passes the LSP on to all its neighbors, who
then turn around and do the same thing, the most recent copy of the LSP
eventually reaches all nodes.

.. _fig-flood:
.. figure:: routing/figures/f03-32-9780123850591.png
   :width: 500px
   :align: center

   Flooding of link-state packets: (a) LSP arrives at
   node X; (b) X floods LSP to A and C; (c) A and C flood LSP to B
   (but not X); (d) flooding is complete.

:numref:`Figure %s <fig-flood>` shows an LSP being flooded in a small
network.  Each node becomes shaded as it stores the new LSP. In
:numref:`Figure %s(a) <fig-flood>` the LSP arrives at node X, which
sends it to neighbors A and C in :numref:`Figure %s(b) <fig-flood>`. A
and C do not send it back to X, but send it on to B. Since B receives
two identical copies of the LSP, it will accept whichever arrived
first and ignore the second as a duplicate. It then passes the LSP
onto D, which has no neighbors to flood it to, and the process is
complete.

Each node generates LSPs under two circumstances. Either the expiry of
a periodic timer or a change in topology can cause a node to generate
a new LSP. However, the only topology-based reason for a node to
generate an LSP is if one of its directly connected links or immediate
neighbors has gone down. As noted above, either a link-layer-specific
alarm or the failure to receive some sort of periodic “hello” packet
can alert a node to the loss of connectivity to a neighbor (or failure
of the neighboring node itself). The link to that neighbor will be
declared down, and a new LSP will be generated to reflect this fact.

One of the important design goals of a link-state protocol’s flooding
mechanism is that the newest information must be flooded to all nodes as
quickly as possible, while old information must be removed from the
network and not allowed to circulate. In addition, it is clearly
desirable to minimize the total amount of routing traffic that is sent
around the network; after all, this is just overhead from the
perspective of those who actually use the network for their
applications. The next few paragraphs describe some of the ways that
these goals are accomplished.

One easy way to reduce overhead is to avoid generating LSPs unless
absolutely necessary. This can be done by using very long timers—often
on the order of hours—for the periodic generation of LSPs. Given that
the flooding protocol is truly reliable when topology changes, it is
safe to assume that messages saying “nothing has changed” do not need to
be sent very often.

To make sure that old information is replaced by newer information, LSPs
carry sequence numbers. Each time a node generates a new LSP, it
increments the sequence number by 1. Unlike most sequence numbers used
in protocols, these sequence numbers are not expected to wrap, so the
field needs to be quite large (say, 64 bits). If a node goes down and
then comes back up, it starts with a sequence number of 0. If the node
was down for a long time, all the old LSPs for that node will have timed
out (as described below); otherwise, this node will eventually receive a
copy of its own LSP with a higher sequence number, which it can then
increment and use as its own sequence number. This will ensure that its
new LSP replaces any of its old LSPs left over from before the node went
down.

LSPs also carry a time to live. This is used to ensure that old
link-state information is eventually removed from the network. A node
always decrements the TTL of a newly received LSP before flooding it to
its neighbors. It also “ages” the LSP over time while it is stored in the node.
When the TTL reaches 0, the node refloods the LSP (with the TTL of 0), which
is interpreted by all the nodes in the network as a signal to delete
that LSP.

4.3.2 Route Calculation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Once a given node has a copy of the LSP from every other node, it is
able to compute a complete map for the topology of the network, and from
this map it is able to decide the best route to each destination. The
question, then, is exactly how it calculates routes from this
information. The solution is based on a well-known algorithm from graph
theory—Dijkstra’s shortest-path algorithm.

We first define Dijkstra’s algorithm in graph-theoretic terms. Imagine
that a node takes all the LSPs it has received and constructs a
graphical representation of the network, in which N denotes the set of
nodes in the graph, l(i,j) denotes the non-negative cost (weight)
associated with the edge between nodes i, j in N and l(i, j) = ∞ if no
edge connects i and j. In the following description, we let s in N
denote this node, that is, the node executing the algorithm to find the
shortest path to all the other nodes in N. Also, the algorithm maintains
the following two variables: M denotes the set of nodes incorporated so
far by the algorithm, and C(n) denotes the cost of the path from s to
each node n. Given these definitions, the algorithm is defined as
follows:

::

   M = {s}
   for each n in N - {s}
       C(n) = l(s,n)
   while (N != M)
       M = M + {w} such that C(w) is the minimum for all w in (N-M)
       for each n in (N-M)
       C(n) = MIN(C(n), C(w)+l(w,n))

Basically, the algorithm works as follows. We start with M containing
this node s and then initialize the table of costs (the array ``C(n)``)
to other nodes using the known costs to directly connected nodes. We
then look for the node that is reachable at the lowest cost (w) and add
it to M. Finally, we update the table of costs by considering the cost
of reaching nodes through w. In the last line of the algorithm, we
choose a new route to node n that goes through node w if the total cost
of going from the source to w and then following the link from w to n is
less than the old route we had to n. This procedure is repeated until
all nodes are incorporated in M.

In practice, each switch computes its routing table directly from the
LSPs it has collected using a realization of Dijkstra’s algorithm called
the *forward search* algorithm. Specifically, each switch maintains two
lists, known as ``Tentative`` and ``Confirmed``. Each of these lists
contains a set of entries of the form ``(Destination, Cost, NextHop)``.
The algorithm works as follows:

1. Initialize the ``Confirmed`` list with an entry for myself; this
   entry has a cost of 0.

2. For the node just added to the ``Confirmed`` list in the previous
   step, call it node ``Next`` and select its LSP.

3. For each neighbor (``Neighbor``) of ``Next``, calculate the cost
   (``Cost``) to reach this ``Neighbor`` as the sum of the cost from
   myself to ``Next`` and from ``Next`` to ``Neighbor``.

   1. If ``Neighbor`` is currently on neither the ``Confirmed`` nor the
      ``Tentative`` list, then add ``(Neighbor, Cost, NextHop)`` to the
      ``Tentative`` list, where ``NextHop`` is the direction I go to
      reach ``Next``.

   2. If ``Neighbor`` is currently on the ``Tentative`` list, and the
      ``Cost`` is less than the currently listed cost for ``Neighbor``,
      then replace the current entry with ``(Neighbor, Cost, NextHop)``,
      where ``NextHop`` is the direction I go to reach ``Next``.

4. If the ``Tentative`` list is empty, stop. Otherwise, pick the entry
   from the ``Tentative`` list with the lowest cost, move it to the
   ``Confirmed`` list, and return to step 2.

.. _fig-lsroute:
.. figure:: routing/figures/f03-33-9780123850591.png
   :width: 350px
   :align: center

   Link-state routing: an example network.

This will become a lot easier to understand when we look at an
example.  Consider the network depicted in :numref:`Figure %s
<fig-lsroute>`. Note that, unlike our previous example, this network
has a range of different edge costs. :numref:`Table %s <tab-ls-trace>`
traces the steps for building the routing table for node D. We denote
the two outputs of D by using the names of the nodes to which they
connect, B and C. Note the way the algorithm seems to head off on
false leads (like the 11-unit cost path to B that was the first
addition to the ``Tentative`` list) but ends up with the least-cost
paths to all nodes.

.. _tab-ls-trace:
.. table:: Steps for Building Routing Table for Node D.


  +---------+-------------------+-------------------+-------------------+
  | Step    | Confirmed         | Tentative         | Comments          |
  +=========+===================+===================+===================+
  | 1       | (D,0,–)           |                   | Since D is the    |
  |         |                   |                   | only new member   |
  |         |                   |                   | of the confirmed  |
  |         |                   |                   | list, look at its |
  |         |                   |                   | LSP.              |
  +---------+-------------------+-------------------+-------------------+
  | 2       | (D,0,–)           | (B,11,B) (C,2,C)  | D’s LSP says we   |
  |         |                   |                   | can reach B       |
  |         |                   |                   | through B at cost |
  |         |                   |                   | 11, which is      |
  |         |                   |                   | better than       |
  |         |                   |                   | anything else on  |
  |         |                   |                   | either list, so   |
  |         |                   |                   | put it on         |
  |         |                   |                   | ``Tentative``     |
  |         |                   |                   | list; same for C. |
  +---------+-------------------+-------------------+-------------------+
  | 3       | (D,0,–) (C,2,C)   | (B,11,B)          | Put lowest-cost   |
  |         |                   |                   | member of         |
  |         |                   |                   | ``Tentative`` (C) |
  |         |                   |                   | onto              |
  |         |                   |                   | ``Confirmed``     |
  |         |                   |                   | list. Next,       |
  |         |                   |                   | examine LSP of    |
  |         |                   |                   | newly confirmed   |
  |         |                   |                   | member (C).       |
  +---------+-------------------+-------------------+-------------------+
  | 4       | (D,0,–) (C,2,C)   | (B,5,C) (A,12,C)  | Cost to reach B   |
  |         |                   |                   | through C is 5,   |
  |         |                   |                   | so replace        |
  |         |                   |                   | (B,11,B). C’s LSP |
  |         |                   |                   | tells us that we  |
  |         |                   |                   | can reach A at    |
  |         |                   |                   | cost 12.          |
  +---------+-------------------+-------------------+-------------------+
  | 5       | (D,0,–) (C,2,C)   | (A,12,C)          | Move lowest-cost  |
  |         | (B,5,C)           |                   | member of         |
  |         |                   |                   | ``Tentative`` (B) |
  |         |                   |                   | to ``Confirmed``, |
  |         |                   |                   | then look at its  |
  |         |                   |                   | LSP.              |
  +---------+-------------------+-------------------+-------------------+
  | 6       | (D,0,–) (C,2,C)   | (A,10,C)          | Since we can      |
  |         | (B,5,C)           |                   | reach A at cost 5 |
  |         |                   |                   | through B,        |
  |         |                   |                   | replace the       |
  |         |                   |                   | ``Tentative``     |
  |         |                   |                   | entry.            |
  +---------+-------------------+-------------------+-------------------+
  | 7       | (D,0,–) (C,2,C)   |                   | Move lowest-cost  |
  |         | (B,5,C) (A,10,C)  |                   | member of         |
  |         |                   |                   | ``Tentative`` (A) |
  |         |                   |                   | to ``Confirmed``, |
  |         |                   |                   | and we are all    |
  |         |                   |                   | done.             |
  +---------+-------------------+-------------------+-------------------+

The link-state routing algorithm has many nice properties: It has been
proven to stabilize quickly, it does not generate much traffic, and it
responds rapidly to topology changes or node failures. On the downside,
the amount of information stored at each node (one LSP for every other
node in the network) can be quite large. This is one of the fundamental
problems of routing and is an instance of the more general problem of
scalability. Some solutions to both the specific problem (the amount of
storage potentially required at each node) and the general problem
(scalability) will be discussed in the next section.


The Open Shortest Path First Protocol (OSPF)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

There are two widely used link-state routing protocols: OSPF and IS-IS.
Both are open standards: OSPF was created under the auspices of the Internet
Engineering Task Force (IETF), while  IS-IS is an ISO
standard. At a high level, both protocols follow the approach
described above. We provide some more details of OSPF to illustrate the
amount of complexity that goes into bridging between theory and
practice in the world of routing.

OSPF adds quite a number of features to the
basic link-state algorithm described above, including the following:

-  *Authentication of routing messages*—One feature of distributed
   routing algorithms is that they disperse information from one node to
   many other nodes, and the entire network can thus be impacted by bad
   information from one node. For this reason, it’s a good idea to be
   sure that all the nodes taking part in the protocol can be trusted.
   Authenticating routing messages helps achieve this. Early versions of
   OSPF used a simple 8-byte password for authentication. This is not a
   strong enough form of authentication to prevent dedicated malicious
   users. Strong cryptographic authentication was later added.

-  *Additional hierarchy*—Hierarchy is one of the fundamental tools used
   to make systems more scalable. OSPF introduces another layer of
   hierarchy into routing by allowing a domain to be partitioned into
   *areas*. This means that a router within a domain does not
   necessarily need to know how to reach every network within that
   domain—it may be able to get by knowing only how to get to the right
   area. Thus, there is a reduction in the amount of information that
   must be transmitted to and stored in each node.

-  *Load balancing*—OSPF allows multiple routes to the same place to be
   assigned the same cost and will cause traffic to be distributed
   evenly over those routes, thus making better use of the available
   network capacity.

.. _fig-ospf:
.. figure:: routing/figures/f03-34-9780123850591.png
   :width: 400px
   :align: center

   OSPF header format.

There are several different types of OSPF messages, but all begin with
the same header, as shown in :numref:`Figure %s <fig-ospf>`. The
``Version`` field is currently set to 2, and the ``Type`` field may
take the values 1 through 5. The ``SourceAddr`` identifies the sender
of the message, and the ``AreaId`` is a 32-bit identifier of the area
in which the node is located. The entire packet, except the
authentication data, is protected by a 16-bit checksum using the same
algorithm as the IP header. The ``Authentication type`` is 0 if no
authentication is used; otherwise, it may be 1, implying that a simple
password is used, or 2, which indicates that a cryptographic
authentication checksum is used. In the latter cases, the
``Authentication`` field carries the password or cryptographic
checksum.

Of the five OSPF message types, type 1 is the “hello” message, which a
router sends to its peers to notify them that it is still alive and
connected as described above. The remaining types are used to request,
send, and acknowledge the receipt of link-state messages. The basic
building block of link-state messages in OSPF is the link-state
advertisement (LSA). One message may contain many LSAs. We provide a few
details of the LSA here.

Like any internetwork routing protocol, OSPF must provide information
about how to reach networks. Thus, OSPF must provide a little more
information than the simple graph-based protocol described above.
Specifically, a router running OSPF may generate link-state packets that
advertise one or more of the networks that are directly connected to
that router. In addition, a router that is connected to another router
by some link must advertise the cost of reaching that router over the
link. These two types of advertisements are necessary to enable all the
routers in a domain to determine the cost of reaching all networks in
that domain and the appropriate next hop for each network.

.. _fig-ospf-lsa:
.. figure:: routing/figures/f03-35-9780123850591.png
   :width: 450px
   :align: center

   OSPF link-state advertisement.

:numref:`Figure %s <fig-ospf-lsa>` shows the packet format for a
type 1 link-state advertisement. Type 1 LSAs advertise the cost of
links between routers.  Type 2 LSAs are used to advertise networks to
which the advertising router is connected, while other types are used
to support additional hierarchy as described in the next section. Many
fields in the LSA should be familiar from the preceding
discussion. The ``LS Age`` is the equivalent of a time to live, except
that it counts up and the LSA expires when the age reaches a defined
maximum value. The ``Type`` field tells us that this is a type 1 LSA.

In a type 1 LSA, the ``Link state ID`` and the ``Advertising router``
field are identical. Each carries a 32-bit identifier for the router
that created this LSA. While a number of assignment strategies may be
used to assign this ID, it is essential that it be unique in the routing
domain and that a given router consistently uses the same router ID. One
way to pick a router ID that meets these requirements would be to pick
the lowest IP address among all the IP addresses assigned to that
router. (Recall that a router may have a different IP address on each of
its interfaces.)

The ``LS sequence number`` is used exactly as described above to detect
old or duplicate LSAs. The ``LS checksum`` is similar to others we have
seen in other protocols; it is, of course, used to verify that data has
not been corrupted. It covers all fields in the packet except
``LS Age``, so it is not necessary to recompute a checksum every time
``LS Age`` is incremented. ``Length`` is the length in bytes of the
complete LSA.

Now we get to the actual link-state information.  Ignoring ``TOS``
information for a moment, each link in the LSA is represented by a
``Link ID``, some ``Link Data``, and a ``metric``. The first two of
these fields identify the link; a common way to do this would be to
use the router ID of the router at the far end of the link as the
``Link ID`` and then use the ``Link Data`` to disambiguate among
multiple parallel links if necessary. The ``metric`` is of course the
cost of the link. ``Link type`` tells us something about the link—for
example, if it is a point-to-point link.

The TOS information is mostly present for historical reasons. In
theory it was possible for OSPF to choose different routes for IP
packets based on the value in their Type Of Service (TOS) field, e.g.,
to send low-latency packets over a different path than bulk
data. However, in the decades since OSPF was defined, the meaning of
the IP TOS field has changed and this capability has not been widely
deployed.
