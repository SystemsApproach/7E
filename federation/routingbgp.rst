6.4 Sharing Routes
-------------------

In chapter 4 we saw how routes could be calculated in networks using
distributed algorithms such as link state routing and distance vector
routing. One might expect similar approaches could be applied to
routing among networks, but there are two main challenges that
complicate the problem of routing among federated networks. First
there is the issue of scale. There are tens of thousands of autonomous
systems (ASes) in the Internet and they span the globe, so its not clear that
the routing protocols we have seen to date could scale to that level
even if we treated each AS as a simple point in a graph. But more
importantly, the problem definition is not "find the shortest path to
destination X". Instead, it is "find a path to X that matches the
policies of the providers who can deliver traffic to X". Ever since
the Internet moved past the point of being a simple tree-like topology
with the NSFNET as its root, with the introduction of multiple
"backbone" providers in the 1990s, the protocol used to determine paths
among the Internet's autonomous systems has been BGP, the
Border Gateway Protocol.

The Internet is organized as autonomous systems, each of which is
under the control of a single administrative entity. A corporation’s
complex internal network might be a single AS, as may the national or regional
network of any single Internet Service Provider (ISP). :numref:`Figure
%s <fig-autonomous>` shows a simple network with two autonomous
systems.

.. _fig-autonomous:
.. figure:: federation/figures/f04-03-9780123850591.png
   :width: 400px
   :align: center

   A network with two autonomous systems.

The basic idea behind autonomous systems is (a) to allow the federated
networks of the Internet to operate independently from each other
while providing a global service (b) to support aggregation of routing
information in a large internet, thus improving scalability. We can divide
the global routing problem into two parts: routing within a single autonomous
system (which we covered in Chapter 4) and routing between autonomous
systems. Autonomous systems are also known as
routing *domains*, so we refer to the two parts of the routing problem as
interdomain routing and intradomain routing. The AS model decouples
the intradomain routing that takes place in one AS from that taking
place in another. Thus, each AS can run whatever intradomain routing
protocols it chooses. An AS can even use static routes or multiple
protocols in different parts of the AS, if desired.


6.4.1 Challenges in Interdomain Routing
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The first part of the interdomain routing
problem is to enable different ASes to share reachability
information—descriptions of the set of IP addresses that can be
reached via a given AS—with each other. Once that information is
shared, the second part of the problem is selecting paths that are
correct (that is, they lead to the destination), loop-free, and consistent with
the policies of the ASes involved.

What do we mean when we talk about the policies of an AS? They are
quite varied, but they commonly come down to whose traffic they are
willing to carry, and who they expect to carry their traffic. As
discussed in the section on address allocation for IPv6, there are
networks that function as providers and others that appear as their
customers. A mid-level ISP might be a customer of a large Tier-1
provider while also acting as the provider for smaller
customers. Customers expect their provider(s) to carry their traffic
in both directions, inbound and outbound. Providers agree to carry
traffic to and from their customers. Conversely, a customer would not
expect to deliver traffic to some unrelated prefix that is neither its
own nor belonging to one of its customers.

A simple example routing policy implemented at a particular AS might
look like this: “Whenever possible, I prefer to send traffic via AS X
than via AS Y, but I’ll use AS Y if it is the only path, and I never
want to carry traffic from AS X to AS Y or *vice versa*.” Such a
policy would be typical when I have paid money to both AS X and AS Y
to connect my AS to the rest of the Internet, and AS X is my preferred
provider of connectivity, with AS Y being the fallback. Because I view
both AS X and AS Y as providers (and presumably I paid them to play
this role), I don’t expect to help them out by carrying traffic
between them across my network (this is called *transit* traffic). The
more autonomous systems I connect to, the more complex policies
I might have, especially when you consider backbone providers, who may
interconnect with dozens of other providers and hundreds of customers
and have different economic arrangements (which affect routing
policies) with each one.

A key design goal of interdomain routing is that policies like the
example above, and much more complex ones, should be supported by the
interdomain routing system. To make the problem harder, I need to be
able to implement such a policy without any help from other autonomous
systems, and in the face of possible misconfiguration or malicious
behavior by other autonomous systems. Furthermore, there is often a
desire to keep the policies *private*, because the entities that run the
autonomous systems—mostly ISPs—are often in competition with each other
and don’t want their economic arrangements made public.

There have been two major interdomain routing protocols in the history
of the Internet. The first was the Exterior Gateway Protocol (EGP),
which had a number of limitations, perhaps the most severe of which
was that it constrained the topology of the Internet rather
significantly.  EGP was designed when the Internet had a treelike
topology, with a single backbone, and did not allow for the topology
to become more general.

