.. index:: NAT: Network Address Translation
.. index:: IPv6: Internet Protocol version 6

|Fed|.3 Scale
-------------

As the Internet has grown in its geographic reach, the number of
networks, and the number of hosts it connects, various aspects of its
design have come under stress. Most notably, the IPv4 address space,
while seemingly capable of addressing 4 billion hosts, was under
pressure long before that many hosts were connected. In this section
we examine the range of techniques that have been applied to enable
the Internet to keep on scaling up.


|Fed|.3.1 Subnetting and Classless Addressing
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The original "classful" design for IP addresses uses the network part
of the address to identify exactly one physical network. It turns out that this
approach has a couple of drawbacks. First is address assignment
efficiency, and the second is routing scalability.

With the original addressing scheme, each
network, no matter how small, needs at least a class C network
address. Even worse, any network with more than 255 hosts needs
a class B address. This may not seem like a big deal, and indeed it
wasn’t when the Internet was first envisioned, but there are only a
finite number of network numbers, and there are far fewer class B
addresses than class Cs. Class B addresses were in particularly
high demand because you never know if your network might expand beyond
255 nodes, so it is easier to use a class B address from the start than
to have to renumber every host when you run out of room on a class C
network. Hence the problem of address assignment inefficiency:
A network with two nodes uses an entire class C network address, thereby
wasting 253 perfectly useful addresses; a class B network with slightly
more than 255 hosts wastes over 64,000 addresses. Keep doing this for
long enough and the 4 billion IP addresses would be used up long before
there are even a billion devices on the Internet.


Now consider the impact on routing. The amount of state
stored in a node participating in a routing protocol is
proportional to the number of other nodes, and routing in an
internet consists of building up forwarding tables that tell a router
how to reach different networks. Thus, the more network numbers there
are in use, the bigger the forwarding tables get. Big forwarding tables
add costs to routers. They are also slower to construct and
to search than
smaller tables for a given technology, so they degrade router
performance. This provides another motivation for assigning network
numbers carefully.

*Subnetting* provides a first step to tackling both of these problems.
The idea is to take a single IP network
number and allocate the IP addresses with that network number to several
physical networks, which are now referred to as *subnets*. For this to
work, the subnets should be
close to each other. This is because from a distant point in the
Internet, they will all look like a single network, having only one
network number between them. This means that a router will only be able
to select one route to reach any of the subnets, so they had better all
be in the same general direction. A perfect situation in which to use
subnetting is a large campus or corporation that has many physical
networks. From outside the campus, all you need to know to reach any
subnet inside the campus is where the campus connects to the rest of the
Internet. This is often at a single point, so one entry in your
forwarding table will suffice. Even if there are multiple points at
which the campus is connected to the rest of the Internet, knowing how
to get to one point in the campus network is still a good start.

The mechanism by which a single network number can be shared among
multiple networks involves configuring all the nodes on each subnet with
a *subnet mask*. With simple IP addresses, all hosts on the same network
must have the same network number. The subnet mask enables us to
introduce a *subnet number*; all hosts on the same physical network will
have the same subnet number, which means that hosts may be on different
physical networks but share a single network number. This concept is
illustrated in :numref:`Figure %s <fig-subaddr>`.

.. _fig-subaddr:
.. figure:: federation/figures/f03-20-9780123850591.png
   :width: 350px
   :align: center

   Subnet addressing.

What subnetting means to a host is that it is now configured with both
an IP address and a subnet mask for the subnet to which it is
attached.  For example, host H1 in :numref:`Figure %s <fig-subnet>` is
configured with an address of 128.96.34.15 and a subnet mask of
255.255.255.128. (All hosts on a given subnet are configured with the
same mask; that is, there is exactly one subnet mask per subnet.) The
bitwise AND of these two numbers defines the subnet number of the host
and of all other hosts on the same subnet. In this case, 128.96.34.15
AND 255.255.255.128 equals 128.96.34.0, so this is the subnet number
for the topmost subnet in the figure.

