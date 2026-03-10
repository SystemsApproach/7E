6.2 Heterogeneity
-------------------

To deal with the fact that the Internet supports heterogeneity in
terms of the underlying networks that it can accommodate and the
applications it will support, we need to come up with a *service
model*. This is, what is the host-to-host service that our
internetwork will provide? Once we have decided on the service, we can
then define a set of protocols that enable that service to be
delivered over heterogeneous networks.

6.2.1 Service Model
~~~~~~~~~~~~~~~~~~~~

The main concern in defining a service model for an internetwork is that
we can only offer a host-to-host service that can somehow
be provided over each of the underlying physical networks. For example,
it would be no good deciding that our internetwork service model was
going to provide guaranteed delivery of every packet in 100 ms or less if
there were underlying network technologies that could arbitrarily delay
packets. The philosophy used in defining the IP service model,
therefore, was to make it undemanding enough that just about any network
technology that might turn up in an internetwork would be able to
provide the necessary service.

The IP service model can be thought of as having two parts: an
addressing scheme, which provides a way to identify all hosts in the
internetwork, and a datagram (connectionless) model of data delivery.
This service model is sometimes called *best effort* because, although
IP makes every effort to deliver datagrams, it makes no guarantees. We
postpone a discussion of the addressing scheme for now and look first at
the data delivery model.

Before looking at the details of the IP service model, we first
explain a bit of history. At the time IP was defined, the decision to
make it connectionless was revolutionary. Telcos were the dominate
communication provider, and they supported circuit-based service
models. The circuit model defines a procedure to first establish an
end-to-end connection, which includes allocating resources at all the
switches along the circuit's path. Expecting a packet to successfully
traverse a sequence of best-effort packet switches, each of which
statistically multiplexes its resources, was innovative. As a
consequence, early Internet documents referred to IP as providing a
"connectionless datagram" service model, so as to distinguish it from
the current standard practice. We usually refer to them as IP packets
(rather than IP datagrams) in this book, since they are the current
best practice, but any of the terms "best effort", "statistical
multiplexing", and "connectionless datagram" should interpreted as
roughly equivalent ways of talking about the same idea.

Packet Delivery
++++++++++++++++++

The IP packet is fundamental to the Internet Protocol. Every packet
carries enough information to let the network forward the packet to
its correct destination; there is no need for any advance setup
mechanism to tell the network what to do when the packet arrives. You
just send it, and the network makes its best effort to get it to the
desired destination.  The “best-effort” part means that if something
goes wrong and the packet gets lost, corrupted, misdelivered, or in
any way fails to reach its intended destination, the network does
nothing—it made its best effort, and that is all it has to do. It does
not make any attempt to recover from the failure. This is sometimes
called an *unreliable* service.

Best-effort, connectionless service is about the simplest service you
could ask for from an internetwork, and this is its great strength. For
example, if you provide best-effort service over a network that provides
a reliable service, then that’s fine—you end up with a best-effort
service that just happens to always deliver the packets. If, on the
other hand, you had a reliable service model over an unreliable network,
you would have to put lots of extra functionality into the routers to
make up for the deficiencies of the underlying network. Keeping the
routers as simple as possible was important in the early Internet.

The ability of IP to “run over anything” is frequently cited as one of
its most important characteristics. It is noteworthy that many of the
technologies over which IP runs today did not exist when IP was
invented. So far, no networking technology has been invented that has
proven too bizarre for IP. In principle, IP can run over a network that
transports messages using carrier pigeons (as satirically
described in RFC 1149!).

Best-effort delivery does not just mean that packets can get lost or delayed.
Sometimes they can get delivered out of order, and sometimes the same
packet can get delivered more than once. The higher-level protocols or
applications that run above IP need to be aware of all these possible
failure modes and deal with them.

Packet Format
+++++++++++++++++

Clearly, a key part of the IP service model is the type of packets
that can be carried. The IP packet consists of a
header followed by a number of bytes of data. The format of the header
(for IP version 4)
is shown in :numref:`Figure %s <fig-iphead>`. As we saw in Chapter 4,
packet headers at the
internetworking layer and above are almost always designed to
align on 32-bit boundaries to simplify the task of processing them in
software. Thus, the common way of representing them (used in Internet
RFCs, for example) is to draw them as a succession of
32-bit words. The top word is the one transmitted first, and the
leftmost byte of each word is the one transmitted first. In this
representation, you can easily recognize fields that are a multiple of
8 bits long. On the odd occasion when fields are not an even multiple
of 8 bits, you can determine the field lengths by looking at the bit
positions marked at the top of the packet.

