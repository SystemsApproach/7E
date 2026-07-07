.. index:: VPN: Virtual Private Network

|Virt|.3 Virtual Private Networks (VPNs)
-----------------------------------------------

VPNs come in many different flavors, but the common theme is that they
create the illusion of a *private* network using shared
infrastructure. In many, but not all, cases, that shared
infrastructure is the public Internet. So whereas VLANs provide
minimal isolation among tenants on a single physical LAN, VPNs often
provide stronger protection for the traffic traversing the public
Internet. They do this using encryption.

Any VPN requires that we establish connectivity among a set of
endpoints. The connections must offer some level of privacy to the
entities communicating between those endpoints. Furthermore, to
qualify as a *virtual* private network, a VPN creates the illusion of
being dedicated to a group of users, even though the underlying
infrastructure is shared more widely. In practice, this means that a
VPN is almost always built as some sort of *overlay* on shared
infrastructure. This was not always the case; dedicated networks using
technologies such as frame relay and MPLS have also been used to
deliver VPN services. We return to MPLS later in this section, but
using some sort of tunnel over IP is most common today, so we start
our discussion there.


|Virt|.3.1 Tunnels
~~~~~~~~~~~~~~~~~~~~~~

We can think of an IP tunnel as a virtual point-to-point link between
a pair of nodes that are actually separated by an arbitrary number of
networks. The virtual link is created within the router at the
entrance to the tunnel, either by manual configuration or using some
control protocol. The information required to create this virtual link
includes the IP address of the router at the far end of the
tunnel. Whenever the router at the entrance of the tunnel wants to
send a packet over this virtual link, it encapsulates the packet,
including its headers, as the payload of another IP packet. The
destination address of the "outer" IP header is the address of the
router at the far end of the tunnel, while the source address is that
of the encapsulating router.

.. _fig-tunnel:
.. figure:: virtual/figures/f03-27-9780123850591.png
   :width: 600px
   :align: center

   A tunnel through an internetwork. 18.5.0.1 is the
   address of R2 that can be reached from R1 across the
   internetwork.

In the forwarding table of the router at the entrance to the tunnel,
this virtual link looks much like a normal link. Consider, for
example, the network in :numref:`Figure %s <fig-tunnel>`. A tunnel has
been configured from R1 to R2 and assigned a virtual interface number
of 0. The forwarding table in R1 might therefore look like
:numref:`Table %s <tab-tunneltab>`.

.. _tab-tunneltab:
.. table:: Forwarding Table for Router R1.
   :align: center
   :widths: auto

   +------------+---------------------+
   | NetworkNum | NextHop             |
   +============+=====================+
   | 1          | Interface 0         |
   +------------+---------------------+
   | 2          | Virtual interface 0 |
   +------------+---------------------+
   | Default    | Interface 1         |
   +------------+---------------------+

R1 has two physical interfaces. Interface 0 connects to network 1;
interface 1 connects to a large internetwork and is thus the default for
all traffic that does not match something more specific in the
forwarding table. In addition, R1 has a virtual interface, which is the
interface to the tunnel. Suppose R1 receives a packet from network 1
that contains an address in network 2. The forwarding table says this
packet should be sent out virtual interface 0. In order to send a packet
out this interface, the router takes the packet, adds an IP header
addressed to R2, and then proceeds to forward the packet as if it had
just been received. R2’s address is 18.5.0.1; since the network number
of this address is 18, not 1 or 2, a packet destined for R2 will be
forwarded out the default interface into the internetwork.

Once the packet leaves R1, it looks to the rest of the world like a
normal IP packet destined to R2, and it is forwarded accordingly. All
the routers in the internetwork forward it using normal means, until
it arrives at R2. When R2 receives the packet, it finds that it
carries its own address, so it removes the IP header and looks at the
payload of the packet. What it finds is an inner IP packet whose
destination address is in network 2. R2 now processes this packet like
any other IP packet it receives. Since R2 is directly connected to
network 2, it forwards the packet on to that network. :numref:`Figure
%s <fig-tunnel>` shows the change in encapsulation of the packet as it
moves across the network.

