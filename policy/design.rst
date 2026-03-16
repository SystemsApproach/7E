7.1 Design Issues
------------------------------------

Although we have been using the term "autonomous system" somewhat
informally, the Internet defines it as a first-class abstraction. That
is, the Internet is organized as collection of *autonomous systems
(ASes)*, each of which is assigned a unique AS number. An AS
represents a single administrative entity that controls some set of
networks. A corporation’s complex internal network might be a single
AS, as may the national or regional network of any single Internet
Service Provider (ISP). :numref:`Figure %s <fig-autonomous>` shows a
simple internet with two autonomous systems.

.. _fig-autonomous:
.. figure:: policy/figures/autonomous.png
   :width: 600px
   :align: center

   An internet with two autonomous systems.

The basic idea behind autonomous systems is (a) to allow the federated
networks of the Internet to operate independently from each other
while providing a global service, and (b) to support aggregation of
routing information in a large internet, thus improving
scalability. We can divide the global routing problem into two parts:
routing within a single autonomous system (which we covered in
Chapter 4) and routing between autonomous systems. Autonomous systems
are also known as routing *domains*, so we refer to the two parts of
the routing problem as interdomain routing and intradomain
routing. The AS model decouples the intradomain routing that takes
place in one AS from that taking place in another. Thus, each AS can
run whatever intradomain routing protocols it chooses, and use
whatever system of link metrics it wants. An AS can even use static
routes or multiple protocols in different parts of the AS, if desired.

The first part of the interdomain routing problem is to enable
different ASes to share reachability information—descriptions of the
set of IP addresses that can be reached via a given AS—with each
other. Once that information is shared, the second part of the problem
is selecting paths that are correct (that is, they lead to the
destination), loop-free, and consistent with the policies of the ASes
involved.

What do we mean when we talk about the policies of an AS? They are
quite varied, but they commonly come down to whose traffic they are
willing to carry, and who they expect to carry their traffic.  There
are networks that function as providers and others that appear as
their customers. A mid-level ISP might be a customer of a large Tier-1
provider while also acting as the provider for smaller
customers. Customers expect their provider(s) to carry their traffic
in both directions, inbound and outbound. Providers agree to carry
traffic to and from their customers. Conversely, a customer would not
expect to deliver traffic to some unrelated prefix that is neither its
own nor belonging to one of its customers.

A simple example routing policy implemented at a particular AS might
look like this: *“Whenever possible, I prefer to send traffic via AS X
than via AS Y, but I’ll use AS Y if it is the only path, and I never
want to carry traffic from AS X to AS Y or vice versa.”* Such a
policy would be typical when I have paid money to both AS X and AS Y
to connect my AS to the rest of the Internet, and AS X is my preferred
provider of connectivity, with AS Y being the fallback. Because I view
both AS X and AS Y as providers (and presumably I paid them to play
this role), I don’t expect to help them out by carrying traffic
between them across my network (this is called *transit* traffic). The
more autonomous systems an AS connects to, the more complex policies
it might have, especially when you consider "backbone" providers
(those closest to the core of the Internet), who may
interconnect with dozens of other providers and hundreds of customers
and have different economic arrangements (which affect routing
policies) with each one.

A key design goal of interdomain routing is that policies like the
example above, and much more complex ones, should be supported by the
interdomain routing system. To make the problem harder, each AS needs to be
able to implement such a policy without any help from other autonomous
systems, and in the face of possible misconfiguration or malicious
behavior by other autonomous systems. Furthermore, there is often a
desire to keep the policies *private*, because the entities that run the
autonomous systems—mostly ISPs—are often in competition with each other
and don’t want their economic arrangements made public.

There have been two major interdomain routing protocols in the history
of the Internet. The first was the Exterior Gateway Protocol (EGP),
which had several limitations, the most severe being that it assumed
the Internet has a treelike topology, with a single backbone at the root.
Originally, that backbone was the ARPANET; later it was NSFNET. The
Border Gateway Protocol (BGP) replaced EGP, and it has iterated
through four versions (we're now on BGP-4). BGP is often regarded as
one of the more complex technologies of the Internet.

BGP makes virtually no assumptions about how autonomous systems are
interconnected—they form an arbitrary graph. This model is clearly
general enough to accommodate non-tree-structured internetworks, like
the simplified picture of a multi-provider Internet shown in
:numref:`Figure %s <fig-inet-1995>`. There is some structure to the
Internet, but it’s nothing like as simple as a tree, and BGP makes no
assumptions about such structure.

.. _fig-inet-1995:
.. figure:: policy/figures/f04-04-9780123850591.png
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
interconnection of autonomous systems, we start by defining a few
terms. We define *local traffic* as traffic that originates at or
terminates on nodes within an AS, and *transit traffic* as traffic that
passes through an AS.

As noted above, scaling the routing system has been a concern for more
than three decades. An Internet backbone router must be able to
forward any packet destined anywhere in the Internet. That means
having a routing table that will provide a match for any valid IP
address. While CIDR has helped to control the number of distinct
prefixes that are carried in the Internet’s routing system, there is
inevitably a lot of routing information to pass around—the number of
prefixes now exceeds one million.

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
policies as noted above. For example, a given AS might be willing to trust a
particular provider only when it advertises reachability to certain
prefixes, and thus would define a policy that says, “Use AS X to reach
only prefixes :math:`p` and :math:`q`, if and only if AS X advertises
reachability to those prefixes.”
