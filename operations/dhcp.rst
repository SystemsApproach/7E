.. index:: DHCP: Dynamic Host Configuration Protocol

|Ops|.2. Host Configuration
-----------------------------------

Before getting to the general problem of network configuration, we
start with a bootstrapping step that happens any time you open your
laptop and hope to connect to Wi-Fi: acquiring an IP address. While
this might seem like a problem to be addressed in Part III, where we
take up issues at the edge of the network, assigning IP addresses is
the responsibility of the network.  It is also an operational problem
in that sense that IP addresses are a kind of resource that needs to
be managed. Either we find a way to automate the solution, or an
operator (system admin) has to do it manually, on a case-by-case
basis.

We have already seen one example of the network assigning IP address
to connected devices, and that was in Section |Shared|.4.3, where we
described how Mobile Cellular network authenticates UEs. For the rest
of the Internet, the Dynamic Host Configuration Protocol (DHCP), is
the mechanism that implements address assignment. DHCP is actually
more general, in that it is used to configure other parameters hosts
need to successfully send and receive packets; for example, their
subnet mask, default router, and DNS server. (Remember that a
machine's Ethernet address is typically burned into the NIC, but its
IP address depends on what network it tries to connect to.)

It's worth noting that DHCP wasn't part of the original Internet's
design. IP address configuration was a manual step in the early years,
and it was only as the Internet started to spread to home networks and
small offices without IT staff
that the need for autoconfiguration became sufficiently pressing to
lead to the development of DHCP.

DHCP relies on the existence of a DHCP server to provide configuration
information to hosts. There is at least one DHCP server for an
administrative domain. At the simplest level, this server implements a
centralized repository for host configuration information, so in
principle, a network administrator could maintain a static list of
address assignments on this server. Each host could then contact the
server when it boots up, and retrieve its configuration.  In this
model, the configuration information for each host is stored in a
table that is indexed by some form of unique client identifier,
typically the hardware address (e.g., the Ethernet address of its
network adaptor).

A more sophisticated use of DHCP saves the network administrator from
even having to assign addresses to individual hosts. In this model, the
DHCP server maintains a pool of available addresses that it hands out to
hosts on demand. This considerably reduces the amount of configuration
an administrator must do, since now it is only necessary to allocate a
range of IP addresses (all with the same network number) to each
network.

Since the goal of DHCP is to minimize the amount of manual
configuration required for a host to function, it would rather defeat
the purpose if each host had to be configured with the address of a
DHCP server. Thus, the first problem faced by DHCP is that of server
discovery.

To contact a DHCP server, a newly booted or attached host sends a
``DHCPDISCOVER`` message to a special IP address (255.255.255.255)
that is an IP broadcast address. This means it will be received by all
hosts and routers on that network. (Routers do not forward such
packets onto other networks, preventing broadcast to the entire
Internet.) In the simplest case, one of these nodes is the DHCP server
for the network.  The server would then reply to the host that
generated the discovery message (all the other nodes would ignore
it). However, it is not really desirable to require one DHCP server on
every network, because this still creates a potentially large number
of servers that need to be correctly and consistently configured.
Thus, DHCP uses the concept of a *relay agent*. There is at least one
relay agent on each network, and it is configured with just one piece
of information: the IP address of the DHCP server. When a relay agent
receives a ``DHCPDISCOVER`` message, it unicasts it to the DHCP server
and awaits the response, which it will then send back to the
requesting client. The process of relaying a message from a host to a
remote DHCP server is shown in :numref:`Figure %s <fig-dhcp-relay>`.

.. _fig-dhcp-relay:
.. figure:: operations/figures/f03-24-9780123850591.png
   :width: 500px
   :align: center

   A DHCP relay agent receives a broadcast DHCPDISCOVER
   message from a host and sends a unicast DHCPDISCOVER to the DHCP
   server.

:numref:`Figure %s <fig-dhcp>` below shows the format of a DHCP
message, which is sent using UDP. Note that DHCP was derived from an
earlier protocol called BOOTP, and some of the packet fields are thus
not strictly relevant to host configuration. When trying to obtain
configuration information, the client puts its hardware address (e.g.,
its Ethernet address) in the ``chaddr`` field. The DHCP server replies
by filling in the ``yiaddr`` (“your” IP address) field and sending it
to the client. Other information such as the default router to be used
by this client can be included in the ``options`` field.

.. _fig-dhcp:
.. figure:: operations/figures/f03-25-9780123850591.png
   :width: 400px
   :align: center

   DHCP packet format.

In the case where DHCP dynamically assigns IP addresses to hosts, it
is clear that hosts cannot keep addresses indefinitely, as this would
eventually cause the server to exhaust its address pool. At the same
time, a host cannot be depended upon to give back its address, since
it might have crashed, been unplugged from the network, or been turned
off.  Thus, DHCP allows addresses to be leased for some period of
time. Once the lease expires, the server is free to return that
address to its pool. A host with a leased address needs to renew the
lease periodically if in fact it is still connected to the network and
functioning correctly.
