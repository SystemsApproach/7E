|Ops|.2 Configuration Management 
--------------------------------------

We first look at the configuration side of the operations problem,
which includes a bootstrapping step that anyone that's openned their
laptop in the hopes of getting a Wi-Fi connection has triggered:
aquiring an IP address from the network.

|Ops|.2.1 Host Configuration 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

We have been treating hosts as connected to the edge of the packet
delivery network, but in addition to sending and receiving packets,
edge hosts interact with the underlying network in one other critical
way: they ask the network to assign them an IP address. They also
learn other configuration parameters that they will need to
successfully send and receive packets, including their subnet mask,
default router, and DNS server. As we saw in Section |Shared|.4.3, the
Mobile Cellular network includes a mechanism for authenticating
devices and assigning IP addresses to them, but for most of the rest
of the Internet, the Dynamic Host Configuration Protocol (DHCP), is
the mechanism that implements this functionality.  (Remember that a
machine's Ethernet address is typically burned into the NIC, but its
IP address depends on what network it tries to connect to.)

DHCP relies on the existence of a DHCP server that is responsible for
providing configuration information to hosts. There is at least one DHCP
server for an administrative domain. At the simplest level, the DHCP
server can function just as a centralized repository for host
configuration information. Consider, for example, the problem of
administering addresses in the internetwork of a large company. DHCP
saves the network administrators from having to walk around to every
host in the company with a list of addresses and network map in hand and
configuring each host manually. Instead, the configuration information
for each host could be stored in the DHCP server and automatically
retrieved by each host when it is booted or connected to the network.
However, the administrator would still pick the address that each host
is to receive; he would just store that in the server. In this model,
the configuration information for each host is stored in a table that is
indexed by some form of unique client identifier, typically the hardware
address (e.g., the Ethernet address of its network adaptor).

A more sophisticated use of DHCP saves the network administrator from
even having to assign addresses to individual hosts. In this model, the
DHCP server maintains a pool of available addresses that it hands out to
hosts on demand. This considerably reduces the amount of configuration
an administrator must do, since now it is only necessary to allocate a
range of IP addresses (all with the same network number) to each
network.

Since the goal of DHCP is to minimize the amount of manual configuration
required for a host to function, it would rather defeat the purpose if
each host had to be configured with the address of a DHCP server. Thus,
the first problem faced by DHCP is that of server discovery.

To contact a DHCP server, a newly booted or attached host sends a
``DHCPDISCOVER`` message to a special IP address (255.255.255.255) that
is an IP broadcast address. This means it will be received by all hosts
and routers on that network. (Routers do not forward such packets onto
other networks, preventing broadcast to the entire Internet.) In the
simplest case, one of these nodes is the DHCP server for the network.
The server would then reply to the host that generated the discovery
message (all the other nodes would ignore it). However, it is not really
desirable to require one DHCP server on every network, because this
still creates a potentially large number of servers that need to be
correctly and consistently configured. Thus, DHCP uses the concept of a
*relay agent*. There is at least one relay agent on each network, and it
is configured with just one piece of information: the IP address of the
DHCP server. When a relay agent receives a ``DHCPDISCOVER`` message, it
unicasts it to the DHCP server and awaits the response, which it will
then send back to the requesting client. The process of relaying a
message from a host to a remote DHCP server is shown in :numref:`Figure
%s <fig-dhcp-relay>`.

.. _fig-dhcp-relay:
.. figure:: operations/figures/f03-24-9780123850591.png
   :width: 500px
   :align: center

   A DHCP relay agent receives a broadcast DHCPDISCOVER
   message from a host and sends a unicast DHCPDISCOVER to the DHCP
   server.

:numref:`Figure %s <fig-dhcp>` below shows the format of a DHCP
message. The message is actually sent using a protocol called the
*User Datagram Protocol* (UDP) that runs over IP. UDP is discussed in
detail in the next chapter, but the only interesting thing it does in
this context is to provide a demultiplexing key that says, “This is a
DHCP packet.”

.. _fig-dhcp:
.. figure:: operations/figures/f03-25-9780123850591.png
   :width: 400px
   :align: center

   DHCP packet format.

DHCP is derived from an earlier protocol called BOOTP, and some of the
packet fields are thus not strictly relevant to host configuration. When
trying to obtain configuration information, the client puts its hardware
address (e.g., its Ethernet address) in the ``chaddr`` field. The DHCP
server replies by filling in the ``yiaddr`` (“your” IP address) field
and sending it to the client. Other information such as the default
router to be used by this client can be included in the ``options``
field.