.. _fig-subnet:
.. figure:: federation/figures/subnet.png
   :width: 700px
   :align: center

   An example of subnetting.

When the host wants to send a packet to a certain IP address, the first
thing it does is to perform a bitwise AND between its own subnet mask
and the destination IP address. If the result equals the subnet number
of the sending host, then it knows that the destination host is on the
same subnet and the packet can be delivered directly over the subnet. If
the results are not equal, the packet needs to be sent to a router to be
forwarded to another subnet. For example, if H1 is sending to H2, then
H1 ANDs its subnet mask (255.255.255.128) with the address for H2
(128.96.34.139) to obtain 128.96.34.128. This does not match the subnet
number for H1 (128.96.34.0) so H1 knows that H2 is on a different
subnet. Since H1 cannot deliver the packet to H2 directly over the
subnet, it sends the packet to its default router R1.

The forwarding table of a router also changes when we introduce
subnetting. Recall that we previously had a forwarding table that
consisted of entries of the form ``(NetworkNum, NextHop)``. To support
subnetting, the table must now hold entries of the form
``(SubnetNumber, SubnetMask, NextHop)``. To find the right entry in the
table, the router ANDs the packet’s destination address with the
``SubnetMask``\ for each entry in turn; if the result matches the
``SubnetNumber`` of the entry, then this is the right entry to use, and
it forwards the packet to the next hop router indicated. In the example
network of :numref:`Figure %s <fig-subnet>`, router R1 would have the entries
shown in :numref:`Table %s <tab-subnettab>`.

.. _tab-subnettab:
.. table:: Example Forwarding Table with Subnetting.
   :align: center
   :widths: auto

   +---------------+-----------------+-------------+
   | SubnetNumber  | SubnetMask      | NextHop     |
   +===============+=================+=============+
   | 128.96.34.0   | 255.255.255.128 | Interface 0 |
   +---------------+-----------------+-------------+
   | 128.96.34.128 | 255.255.255.128 | Interface 1 |
   +---------------+-----------------+-------------+
   | 128.96.33.0   | 255.255.255.0   | R2          |
   +---------------+-----------------+-------------+

Continuing with the example of a packet from H1 being sent to H2, R1
would AND H2’s address (128.96.34.139) with the subnet mask of the first
entry (255.255.255.128) and compare the result (128.96.34.128) with the
network number for that entry (128.96.34.0). Since this is not a match,
it proceeds to the next entry. This time a match does occur, so R1
delivers the packet to H2 using interface 1, which is the interface
connected to the same network as H2.

We can now describe the forwarding algorithm in the following
way:

::

   D = destination IP address
   for each forwarding table entry (SubnetNumber, SubnetMask, NextHop)
       D1 = SubnetMask & D
       if D1 = SubnetNumber
           if NextHop is an interface
               deliver packet directly to destination
           else
               deliver packet to NextHop (a router)

Although not shown in this example, a default route would usually be
included in the table and would be used if no explicit matches were
found. Note that this conceptual description of the algorithm,
involving repeated ANDing of the destination address with a subnet
mask and a linear table search, would be very inefficient. Efficient forwarding
algorithms, often implemented in hardware, are widely used.

An important consequence of subnetting is that different parts of the
internet see the world differently. From outside our hypothetical
campus, routers see a single network. In the example above, routers
outside the campus see the collection of networks in :numref:`Figure
%s <fig-subnet>` as just the network 128.96, and they keep one entry in
their forwarding tables to tell them how to reach it. Routers within the
campus, however, need to be able to route packets to the right subnet.
Thus, not all parts of the internet see exactly the same routing
information. This is an example of an *aggregation* of routing
information, which is fundamental to scaling of the routing system. The
next section shows how aggregation can be taken to another level.

Classless Addressing
++++++++++++++++++++

