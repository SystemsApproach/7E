|Virt|.5 Microsegmentation
---------------------------------

Network virtualization has certainly had an impact on networking,
particularly in the datacenter, in the years since VL2. The technology
is now widespread in telcos and large enterprise datacenters. It is
also ubiquitous in the datacenters of large cloud companies, as an
essential component of delivering infrastructure as a service.

.. TODO -- We are talking about firewalls for the first time in this
   chapter. Do they need to appear elsewhere in the book? Is it
   sufficient to add a bit more on firewalls at their first appearance
   here?

One of the interesting side-effects of network virtualization is that
it enabled a change in the way security is implemented in the
datacenter. As noted above, network virtualization enables security
features such as firewalling to be implemented in a distributed manner, in software. It
also makes it relatively straightforward to create a large number of
isolated networks, compared to the traditional approach of configuring
VLANs by hand. These two factors combined to lead to the idea of
*microsegmentation*.

Microsegmentation stands in contrast to traditional approaches to
segmenting networks, in which large sets of machines
connect to a "zone" with firewalls placed between zones. Firewalls are
a type of security appliance that is configured to selectively block
some subset of the traffic passing through it.

A firewall typically sits between two regions
of a network, as illustrated in :numref:`Figure %s <fig-firewall>`,
protecting the flow of traffic from one region to another. A common
location would be a strategic point such as the ingress/egress point
to an enterprise network or data center. Of course, unlike a physical
firewall that blocks everything in its path, a network firewall is
really a sort of packet filter that makes decisions about exactly what
traffic is allowed to pass through it.

.. _fig-firewall:
.. figure:: virtual/figures/f08-20-9780123850591.png
   :width: 600px
   :align: center

   A firewall filters packets flowing between a site and the rest of the
   Internet.

Historically, a firewall has been implemented as an “appliance”: a
dedicated system with one job, filtering packets. Firewalls have also
been provided as a set of features available on a router, and a
“personal firewall” may be implemented on an end-user
machine. Firewalls are usually configured to allow or disallow traffic based on IP
addresses, port numbers, and sometimes other information such as the
URL of an HTTP request. 

Firewalls are used not just at the perimeter of a network as in
:numref:`Figure %s <fig-firewall>` but are also placed between zones
or regions within an enterprise or a datacenter. With firewalls being
physical appliances that need to be configured, it is common to have
just a few of them and to divide the network into large zones.  This
makes for simple network configuration, but it also means that lots of
machines end up in the same zone even when there is no need for them
to communicate. Furthermore, the complexity of firewall rules tends to
grow over time as more and more rules would need to be added to
describe the traffic allowed to pass from one zone to another.

By contrast, network virtualization allows for the creation of
microsegments, which are narrowly defined virtual networks that
determine both which machines can communicate with each other and how
they can do so. The key idea is that a microsegment is a lightweight
construct created in software.  An operator can define the precise
communication policy to be applied to a small number of virtual
machines or containers, and the SDN system implements that policy.

For example, a three-tier application is a common structure in which
one tier of VMs face the Internet, a second tier implements some
application logic, and a third tier stores data for the application. A
single three-tier application can have
its own microsegmentation policy which says that the machines in the
Internet-facing tier of the application can talk to the machines in the
application tier on some set of specified ports, but that Internet-facing
machines may not talk to each other. This is a policy that was
difficult to implement in the past, because all the Internet-facing
machines would sit on the same network segment.

Prior to microsegmentation, the complexity of configuring segments was
such that machines from many applications would likely sit on the same
segment, creating opportunities for an attack to spread from one
application to another. The lateral movement of attacks within
datacenters has been well documented as a key strategy of successful
cyberattacks over many years.

Consider the arrangement of VMs and the conventional firewall in
:numref:`Figure %s <fig-standard-firewall>`. Suppose that, without
network virtualization, we wanted to put VM A and VM B in different
segments and apply a firewall rule for traffic going from VM A to
VM B. We would have to configure two VLANs in the physical network,
connect A to one of them, and B to the other (so A and B can't
directly communicate), and then configure the routing such that the
path from the first VLAN to the second passes through the firewall. If
at some point VM A was moved to another server, we'd have to make sure
the appropriate VLAN reached that server, connect VM A to it, and
ensure that the routing configuration was still forcing traffic
through the firewall. This situation is admittedly a little contrived,
but it demonstrates why microsegmentation was effectively impossible
before the arrival of network virtualization.


Microsegmentation has become an accepted best practice for datacenter
networking, providing a starting point for "zero-trust"
networking. This illustrates the far-reaching impact of network
virtualization, one of the key use cases of SDN.
