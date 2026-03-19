7.2 Sharing Routes and Routing Policy
---------------------------------------

Each AS has one or more *border routers* through which packets enter and
leave the AS. In our simple example in :numref:`Figure %s <fig-autonomous>`,
routers R2 and R4 would be border routers. (Over the years, routers have
sometimes also been known as *gateways*, hence the names of the
protocols BGP and EGP). A border router is simply an IP router that is
charged with the task of forwarding packets between autonomous systems.

Each AS that participates in BGP must also have at least one *BGP*
speaker, a router that “speaks” BGP to other BGP speakers in other
autonomous systems. It is common to find that border routers are also
BGP speakers, but that does not have to be the case.

BGP does not belong to either of the two main classes of routing
protocols covered previously, distance-vector or link-state. Unlike
these protocols, BGP advertises *complete paths* as an enumerated list
of autonomous systems to reach a particular network. It is sometimes
called a *path-vector* protocol for this reason. The advertisement of
complete paths is necessary to enable the sorts of policy decisions
described above to be made in accordance with the wishes of a
particular AS. It also enables routing loops to be readily
detected. The number of AS hops in a path *may* be used as a routing
metric, particularly if there is more than one policy-compliant path
to choose from.

.. _fig-bgpeg:
.. figure:: policy/figures/f04-05-9780123850591.png
   :width: 500px
   :align: center

   Example of a network running BGP.

To see how this works, consider the simple example network in
:numref:`Figure %s <fig-bgpeg>`.  A BGP speaker for the
AS of provider A (AS 2) would advertise reachability
information for each of the network numbers assigned to customers P
and Q. Thus, it would say, in effect, *“Networks 128.96, 192.4.153,
192.4.32, and 192.4.3 can be reached directly from AS 2.”* The backbone
network, on receiving this advertisement, can advertise, *“Networks
128.96, 192.4.153, 192.4.32, and 192.4.3 can be reached along the path
(AS 1, AS 2).”* Similarly, it could advertise, *“Networks 192.12.69,
192.4.54, and 192.4.23 can be reached along the path (AS 1, AS 3).”*

.. _fig-aspath:
.. figure:: policy/figures/f04-06-9780123850591.png
   :width: 500px
   :align: center

   Example of loop among autonomous systems.

An important job of BGP is to prevent the establishment of looping
paths. For example, consider the network illustrated in
:numref:`Figure %s <fig-aspath>`. It differs from :numref:`Figure %s
<fig-bgpeg>` only in the addition of an extra link between AS 2 and AS
3, but the effect now is that the graph of autonomous systems has a
loop in it. Suppose AS 1 learns that it can reach network 128.96
through AS 2, so it advertises this fact to AS 3, who in turn
advertises it back to AS 2. In the absence of any loop prevention
mechanism, AS 2 could now decide that AS 3 was the preferred route for
packets destined for 128.96. If AS 2 starts sending packets addressed
to 128.96 to AS 3, AS 3 would send them to AS 1; AS 1 would send them
back to AS 2; and they would loop forever.  This is prevented by
carrying the complete AS path in the routing messages. In this case,
the advertisement for a path to 128.96 received by AS 2 from AS 3
would contain an AS path of (AS 3, AS 1, AS 2, AS 4).  AS 2 sees
itself in this path, and thus concludes that this is not a useful path
for it to use.

In order for this loop prevention technique to work, the AS numbers
carried in BGP clearly need to be unique. For example, AS 2 can only
recognize itself in the AS path in the above example if no other AS
identifies itself in the same way. AS numbers are now 32-bits long,
and they are assigned by the same registries that allocate IP
addresses to assure uniqueness.

A given AS must only advertise routes that it uses itself. That is, if
a BGP speaker has a choice of several different routes to a
destination, it will choose the best one according to its own local
policies, and then that will be the route it advertises.  Furthermore,
a BGP speaker is not required to advertise any route to a
destination, even if it has one. This is how an AS can implement a
policy of not providing transit—by refusing to advertise routes to
prefixes that are not contained within that AS, even if it knows how
to reach them.

Given that links fail and policies change, BGP speakers need to be
able to cancel previously advertised paths. This is done with a form
of negative advertisement known as a *withdrawn route*. Both positive
and negative reachability information are carried in a BGP update
message, the format of which is shown in :numref:`Figure %s
<fig-bgpup>`. (Note that the fields in this figure are multiples of
16 bits, unlike other packet formats in this chapter.)