While R2 is acting as the endpoint of the tunnel, there is nothing to
prevent it from performing the normal functions of a router. For
example, it might receive some packets that are not tunneled, but that
are addressed to networks that it knows how to reach, and it would
forward them in the normal way. With tunneling as a building block, we
can start to build virtual private networks.

|Virt|.3.2 Encrypted Tunnels
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The first type of VPN that we focus on here uses tunneling combined
with encryption to provide private connectivity across the shared
infrastructure of the Internet. VPN requirements vary among different
use cases, so we begin our discussion by looking at some of the most
common uses for VPNs.

*Remote Access VPNs* are commonly used to support remote workers,
telecommuters, or contractors who need access to corporate
resources. :numref:`Figure %s <fig-remotevpn>` shows a simple example
where a remote user tunnels across the Internet to connect to their
corporate office.

.. _fig-remotevpn:
.. figure:: virtual/figures/remotevpn.png
   :width: 600px
   :align: center

   A remote user connects via a tunnel to a corporate site.

*Site-to-Site VPNs* are generally used to interconnect the sites of an
enterprise, which could include datacenters, main corporate offices,
and branch offices. :numref:`Figure %s <fig-sitevpn>` shows a simple
example for three sites of different sizes.

.. _fig-sitevpn:
.. figure:: virtual/figures/sitevpn.png
   :width: 600px
   :align: center

   A corporate VPN connects a main office, a branch office, and a datacenter.

Viewed at this level of abstraction, there are obvious similarities
between these two VPN classes. They are not entirely non-overlapping
but they help us identify the requirements. The differences become
apparent when we look at the types of devices that terminate tunnels
and the methods used to establish them.

Remote access VPNs usually establish tunnels directly from a client
device, such as a phone or a laptop, to a device at the edge of the
corporate network called a VPN gateway or concentrator. Some VPN
client software performs this task, with WireGuard and OpenVPN being
two examples of open source, multi-platform clients. There are plenty
of proprietary options as well.

.. admonition:: Further Reading

   `Wireguard: Fast, Modern, Secure VPN Tunnel <https://www.wireguard.com/>`__.

   `OpenVPN: Secure VPN Solutions <https://openvpn.net>`__.

An encrypted tunnel is just like the tunnels described above, with the
additional twist that everything that is sent as a payload in the
outer header is encrypted using some cryptographic algorithm. We
discuss encryption in more detail in Chapter |TLS|, but for now, it's
sufficient to know that for traffic to be *decrypted* at the far end
of the tunnel, the two ends of the tunnel have to share some secret.
Distributing those secrets is part of the process of establishing a
VPN, and the details are closely related to the choice of encryption
technique.

OpenVPN leverages TLS (described in Chapter |TLS|) to build the
encrypted tunnels from client to server. While this mostly follows the
same protocol as described in Chapter |TLS|, the additional step of
authenticating the client is almost always required in VPN use cases,
unlike most Web usages of TLS. Client certificates may be used, but
this raises the issue of how certificates can be reliably distributed
to client devices. One option is that they are provisioned by a
corporate IT department as part of setting up client devices. OpenVPN
also allows for other authentication methods, including username plus
password and optionally multi-factor authentication.

WireGuard is a more recent implementation of encrypted tunnels that
aims to address some shortcomings that have emerged over years of
using OpenVPN tunnels and similar approaches. A 2017 NDSS paper by
Jason Donenfeld lays out the design philosophy of WireGuard. Compared
to OpenVPN, it is less complex by virtue of reducing the set of
cryptographic algorithms that it supports. It establishes "stateless"
tunnels—that is, there is no TLS connection to establish. Finally, it
is implemented in the operating system kernel, another contrast to
OpenVPN, so as to improve performance. For further details we refer
you to the paper.

