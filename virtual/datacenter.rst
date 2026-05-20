|Virt|.4 Virtual Networks for Datacenters
-----------------------------------------------

Network virtualization as we understand it today is closely linked to
the evolution of modern datacenters, in which large numbers of
commodity servers communicate with each other to solve computational
tasks. These datacenters are common for both large cloud providers
(e.g., AWS, Azure, Google), as well as many enterprise organizations.
Some of the challenges involved in building networks for such
datacenters were laid out in the VL2 paper from Microsoft Research back
in 2009 (see :ref:`Further Reading <reading-VL2>`).

In such datacenters, there is a substantial amount of “east-west”
traffic; that is, server-to-server traffic, as distinct from
“north-south” traffic entering or leaving the datacenter. To
efficiently support high volumes of traffic between any pair of
servers in the datacenter, leaf-spine fabrics of the sort described
in Chapter |Routing| became popular due to their high cross-sectional
bandwidth and scalable layer-3 forwarding.

At the same time, server virtualization became mainstream, which had
several implications for datacenter operations. Provisioning virtual
machines (VMs) can be carried out entirely in software, by contrast to
installing and configuring physical servers, a time-consuming and
manual process. As the ease of provisioning a VM shrank the time to
obtain computational resources from days to minutes or seconds, it
exposed the fact that network configuration was now the "long
pole”—the slowest task to be completed before a user could put their
infrastructure to work. Hence, there was a recognition that network
configuration and provisioning needed to move towards a model more
similar to virtual compute, setting the stage for network
virtualization.