.. _fig-bgpup:
.. figure:: policy/figures/f04-07-9780123850591.png
   :width: 200px
   :align: center

   BGP-4 update packet format.

Unlike the routing protocols described in the previous chapter, BGP is
defined to run on top of TCP, the reliable transport protocol. Because
BGP speakers can count on TCP to be reliable, this means that any
information that has been sent from one speaker to another does not need
to be sent again. Thus, as long as nothing has changed, a BGP speaker
can simply send an occasional *keepalive* message that says, in effect,
*“I’m still here and nothing has changed.”* If that router were to crash
or become disconnected from its peer, it would stop sending the
keepalives, and the other routers that had learned routes from it would
assume that those routes were no longer valid.

7.2.1 AS Relationships and Policies
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Having said that policies may be arbitrarily complex, there turn out
to be a few common ones, reflecting common relationships between
autonomous systems. The most common relationships are illustrated in
:numref:`Figure %s <fig-as-rels>`. The three common relationships and
the policies that go with them are as follows:

.. _fig-as-rels:
.. figure:: policy/figures/f04-08-9780123850591.png
   :width: 500px
   :align: center

   Common AS relationships.

-  *Provider-Customer—*\ Providers are in the business of connecting
   their customers to the rest of the Internet. A customer might be
   a corporation, or it might be a smaller ISP (which may have customers
   of its own). So the common policy is to advertise all the routes I
   know about to my customer, and advertise routes I learn from my
   customer to everyone.

-  *Customer-Provider—*\ In the other direction, the customer wants to
   get traffic directed to them (and their customers, if they have them) by
   their provider, and wants to be able to send traffic to the rest of
   the Internet through the provider. So the common policy in this case
   is to advertise my own prefixes and routes learned from my customers
   to my provider, advertise routes learned from my provider to my
   customers, but don’t advertise routes learned from one provider to
   another provider. That last part is to make sure the customer doesn’t
   find themselves in the business of carrying traffic from one provider to
   another, which isn’t in their interests since they are paying the providers
   to carry traffic.

-  *Peer—*\ The third option is a symmetrical peering between autonomous
   systems. Two providers who view themselves as equals usually peer so
   that they can get access to each other’s customers without having to
   pay another provider. The typical policy here is to advertise routes
   learned from my customers to my peer, advertise routes learned from
   my peer to my customers, but don’t advertise routes from my peer to
   any provider or *vice versa*.

One thing to note about this figure is the way it has brought back some
structure to the apparently unstructured Internet. At the bottom of
the hierarchy we have the networks that are customers of one or
more providers, and as we move up the hierarchy we see providers who
have other providers as their customers. At the top, we have providers
who have customers and peers but are not customers of anyone. These
providers are known as the *Tier-1* providers. The Tier-1 providers
have no-one higher up to depend on, so they must carry all the
Internet's  routable prefixes in their routing tables.

.. _key-scaling:
.. admonition:: Key Takeaway

   How does all this help us to build scalable networks? First, the
   number of nodes participating in BGP is on the order of the number
   of autonomous systems, which is much smaller than the number of
   networks. Second, finding a good interdomain route is only a matter
   of finding a path to the right border router, of which there are
   only a few per AS. Thus, we have neatly subdivided the routing
   problem into manageable parts, once again using a new level of
   hierarchy to increase scalability. The complexity of interdomain
   routing is now on the order of the number of autonomous systems,
   and the complexity of intradomain routing is on the order of the
   number of networks in a single AS.

There is a robust field of research that focuses on inference of the
relationships between ASes. While the business relationships between
providers are normally kept private among the parties, it is often
possible to infer who is the customer and who is the provider among a
pair of connected ASes by observing BGP advertisements. Assessing when
a relationship is peer-to-peer is a bit harder and relies on
heuristics. The underlying data is collected by viewing BGP
advertisements from different vantage points around the Internet. One
of the early papers describing this work is from CAIDA (the Center for
Applied Internet Data Analysis). CAIDA continues to report the
inferred AS relationships data set, and the site is a wonderful source
of data and visualizations that shed light on the workings of the
Internet.