The replacement for EGP was the Border Gateway Protocol (BGP), which has
iterated through four versions (we're now on BGP-4). BGP is often regarded as one of
the more complex technologies of the Internet. We’ll cover some of its high
points here.

Unlike its predecessor EGP, BGP makes virtually no assumptions about how
autonomous systems are interconnected—they form an arbitrary graph. This
model is clearly general enough to accommodate non-tree-structured
internetworks, like the simplified picture of a multi-provider Internet
shown in :numref:`Figure %s <fig-inet-1995>`. As we noted previously,
there is some structure to the Internet, but it’s nothing
like as simple as a tree, and BGP makes no assumptions about such
structure.

.. _fig-inet-1995:
.. figure:: federation/figures/f04-04-9780123850591.png
   :width: 600px
   :align: center

   A simple multi-provider Internet.

Today’s Internet consists of a richly interconnected set of networks,
mostly operated by private companies (Internet Service
Providers or ISPs). Many ISPs exist mainly to provide service to “consumers” (i.e.,
individuals with computers in their homes), while others offer
something more like the old backbone service, interconnecting other
providers and some larger corporations. Often, many providers arrange to
interconnect with each other at a single *peering point*.

To get a better sense of how we might manage routing among this complex
interconnection of autonomous systems, we can start by defining a few
terms. We define *local traffic* as traffic that originates at or
terminates on nodes within an AS, and *transit traffic* as traffic that
passes through an AS.

As noted above, scaling the routing system has been a concern for at
least three decades. An Internet backbone router must be
able to forward any packet destined anywhere in the Internet. That means
having a routing table that will provide a match for any valid IP
address. While CIDR has helped to control the number of distinct
prefixes that are carried in the Internet’s backbone routing, there is
inevitably a lot of routing information to pass around—the number of
prefixes has exceeded one million by 2025.

A further challenge in interdomain routing arises from the autonomous
nature of the domains. Each domain may run its own interior
routing protocols and use any scheme it chooses to assign metrics to
paths. This means that it is impossible to calculate meaningful path
costs for a path that crosses multiple autonomous systems. A cost of
1000 across one provider might imply a great path, but it might mean an
unacceptably bad one from another provider. As a result, interdomain
routing advertises only *reachability*. The concept of reachability is
basically a statement that “you can reach this network through this AS.”
This means that for interdomain routing to pick an optimal path is
essentially impossible.

The autonomous nature of interdomain raises issue of trust. Provider A
might be unwilling to believe certain advertisements from provider B for
fear that provider B will advertise erroneous routing information. For
example, trusting provider B when he advertises a great route to
anywhere in the Internet can be a disastrous choice if provider B turns
out to have made a mistake configuring his routers or to have
insufficient capacity to carry the traffic.

The issue of trust is also related to the need to support complex
policies as noted above. For example, I might be willing to trust a
particular provider only when he advertises reachability to certain
prefixes, and thus I would have a policy that says, “Use AS X to reach
only prefixes :math:`p` and :math:`q`, if and only if AS X advertises
reachability to those prefixes.”

6.4.2 Basics of BGP
~~~~~~~~~~~~~~~~~~~~

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
.. figure:: federation/figures/f04-05-9780123850591.png
   :width: 500px
   :align: center

   Example of a network running BGP.

To see how this works, consider the very simple example network in
:numref:`Figure %s <fig-bgpeg>`.  A BGP speaker for the
AS of provider A (AS 2) would be able to advertise reachability
information for each of the network numbers assigned to customers P
and Q. Thus, it would say, in effect, “The networks 128.96, 192.4.153,
192.4.32, and 192.4.3 can be reached directly from AS 2.” The backbone
network, on receiving this advertisement, can advertise, “The networks
128.96, 192.4.153, 192.4.32, and 192.4.3 can be reached along the path
(AS 1, AS 2).” Similarly, it could advertise, “The networks 192.12.69,
192.4.54, and 192.4.23 can be reached along the path (AS 1, AS 3).”

.. _fig-aspath:
.. figure:: federation/figures/f04-06-9780123850591.png
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
.. figure:: federation/figures/f04-07-9780123850591.png
   :width: 200px
   :align: center

   BGP-4 update packet format.

Unlike the routing protocols described in the previous chapter, BGP is
defined to run on top of TCP, the reliable transport protocol. Because
BGP speakers can count on TCP to be reliable, this means that any
information that has been sent from one speaker to another does not need
to be sent again. Thus, as long as nothing has changed, a BGP speaker
can simply send an occasional *keepalive* message that says, in effect,
“I’m still here and nothing has changed.” If that router were to crash
or become disconnected from its peer, it would stop sending the
keepalives, and the other routers that had learned routes from it would
assume that those routes were no longer valid.

6.4.3 AS Relationships and Policies
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Having said that policies may be arbitrarily complex, there turn out
to be a few common ones, reflecting common relationships between
autonomous systems. The most common relationships are illustrated in
:numref:`Figure %s <fig-as-rels>`. The three common relationships and
the policies that go with them are as follows:

.. _fig-as-rels:
.. figure:: federation/figures/f04-08-9780123850591.png
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

6.4.4 Integrating Interdomain and Intradomain Routing
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

While the preceding discussion illustrates how a BGP speaker learns
interdomain routing information, the question still remains as to how
all the other routers in a domain get this information. There are
several ways this problem can be addressed.

Let’s start with a very simple situation, which is also very common. A
*stub* AS is one that only carries local traffic. If such an AS is
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
provider AS. This would be an advertisement of the sort, “I have a link
to 192.4.54/24 of cost X.” This would cause other routers in the
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
.. figure:: federation/figures/f04-09-9780123850591.png
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
.. figure:: federation/figures/f04-10-9780123850591.png
   :width: 500px
   :align: center

   BGP routing table, IGP routing table, and combined
   table at router B.