.. _fig-iphead:
.. figure:: introduction/figures/iphdr.png
   :width: 400px
   :align: center

   IPv4 packet header.

Looking at each field in the IP header, we see that the “simple” model
of best-effort datagram delivery still has some subtle features. The
``Version`` field specifies the version of IP. The most widely deployed version
of IP is 4, which is typically called *IPv4*. Observe that putting this
field right at the start of the packet makes it easy for everything
else in the packet format to be redefined in subsequent versions; the
header processing software starts off by looking at the version and then
branches off to process the rest of the packet according to the
appropriate format. The next field, ``HLen``, specifies the length of
the header in 32-bit words. When there are no options, which is most of
the time, the header is 5 words (20 bytes) long. The 8-bit ``TOS`` (type
of service) field has had a number of different definitions over the
years, but its basic function is to allow packets to be treated
differently based on application needs. For example, the ``TOS`` value
might determine whether or not a packet should be placed in a special
queue that receives low delay.

The next 16 bits of the header contain the ``Length`` of the packet,
including the header. Unlike the ``HLen`` field, the ``Length`` field
counts bytes rather than words. Thus, the maximum size of an IP packet
is 65,535 bytes. The physical network over which IP is running, however,
typically does not support such long packets. For this reason, IP supports a
fragmentation and reassembly process. The second word of the header
contains information about fragmentation, and the details of its use are
presented in the following section entitled “Fragmentation and
Reassembly.”

Moving on to the third word of the header, the next byte is the ``TTL``
(time to live) field. Its name reflects its historical meaning rather
than the way it is commonly used today. The intent of the field is to
catch packets that have been going around in routing loops and discard
them, rather than let them consume resources indefinitely. Originally,
``TTL`` was set to a specific number of seconds that the packet would be
allowed to live, and routers along the path would decrement this field
until it reached 0. However, since it was rare for a packet to sit for
as long as 1 second in a router, and routers did not all have access to
a common clock, most routers just decremented the ``TTL`` by 1 as they
forwarded the packet. Thus, it became more of a hop count than a timer,
which is still a perfectly good way to catch packets that are stuck in
routing loops. One subtlety is in the initial setting of this field by
the sending host: Set it too high and packets could circulate rather a
lot before getting dropped; set it too low and they may not reach their
destination. The value 64 is the current default.

The ``Protocol`` field is simply a demultiplexing key that identifies
the higher-level protocol to which this IP packet should be passed.
There are values defined for the TCP (Transmission Control Protocol—6),
UDP (User Datagram Protocol—17), and many other protocols that may sit
above IP in the protocol graph.

The ``Checksum`` is calculated by considering the entire IP header as a
sequence of 16-bit words, adding them up using ones’ complement
arithmetic, and taking the ones’ complement of the result. Thus, if any
bit in the header is corrupted in transit, the checksum will not contain
the correct value upon receipt of the packet. Since a corrupted header
may contain an error in the destination address—and, as a result, may
have been misdelivered—it makes sense to discard any packet that fails
the checksum. It should be noted that this type of checksum does not
have the same strong error detection properties as a CRC, but it is much
easier to calculate in software.

The last two required fields in the header are the ``SourceAddr`` and
the ``DestinationAddr`` for the packet. The latter is the key to
datagram delivery: every packet contains a full address for its intended
destination so that forwarding decisions can be made at each router. The
source address is required to allow recipients to decide if they want to
accept the packet and to enable them to reply. IP addresses are
discussed in a later section—for now, the important thing to know is
that IP defines its own global address space, independent of whatever
physical networks it runs over. This is one of the keys
to supporting heterogeneity.

Finally, there may be a number of options at the end of the header. The
presence or absence of options may be determined by examining the header
length (``HLen``) field. While options are used fairly rarely, a
complete IP implementation must handle them all. It is commonly the
case that routers process options as an exception less
efficiently that normal "fast path" processing.