Subnetting has a counterpart, sometimes called *supernetting*, but more
often called *Classless Interdomain Routing* or CIDR, pronounced
“cider.” CIDR takes the subnetting idea to its logical conclusion by
essentially doing away with address classes altogether. Why isn’t
subnetting alone sufficient? In essence, subnetting only allows us to
split a classful address among multiple subnets, while CIDR allows us to
coalesce several classful addresses into a single “supernet.” This
further tackles the address space inefficiency noted above, and does so
in a way that keeps the routing system from being overloaded.

To see how the issues of address space efficiency and scalability of the
routing system are coupled, consider the hypothetical case of a company
whose network has 256 hosts on it. That is slightly too many for a Class
C address, so you would be tempted to assign a class B. However, using
up a chunk of address space that could address 65535 to address 256
hosts has an efficiency of only 256/65,535 = 0.39%. Even though
subnetting can help us to assign addresses carefully, it does not get
around the fact that any organization with more than 255 hosts, or an
expectation of eventually having that many, wants a class B address.

The first way you might deal with this issue would be to allocate
Class B addresses only to large organizations with tens of thousands
of hosts. Any smaller organization can have a number of class C
addresses to cover the expected number of hosts. This would
more accurately match the amount of address space consumed to
the size of the organization. For any organization with at least
256 hosts, we can guarantee an address utilization of at least 50%,
and typically much more. (Sadly, even if you can justify a request of
a class B network number, don’t bother, because they were all spoken
for long ago.)

This solution, however, raises a problem that is at least as serious:
excessive numbers of networks need to be advertised in the routing
protocols. If a single site has, say, 16 class C network numbers
assigned to it, that means every Internet backbone router needs 16
entries in its routing tables to direct packets to that site. This is
true even if the path to every one of those networks is the same. If
we had assigned a class B address to the site, the same routing
information could be stored in one table entry. However, our address
assignment efficiency would then be only 16 x 255 / 65,536 = 6.2%.

CIDR, therefore, balances the need to minimize the number of
routes that are advertised with the need to hand out
addresses efficiently. To do this, CIDR helps us to *aggregate* routes.
That is, it lets us use a single entry in a forwarding table to tell us
how to reach a lot of different networks. It does this by
breaking the rigid boundaries between address classes. CIDR works with
*prefixes* rather than networks.

Consider our organization with 16 class C
network numbers. Instead of handing out 16 addresses at random, we can
hand out a block of *contiguous* class C addresses. Suppose we assign
the class C network numbers from 192.4.16 through 192.4.31. Observe that
the top 20 bits of all the addresses in this range are the same
(``11000000 00000100 0001``). Thus, what we have effectively created is
a 20-bit network number—something that is between a class B network
number and a class C number in terms of the number of hosts that it can
support. In other words, we get both the high address efficiency of
handing out addresses in chunks smaller than a class B network, and a
single network prefix that can be used for routing. Observe
that, for this scheme to work, we need to hand out blocks of class C
addresses that share a common prefix, which means that each block must
contain a number of class C networks that is a power of two.

CIDR requires a new type of notation to represent prefixes, because the prefixes can be of any length.
The convention is to place a ``/X`` after the prefix, where ``X`` is the
prefix length in bits. So, for the example above, the 20-bit prefix for
all the networks 192.4.16 through 192.4.31 is represented as
192.4.16/20. By contrast a 24-bit prefix (a Class C in the old
terminology) is written as 192.4.16/24.
Today, with CIDR being the norm, it is more common to hear people talk
about “slash 24” prefixes than class C networks. Note that representing
a network address in this way is similar to the\ ``(mask, value)``
approach used in subnetting, as long as ``masks`` consist of contiguous
bits starting from the most significant bit (which in practice is almost
always the case).

.. _fig-cidreg:
.. figure:: federation/figures/f03-22-9780123850591.png
   :width: 500px
   :align: center

   Route aggregation with CIDR.