.. admonition:: Further Reading

   J. Donenfeld. `WireGuard: Next Generation Kernel Network Tunnel
   <https://www.ndss-symposium.org/ndss2017/ndss-2017-programme/WireGuard-next-generation-kernel-network-tunnel/>`__.
   NDSS, 2017.

Encrypted tunnels, client authentication, and a gateway or
concentrator to terminate the tunnels is pretty much all that is
needed to deliver a remote access VPN. A concentrator is just an
appliance that can handle a large number of VPN tunnels at once.
It provides the necessary administrative controls for managing user
accounts and authentication, and forwards the VPN traffic between
tunnels and the corporate network. Note that a remote access VPN will
almost always have to solve the problem of how to get traffic through
the corporate firewall. There is a significant weakness to this
approach: once inside the firewall, there are few internal controls on
what the VPN user can access. So the VPN represents a weak point that
is often exploited by attackers. Some ways to address this issue are
discussed below.

.. TODO -- Don't really have firewall coverage anywhere yet. This also
   comes up in the datacenter section. Maybe a Firewall sidebar in the
   design section makes sense. The relationship between virtual nets
   and firewalls is interesting in its own right.

Site-to-site VPNs differ from remote access VPNs in that they aim to connect
entire networks together, not just the devices of single remote
users. And because office buildings don't move around the way users
do, these VPNs are relatively static. Thus, one early approach to
building site-to-site VPNs was to simply configure tunnels statically
from a router at the edge of one site to a router at the edge of
another. Keys could also be statically configured. This would be OK
for a small VPN, but as the number of sites increases, the
configuration overhead becomes considerable. Furthermore, there is not
likely to be an on-site routing and security expert at every branch
office, so the configuration would have to be set once before the
router was shipped out to the branch, and after that point, changes
become difficult, especially if the router becomes unreachable for
some reason. On top of this, if the connectivity among sites is
anything other than a hub and spoke, then the issue of correctly
configuring routing protocols to forward traffic across the mesh of
tunnels becomes significant. MPLS/BGP VPNs, discussed below, arose as one
solution to the challenges of building site-to-site VPNs.



|Virt|.3.3 Mesh VPNs
~~~~~~~~~~~~~~~~~~~~

Another approach to VPNs that combines some of the features of remote
access VPNs and site-to-site VPNs is referred to as Mesh VPNs. Like a
remote VPN, a mesh VPN builds tunnels that terminate directly on
client devices. However, rather than connecting the other end of the
tunnel to a central VPN gateway or concentrator, mesh VPNs build a
mesh of tunnels among client devices. The effect is to create a VPN
that interconnects the set of authorized client devices almost as if
they were on the same private LAN, even though they can be located
anywhere in the Internet.

There are numerous implementations of the mesh VPN approach, with
Tailscale being a well-known implementation that contains a mixture of
open source and proprietary components. Tailscale is
built using WireGuard as the tunneling protocol, and adds a control
and management plane to ease the task of setting up and managing the
mesh. For example, WireGuard makes the assumption that public keys
have been set up at the tunnel endpoints before the tunnel is
established; Tailscale supplies a central coordination service to
generate and distribute those keys.

One notable aspect of Tailscale is that it assumes that client devices
are likely to be sitting in networks that use private addresses and
are connected to the Internet through a NAT (network address
translation) device. (See Section |Fed|.3 for a discussion of NAT.)
This problem doesn't exist when building a tunnel to a VPN
concentrator with a public IP address, or between a pair of edge
routers, but it has to be solved if you want to build client-to-client
tunnels. There are quite a few details to getting this to work,
especially given that NAT devices don't all behave the same way, and
there may be firewalls to traverse as well. An IETF standard called
STUN (Session Traversal Utilities for NAT) plays an important part,
and the centralized control plane helps to resolve some of the more
difficult corner cases. You can read more about the issues to be
solved in the Tailscale blog post.

