6.1 Design Issues
------------------

While it is easy enough to read the specifications for any Internet
protocol, since they are all documented in *Requests For Comments*
(RFCs), it can often be hard to understand the overall
architecture that is embodied by those protocols and the rationale
behind it. Fortunately, the design philosophy of the Internet was
described at length by David Clark, often referred to as "the
architect of the Internet".


.. _reading_darpa:
.. admonition:: Further Reading

   David D. Clark. `The design philosophy of the DARPA Internet
   Protocols <https://dl.acm.org/doi/10.1145/205447.205458>`__.
   Proc. ACM SIGCOMM, 1988.

The fundamental design goal of the Internet was to interconnect the
small handful of networks that existed at the time of its conception
in the 1970s. As noted above, the decision that
different networks would not be modified to fit into the Internet was
pivotal.

Since the networks that were being connected, especially the ARPANET,
were packet-switched, there was also a decision to make the Internet
itself a packet-switched architecture. This seems so obvious in
retrospect that it is worth remembering that circuit switching was the
dominant network technology of the time and probably could have been
made to work, albeit less efficiently.

With these two decisions made, the basic design of the Internet
follows: a network composed of distinct, unmodified
networks, which are interconnected using packet switches to forward packets
from one constituent network to the other. In other words, the Internet is a
*logical* network built out of a collection of physical networks.

The Internet is also the canonical example of the more general concept
of an *internetwork*. We use this term, or sometimes just *internet*
with a lowercase *i*, to refer to an arbitrary collection of networks
interconnected to provide some sort of host-to-host packet delivery
service. For example, a corporation with many sites might construct a
private internetwork by interconnecting the LANs at their different
sites with point-to-point links (or some other network service)
provided by a telecommunications company. When we are talking about
the widely used global internetwork to which a large percentage of the
world's population are now connected, we call it the *Internet* with a
capital *I.* (We are out of step with some style guides which no
longer capitalize the Internet at any time, because we think the
distinction matters in a book such as this one.) In keeping with the
first-principles approach of this book, we mainly want you to learn
about the principles of “lowercase *i*” internetworking, but we
illustrate these ideas with real-world examples from the “big *I*”
Internet.


.. _fig-inet:
.. figure:: federation/figures/inet.png
   :width: 600px
   :align: center

   A simplified internetwork, showing possible connectivity from a set
   of Wi-Fi connected workstations to a set of servers in a remote
   datacenter, via a PON access link and an Ethernet-based
   backbone. The path traverses routers R1, R2, and R3.


:numref:`Figure %s <fig-inet>` shows an example internetwork. An
internetwork is often referred to as a “network of networks” because
it is made up of lots of smaller networks. In this figure, we see a
Wi-Fi based local network, a PON-based access network, a point-to-point
Ethernet (representing a wide-area backbone link), and an Ethernet-based
switching fabric in a datacenter. The nodes that interconnect the
networks are called *routers*.  They historically were also sometimes
called *gateways*, but since this term has several other connotations,
we restrict our usage to router.\ [#]_

.. [#] As we saw in Chapter 2, whether a switch forwards L2 frames or
       L3 packets is often a matter of configuration. This means our
       other option is to call them L3 switches, but we reserve that
       name for when we use IP *within* a packet-switched network—as
       is often the case with a datacenter switching fabric or a
       wide-area backbone—and we reserve the word "router" for a
       forwarding device that interconnects two or more distinct
       networks. This distinction is depicted in figures by using a
       cylinder icon for a router.

.. _fig-ip-graph:
.. figure:: federation/figures/ip-graph.png
   :width: 600px
   :align: center

   A simple internetwork, showing the protocol layers used to connect
   H1 to H5 in the previous figure. ETH denotes the MAC protocol that
   connects to an Ethernet, PON denotes the MAC protocol that connects
   to a Passive Optical Network, and Wi-Fi denotes the MAC protocol
   that connects to Wi-Fi.

The *Internet Protocol* is the key tool used today to build scalable,
heterogeneous internetworks. It was originally known as the Kahn-Cerf
protocol after its inventors. One way to think of IP is that it runs
on all the nodes (both hosts and routers) in a collection of networks
and defines the infrastructure that allows these nodes and networks to
function as a single logical internetwork. For example,
:numref:`Figure %s <fig-ip-graph>` shows how hosts H1 and H5 are
logically connected by the internet in :numref:`Figure %s <fig-inet>`,
including the protocol graph running on each node. Note that
higher-level protocols, such as TCP and UDP, typically run on top of
IP on the hosts.

The design goals listed in the design philosophy paper give a good
overview of the issues that need to be tackled in an internetwork:

 - Internet communication must continue despite loss of networks or
   routers.\ [#]_

 - The Internet must support multiple types of communications service.

 - The Internet architecture must accommodate a variety of networks.

 - The Internet architecture must permit distributed management of its
   resources.

 - The Internet architecture must be cost-effective.

 - The Internet architecture must permit host attachment with a low
   level of effort.

 - The resources used in Internet architecture must be accountable.

.. [#] In the original paper the word "gateways" is used here.

Some of these will look familiar from our discussion of requirements
in Chapter 1. The idea that the Internet keeps working in the face of
failure of any network or router is widely understood—although the
claim that it was meant to survive nuclear war seems to be a
conflation of ideas from other research, according to the authoritative
history by Leiner *et al*.

.. _reading_inet:
.. admonition:: Further Reading

   B. Leiner, et al. `A Brief History of the Internet
   <https://dl.acm.org/doi/10.1145/1629607.1629613>`__.
   SIGCOMM CCR, October 2009.

The second and third requirements are captured by the hourglass shape
we saw in Chapter 1. Many types of communications
service (or applications) can run over the Internet, and many
different types of networks (included those not yet invented at the
time) can be accommodated within the Internet's architecture. We refer
to this property as heterogeneity.

Distributed management of resources is one of the key problems that we
discuss further in this and the next chapter. This captures the idea that
individual networks are autonomously managed. The routing system of
the Internet in particular has evolved to enable the
operators of the autonomous networks to set policies about how the
resources that they control are used by other networks to which they
connect.

We discussed cost-effective resource usage in Chapter 1, and this is
one of the main reasons that packet switching emerged as the
technology used in Internet routers. Being general
purpose and being cost-effective were somewhat in tension, with
generality having been given higher weight. Optimizing for a single
application, as the telephone network did, could be cost-effective but
lacks generality.

The low level of effort to attach hosts ultimately came down to the fact
that, as long as your host could attach to *some* physical network,
you could connect it to the Internet. All that was required was a
TCP/IP implementation running on the host. As we discussed in Chapter
2, such implementations with a well-defined API for applications
became ubiquitous and enabled the rapid growth of Internet
applications.

Finally, Clark admits that accountability of resource usage has been
one requirement that wasn't well met by the Internet of 1988, and it's
arguably not much better today.

Interestingly, there is no mention of scale in these requirements. A
likely explanation for this is that computers were still a scarce
resource in the 1970s and local area networks were yet to take off. So
while the Internet was designed to accommodate networks in a very
general way, scaling to billions of devices didn't make the
requirements list. Fortunately the decentralized approach to the
Internet did foster scalable growth from the start. The problems
of scale that have appeared over the decades include both the need to
address billions of devices and the need to support routing among over
a million networks. These issues, as we will see in this chapter, have
been addressed with varying degrees of success.