Aggregation of routes is not limited to the case of a campus. Consider
an Internet service provider who provides connectivity to a large
number of customers. It is standard practice now for such an ISP to
assign prefixes to customers in such a way that many different
customer networks share a common, shorter address prefix.  Consider
the example in :numref:`Figure %s <fig-cidreg>`. Assume that eight
customers served by the provider network have each been assigned
adjacent 24-bit network prefixes. Those prefixes all start with the
same 21 bits. Since all of the customers are reachable through the
same provider network, it can advertise a single route to all of them
by just advertising the common 21-bit prefix they share. And it can do
this even if not all the 24-bit prefixes have been handed out, as long
as the provider ultimately *will* have the right to hand out those
prefixes to a customer. To achieve this, Internet address registrars
allocate contiguous chunks of address space to providers in
advance. Providers then assign addresses from that space to their customers
as needed. Note that there is no
need for all customer prefixes to be the same length.

IP Forwarding Revisited
++++++++++++++++++++++++

In our discussion of IP forwarding so far, we have assumed that we
could find the network number in a packet and then look up that number
in a forwarding table. CIDR means that "network numbers" (prefixes)
may be of any length, from 2 to 32 bits (at least in
theory). Furthermore, it is sometimes possible to have prefixes in the
forwarding table that “overlap,” in the sense that some addresses may
match more than one prefix. For example, we might find both 171.69/16
(a 16-bit prefix) and 171.69.10/24 (a 24-bit prefix) in the forwarding
table of a single router. In this case, a packet destined to, say,
171.69.10.5 clearly matches both prefixes. The rule in this case is
based on the principle of “longest match”; that is, the packet matches
the longest prefix, which would be 171.69.10 in this example. On the
other hand, a packet destined to 171.69.20.5 would match 171.69 and
*not* 171.69.10, and in the absence of any other matching entry in the
routing table 171.69 would be the longest match.

The task of efficiently finding the longest match between an IP
address and the variable-length prefixes in a forwarding table has
been a fruitful field of research for many years. The most well-known
algorithm uses an approach known as a *PATRICIA tree*, which was
actually developed well in advance of CIDR. High-end switches, as
described in Chapter |Tech|, offer a hardware-based alternative, often by
performing address lookups in Ternary Content Addressable Memory
(TCAM). This supports constant-time lookups, but TCAM is often limited
in size. This means it is often treated as a cache of the most
frequently used forwarding entries; a fallback lookup algorithm is
still needed.


|Fed|.3.2 Network Address Translation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Network Address Translation (NAT) has a long and somewhat
controversial history. It seems the idea was independently invented by
a few people, but the first publication describing it was by Paul
Tsuchiya and Tony Eng in 1993. It was conceived at around the same
time that work was getting started on IP version 6, and to tackle the
same basic issues: address space depletion and scalability of
routing.

The aspect of NAT that probably caused the most controversy is that it
gives up on one of the core properties of the Internet Protocol: a
global address space in which every device has its own
address. Instead, NAT uses a pool of local addresses in certain areas
of the Internet, such as a home or corporate network, and then maps
local addresses to global addresses at the border of this network, as
illustrated in :numref:`Figure %s <fig-nat>`. These local addresses
are not globally unique. When we mentioned that this chapter was
written on a computer with the address ``192.168.68.69``, some readers
may have noticed that this number comes from the "private" range
``192.168.0.0/16``. That is because the computer is sitting in a home
network that is separated from the public Internet by a NAT
device—much like the majority of home computers in use today.

At the time of NAT's invention, most hosts connected to the Internet
were inside corporate and university campus networks.  Most of the
communication was among hosts inside such a network, with only a small
amount of communication taking place with hosts outside the campus
network. Thus, the hosts in the campus network could mostly get by
with addresses that were only locally unique. When some host needed to
communicate with the broader Internet, e.g. to access a web site, it
could be dynamically given a globally unique address from a pool
reserved for that purpose. Thus, a corporate network with hundreds of
hosts could use private addresses for its hosts.  A handful of global
addresses would suffice for externally bound traffic. The local
addresses could be used over and over again in different corporations,
thus conserving the pool of IP addresses, and they would never be
advertised into the Internet backbone, thus helping with the
scalability of routing. These addresses for local use are now known as
private addresses, with ``192.168.0.0/16`` being one prefix in the
private space.

