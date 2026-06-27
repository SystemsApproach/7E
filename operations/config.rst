.. index:: SNMP: Simple Network Management Protocol
.. index:: MIB: Management Information Base
.. index:: XML: Extensible Markup Language
.. index:: YAML: YAML Ain't Markup Language
.. index:: gNMI: gRPC Network Management Interface
.. index:: gNOI: gRPC Network Operations Interface
.. index:: DHCP: Dynamic Host Configuration Protocol
.. index:: YANG: Yet Another Next Generation

|Ops|.2 Configuration Management
--------------------------------------

This section looks at the configuration side of the operations
problem, with a focus on the protocols, interfaces, data models, and
open source tools commonly used to build a network management system.
Every network adopts its own operational practices, so there is no
single "solution" that we can point to. There are some broad trends
among the large cloud operators that we use to illustrate the
approaches to configuration management, while more traditional network
operators might choose a different set of tools. We aim here to
illustrate the set of problems that need to be tackled and show some
of the tools in widespread use, but this is necessarily a partial view
of a broad landscape of operational tooling.

|Ops|.2.1 Host Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

We start with a bootstrapping step that happens any time you open your
laptop and hope to connect to Wi-Fi:
acquiring an IP address. While this might seem like a problem to be
addressed in Part III, where we take up issues at the edge of the
network, assigning IP addresses is the responsibility of the network.
It is also an operational problem in that sense that IP addresses are
a kind of resource that needs to be managed. Either we find a way to
automate the solution, or an operator (system admin) has to do it
manually, on a case-by-case basis.

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

|Ops|.2.2 Configuration Interface
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

DHCP targets the configuration problem at the boundary between the
network and the hosts that connect to the network. We now turn our
attention to the more general problem of configuring all the switches
and routers—plus the software they run—inside the network. As outlined
in the previous section, the core problem is to define the set of
variables available for operators to ``GET`` and ``SET`` on these
devices, with the additional requirement that this dictionary of
variables should be uniform across devices (i.e., be vendor-agnostic).
We also need an over-the-wire protocol to remotely invoke these
operations, but that's the easy part, as we'll explain below.

The Internet has gone through a decades-long exercise defining such a
dictionary, resulting in the *Management Information Base (MIB)*,
which is used in conjunction with the *Simple Network Management
Protocol (SNMP)*; the latter is the protocol used to issue ``GET`` and
``SET`` commands for MIB-defined variables. SNMP has slightly
different names for these operations, specifically ``GetRequest`` and
``SetRequest``, plus other operations designed to simplify the process
of walking through an array of related variables, but the idea is
straightforward. In any case, our focus is on the variable dictionary.

.. admonition:: Further Reading

   J. Case, M. Fedor, M. Schoffstall, and J. Davin.  `Simple Network
   Management Protocol (SNMP)
   <https://www.rfc-editor.org/info/rfc1157>`__. RFC 1157, May 1990.

   K. McCloghrie and M. Rose. `Management Information Base for Network
   Management of TCP/IP-based Internets: MIB-II
   <https://www.rfc-editor.org/info/rfc1213>`__. RFC 1213, March 1991.

You can learn more about the basics of SNMP and the MIB from RFCs 1157
and 1213, respectively, and if you want to follow the history of
incremental refinements, there is a long list of follow-on RFCs. But
all of this work is based on an approach that pre-dates the
availability of modern modeling languages, of which YANG has become
the widely-accepted solution. YANG—which stands for *Yet Another Next
Generation*, a name chosen to poke fun at how often a do-over proves
necessary—can be viewed as a restricted version of XSD, which is a
language for defining a schema for XML.  YANG defines the structure of
the data, but unlike XSD, it is not XML-specific. Instead, YANG can be
used in conjunction with different over-the-wire message formats,
including XML, but also YAML, protobufs, and JSON. If these acronyms
are unfamiliar, or the distinction between a markup language and a
schema for a markup language is fuzzy, a gentle introduction is
available online.

.. TODO -- Another example of where a stand-alone "piece" of 6E might
   be useful. Maybe a "Markup Language" sidebar is a better approach;
   just survey all the related acronyms with a dose of taxonomy.

.. admonition:: Further Reading

   M. Bjorklund (Ed.). `YANG: A Data Modeling Language for the Network
   Configuration Protocol (NETCONF)
   <https://www.rfc-editor.org/info/rfc6020>`__. RFC 6020,
   October 2010.

   `Markup Languages (XML) <https://book.systemsapproach.org/data/presentation.html#markup-languages-xml>`__.
   *Computer Networks: A Systems Approach*, 2020.