Because mesh VPNs build tunnels all the way from client to client,
they also avoid one of the drawbacks of traditional VPNs, which is the
existence of trusted network zones, such as the network behind the
firewall to which a remote access or site-to-site VPN would give
access. In this respect, they embrace the idea of *zero trust
networking*, in that there is no "trusted" zone; only explicitly
allowed connections among specific devices are possible.

.. admonition:: Further Reading

   A. Pennarun. `How Tailscale Works
   <https://tailscale.com/blog/how-tailscale-works>`__.  Tailscale
   blog, 2020.

|Virt|.3.4 MPLS/BGP VPNs
~~~~~~~~~~~~~~~~~~~~~~~~~

The complexity of configuring and managing a VPN comprised of
encrypted tunnels is one reason why MPLS/BGP VPNs—which outsource most
of the complexity of VPN management to a service provider—became such
a successful service offering in the early 2000s. MPLS does not
protect privacy using encryption, but it does solve the issues of
routing traffic among large numbers of sites, and with respect to
security, ensures that the traffic belonging to one customer does not
leak to the network of another. Said another way, MPLS isolates users
(private networks) from each other, but users must trust that the
service provider does not snoop on the traffic they carry. This is a
trust assumption that typically does not hold for a VPN that traverses
the public Internet.


.. _fig-mpls-vpn:
.. figure:: virtual/figures/MPLS-VPN.png
   :width: 600px
   :align: center

   Example of a layer 3 VPN. Four sites from an organization receive a virtually private IP service from a
   provider.


MPLS/BGP VPNs are reasonably complex, but the abstraction that they
offer in the common case is easy to describe. Each customer is
presented with a private IP network that interconnects the sites of
that customer. In a simple configuration, the edge router at each
customer site has a default route pointing to the provider
network. All the sites of the customer VPN in :numref:`Figure %s
<fig-mpls-vpn>` can send traffic to each other, and the sites of other
customers connected to the same provider can similarly send traffic to
each other, but no traffic can flow from sites of one customer to the
other.

The enforcement of isolation between different customers starts by
configuring the interfaces on the routers at the provider edge that
connect to customer VPN sites. An interface is associated with a
particular *virtual routing and forwarding table* (VRF). So in the
case where two customers connect to a single provider edge router,
there are at least two VRFs on that edge router.

All the provider edge routers exchange routing information using a
version of BGP that supports VPN routes. These BGP routes look much
like the ones we discussed in Chapter |BGP|, but they contain
additional information to disambiguate between the routing information
of different customers. This means that even if two customers happened
to use the same internal IP addresses (which happens often enough when
using the IP address ranges designated for private use) the provider's
instance of BGP can tell the difference between the routing
information for customer A and that for customer B. There is further
information in the BGP messages (known as *route targets*) to ensure
that customer A's routes are placed into the correct VRF, and likewise
for customer B.

When it comes time to send a packet from one site to another site in
the same organization, the provider edge router looks up the
destination address in the appropriate VRF. The appropriate VRF is
determined by the interface on which the packet arrived. This
contrasts with the forwarding model described previously for typical
IP routers, which is the same no matter which interface the packet
arrives on. The forwarding table not only tells the router where to
send the packet next, but also contains the information required to
correctly encapsulate it for its trip across the backbone.

.. _fig-mpls-hdr:
.. figure:: virtual/figures/mpls-hdr.png
   :width: 600px
   :align: center

   A customer IP packet is encapsulated with two MPLS labels to allow
   it to be forwarded correctly across the provider's backbone.

In order to send the packet across the provider's backbone, the packet
is encapsulated with an MPLS header as shown in :numref:`Figure %s
<fig-mpls-hdr>`.  This header functions much like a tunnel
encapsulation, and is removed as the packet leaves the provider
network on its way to the destination VPN site. Usually, a pair of
MPLS labels are used. The outer label, which we have indicated as the
Provider Path Label, is associated with a path across the provider's
network to a particular egress provider router. The second label maps
to the customer route that was passed around in BGP. So the outer
label serves to tunnel the packet across the provider backbone, and
the ``VPN label`` allows it to be forwarded to the current customer
site upon egress. Both labels are removed before the packet is passed
off to the customer, so they only see IP packets passing between
sites.