A subsequent development reduced the requirement from a pool of global
addresses per campus to a single global address, and this is how much
of the Internet is constructed today.  The vast majority of
traffic running over IP is either TCP or UDP, both of which have a
16-bit source port and 16-bit destination port (for reasons we will
discuss in Chapter |TCP|). So it is possible for many different hosts in
the private network to *simultaneously* use the same global address
for external communication by using the TCP or UDP port field as a key
to distinguish one host from another. This is probably best explained
with an example.



.. _fig-nat:
.. figure:: federation/figures/NAT.png
   :width: 500px
   :align: center

   A NAT operation in progress. 1. Original packet from
   client. 2. Packet destined for server after NAT. 3. Reply from
   server. 4. Translated reply packet continues back to client.



When the client tries to send a packet to the destination server, it
puts its own local, private address in the source address field, and
the server's public address in the destination address field. It uses
a random source port and the designated port (80) for HTTP
traffic. When the packet reaches the NAT device, it is intercepted and
the source address is replaced with the external, public address
assigned to the NAT device. The source port is also modified; the only
requirement is that the NAT use a different source port for every
active session. The mapping from local source address and port to the
external values is stored in a table. When the packet reaches the
server, it replies by copying the source address and port into the
destination fields of the packet and sending it back to the NAT. When
the NAT receives the packet, it uses its mapping table to map the
destination fields back to the correct values to return the packet to
the original client.

The NAT table is basically a cache of currently active "flows". So if
the client keeps talking to the same server, the same table entry will
keep being used. At some point, if the client stops sending traffic to
the server or the server stops replying, the NAT table entry can be
aged out of the cache.

Why does it matter that NAT gives up on globally unique addresses? One
problem is that it now becomes difficult to initiate a connection to a
device behind a NAT, because you don't know how to address it, and
NAT devices typically won't create new table entries for flows that
start outside the NAT. So, it's not easy to run a server out of your
home network, for example. Establishing direct IP connectivity between
two devices that are both behind NATs is hard, since neither has a
public address to get the connection established. Techniques to work
around these problems have been developed (STUN is an IETF standard
for NAT traversal, for example) but certainly this is a far cry from the simple,
global addressing model of the original Internet.

The path back to globally unique addresses required an expansion of
the IP address space, which is exactly what IP version 6 provides. You
can read about the initial design of NAT in the 1993 paper below; Paul
Francis and Paul Tsuchiya are the same person (he changed his last
name) and he wrote a short piece looking at why NAT was so widely
adopted in 2015.



.. _reading_nat:
.. admonition:: Further Reading

   P. Tsuchiya and T. Eng.  `Extending the IP Internet Through Address
   Reuse. <https://dl.acm.org/doi/10.1145/173942.173944>`__. ACM SIGCOMM
   CCR, January 1993.

   P. Francis. `Network Address Translation (NAT)
   <https://dl.acm.org/doi/10.1145/2766330.2766340>`__. ACM SIGCOMM
   CCR, April 2015.

|Fed|.3.3 IP Version 6
~~~~~~~~~~~~~~~~~~~~~~

The motivation for defining a new version of IP is simple: to deal
with exhaustion of the IP address space. CIDR helped considerably to
contain the rate at which the Internet address space was being
consumed and also helped to control the growth of routing table
information needed in the Internet’s routers. However, these
techniques are no longer adequate. In particular, it is
impossible to achieve 100% address utilization efficiency, so the
address space was consumed well before the 4 billionth host was
connected to the Internet. Even if we were able to use all 4 billion
addresses, we are well past that number of hosts now, admittedly many
of them sitting behind NAT devices.  With the clarity of 20/20 hindsight,
a 32-bit address space was quite small.

The IETF began looking at the problem of expanding the IP address
space in 1991, and several alternatives were proposed. Larger
addresses require a new version of the Internet Protocol and hence new
software for every host and router in the Internet. This is clearly
not a trivial matter—it is a major change that needs to be thought
about very carefully.

