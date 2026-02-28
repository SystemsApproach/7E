6.3 Scale
-----------

As the Internet has grown in its geographic reach, the number of
networks, and the number of hosts it connects, various aspects of its
design have come under stress. Most notably, the IPv4 address space,
while seemingly capable of addressing 4 billion hosts, was under
pressure long before that many hosts were connected. In this section
we examine the range of techniques that have been applied to enable
the Internet to keep on scaling up.


6.3.1 Subnetting and Classless Addressing
-----------------------------------------

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
.. figure:: federation/figures/f03-21-9780123850591.png
   :width: 500px
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

The forwarding table of a router also changes slightly when we introduce
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

Continuing with the example of a datagram from H1 being sent to H2, R1
would AND H2’s address (128.96.34.139) with the subnet mask of the first
entry (255.255.255.128) and compare the result (128.96.34.128) with the
network number for that entry (128.96.34.0). Since this is not a match,
it proceeds to the next entry. This time a match does occur, so R1
delivers the datagram to H2 using interface 1, which is the interface
connected to the same network as H2.

We can now describe the datagram forwarding algorithm in the following
way:

::

   D = destination IP address
   for each forwarding table entry (SubnetNumber, SubnetMask, NextHop)
       D1 = SubnetMask & D
       if D1 = SubnetNumber
           if NextHop is an interface
               deliver datagram directly to destination
           else
               deliver datagram to NextHop (a router)

Although not shown in this example, a default route would usually be
included in the table and would be used if no explicit matches were
found. Note that a naive implementation of this algorithm—one involving
repeated ANDing of the destination address with a subnet mask that may
not be different every time, and a linear table search—would be very
inefficient.

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
~~~~~~~~~~~~~~~~~~~~

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
~~~~~~~~~~~~~~~~~~~~~~~

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

The task of efficiently finding the longest match between an IP address
and the variable-length prefixes in a forwarding table has been a
fruitful field of research for many years. The most well-known algorithm
uses an approach known as a *PATRICIA tree*, which was actually
developed well in advance of CIDR.

6.3.2 Network Address Translation
---------------------------------

Network Address Translation (NAT) has a long and somewhat
controversial history. It seems the idea was independently invented by
a few people, but the first publication describing it was by Paul
Tsuchiya and Tony Eng in 1993. It was conceived at around the same
time that work was getting start on IP version 6, and to tackle the
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
discuss in Chapter 11). So it is possible for many different hosts in
the private network to *simultaneously* use the same global address
for external communication by using the TCP or UDP port field as a key
to distinguish one host from another. This is probably best explained
with an example. 



.. _fig-nat:
.. figure:: federation/figures/NAT.png
   :width: 500px
   :align: center

   A NAT operation in progesss. 1. Original packet from
   client. 2. Packet destined for server after NAT. 3. Reply from
   server. 4. Translated reply packet continues back to client.



When the client tries to send a packet to the destatation server, it
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



.. _reading_nat:
.. admonition:: Further Reading

   P. Tsuchiya and T. Eng.  `Extending the IP internet through address
   reuse. <https://dl.acm.org/doi/10.1145/173942.173944>`__. SIGCOMM
   CCR, January 1993.

   P. Francis. `Network Address Translation
   <https://dl.acm.org/doi/epdf/10.1145/2766330.2766340>`__. SIGCOMM
   CCR, April 2015.


6.3.3 IP Version 6
------------------

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