A second effect of server virtualization was to enable virtual machine
mobility. This introduced some real challenges for datacenter
networking. In the absence of network virtualization, the IP address
of a VM is drawn from the physical network on which it resides, and
must be specific to a subnet that connects to the server hosting the
VM.\ [#]_ So if a VM is to migrate to another server, either it needs
to move to a server where that subnet is also present, or it needs a
new IP address. The first choice limits where it can move within the
datacenter, which affects the efficiency of resource usage. The second
option is quite a disruptive thing: TCP connections are dropped, and
applications may need to be restarted. Furthermore, some applications
depend on layer-2 adjacency between communicating peers, and thus
depend on some set of VMs staying in a given subnet even as they move
around within the datacenter.

.. [#] Technically more than one subnet can connect to a given server
       in which case an IP address for a VM needs to be
       drawn from one of those subnets.

One proposed solution to this issue was to make layer-2 subnets ever
larger in the physical network, but that is not really a scalable
solution. Large datacenters invariably use layer-3 networking to
connect racks of servers.

The approach proposed by Greenberg, *et al.* can be considered a
first step in network virtualization. They created a *Virtual Layer 2
(VL2)* network such that the IP addresses used by virtual machines are
decoupled from the addresses used in the physical network, thus
solving the mobility-related issues described above. A VM draws its IP
address from a *virtual* layer 2 network, and VL2 constructs the
appropriate overlay to extend that virtual network wherever it needs
to go, even as VMs migrate across physical networks.

Solving the problem of network configuration is a bit more
complex. Networks are not just simple subnets to connect servers; they
have a host of features that need to be configured, including VLANs
(or some equivalent construct to segment the network), firewall rules,
network address translation (NAT) rules, and so on. It is the
complexity of these tasks that made network configuration the barrier
to agility in datacenter configuration.

The solution that emerged to tackle these issues of configuration and
provisioning was based on SDN. The key insight is that a central API
to an SDN controller provides the ideal way to specify the desired
behavior of the virtual network, with the logically centralized
controller then taking responsibility for figuring out how to
implement the network with the available resources, such as virtual
switches in the hypervisors of the datacenter's servers. The core
principles of SDN—separation of data plane from control plane, and
centralization of the controller to manage a multitude of switching
elements—provide the basis for this approach. The rest of this section
digs deeper into how this works.


.. _reading-VL2:

.. admonition:: Further Reading

   A. Greenberg et al. `VL2: a scalable and flexible data center network
   <https://dl.acm.org/doi/10.1145/1594977.1592576>`__.
   ACM SIGCOMM '09 Symposium, August 2009.



|Virt|.4.1 Architecture
~~~~~~~~~~~~~~~~~~~~~~~~

The simplest possible network virtualization system is shown in
:numref:`Figure %s <fig-basic-virt>`. Virtual switches, which are
software implementations of a range of switching functions, reside at end
hosts. Virtual machines connect to those virtual switches. The
network virtualization controller exposes a northbound API that
receives inputs describing the intended state of a virtual
network. For example, an API request could specify *“VM1 and VM2
should reside on the same virtual layer 2 subnet, network X”.* It is
the responsibility of the controller to determine where those virtual
machines are located, and then to send control commands to the
appropriate virtual switches to create the virtual network abstraction
that is required. Let’s look more closely at that abstraction.

.. _fig-basic-virt:
.. figure:: virtual/figures/Slide44.png
    :width: 600px
    :align: center

    A Basic Network Virtualization System.

Since the VMs should be free to move around the datacenter, their IP
addresses need to be independent of the physical network topology
(indicated by the underlay network in the figure). In particular, we
don’t want any given VM to be restricted in its location by the subnet
addressing of the underlying physical network. For this reason,
network virtualization systems invariably make use of an overlay
encapsulation such as VXLAN, NVGRE, or GENEVE. Encapsulation is a
low-level mechanism that solves an important problem: decoupling the
address space of the virtual network from that of the physical
network. However, it is worth noting that they are just a building
block, and not a complete network virtualization solution. We look
more closely at network virtualization overlay encapsulations in
Section |Virt|.4.2.

.. _fig-encaps-nv:
.. figure:: virtual/figures/Slide45.png
    :width: 600px
    :align: center

    Encapsulation decouples virtual network addresses from physical network.

One thing to notice about virtual network encapsulation, as
illustrated in :numref:`Figure %s <fig-encaps-nv>`, is that there are a
set of *outer* headers that are used by the physical network to
deliver the packet to the appropriate end host, and there are a set of
*inner* headers that are meaningful only in the context of a
particular virtual network. This is how encapsulation decouples the
virtual network addressing from that of the physical. A second thing
to notice is that this particular flavor of encapsulation, known as
VXLAN and specified in RFC 7348, is overlaid on top of UDP.

.. admonition:: Further Reading

   M. Mahalingam et al. `Virtual eXtensible Local Area Network (VXLAN)
   <https://www.rfc-editor.org/info/rfc7348>`__. RFC 7348,
   August 2014.

This simple example also shows one of the tasks that must be
implemented by the network virtualization controller. When a VM wants
to communicate with one of its peers in a virtual network, it needs to
apply the appropriate outer header, which is a function of the current
server location of the VM. Providing the mapping from target VM to
outer header is a natural task for the centralized controller. In VL2
this is referred to as a *directory service.*

To better understand the functions of the network virtualization
controller, we need to look a bit more closely at the definition of a
virtual network.


Virtual Networks Defined
+++++++++++++++++++++++++++++++++

As noted above, virtual networks are analogous to virtual machines.
Virtual machines provide a faithful reproduction of the features of a
physical server, complete with processor, memory, peripherals, and
so on. The reproduction is so complete that an unmodified operating
system can run on the virtual machine exactly as if it were running on
a physical machine.

By analogy, virtual networks must also reproduce the full feature set
of a physical network. This means that a virtual network includes
routing, switching, addressing, and often higher layer features such as NAT,
firewalling, and load balancing. Just as an unmodified operating
system can run on a VM exactly as it would on a physical machine, an
unmodified distributed application should be able to run on a virtual
network exactly as it would on a physical network. This is clearly a
more elaborate proposition than a VLAN.

Importantly, a virtual network needs to keep operating correctly even
as VMs move around. Thus, we can begin to see that the role for a
network virtualization controller is to accept a specification of the
desired virtual network and then ensure that this network is correctly
implemented on the appropriate resources as conditions change and VMs
move. We formalize this role for the controller in the
next section.

Management, Control, and Data Planes
++++++++++++++++++++++++++++++++++++

We can now look more closely at the basic architecture of a network
virtualization system. In contrast to early types of virtual networks
such as VLANs and VPNs, a modern network virtualization system exposes
a northbound API by which virtual networks are created and
managed. Through calls to this API, the topology and services of a
virtual network are specified—either by a human user or by another
piece of software such as a cloud automation platform. Typical API
requests might say *“Create a layer 2 subnet”*, *“Attach VM A to
subnet X”* or *“Apply firewall policy P to traffic entering VM B”*. As
shown in :numref:`Figure %s <fig-three-planes>`, these API requests
lead to the creation of *desired state*—the state that the network
should be in. It is common to refer to the part of the system that
receives API requests and stores them in a desired state database as
the management plane.

.. _fig-three-planes:
.. figure:: virtual/figures/Slide46.png
    :width: 450px
    :align: center

    The Three Planes of a Network Virtualization System.

At the bottom of :numref:`Figure %s <fig-three-planes>` is the data
plane. Commonly, this is a set of *Virtual Switches (vSwitches)* that
run inside hypervisors or container hosts. The data plane is where
virtual networks are implemented. As we saw in the example, a
virtual switch forwards packets between VMs and the physical network,
and to do so it needs to apply appropriate headers to the packets. The
data plane also has information about the current state of the system,
such as the locations of VMs, that needs to be taken into account by
the higher layers of the network virtualization system. This is
indicated by the *discovered state*.

At the heart of the system is the control plane. It sits between the
desired state and the actual state of the system. As the control plane
receives discovered state information from the data plane, it compares
this against the desired state. If the desired state does not match
the actual state, the control plane calculates the necessary changes
and pushes them to the data plane, as indicated by the *control
directives* arrow. This paradigm, of continuously reconciling actual
state with desired state, is a common one in distributed systems.

The mapping between this architecture (:numref:`Figure %s
<fig-three-planes>`) and the one depicted in :numref:`Figure %s
<fig-sdn>` in Chapter |Routing| is
straightforward. At the base is a distributed data plane, be it
assembled from bare-metal switches or software switches, on top of
which a centralized controller collects operational state and issues
control directives. When implemented in a general, use-case agnostic
way, this controller is called a Network OS. At the top-most level is
a management layer that serves API requests, and understands the abstraction of
a virtual network. This management layer can be thought of as an
application that runs on the network OS. In short, the
architecture presented in this Chapter is purpose-built to support
virtual networks, whereas the one outlined in Chapter |Routing| is intended to
be general-purpose.

Consider a simple example. We want to create a virtual network that
connects two VMs, A and B, to a single L2 subnet. We can express that
intent by a set of API requests; for example, create the subnet, connect A to
the subnet, connect B to the subnet. These API requests are
accepted by the management plane and stored as desired state. The
control plane observes changes in desired state that are not yet
reflected in the actual state, so it needs to determine where A and B
are located, and the IP addresses of the relevant hypervisors. With
this information, it determines what the encapsulation of packets
should be if A and B are to communicate with each other. From this, it
computes a set of control directives that need to be installed into
the appropriate vSwitches. These directives are pushed to the vSwitches,
expressed, for example, as a set of OpenFlow rules.

If, at some later time, one of the VMs moves to a different
hypervisor, this information is passed to the control plane, which
detects that the actual state no longer corresponds to the desired
state. That triggers a fresh computation to determine the updates that
need to be pushed to the data plane, such as new forwarding rules to
the appropriate set of vSwitches, and deletion of data plane state at
the hypervisor that no longer hosts one of the VMs.

With this architecture, we can implement a rich set of features for
virtual networks. Provided the data plane has sufficient richness to
implement forwarding rules for firewalls, load balancers, and so on,
it is now possible to build a network virtualization system that
accurately recreates the features of a physical network in software.


Distributed Services
++++++++++++++++++++++++++

Software implementations of network functions such as firewalling,
load balancing, and routing are essential aspects of network
virtualization. However, it is not simply a matter of implementing a
traditional network device in software. Consider the example of a
firewall. A conventional firewall is implemented as a *choke point:*
the network is set up in such a way that traffic must pass through the
firewall to get from one part of the network to another.

.. _fig-standard-firewall:
.. figure:: virtual/figures/Slide47.png
    :width: 600px
    :align: center

    A conventional firewall (not distributed).


Consider the example in :numref:`Figure %s
<fig-standard-firewall>`. If traffic sent from VM A to VM C needs to
be processed at a firewall in a conventional network, it needs to be
routed over a path that traverses the firewall, not necessarily the
shortest path from A to C. In the more extreme case of traffic from VM
A to VM B, which sit on the same host, the traffic from A to B needs
to be sent out of the host, across the network to the firewall, and
then back to B. This is clearly not efficient, and consumes both
network resources and, in the latter case, NIC bandwidth for the
hairpinned traffic. Furthermore, the firewall itself has the
potential to become a bottleneck, as all traffic requiring treatment
must pass up to that centralized device.

.. _fig-dist-firewall:
.. figure:: virtual/figures/Slide48.png
    :width: 600px
    :align: center

    A distributed firewall.

Now consider :numref:`Figure %s <fig-dist-firewall>`, which
illustrates a distributed firewall implementation. In this case,
traffic sent from VM A to VM C can be processed by a firewall function
at either (or both) of the virtual switches that it traverses, and still
be sent over the shortest path through the network underlay between
the two hosts, without hairpinning to an external firewall.
Furthermore, traffic from VM A to VM B need never even leave the host
on which those two VMs reside, passing only through the virtual switch
on that host to receive the necessary firewall treatment.

A significant side effect of distributing a service in this way is
that there is no longer a central bottleneck. Every time another
server is added to host some more VMs, there is a new virtual switch
with capacity to do some amount of distributed service
processing. This means it is relatively simple to scale out the amount
of firewalling (or whatever other service is being delivered) in this
way.

This same approach applies to many other services that might formerly
have been performed in a dedicated box: routing, load balancing,
intrusion detection, and so on. This is not to say that these services
are trivial to implement in a distributed manner in all cases. But
with a centralized control plane, we are able to provision and
configure these services via an API (or a GUI) in one location, and
implement them in a distributed manner with the efficiency and
performance benefits outlined here.


|Virt|.4.2 Building Blocks
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Now that we understand the architecture of network virtualization
systems, let's look at some of the building blocks used to construct
such a system.

Virtual Network Encapsulation
+++++++++++++++++++++++++++++++++

As we noted above, network virtualization requires some sort of
encapsulation so that the addressing in the virtual network can be
decoupled from that of the physical network. Inventing new ways to
encapsulate packets seems to be a popular pastime for network
architects and engineers, and there were a few potential candidates
available already when network virtualization appeared on the
scene. None of them quite fit the bill however, and several more have
been developed over the last decade.

While VXLAN attracted considerable attention when it was first
introduced in 2012, it was by no means the last word in network
virtualization encapsulation. After many years of experimentation and
collaboration among software and hardware vendors and other IETF
participants, an encapsulation that combined most of the desired
features was developed and standardized. The following RFC describes
GENEVE and the set of requirements that it was developed to meet.


.. _reading_Geneve:
.. admonition:: Further Reading

   J. Gross, I. Ganga, and T. Sridhar (Eds.). `Geneve: Generic Network
   Virtualization Encapsulation <https://www.rfc-editor.org/info/rfc8926>`__.
   RFC 8926, November 2020.

A notable feature of GENEVE is its extensibility. This represented
something of a compromise between those building software-based
systems like the one described below and those building hardware
endpoints designed to support network virtualization, such as some
datacenter switches. Fixed headers make life easy for
hardware, but limit flexibility for future expansion. In the end,
GENEVE included an options scheme that could be efficiently processed
(or ignored) by hardware while still giving the required
extensibility.

.. _fig-geneve:
.. figure:: virtual/figures/Slide49.png
    :width: 550px
    :align: center

    GENEVE Header Format.

As shown in :numref:`Figure %s <fig-geneve>`, GENEVE includes a
set of variable length options. The presence of options was a critical
feature that built on the experience of earlier systems, where it was
realized that the limited space in a VXLAN header was insufficient to
pass metadata related to virtual networks from one end of a tunnel to
another. An example use of such metadata is to convey the logical
source port of a packet so that subsequent processing of that packet
can take its source port into consideration. There is a general point
here that, since virtual networks evolve over time with increasingly
sophisticated features implemented in software, it is important not to
constrain the information that can be passed around inside a virtual
network with an overly restrictive packet encapsulation. Similar
lessons about extensibility have been learned for IP and TCP, with
QUIC (discussed in Chapter |Message|) being another example of a
protocol with highly extensible and flexible headers.

Virtual Switches
++++++++++++++++++++

The Virtual Switch plays a critical role in network
virtualization. It is the main component of the data plane, and the
richness of its feature set determines the ability of a network
virtualization system to accurately reproduce the features of a
physical network. The most widely deployed virtual switch is *Open
vSwitch (OVS).*


.. _reading_OVS:
.. admonition:: Further Reading

   B. Pfaff et al. `The Design and Implementation of Open vSwitch
   <https://www.usenix.org/system/files/conference/nsdi15/nsdi15-paper-pfaff.pdf>`__.
   Usenix NSDI '15 Symposium, May 2015.

Open vSwitch has been used in both proprietary systems and open source
systems such as *Open Virtual Network (OVN)* described below.
It was designed to have the necessary flexibility to meet the
requirements of network virtualization while also providing high
performance.

.. _fig-ovs-blocks:
.. figure:: virtual/figures/Slide50.png
    :width: 500px
    :align: center

    Open vSwitch Functional Blocks.

As depicted in :numref:`Figure %s <fig-ovs-blocks>`, OVS is programmed
by the control plane using OpenFlow, reflecting its origins in the SDN
movement. It also receives configuration
information over a separate channel using the *Open vSwitch Database
(OVSDB)* protocol.

OVSDB as depicted in the Figure refers to an RPC protocol used to
access the database (called ``ovsdb-server``), but in general, OVSDB
can refer to either the protocol or the database. The OVSDB protocol uses
a JSON-based message format. It
is also worth noting that OVSDB has taken on a life of its own, beyond
the role shown in :numref:`Figure %s <fig-ovs-blocks>`, as a general
way to represent network forwarding state. We'll see an example of
this broader role below.


As for the OVS data plane, performance has been achieved via a long
series of optimizations described in the Pfaff paper, notably a
fast-path in the kernel that uses a flow cache to forward all packets
in a flow after the first. The first packet in a flow is passed to the
userspace daemon ``ovs-vswitchd``, which looks up the flow in a set of
tables. This set of tables, being implemented in software, can be
effectively unlimited in number, a distinct advantage over hardware
implementations of OpenFlow switching. This enables the high degree of
flexibility that is required in network virtualization.


Note that OVS can be used not only to forward packets between VMs and
the outside world, but can also be used in container environments, to
forward packets among containers on the same or different hosts. Thus
a network virtualization system for containers can be built from many
of the same components as one for VMs, and mixed environments (where
containers and VMs communicate in a single virtual network) are also
possible.

|Virt|.4.3 Example System
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

There have been several successful implementations of network
virtualization systems, including VL2 and several commercial systems.
This section describes *Open Virtual Network (OVN)*, a well-documented
open source project, as an example of a network virtualization system.

OVN is built as a set of enhancements to OVS, leveraging OVS for the
data plane and a set of databases (built on OVSDB) for its control and
management planes. The high level architecture of OVN is shown in
:numref:`Figure %s <fig-ovn-arch>`.

.. _fig-ovn-arch:
.. figure:: virtual/figures/Slide51.png
    :width: 350px
    :align: center

    OVN High-level Architecture.


An important aspect of OVN is its use of two databases (referred to as
Northbound and Southbound) to store state. These databases are
implemented using OVSDB, which was originally created to store
configuration state for OVS, as discussed in Section |Virt|.4.2. In OVN,
OVSDB has a larger role, being used for both configuration state and
control state.

OVN is assumed to operate in an environment where a *Cloud Management
System (CMS)* is responsible for the creation of virtual networks. This is
likely to be OpenStack, which was the first CMS to be supported by
OVN. The OVN/CMS plugin is responsible for mapping abstractions that
match those of the CMS into generic virtual network abstractions that
can be stored in the *Northbound Database*. OVN uses an instance of
``ovsdb-server`` to implement this database. We can think of the plugin as the
management plane and the Northbound DB as the desired state repository.

The control plane of OVN demonstrates a significant novel feature
compared to the generic architecture of :numref:`Figure %s
<fig-three-planes>`. Importantly, it is divided into a centralized
component, known as *ovn-northd*, and a distributed component that
runs on every hypervisor, called the *OVN controller*. Recall that in
Section |Routing|.5.1 we discussed the trade-off between centralized and
distributed control for SDN; in OVN, a hybrid model is used. The
decision to make this split followed the experience of the OVN team in
working on earlier systems where the centralized controller had a full
view of all flows, which presented a scaling challenge. OVN retains
*logically* centralized control, so that a single API entry point can
be used to create networks, query status, and so on, but distributes
out to each hypervisor the control functions related to physical
information such as the location of VMs in specific servers. This led
to significant gains in the scalability of the system.

A centralized component, ``ovn-northd``,
translates the logical network configuration, expressed in terms of
conventional network concepts like switching and routing, into logical
datapath flows, which it stores in the *OVN Southbound
Database*. We can see how logical flows work with an example shown in
:numref:`Figure %s <fig-ovn-tables>`.


.. _fig-ovn-tables:
.. figure:: virtual/figures/Slide52.png
    :width: 700px
    :align: center

    Logical and Physical Flows in OVN.


Logical data path flows provide an abstract representation
of the forwarding rules that are populated in the data
plane, specified in a way that is independent of the physical
location of VMs. In this example, we have created a logical switch
``LS1``, with two ports, ``LP1`` and ``LP2``. Each port connects to a VM and the
MAC addresses of the VMs are ``AA`` and ``BB`` as shown in the
``Logical_Port`` table.

The ``Logical_Flows`` table, built from the information in the
``Logical_Switch`` and ``Logical_Port`` tables, shows how packets are
to be forwarded to implement this logical switch. For example, the
first row indicates that packets destined for a MAC address of ``AA``
need to be sent to port ``LP1``. But there is not enough information
to actually forward packets in this flow, because that depends on
which hypervisors currently host those VMs.  Providing the binding of
physical hypervisor nodes to VMs is a task performed by the OVN
controller running on the appropriate hypervisor. This is an example
of *discovered state*, in the sense that the hypervisors discover the
location of VMs and report it up to the database. So we see that the
controller on ``HV1`` (hypervisor 1) has reported into the ``Chassis``
table that it can be reached using the Geneve encapsulation at IP
address ``10.0.0.10``. And that same hypervisor has reported into the
``Port_Binding`` table that it is hosting the VM with ``LP1``.


In order to program the data plane, the OVN controller for
each hypervisor queries the OVN Southbound DB to identify the logical
flows that are relevant to it, based on the VMs that it currently
hosts. Combined with the information provided by other hypervisors
regarding the location of other VMs, it is able to construct the rules
that need to be programmed into the instance of OVS running
locally on the hypervisor in question. (Note that the OVS instances
shown in :numref:`Figure %s <fig-ovn-arch>` include all the components
shown as part of the OVS data plane in :numref:`Figure %s
<fig-ovs-blocks>`.)  Continuing with the example above, hypervisor 2 needs a
flow rule in OVS to forward packets from ``LP2`` to ``LP1``. It is able to
see this by looking at the ``Logical_Flows`` in OVN Southbound DB, and it
is able to determine the details of how to encapsulate packets and
forward them to the right destination server using information in the
``Port_Binding`` and ``Chassis`` tables. You see the results for both
hypervisors in the table at the bottom of the figure, which is *not*
part of the Southbound DB but is a collation of the flows computed at
the two hypervisors.

You can find a lot more detail on OVN in its online documentation,
including descriptions of the structure of the Northbound and
Southbound databases.

.. _reading_OVN:
.. admonition:: Further Reading

   Open Virtual Network. `OVN Reference Guide
   <https://docs.ovn.org/en/latest/ref/index.html>`__.


Everything discussed up to this point has assumed that we are talking
about VMs as the endpoints for our virtual networks, but everything
that works for VMs also works for containers (glossing over some
implementation details). We can connect a set of container hosts to
the OVN Southbound DB and they can create flow rules for their OVS instances to
build virtual networks for the containers they are hosting. In this
case, the "cloud management system" that OVN integrates with is likely
to be a container management system, such as Kubernetes.

.. TODO -- It might make sense to end the datacenter section here
   (with the capstone example) and then elevate Microsegmentation to
   top-level section 9.5. The lead paragraph would need to be modified
   a bit, but this seems like a "stand-alone" nugget worth highlighting.

|Virt|.4.4 Microsegmentation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Network virtualization has certainly had an impact on networking,
particularly in the datacenter, in the years since VL2. The technology is now widespread
in Telcos and large enterprise datacenters. It is also ubiquitous in
the datacenters of large cloud companies, as an essential component of
delivering infrastructure as a service.

..
  TODO We are talking about firewalls for the first time in this chapter -
  do they need to appear elsewhere in the book? Is it sufficient to add
  a bit more on firewalls at their first appearance here

One of the interesting side-effects of network virtualization is that
it enabled a change in the way security is implemented in the
datacenter. As noted above, network virtualization enables security
features to be implemented in a distributed manner, in software. It
also makes it relatively straightforward to create a large number of
isolated networks, compared to the traditional approach of configuring
VLANs by hand. These two factors combined to lead to the idea of
*microsegmentation*.

Microsegmentation stands in contrast to traditional approaches to
segmenting networks, in which relatively large sets of machines would
connect to a "zone" and then firewalls would be used to filter traffic
passing between zones. While this made for relatively simple network
configuration, it meant that lots of machines would be in the same
zone even if there was no need for them to communicate. Furthermore,
the complexity of firewall rules would grow over time as more and more
rules would need to be added to describe the traffic allowed to pass
from one zone to another.

By contrast, network virtualization allows for the creation of
microsegments, which are narrowly defined virtual networks that
determine both which machines can communicate with each other and how
they can do so. For example, a three-tier application can have
its own microsegmentation policy which says that the machines in the
web-facing tier of the application can talk to the machines in the
application tier on some set of specified ports, but that web-facing
machines may not talk to each other. This is a policy that was
difficult to implement in the past, because all the web-facing
machines would sit on the same network segment.

Prior to microsegmentation, the complexity of configuring segments was
such that machines from many applications would likely sit on the same
segment, creating opportunities for an attack to spread from one
application to another. The lateral movement of attacks within
datacenters has been well documented as a key strategy of successful
cyberattacks over many years.

Consider the arrangement of VMs and the firewall in :numref:`Figure %s
<fig-standard-firewall>`. Suppose that, without network
virtualization, we wanted to put VM A and VM B in different segments
and apply a firewall rule for traffic going from VM A to VM B. We
would have to configure two VLANs in the physical network, connect A
to one of them, and B to the other, and then configure the routing
such that the path from the first VLAN to the second passed through
the firewall. If at some point VM A was moved to another server, we'd
have to make sure the appropriate VLAN reached that server, connect VM
A to it, and ensure that the routing configuration was still forcing
traffic through the firewall. This situation is admittedly a little
contrived, but it demonstrates why microsegmentation was effectively
impossible before the arrival of network virtualization.


Microsegmentation has become an accepted best practice for datacenter
networking, providing a starting point for "zero-trust"
networking. This illustrates the far-reaching impact of network
virtualization.

.. sidebar:: Is Network Virtualization SDN?

  *There is a case to be made that Network Virtualization didn't change
  the way physical networks are built and thus can't be called
  "SDN". It simply runs as an overlay on top of a standard L2/L3
  network, which might run traditional routing protocols and be
  configured one box at a time. This argument seems to be a less
  prevalent view now that network virtualization has become so
  widespread, but it misses the point.*

  *Simply stated, Network Virtualization adheres to the core
  architectural principles laid out by SDN's inventors (and summarized
  in Section |Routing|.5). There is a clear separation between control and
  data planes, with a centralized controller responsible for a
  distributed set of forwarding elements. It even uses OpenFlow as one
  possible control interface—admittedly an
  implementation detail and not fundamental to SDN. Finally, the fact
  that network virtualization uses a completely programmable
  forwarding plane, as exemplified by OVS, also places it squarely in
  the SDN universe.*

  *SDN has always been an approach to building and operating networks,
  applied to isolated domains where it provides value. There is no
  requirement of universality. Datacenter underlays, as exemplified
  by leaf-spine switching fabrics, are one such domain. Virtual
  networking overlays are another such domain. Both are even deployed
  simultaneously in the same datacenters, without either being aware
  that the other exists. Going forward, it is possible these two
  domains might share some technologies (e.g., a common Network OS, a
  common language for writing forwarding functions, a common
  toolchain to generate the control interface). We may see the line
  separating the two domains begin to blur, if an overlay-aware
  underlay or underlay-aware overlay is shown to provide value.*