In the case where DHCP dynamically assigns IP addresses to hosts, it is
clear that hosts cannot keep addresses indefinitely, as this would
eventually cause the server to exhaust its address pool. At the same
time, a host cannot be depended upon to give back its address, since it
might have crashed, been unplugged from the network, or been turned off.
Thus, DHCP allows addresses to be leased for some period of time. Once
the lease expires, the server is free to return that address to its
pool. A host with a leased address clearly needs to renew the lease
periodically if in fact it is still connected to the network and
functioning correctly.

|Ops|.2.2 Configuration Interface
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**[Introduce SNMP and MIB, but then pivot to a modern take. Following
is taken from Switch OS section of the SDN book.]**

A core challenge of configuring and operating any network device is to
define the set of variables available for operators to ``GET`` and
``SET`` on the device, with the additional requirement that this
dictionary of variables should be uniform across devices (i.e., be
vendor-agnostic). The Internet has gone through a decades-long
exercise defining such a dictionary, resulting in the *Management
Information Base (MIB)* used in conjunction with SNMP. But the MIB was
more focused on *reading* device status variables than *writing*
device configuration variables, where the latter has historically been
done using the device’s *Command Line Interface (CLI)*. One
consequence of the SDN transformation is to nudge the industry towards
support for programmatic configuration APIs. This means revisiting the
information model for network devices.

The main technical advance that was not prevalent in the early days of
SNMP and MIB is the availability of pragmatic modeling languages,
where YANG is the leading choice to have emerged over the last few
years. YANG—which stands for *Yet Another Next Generation*, a name
chosen to poke fun at how often a do-over proves necessary—can be
viewed as a restricted version of XSD, which is a language for
defining a schema for XML. YANG defines the structure of the data, but
unlike XSD, it is not XML-specific. Instead, YANG can be used in
conjunction with different over-the-wire message formats, including
XML, but also protobufs and JSON. If these acronyms are unfamiliar, or
the distinction between a markup language and a schema for a markup
language is fuzzy, a gentle introduction is available online.

.. _reading_xml:
.. admonition:: Further Reading

   `Markup Languages (XML)
   <https://book.systemsapproach.org/data/presentation.html#markup-languages-xml>`__.
   *Computer Networks: A Systems Approach*, 2020.

What’s important about going in this direction is that the data model
that defines the semantics of the variables available to be read and
written is available in a programmatic form; it’s not just text in a
standards document. Moreover, while it is true that all hardware
vendors promote the unique capabilities of their products, it is not a
free-for-all, with each vendor defining a unique model. This is because
the network operators that buy network hardware have a strong
incentive to drive the models for similar devices towards convergence,
and vendors have an equally strong incentive to adhere to those
models. YANG makes the process of creating, using, and modifying
models programmable and hence, adaptable to this iterative process.

.. sidebar:: Cloud Best Practices

     *Our commentary on OpenConfig vs. NETCONF is grounded in a
     fundamental tenet of SDN, which is about bringing best
     practices in cloud computing to the network. It involves big
     ideas like implementing the network control plane as a
     scalable cloud service, but it also includes more narrow
     benefits, such as using modern messaging frameworks like
     gRPC and protobufs.*

     *The advantages in this particular case are tangible: (1)
     improved and optimized transport using HTTP/2 and
     protobuf-based marshalling instead of SSH plus hand-coded
     marshalling; (2) binary data encodings instead of text-based
     encoding; (3) diff-oriented data exchange instead of
     snapshot-based responses; and (4) native support for server
     push and client streaming.*

This is where an industry-wide standardization effort, called
*OpenConfig*, comes into play. OpenConfig is a group of network
operators trying to drive the industry towards a common set of
configuration models using YANG as its modeling language. OpenConfig
is officially agnostic as to the over-the-wire protocol used to access
on-device configuration and status variables, but gNMI (gRPC Network
Management Interface) is one approach it is actively pursuing. And as
you might guess from its name, gNMI uses gRPC (which in turn runs on
top of HTTP/2). This means gNMI also adopts protobufs as the way it
specifies the data actually communicated over the HTTP
connection. Thus, gNMI is intended as a standard management interface
for network devices.

For completeness, note that NETCONF is another of the post-SNMP
protocols for communicating configuration information to network
devices. OpenConfig also works with NETCONF, but our current
assessment is that gNMI has the weight of industry behind it as the
future management protocol. For this reason, it
is the one we highlight in our description of the full SDN software
stack.

OpenConfig defines a hierarchy of object types. For example, the YANG
model for network interfaces looks like this:

.. literalinclude:: operations/code/iface.yang

This is a base model that can be augmented, for example, to model an Ethernet interface:

.. literalinclude:: operations/code/eth.yang

Other similar augmentations might be defined to support link
aggregation, IP address assignment, VLAN tags, and so on.

Each model in the OpenConfig hierarchy defines a combination of a
configuration state that can be both read and written by the client
(denoted ``rw`` in the examples) and an operational state that reports
device status (denoted ``ro`` in the examples, indicating it is
read-only from the client-side). This distinction between declarative
configuration state and runtime feedback state is a fundamental aspect
of any network device interface, where OpenConfig is explicitly
focused on generalizing the latter to include network telemetry data
the operator needs to track.

Having a meaningful set of models is necessary, but a full
configuration system includes other elements as well. In our case,
there are three important points to make about the relationship
between Stratum and the OpenConfig models.

The first is that Stratum depends on a YANG toolchain. :numref:`Figure
%s <fig-yang>` shows the steps involved in translating a set of
YANG-based OpenConfig models into the client-side and server-side gRPC
stubs used by gNMI. The toolchain supports multiple target programming
languages (Stratum happens to use C++), where the client and server
sides of the gRPC need not be written in the same language.

.. _fig-yang:
.. figure:: operations/figures/Slide25.png
    :width: 550px
    :align: center

    YANG toolchain used to generate gRPC-based runtime for gNMI.

Keep in mind that YANG is not tied to either gRPC or gNMI. The
toolchain is able to start with the very same OpenConfig models but
instead produce XML or JSON representations for the data being
read from or written to network devices using, for example, NETCONF or
RESTCONF, respectively. But in our context, the target is protobufs,
which Stratum uses to support gNMI running over gRPC.

The second point is that gNMI defines a specific set of gRPC methods to
operate on these models. The set is defined collectively as a Service
in the protobuf specification:

.. literalinclude:: operations/code/service.proto

The ``Capabilities`` method is used to retrieve the set of model
definitions supported by the device. The ``Get`` and ``Set`` methods
are used to read and write the corresponding variable defined in some
models. The ``Subscribe`` method is used to set up a stream of
telemetry updates from the device. The corresponding arguments and
return values (e.g., ``GetRequest``, ``GetResponse``) are defined
by a protobuf ``Message`` and include various fields from the YANG
models. A given field is specified by giving its fully qualified
path name in the data model tree.

The third point is that Stratum does not necessarily care about the
full range of OpenConfig models. This is because—as a Switch OS
designed to support a centralized Controller—Stratum cares about
configuring various aspects of the data plane but is not typically
involved in configuring control plane protocols like BGP. Such control
plane protocols are no longer implemented on the switch in an
SDN-based solution (although they remain in scope for the Network OS,
which implements their centralized counterpart). To be specific,
Stratum tracks the following OpenConfig models: Interfaces, VLANs,
QoS, and LACP (link aggregation), in addition to a set of system and
platform variables (of which the switch’s fan speed is everyone’s
favorite example).

We conclude this section by briefly turning our attention to gNOI, but
there isn't a lot to say. This is because the underlying mechanism
used by gNOI is exactly the same as for gNMI, and in the larger scheme
of things, there is little difference between a switch’s configuration
interface and its operations interface. Generally speaking, persistent
state is handled by gNMI (and a corresponding YANG model is defined),
whereas clearing or setting ephemeral state is handled by gNOI. It is
also the case that non-idempotent actions like reboot and ping tend to
fall under gNOI's domain. In any case, the two are closely enough
aligned to collectively be referred to as gNXI.

As an illustrative example of what gNOI is used for, the following is
the protobuf specification for the ``System`` service:

.. literalinclude:: operations/code/system.proto

where, for example, the following protobuf message defines the
``RebootRequest`` parameter:

.. literalinclude:: operations/code/reboot.proto

As a reminder, if you are unfamiliar with protobufs, a brief overview is available online.

.. _reading_protobuf:
.. admonition:: Further Reading

   `Protocol Buffers
   <https://book.systemsapproach.org/data/presentation.html#protobufs>`__.
   *Computer Networks: A Systems Approach*, 2020.


|Ops|.2.3 Configuration-as-Code
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**[Diagram a simple pipeline, including a Config repo; maybe also an
Inventory repo. Talk about Lifecyle Mgmt in general, and why treating
configuration as code is a good idea. Also point to verification
tools.  The Velocity section (10.4) builds on this pipeline to also
include code, which motivates GitOps and similar topics.]**