.. _reading_relations:
.. admonition:: Further Reading

   X. Dimitropoulos et al. `AS Relationships: Inference and Validation
   <https://dl.acm.org/doi/10.1145/1198255.1198259>`__.
   SIGCOMM CCR, January 2007.

   CAIDA. `Center for Applied Internet Data Analysis <https://www.caida.org>`__.

7.2.2 Integrating Interdomain and Intradomain Routing
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

While the preceding discussion illustrates how a BGP speaker learns
interdomain routing information, the question still remains as to how
all the other routers in a domain get this information. There are
several ways this problem can be addressed.

Let’s start with a simple situation, which is also very common. A
*stub* AS is one that carries only local traffic. If such an AS is
*single-homed*, i.e., it only connects to one provider, then the
border router connecting to that provider is clearly the only choice for all
routes that are outside the AS. Such a router can inject a *default
route* into the intradomain routing protocol. In effect, this is a
statement that any network that has not been explicitly advertised in
the intradomain protocol is reachable through the border router. The
default entry in the forwarding table comes after all the more specific
entries, and it matches anything that failed to match a specific entry.

The next step up in complexity is to have the border routers inject
specific routes they have learned from outside the AS. Consider, for
example, the border router of a provider AS that connects to a customer
AS. That router could learn that the network prefix 192.4.54/24 is
located inside the customer AS, either through BGP or because the
information is configured into the border router. It could inject a
route to that prefix into the routing protocol running inside the
provider AS. This would be an advertisement of the sort, *“I have a link
to 192.4.54/24 of cost X.”* This would cause other routers in the
provider AS to learn that this border router is the place to send
packets destined for that prefix.

The final level of complexity comes in larger provider networks, which learn so
much routing information from BGP that it becomes too costly to inject
it into the intradomain protocol. For example, if a border router were
to inject 10,000 prefixes that it learned about from another AS, it would
have to send very big link-state packets to the other routers in that
AS, and their shortest-path calculations would become very
complex. For this reason, the routers in a backbone network use a
variant of BGP called *interior BGP* (iBGP) to effectively redistribute
the information that is learned by the BGP speakers at the edges of the
AS to all the other routers in the AS. (The other variant of BGP,
discussed above, runs between autonomous systems and is called *exterior
BGP*, or eBGP). iBGP enables any router in the AS to learn the best
border router to use when sending a packet to any address. At the same
time, each router in the AS keeps track of how to get to each border
router using a conventional intradomain protocol with no injected
information. By combining these two sets of information, each router in
the AS is able to determine the appropriate next hop for all prefixes.

.. _fig-ibgp:
.. figure:: policy/figures/f04-09-9780123850591.png
   :width: 500px
   :align: center

   Example of interdomain and intradomain routing. All
   routers run iBGP and an intradomain routing protocol. Border
   routers A, D, and E also run eBGP to other autonomous
   systems.

To see how this all works, consider the simple example network,
representing a single AS, in :numref:`Figure %s <fig-ibgp>`. The three
border routers, A, D, and E, speak eBGP to other autonomous systems
and learn how to reach various prefixes. These three border routers
communicate with each other and with the interior routers B and C by
building a mesh of iBGP sessions among all the routers in the
AS. Let’s now focus in on how router B builds up its complete view of
how to forward packets to any prefix. Look at the top left of
:numref:`Figure %s <fig-ibgptab>`, which shows the information that
router B learns from its iBGP sessions. It learns that some prefixes
are best reached via router A, some via D, and some via E. At the same
time, all the routers in the AS are also running some intradomain
routing protocol such as Routing Information Protocol (RIP) or Open
Shortest Path First (OSPF). (A generic term for intradomain protocols
is an interior gateway protocol, or IGP.) From this completely
separate protocol, B learns how to reach other nodes *inside* the
domain, as shown in the top right table. For example, to reach router
E, B needs to send packets toward router C. Finally, in the bottom
table, B puts the whole picture together, combining the information
about external prefixes learned from iBGP with the information about
interior routes to the border routers learned from the IGP. Thus, if a
prefix like 18.0/16 is reachable via border router E, and the best
interior path to E is via C, then it follows that any packet destined
for 18.0/16 should be forwarded toward C. In this way, any router in
the AS can build up a complete routing table for any prefix that is
reachable via some border router of the AS.

.. _fig-ibgptab:
.. figure:: policy/figures/f04-10-9780123850591.png
   :width: 500px
   :align: center

   BGP routing table, IGP routing table, and combined
   table at router B.

