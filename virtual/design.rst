|Virt|.1 Design Issues
-----------------------------------------------

The first issue to be addressed when designing an approach to virtual
networking is this: what do you want to virtualize? Or more precisely,
what specific virtual network abstraction do you want to provide? In
this chapter we cover some significantly different abstractions, which
lead to different design choices for their implementation.

Because virtualization often entails the support of many instances of
a virtual network on shared infrastructure, the term "network
virtualization" is sometimes conflated with "network slicing". The
latter term applies to an allocation of resources to some subset of
network traffic—implemented, for example, by one of the packet
schedulers described in Section |Capacity|.2—but does not necessarily
imply that the network is virtualized.

The following are some examples of virtual network abstractions that
are discussed in the remainder of this chapter.

* VLANs (virtual LANs) create the abstraction of an isolated broadcast
  domain on an Ethernet.

* VPNs (Virtual Private Networks) create the abstraction of a private
  internetwork that interconnects the sites and remote users of an
  organization or company.

* Network Virtualization in a datacenter provides the abstraction of a
  fully-featured network connecting a set of VMs or containers that
  persists as the virtual end points move around the datacenter.

Once we have settled on the abstraction, we need to solve a pair of
problems:

1. What is the control plane for establishing and maintaining
   virtual network instances?

2. How can we determine which instance a packet belongs to and handle
   it correctly in the data plane?

In the data plane, the answer invariably involves adding some
additional fields to packets, often using a tunnel header that
encapsulates the packet and separates the addresses used in the
virtual network from the shared physical network on which the service
is delivered. We sometimes refer to *decoupling* the virtual network
from the physical. For example, when allowing end points of a virtual
network to move within a datacenter, we need to decouple the addresses
of the virtual end points from the underlying physical addresses.

Encryption is another common, but not universal, mechanism used to
implement the data plane. When the abstraction presented is equivalent
to a private network, encryption provides a basis for ensuring that
VPN traffic cannot be read by other users, or by the operators that
manage the underlying infrastructure.

As for the control plane, the approaches range from the very basic to
highly automated. VLANs operate at the basic end: the creation of a
VLAN and the association of certain network links with particular VLAN
instances is performed by manual configuration on a switch-by-switch
and port-by-port basis. At the other end of the spectrum, VPNs that
interconnect many sites rely on relatively complex control planes,
such as VPN-specific additions to BGP or the use of SDN controllers.
SDN controllers also find extensive use in the creation and ongoing
management of virtual networks for cloud datacenters.

As with much of networking, scalability is a common concern for
virtual networks. This applies not only to whether the control planes
can handle large numbers of end points and virtual network instances,
but also to the operational cost of configuring virtual networks. VLAN
configuration, for example, is a time-consuming manual process that
has stood in the way of rapid deployment of new services in
datacenters. This is, in part, what motivated the new approaches we
discuss in Section |Virt|.4. Automating configuration tasks to reduce
or remove operational complexity is a recurring theme in the evolution
of network virtualization.

Finally, because one of the motivations for virtual networks of
various types is to isolate traffic, there are often security
implications.  Those implications come up throughout the chapter,
often in contrast to another common security mechanism—firewalls.
Section |VIrt|.5 addresses the issue of security in full, and
describes how network virtualization is now being used in place of
firewalls.