What’s important about this direction is that the data model, which
defines the semantics of the variables available to be read and
written, is available in a programmatic form; it’s not just text in a
standards document. But a modeling language is no better than the
models it defines, and this has proven problematic due to conflicting
incentives. Network operators that buy network hardware have a strong
incentive to drive the models for similar devices towards convergence,
so they are not locked into products from a single vendor.  Vendors,
on the other hand, have an equally strong incentive to emphasize the
uniqueness of their products. This results in a fragmented set of
models.  YANG makes the process of creating, using, and modifying
models programmable and hence, adaptable to an iterative process. The
only question is whether the industry can successfully iterate towards
convergence.

This is where an industry-wide standardization effort, called
*OpenConfig*, comes into play. OpenConfig is a group of network
operators trying to drive the industry towards a common set of
configuration models using YANG as its modeling language. OpenConfig
is officially agnostic as to the over-the-wire protocol used to access
on-device configuration and status variables, but gNMI (gRPC Network
Management Interface) is one approach it is actively pursuing. And as
you might guess from its name, gNMI uses gRPC (a request/response
protocol which runs on top of HTTP—see Chapter |Message|). Thus, gNMI
is intended as a standard management interface for network devices.\
[#]_

.. [#] For completeness, note that NETCONF is the transport protocol
       originally developed in conjunction with YANG, and it still
       enjoys wide adoption, particularly among ISPs.  OpenConfig also
       works with NETCONF, but our current assessment is that gNMI has
       the weight of the large cloud operators behind it as the future
       management protocol, and so we elect to focus on it throughout
       the rest of this chapter.

Returning to the data model, OpenConfig defines a hierarchy of object
types. For example, the YANG model for network interfaces looks like
this:

.. literalinclude:: operations/code/iface.yang

This is a base model that can be augmented, for example, to model an
Ethernet interface:

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
the operator needs to track. (More on telemetry data in the next section.)

Having a meaningful set of models is necessary, but a full
configuration system includes other elements as well. In our case,
there are three important points to make about the relationship
between the OpenConfig models and the devices that need to respond to
requests for OpenConfig-defined variables. Think of this toolset as
being part of the operating system running on every switch or router.

The first is the availability of a YANG toolchain. :numref:`Figure %s
<fig-yang>` shows the steps involved in translating a set of
YANG-based OpenConfig models into the client-side and server-side gRPC
stubs used by gNMI. The toolchain supports multiple target programming
languages, where the client and server sides of the gRPC need not be
written in the same language. With respect to the overview of network
management shown in :numref:`Figure %s <fig-mgmt-system>`, the ``gNMI
Client`` stub runs as part of the Network Management System and an
instance of the the ``gNMI Server`` stub runs on each individual
switch, specifically as part of the Switch OS system running on the
switch's control processor. (See, for example, :ref:`Figure 40
<fig-nbi>` in Section |Tech|.2.5, where the server stub in
:numref:`Figure %s <fig-yang>` implements the switch's NBI.)

.. _fig-yang:
.. figure:: operations/figures/yang-tooling.png
    :width: 550px
    :align: center

    YANG toolchain used to generate gRPC-based runtime for gNMI.

Keep in mind that YANG is not tied to either gRPC or gNMI. The
toolchain is able to start with the very same OpenConfig models but
instead produce XML or JSON representations for the data being read
from or written to network devices using, for example, NETCONF. But in
our context, the target is gNMI.

The second point is that gNMI defines a specific set of gRPC methods to
operate on these models. The set is defined collectively as a ``Service``
in the following specification:

.. literalinclude:: operations/code/service.proto

This specification uses the Protocol Buffers (usually referred to as
protobufs) specification language. We take a closer look at protobufs
in Section |Message|.6, but understanding the essence of the spec is
straightforward.

The ``Capabilities`` method is used to retrieve the set of model
definitions supported by the device. The ``Get`` and ``Set`` methods
are used to read and write the corresponding variable defined in one
of those models. The ``Subscribe`` method is used to set up a stream
of telemetry updates from the device. The corresponding arguments and
return values (e.g., ``GetRequest``, ``GetResponse``) are defined by a
protobuf ``Message`` and include various fields from the YANG
models. A given field is identified with its fully qualified path name
in the data model tree.

The third point is that a given switch does not necessarily care about
the full range of OpenConfig models. This is because a given device
might support control plane protocols like BGP, or it might support an
SDN control plane like the one described in Section |Routing|.5.  For
example, the Switch OS on a datacenter switch might track the
following OpenConfig models: Interfaces, VLANs, QoS, and LACP (link
aggregation), in addition to a set of system and platform variables
(of which the switch’s fan speed is a favorite example).

We conclude this section by briefly turning our attention to a related
interface, called gNOI (Network Operations Interface).  The underlying
mechanism used by gNOI is exactly the same as for gNMI, and in the
larger scheme of things, there is little difference between a switch’s
configuration interface and its operations interface. Generally
speaking, persistent state is handled by gNMI (and a corresponding
YANG model is defined), whereas clearing or setting ephemeral state is
handled by gNOI. (By ephemeral state, we mean settings known on the
device, but without a requirement that any other entity remember the
values so they can be recovered upon restart.) It is also the case
that non-idempotent actions like reboot and ping tend to fall under
gNOI's domain.

As an illustrative example of how gNOI is used, the following is the
protobuf specification for the ``System`` service. In this example,
``Ping``, ``Traceroute``, ``Time``, ``SetPackage``, and ``Reboot`` are
gNOI methods the operator can invoke on a device. Of particular note,
``SetPackage`` is used to instruct the device to download and install
a specified software module. This method would typically be invoked to
upgrade the device, for example, by installing the latest version of BGP.

.. literalinclude:: operations/code/system.proto


|Ops|.2.3 Configuration-as-Code
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

As soon as you scale the network—for example, to the size of a
datacenter or a global backbone—you also need to scale operations; it
is not viable to heavily depend on manual intervention. Being able to
generate the configuration interface from a set of YANG models is an
important part of that, but the configuration settings, themselves,
still have to be entered.  If an operator has to do that by typing
individual values into a web form, then you still have a
problem. Moreover, it's not just that data entry is time
consuming. Every time a change needs to be made, there is an
opportunity to make a mistake.

The solution, which has its origins in cloud operations, is to treat
parameter settings as code; the practice is known as
*Configuration-as-Code*. Typically, this means parameters are
specified in YAML\ [#]_, and the set of YAML
files corresponding to a network's aggregate configuration is managed
in a code repository just like any other collection of C, Java, or
GoLang programs. This is not as big of stretch as it might sound: you
can think of YAML as a declarative programming language.

.. [#] YAML at one time stood for "Yet Another Markup Language" but
       now expands to "YAML Ain't Markup Language" to indicate its use
       in configuration specification rather than document markup.

The following snippet of YAML code shows how one might configure an
Ethernet interface. This file corresponds to the YANG shown in the
previous section.

.. literalinclude:: operations/code/ethernet.yaml

The advantage of managing configuration state as code is that it can
be versioned just like other software modules, with a corresponding
set of version control and release management tools. There could be a
stable version that represents the currently deployed
parameters. Edits can be made, reviewed, and thoroughly tested, and
when there is confidence in its correctness, the changes rolled out to
the operational system. Most importantly, if there is a problem, it's
possible to roll back to an earlier, known-working version of the
configuration state. Testing that a configuration is correct is
clearly an important step in this process, and there are a variety of
tools available to help. Batfish and Minesweeper (described in a
2017 SIGCOMM paper) are popular examples.

.. TODO -- This is a good opportunity to cite some of the most
   notorious configuration failures. Maybe in a sidebar.

.. admonition:: Further Reading

   `Batfish: An open source network configuration analysis tool
   <https://batfish.org/>`__.

   R. Beckett, A. Gupta, R. Mahajan and D. Walker. `A General
   Approach to Network Configuration Verification
   <https://dl.acm.org/doi/abs/10.1145/3098822.3098834>`__.  ACM
   SIGCOMM '17 Symposium, August 2017.

Another aspect of treating configuration variables as code is that it
naturally plugs into a management pipeline, similar to the one
depicted in :numref:`Figure %s <fig-config-pipeline>`. The pipeline
takes input from three sources: an inventory repo, a config repo, and
an image repo. We briefly mentioned inventory in the previous section,
but you can think of it being implemented by a collection of YAML
files representing all the deployed devices. The config repo is
similar to what we've just described, with the exception that instead
of hard-coding some of the parameters, the YAML includes templates that
get filled in with device-specific information. Operators are
responsible for updating these first two repos. Finally, the image
repo holds the latest executable images for the software stack running
on each device (e.g., the latest release of OSPF or BGP). For now, we
assume an upstream provider, for example a vendor, populates the image
repo.  We'll look at how the pipeline extends to the left to account
for networks that also build their own software in Section |Ops|.4.

.. _fig-config-pipeline:
.. figure:: operations/figures/config-pipeline.png
   :width: 550px
   :align: center

   Simple configuration pipeline, with operator-supplied configuration
   and inventory specifications stored in their respective repositories,
   and executable images supplied by an upstream vendor.