.. sidebar:: Fragmentation Considered Harmful

   One of the problems of providing a uniform host-to-host service
   model over a heterogeneous collection of networks is that each
   network technology has its own idea of how large a packet
   can be. For example, classic Ethernet can accept packets up to
   1500 bytes long, but modern-day variants can deliver larger (jumbo)
   packets that carry up to 9000 bytes of payload. This leaves two
   choices for the IP service model: ensure that all IP datagrams are
   small enough to fit inside one packet on any network technology, or
   provide a means by which packets can be fragmented and reassembled
   when they are too big to go over a given network technology. IP
   version 4 chose fragmentation, while IP version 6, benefiting from
   a few decades of experience, opted for a twist on the first option:
   path MTU discovery.

   The central idea here is that every network type has a *maximum
   transmission unit* (MTU), which is the largest IP datagram that it
   can carry in a frame. Note that this value is smaller than the
   largest packet size on that network because the IP datagram needs
   to fit in the *payload* of the link-layer frame. When a host sends
   an IP datagram, therefore, it can choose any size that it wants. A
   reasonable choice is the MTU of the network to which the host is
   directly attached. Then, fragmentation will only be necessary if
   the path to the destination includes a network with a smaller MTU.

   The downsides of fragmentation were identified as early as 1987 in
   a paper titled "Fragmentation Considered Harmful" by Mogul and
   Kent. (That paper inspired an entire genre of future work on "X
   Considered Harmful.") Fragmentation consumes resources, is
   fragile—one lost fragment means an entire datagram is lost—and the
   reassembly process may degrade performance.

   The way that fragmentation can be avoided is called Path
   MTU Discovery. In IPv4, this is done by setting a "don't fragment" bit in the
   header, and sending a packet using the MTU of the host's local
   network. In IPv6, routers do not perform fragmentation, so there is no
   corresponding bit. But in either case, if the packet arrives at a
   router and is found to be too large for its outgoing network, the
   router sends a control message back to the host. That message
   contains the MTU of the outgoing link so the host learns the new
   MTU that it can safely use. This could happen more than once along
   the path but eventually the host discovers an MTU that works.


6.2.2 Global Addresses
~~~~~~~~~~~~~~~~~~~~~~~

In the above discussion of the IP service model, we mentioned that one
of the things that it provides is an addressing scheme. After all, if
you want to be able to send data to any host on any network, there needs
to be a way of identifying all the hosts. Thus, we need a global
addressing scheme—one in which no two hosts have the same address.
Global uniqueness is the first property that should be provided in an
addressing scheme. As we shall see, this has been a rather complicated
story as the growth of the Internet placed increasing stress on the initial
design for global addresses.

Ethernet addresses are globally unique, but that alone does not
suffice for an addressing scheme in a large internetwork. Ethernet
addresses have no structure that can be used to route packets to their
destination. Such addresses are sometimes called *flat*.  In fact,
Ethernet addresses do have a structure for the purposes of
*assignment*—the first 24 bits identify the manufacturer—but this
provides no useful information to routing protocols since this
structure has nothing to do with network topology.

In contrast to Ethernet, IP addresses are
*hierarchical*, by which we mean that they are made up of several
parts that correspond to some sort of hierarchy in the
internetwork. Specifically, IP addresses consist of two parts, usually
referred to as a *network* part and a *host* part. This is a fairly
logical structure for an internetwork, which is made up of many
interconnected networks. The network part of an IP address identifies
the network to which the host is attached; all hosts attached to the
same network have the same network part in their IP address. The host
part then identifies each host uniquely on that particular network.
Thus, in the simple internetwork of :numref:`Figure %s <fig-inet>`,
the addresses of the hosts on network 1, for example, would all have
the same network part and different host parts.

Note that the routers in :numref:`Figure %s <fig-inet>` are attached to two
networks. They need to have an address on each network, one for each
interface. For example, router R1, which sits between the wireless
network and an Ethernet, has an IP address on the interface to the
wireless network whose network part is the same as all the hosts on that
network. It also has an IP address on the interface to the Ethernet that
has the same network part as the hosts on that Ethernet. Thus, bearing
in mind that a router might be implemented as a host with two network
interfaces, it is more precise to think of IP addresses as belonging to
interfaces than to hosts.

Now, what do these hierarchical addresses look like? Unlike some other
forms of hierarchical address, the sizes of the two parts are not the
same for all addresses. Originally, IP addresses were divided into
three different classes, as shown in :numref:`Figure %s <fig-class>`,
each of which defines different-sized network and host parts. (There
are also class D addresses that specify a multicast group and class E
addresses that are currently unused.) In all cases, the address is
32 bits long.

The class of an IP address is identified in the most significant few
bits. If the first bit is 0, it is a class A address. If the first bit
is 1 and the second is 0, it is a class B address. If the first two
bits are 1 and the third is 0, it is a class C address. The different
classes allowed for different divisions of the address space into
"network" part and "host" part, so you could have a few very large
networks and many more small networks.

.. _fig-class:
.. figure:: federation/figures/f03-19-9780123850591.png
   :width: 350px
   :align: center

   IP addresses: (a) class A; (b) class B; (c) class C.

On the face of it, this addressing scheme has a lot of flexibility,
allowing networks of vastly different sizes to be
accommodated. However, it turned out not to be flexible enough, and
led to address assignment inefficiency, as we will see in a
moment. Today, IP addresses are normally “classless”, which allows for
more flexibility in where the network/host split occurs in the
address. The details of this are explained below.

Before we look at how IP addresses get used, it is helpful to look at
some practical matters, such as how you write them down. By convention,
IP addresses are written as four *decimal* integers separated by dots.
Each integer represents the decimal value contained in 1 byte of the
address, starting at the most significant. For example, the address of
the computer on which this sentence was typed is ``192.168.68.69``.

It is important not to confuse IP addresses with Internet domain names,
which are also hierarchical. Domain names tend to be ASCII strings
separated by dots, such as ``cs.princeton.edu``. The important thing
about IP addresses is that they are what is carried in the headers of IP
packets, and it is those addresses that are used in IP routers to make
forwarding decisions.


6.2.3 Packet Forwarding
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

We are now ready to look at the basic mechanism by which IP routers
forward packets in an internetwork. Recall that *forwarding* is the
process of taking a packet from an input and sending it out on the
appropriate output (we looked at this in depth in Chapter 2), while
*routing* is the process of building up the tables that allow the
correct output for a packet to be determined (we saw several examples
of this in Chapter 4). The discussion here focuses on forwarding; we
return to routing in the next chapter.

The main points to bear in mind as we discuss the forwarding of IP
packets are the following:

-  Every packet contains the IP address of the destination host.

-  The network part of an IP address uniquely identifies a single
   physical network that is part of the larger Internet.

-  All hosts and routers that share the same network part of their
   address are connected to the same physical network and can thus
   communicate with each other by sending frames over that network.

-  Every physical network that is part of the Internet has at least one
   router that, by definition, is also connected to at least one other
   physical network; this router can exchange packets with hosts or
   routers on either network.

Forwarding IP packets can therefore be handled in the following way. A
packet is sent from a source host to a destination host, possibly
passing through several routers along the way. Any node, whether it is a
host or a router, first tries to establish whether it is connected to
the same physical network as the destination. To do this, it compares
the network part of the destination address with the network part of the
address of each of its network interfaces. (Hosts normally have only one
interface, while routers normally have two or more, since they are
typically connected to two or more networks.) If a match occurs, then
that means that the destination lies on the same physical network as the
interface, and the packet can be directly delivered over that network. A
later section explains some of the details of this process.

If the node is not connected to the same physical network as the
destination node, then it needs to send the packet to a router. In
general, each node will have a choice of several routers, and so it
needs to pick the best one, or at least one that has a reasonable chance
of getting the packet closer to its destination. The router that it
chooses is known as the *next hop* router. The router finds the correct
next hop by consulting its forwarding table. The forwarding table is
conceptually just a list of ``(NetworkNum, NextHop)``\ pairs. (As we
will see below, forwarding tables in practice often contain some
additional information related to the next hop.) Normally, there is also
a default router that is used if none of the entries in the table
matches the destination’s network number. For a host, it is more
common to simply have a default router and nothing else—this means that all
packets destined for hosts not on the physical network to which the
sending host is attached will be sent to the default router.

We can describe the forwarding algorithm in the following way:

::

   if (NetworkNum of destination = NetworkNum of one of my interfaces) then
       deliver packet to destination over that interface
   else
       if (NetworkNum of destination is in my forwarding table) then
           deliver packet to NextHop router
       else
           deliver packet to default router

For a host with only one interface and only a default router in its
forwarding table, this simplifies to

::

   if (NetworkNum of destination = my NetworkNum) then
       deliver packet to destination directly
   else
       deliver packet to default router

Let’s see how this works in the example internetwork of :numref:`Figure
%s <fig-inet>`. First, suppose that H1 wants to send a packet to H2.
Since they are on the same physical network, H1 and H2 have the same
network number in their IP address. Thus, H1 deduces that it can deliver
the packet directly to H2 over the Ethernet. The one issue that needs
to be resolved is how H1 finds out the correct Ethernet address for
H2—the address resolution mechanism known as ARP, described below,
addresses this issue.

Now suppose H1 wants to send a packet to H5. Since these hosts are on
different physical networks, they have different network numbers, so
H1 deduces that it needs to send the packet to a router. R1 is the
only choice—the default router—so H1 sends the packet over the
wireless network to R1. Similarly, R1 knows that it cannot deliver a
packet directly to H5 because neither of R1’s interfaces are on the
same network as H5. Suppose R1’s default router is R2; R1 sends the
packet to R2 over the PON link.  Assuming R2 has the forwarding table
shown in :numref:`Table %s <tab-ipfwdtab>`, it looks up H5’s network
number (network 4) and forwards the packet over the packet-switched
network to R3. Finally, R3, since it is on the same network as H5,
forwards the packet directly to H5.

.. _tab-ipfwdtab:
.. table:: Forwarding table for Router R2.
   :align: center
   :widths: auto

   +------------+---------+
   | NetworkNum | NextHop |
   +============+=========+
   | 1          | R1      |
   +------------+---------+
   | 4          | R3      |
   +------------+---------+

Note that it is possible to include the information about directly
connected networks in the forwarding table. For example, we could
label the network interfaces of router R2 as interface 0 for the
Ethernet (network 3) and interface 1 for the point-to-point link
(network 2). Then R2 would have the forwarding table shown in
:numref:`Table %s <tab-ipfwdtab2>`.

.. _tab-ipfwdtab2:
.. table:: Complete Forwarding table for Router R2.
   :align: center
   :widths: auto

   +------------+-------------+
   | NetworkNum | NextHop     |
   +============+=============+
   | 1          | R1          |
   +------------+-------------+
   | 2          | Interface 1 |
   +------------+-------------+
   | 3          | Interface 0 |
   +------------+-------------+
   | 4          | R3          |
   +------------+-------------+

Thus, for any network number that R2 encounters in a packet, it knows
what to do. Either that network is directly connected to R2, in which
case the packet can be delivered to its destination over that network,
or the network is reachable via some next hop router that R2 can reach
over a network to which it is connected. In either case, R2 will use
ARP, described below, to find the MAC address of the node to which the
packet is to be sent next.

The forwarding table used by R2 is simple enough that it could be
manually configured. Usually, however, these tables are more complex and
would be built up by running a routing protocol such as one of those
described in a later section. Also note that, in practice, the network
numbers are usually longer (e.g., 128.96).

We can now see how hierarchical addressing—splitting the address into
network and host parts—has taken the first step to addressing scale in
a large network.
Routers contain forwarding tables that list only a set of network
numbers rather than all the nodes in the network. In our simple example,
that meant that R2 could store the information needed to reach all the
hosts in the network (of which there were eight) in a four-entry table.
Even if there were 100 hosts on each physical network, R2 would still
only need those same four entries. This is a good first step (although
by no means the last) in achieving scalability.

.. _key-aggregation:
.. admonition:: Key Takeaway

   This illustrates one of the most important principles of building
   scalable networks: To achieve scalability, you need to reduce the
   amount of information that is stored in each node and that is
   exchanged between nodes. The most common way to do that is
   *hierarchical aggregation*. IP introduces a two-level hierarchy, with
   networks at the top level and nodes at the bottom level. We have
   aggregated information by letting routers deal only with reaching the
   right network; the information that a router needs to deliver a
   packet to any node on a given network is represented by a single
   aggregated piece of information.

6.2.5 Address Translation (ARP)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

We have talked about how to get IP packets to the
right physical network but glossed over the issue of how to get a
packet to a particular host or router on that network. The main issue
is that IP packet contain IP addresses, but the physical interface
hardware on the host or router to which you want to send the packet
only understands the addressing scheme of that particular network. Thus,
we need to translate the IP address to a link-level address that makes
sense on this network (e.g., a 48-bit Ethernet or Wi-Fi address). We can then
encapsulate the IP packet inside a frame that contains that link-level
address and send it either to the ultimate destination or to a router
that promises to forward the packet toward the ultimate destination.


The general solution is for each host to maintain a table of
address pairs; that is, a table mapping IP addresses into physical
addresses. While this table could, in theory, be centrally managed by a system
administrator and then distributed to each host on the network, a more robust
approach is for each host to dynamically learn the contents of the
table using the network. This is accomplished using the Address
Resolution Protocol (ARP). ARP enables each host on a
network to build up a table of mappings between IP addresses and
link-level addresses. Since these mappings may change over time (e.g.,
because an Ethernet card in a host breaks and is replaced by a new one
with a new address), the entries are timed out periodically and removed.
This happens on the order of every 15 minutes. The set of mappings
currently stored in a host is known as the ARP cache or ARP table.

ARP takes advantage of the fact that many link-level network
technologies, such as Ethernet and Wi-Fi, support broadcast. If a host wants to
send an IP packet to a host (or router) that it knows to be on the
same network (i.e., the sending and receiving nodes have the same IP
network number), it first checks for a mapping in the cache. If no
mapping is found, it needs to invoke the Address Resolution Protocol
over the network. It does this by broadcasting an ARP query onto the
network. This query contains the IP address in question (the target IP
address). Each host receives the query and checks to see if it matches
its IP address. If it does match, the host sends a response message that
contains its link-layer address back to the originator of the query. The
originator adds the information contained in this response to its ARP
table.

The query message also includes the IP address and link-layer address of
the sending host. Thus, when a host broadcasts a query message, each
host on the network can learn the sender’s link-level and IP addresses
and place that information in its ARP table. However, not every host
adds this information to its ARP table. If the host already has an entry
for that host in its table, it “refreshes” this entry; that is, it
resets the length of time until it discards the entry. If that host is
the target of the query, then it adds the information about the sender
to its table, even if it did not already have an entry for that host.
This is because there is a good chance that the source host is about to
send it an application-level message, and it may eventually have to send
a response or ACK back to the source; it will need the source’s physical
address to do this. If a host is not the target and does not already
have an entry for the source in its ARP table, then it does not add an
entry for the source. This is because there is no reason to believe that
this host will ever need the source’s link-level address; there is no
need to clutter its ARP table with this information.

.. _fig-arp:
.. figure:: federation/figures/f03-23-9780123850591.png
   :width: 500px
   :align: center

   ARP packet format for mapping IP addresses into Ethernet addresses.

:numref:`Figure %s <fig-arp>` shows the ARP packet format for
IP-to-Ethernet address mappings. In fact, ARP can be used for lots of
other kinds of mappings—the major differences are in the address
sizes. In addition to the IP and link-layer addresses of both sender
and target, the packet contains

-  A ``HardwareType`` field, which specifies the type of physical
   network (e.g., Ethernet)

-  A ``ProtocolType`` field, which specifies the higher-layer protocol
   (e.g., IP)

-  ``HLen`` (“hardware” address length) and ``PLen`` (“protocol” address
   length) fields, which specify the length of the link-layer address
   and higher-layer protocol address, respectively

-  An ``Operation`` field, which specifies whether this is a request or
   a response

-  The source and target hardware (Ethernet) and protocol (IP) addresses

Note that the results of the ARP process can be added as an extra column
in a forwarding table like the one in :numref:`Table %s <tab-ipfwdtab>`.
Thus, for example, when R2 needs to forward a packet to network 3, it
not only finds that the next hop is R3, but also finds the MAC address
to place on the packet to send it to R3.