Originally known as IP Next
Generation, or IPng, once an official IP version
number was assigned, IPng became IPv6. The existing version which we
have discussed above is IPv4. The apparent
discontinuity in numbering is the result of version number 5 being used
for a discontinued experimental protocol many years ago.

A common view in the IETF at the time was that this was a
once-in-a-generation change in which many other problems with IP
beyond the address space could be addressed. Consequently, the IETF
solicited white papers asking for
input on the features that might be desired in a new version of IP. In
addition to the need to accommodate scalable routing and addressing,
some of the other wish list items for IPng included:

-  Support for real-time services

-  Security support

-  Autoconfiguration (i.e., the ability of hosts to automatically
   configure themselves with such information as their own IP address
   and domain name)

-  Enhanced routing functionality, including support for mobile hosts

It is interesting to note that, while many of these features were absent
from IPv4 at the time IPv6 was being designed, support for all of them
has made its way into IPv4 in recent years, often using similar
techniques in both protocols. It can be argued that the freedom to think
of IPv6 as a clean slate facilitated the design of new capabilities for
IP that were then retrofitted into IPv4.

In addition to the wish list, one absolutely non-negotiable feature
for IPv6 was that there must be a transition plan to move from IPv4 to
IPv6. With the Internet being so large and having no centralized
control, it would be completely impossible to have a “flag day” on
which everyone shut down their hosts and routers and installed a new
version of IP. The architects expected a long transition period in
which some hosts and routers would run IPv4 only, some will run IPv4
and IPv6, and some will run IPv6 only. It is unclear if they
anticipated that the transition period would extend beyond 30 years.

IPv6 Addresses and Routing
++++++++++++++++++++++++++++++

First and foremost, IPv6 provides a 128-bit address space, as opposed
to the 32 bits of version 4. Thus, while version 4 can potentially
address 4 billion nodes if address assignment efficiency reaches 100%,
IPv6 can address 3.4 × 10\ :sup:`38` nodes, again assuming 100%
efficiency.  As we have seen, though, 100% efficiency in address
assignment is impossible. Some analysis of other addressing schemes,
such as those of the French and U.S. telephone networks, as well as
that of IPv4, have turned up some empirical numbers for address
assignment efficiency. Based on the most pessimistic estimates of
efficiency drawn from this study, the IPv6 address space is predicted
to provide over 1500 addresses per square foot of the Earth’s surface,
which certainly seems like it should serve us well even when toasters
on Mars have IP addresses.

Address Space Allocation
++++++++++++++++++++++++++

Drawing on the effectiveness of CIDR in IPv4, IPv6 addresses are also
classless, but the address space is still subdivided in various ways
based on the leading bits. Rather than specifying different address
classes, the leading bits specify different uses of the IPv6 address.
The current assignment of prefixes is listed in :numref:`Table %s
<tab-v6tab>`.

.. _tab-v6tab:
.. table:: Address Prefix Assignments for IPv6.
   :align: center
   :widths: auto

   +-----------------+---------------------+
   | Prefix          | Use                 |
   +=================+=====================+
   | 00…0 (128 bits) | Unspecified         |
   +-----------------+---------------------+
   | 00…1 (128 bits) | Loopback            |
   +-----------------+---------------------+
   | 1111 1111       | Multicast addresses |
   +-----------------+---------------------+
   | 1111 1110 10    | Link-local unicast  |
   +-----------------+---------------------+
   | Everything else | Global Unicast      |
   +-----------------+---------------------+

This allocation of the address space warrants a little discussion.
First, the entire functionality of IPv4’s three main address classes (A,
B, and C) is contained inside the “everything else” range. Global
Unicast Addresses are a lot like classless IPv4
addresses, only much longer. These are the main ones of interest at this
point, with over 99% of the total IPv6 address space available to this
important form of address.

