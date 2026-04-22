|Virt|.1 Design Issues
-----------------------------------------------

The first issue to be addressed when designing an approach to virtual
networking is this: what do you want to virtualize? Or more precisely,
what specific virtual network abstraction do you want to provide? In
this chapter we cover some significantly different abstractions, which
lead to different design choices for their implementation.

Because virtualization often entails the support of many instances of
a virtual network on shared infrastructure, the term "network
virtualization" is sometimes conflated with "slicing". The latter term
applies to allocation of resources to some subset of network traffic,
but does not necessarily imply that the network is virtualized.

The following are some examples of virtual network abstractions that
are discussed in the remainder of this chapter.

* VLANs (virtual LANs) create the abstraction of an isolated broadcast
  domain on an Ethernet.

* VPNs (Virtual Private Networks) create the abstraction of a private
  internetwork that interconnects the sites and remote users of an
  organization or company.

* Network Virtualization in the datacenter context provides the
  abstraction of a fully-featured network connecting a set of VMs or
  containers that persists as the virtual end points move around the
  datacenter.


Once we have settled on the abstraction, we need to solve a pair of
problems:

1. What is the control plane for establishing and maintaining
   virtual network instances?

2. How can we determine which instance a packet belongs to and handle
   it correctly in the data plane?

In the data plane, the answer invariably involves adding some
additional fields to packets, often using a tunnel header that
encapsulates the packet and separates the addresses using in the
virtual network from the shared physical network on which the service
is delivered. We sometimes refer to *decoupling* the virtual network
from the physical. For example, when allowing end points of a virtual
network to move within a datacenter, we need to decouple the addresses
of the virtual end points from the underlying physical addressing.

Encryption is another common, but not universal, element of virtual
private network data planes. When the abstraction presented is
equivalent to a private network, encryption provides a basis for
ensuring that VPN traffic cannot be read by other users or operators
of the shared infrastructure.

Control planes for virtual networks range from the very basic to
elaborate, SDN-based controllers. VLANs operate at the basic end: the
creation of a VLAN and the association of certain network links with
particular VLAN instances is performed by manual configuration on a
switch-by-switch and port-by-port basis. VPNs that interconnect many sites of an
organization rely on relatively complex control planes, such as
VPN-specific additions to BGP or the use of SDN controllers.  SDN
controllers also find extensive use in the creation of virtual networks for
cloud data centers.

As with much of networking, scalability is a common concern for
virtual networks. This applies not only to whether the control planes
can handle large numbers of end points and virtual network instances,
but also to the operational cost of configuring virtual networks. VLAN
configuration, for example, is a time-consuming manual process that
has stood in the way of rapid deployment of new services in data
centers. This provided some of the motivation for new approaches such
as the virtual networks we discuss in Section |Virt|.4. Automation of
configuration tasks to reduce or remove operational complexity is a
recurring theme in the evolution of network virtualization.