We have glossed over a lot of detail here, but you can think of
MPLS/BGP VPNs as having both a control plane and a data plane that
support the abstraction of private networks for multiple customers.
The control plane is based on BGP, enhanced to support the need to
isolate one customer from another and to allow for the possibility of
non-unique customer addresses. The data plane is based on MPLS and is
just another form of tunneling that decouples the virtual networks
offered to the customers from the shared infrastructure of the
provider.

To learn more about MPLS/BGP VPNs you can refer to the main RFC or the
book on MPLS listed below.

.. admonition:: Further Reading

   E. Rosen and Y. Rekhter. `BGP/MPLS IP Virtual Private Networks
   (VPNs) <https://www.rfc-editor.org/info/rfc4364>`__. RFC 4364,
   February 2006.

   B. Davie and Y. Rekhter. MPLS: Technology and Applications. Morgan
      Kaufmann Publishers, 2000.


|Virt|.3.5 Software-Defined WANs
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Provisioning a VPN using MPLS, while less complex than most earlier
options, still requires some significant local configuration of both
the Customer Edge (CE) router located at each customer site, and the
Provider Edge (PE) router to which that site would be connected. In
addition, it would typically require the provisioning of a circuit
from the customer site to the nearest point of presence for the
appropriate Telco.

With SD-WAN, the assumption is that every branch and head office has
connectivity to the Internet. An edge router is deployed at each site,
and a centralized control plane is used to automate the configuration
of those routers. An enterprise wants its sites—and only its
authorized sites—to be interconnected by the VPN, and it typically
wants to apply a set of policies regarding security, traffic
prioritization, access to shared services, and so on. The abstraction
is very similar to that provided by MPLS VPNs: a private IP network
connecting the sites of a company, provided over shared
infrastructure. The shared infrastructure just happens to be the
public Internet, which implies encrypted tunnels are required.

The policies regarding connectivity among the sites of a single VPN
are input to a central controller, which can then push out all the
necessary configuration to the edge routers located at each
office. Rather than manually configuring a router or (multiple
routers) every time a new site is added, or configuring tunnels by
hand, it is possible to achieve "zero-touch" provisioning: an
appliance is shipped to the new site with nothing more than a
certificate and an address to contact, which it then uses to contact
the central controller and obtain all the configuration it needs.
Anything that is necessary to build site-to-site tunnels—IP addresses,
routing configuration, secrets, etc.—can be pushed out from the
central controller to the edge router. Changes to configuration or
policies, which might affect many sites, are input centrally and
pushed out to all affected sites.  The idea is illustrated in
:numref:`Figure %s <fig-sd-wan>`.

.. _fig-sd-wan:
.. figure:: virtual/figures/sd-wan.png
    :width: 600px
    :align: center

    An SD-WAN controller receives policies centrally and pushes them
    out to edge switches at various sites. The switches build an
    overlay of tunnels over the Internet or other physical networks,
    and implement policies including allowing direct access to cloud
    services.


It can be hard to determine exactly what properties of SD-WAN have
made it popular, especially as vendors promote the features that
distinguish their solution from the others. Unlike much of SDN, the
control plane protocols used in SD-WAN tend to be proprietary. But it
is certainly true that SD-WAN did enough to reduce the complexity of
building and managing encrypted tunnels to drive adoption of this
approach, often replacing MPLS-based VPNs.

An important benefit offered by SD-WAN over many earlier VPN
approaches is to simplify the task of managing access from a branch
office to a cloud service offered by a third party. It seems natural
that you would choose to access those services directly from an
Internet-connected branch, but traditional VPNs would *backhaul*
traffic to a central site before sending it out to the Internet,
precisely so that security policies could be controlled
centrally. With SD-WAN, the central control over security policy is
achieved, while the data plane remains fully distributed—meaning that
remote sites can directly connect to the cloud services without
backhaul.