The multicast address space is (obviously) for multicast, thereby
serving the same role as class D addresses in IPv4. Note that multicast
addresses are easy to distinguish—they start with a byte of all 1s. We
will see how these addresses are used in a later section.

The idea behind link-local use addresses is to enable a host to
construct an address that will work on the network to which it is
connected without being concerned about the global uniqueness of the
address. This may be useful for autoconfiguration, as we will see below.
Similarly, the site-local use addresses are intended to allow valid
addresses to be constructed on a site (e.g., a private corporate
network) that is not connected to the larger Internet; again, global
uniqueness need not be an issue.

Within the global unicast address space are some important special types
of addresses. A node may be assigned an IPv4-compatible IPv6 address by
zero-extending a 32-bit IPv4 address to 128 bits. A node that is only
capable of understanding IPv4 can be assigned an IPv4-mapped IPv6
address by prefixing the 32-bit IPv4 address with 2 bytes of all 1s and
then zero-extending the result to 128 bits. These two special address
types have uses in the IPv4-to-IPv6 transition (see the sidebar on this
topic).

Address Notation
+++++++++++++++++++

Just as with IPv4, there is some special notation for writing down IPv6
addresses. The standard representation is ``x:x:x:x:x:x:x:x``, where
each ``x`` is a hexadecimal representation of a 16-bit piece of the
address. An example would be

::

   47CD:1234:4422:AC02:0022:1234:A456:0124

Any IPv6 address can be written using this notation. Since there are a
few special types of IPv6 addresses, there are some special notations
that may be helpful in certain circumstances. For example, an address
with a large number of contiguous 0s can be written more compactly by
omitting all the 0 fields. Thus,

::

   47CD:0000:0000:0000:0000:0000:A456:0124

could be written

::

   47CD::A456:0124

Clearly, this form of shorthand can only be used for one set of
contiguous 0s in an address to avoid ambiguity.

The two types of IPv6 addresses that contain an embedded IPv4 address
have their own special notation that makes extraction of the IPv4
address easier. For example, the IPv4-mapped IPv6 address of a host
whose IPv4 address was 128.96.33.81 could be written as

::

   ::FFFF:128.96.33.81

That is, the last 32 bits are written in IPv4 notation, rather than as a
pair of hexadecimal numbers separated by a colon. Note that the double
colon at the front indicates the leading 0s.

Global Unicast Addresses
+++++++++++++++++++++++++

By far the most important sort of addressing that IPv6 must provide is
plain old unicast addressing. It must do this in a way that supports the
rapid rate of addition of new hosts to the Internet and that allows
routing to be done in a scalable way as the number of physical networks
in the Internet grows. Thus, at the heart of IPv6 is the unicast address
allocation plan that determines how unicast addresses will be assigned
to service providers, autonomous systems, networks, hosts, and routers.

As with CIDR, the goal of the IPv6 address allocation plan is to provide
aggregation of routing information to reduce the burden on intradomain
routers. Again, the key idea is to use an address prefix—a set of
contiguous bits at the most significant end of the address—to aggregate
reachability information to a large number of networks and even to a
large number of autonomous systems.

The main way to achieve this is to allocate addresses in a hierarchy
that loosely follows the structure of the Internet. While the Internet is far
from a simple tree structure, it nevertheless displays a sort of
hierarchy with "Tier-1" service providers at the top, lower tier
providers below them, and end customers, both business and
residential, at the leaves of the hierarchy.

Roughly speaking, then, large blocks of address space are allocated to
providers. Those providers can then allocate prefixes within their
block to their customers, who may themselves be providers to other
customers lower in the hierarchy. This ensures that routing
advertisements can be aggregated. A provider can advertise
reachability for a single prefix that covers all their customers. We
will look more closely at how these advertisements work in the next
section. This approach was adopted in IPv4 after the introduction of
CIDR; for IPv6 it has been applied from the start, allowing a cleaner
approach.

The drawback of having providers allocate address blocks to their
customers is that, if a customer decides to change providers, then it
will no longer have an address prefix that is part of the range
allocated to its provider.  For residential customers, moving to a new
provider is likely to just mean they get a whole new set of
addresses. Thanks to the dynamic way in which addresses are assigned
today, they mostly won't be greatly bothered by this. But larger
customers, either corporate networks or those customers who are
themselves providers, generally won't be willing or able to change
their allocated addresses. Thus, one would expect over time to see
that more prefixes appear in the global routing tables due to
customers moving to new providers, independent of the growth of
Internet.

.. _fig-v6addr:
.. figure:: federation/figures/f04-11-9780123850591.png
   :width: 500px
   :align: center

   An IPv6 provider-based unicast address.

One place where aggregation works well is at the national or
continental level. Continental boundaries and oceans form natural divisions in
the Internet topology—submarine fibers are expensive and hence there are
relatively few of them. If all addresses in Europe, for example, had a few
common prefixes, then a great deal of aggregation could be done, and
most routers in other continents would only need one routing table
entry for all networks with the Europe prefix. Providers in Europe
would all select their prefixes such that they began with the European
prefixes. Using this scheme, an IPv6 address might look like
:numref:`Figure %s <fig-v6addr>`. The ``RegistryID`` might be an
identifier assigned to a European address registry, with different IDs
assigned to other continents or countries.  Note that prefixes would
be of different lengths under this scenario.  For example, a provider
with few customers could have a longer prefix (and thus less total
address space available) than one with many customers. There are
regional internet registries for each continent today as well as some
smaller registries at a national level and they form the basis for hierarchical
address allocation.



IPv6 Packet Format
+++++++++++++++++++

Despite the fact that IPv6 extends IPv4 in several ways, its header
format is actually simpler. This simplicity is due to a concerted effort
to remove unnecessary functionality from the protocol. :numref:`Figure
%s <fig-v6header>` shows the result.

The ``Version`` field is in the same place relative
to the start of the header as IPv4’s ``Version`` field so that
header-processing software can immediately decide which header format to
look for. The ``TrafficClass`` and ``FlowLabel`` fields both relate to
quality of service issues.

The ``PayloadLen`` field gives the length of the packet, excluding the
IPv6 header, measured in bytes. The ``NextHeader`` field cleverly
replaces both the IP options and the ``Protocol`` field of IPv4. If
options are required, then they are carried in one or more special
headers following the IP header, and this is indicated by the value of
the ``NextHeader`` field. If there are no special headers, the
``NextHeader`` field is the demux key identifying the higher-level
protocol running over IP (e.g., TCP or UDP); that is, it serves the
same purpose as the IPv4 ``Protocol`` field. Also, fragmentation is
largely avoided, allowing the fragmentation-related fields of IPv4 to
be removed from the IPv6 header. The ``HopLimit`` field is simply the
``TTL`` of IPv4, renamed to reflect the way it is actually used.

.. _fig-v6header:
.. figure:: federation/figures/f04-12-9780123850591.png
   :width: 350px
   :align: center

   IPv6 packet header.

Finally, the bulk of the header is taken up with the source and
destination addresses, each of which is 16 bytes (128 bits) long. Thus,
the IPv6 header is always 40 bytes long. Considering that IPv6 addresses
are four times longer than those of IPv4, this compares quite well with
the IPv4 header, which is 20 bytes long in the absence of options.

The way that IPv6 handles options is quite an improvement over IPv4. In
IPv4, if any options were present, every router had to parse the entire
options field to see if any of the options were relevant. This is
because the options were all buried at the end of the IP header, as an
unordered collection of ‘(type, length, value)’ tuples. In contrast,
IPv6 treats options as *extension headers* that must, if present, appear
in a specific order. This means that each router can quickly determine
if any of the options are relevant to it; in most cases, they will not
be. Usually this can be determined by just looking at the ``NextHeader``
field. The end result is that option processing is much more efficient
in IPv6, which is an important factor in router performance. In
addition, the new formatting of options as extension headers means that
they can be of arbitrary length, whereas in IPv4 they were limited to
44 bytes at most.

