1.2  Network Architecture
---------------------------

The previous section established a substantial set of requirements for
network design—a computer network must provide general,
cost-effective, fair, and robust connectivity among a large number of
computers. As if this weren’t enough, networks do not remain fixed at
any single point in time but must evolve to accommodate changes in
both the underlying technologies upon which they are based as well as
changes in the demands placed on them by application programs.
Furthermore, networks must be manageable by humans of varying levels
of skill. Designing a network to meet these requirements is no
small task.

To help deal with this complexity, network designers often develop a
blueprint—sometimes called a *network architecture*—that guides the
design and implementation of the network. One way to think about a
network architecture is that it specifies a way to modularize the
network; a particular deconstruction of the overall system into a
collection of components. Modularization, in turn, is essentially an
exercise in defining abstractions—identifying the behavior of some
important aspect of the system, encapsulating that behavior in an
object that provides an interface that can be manipulated by other
components of the system, and hiding the details of how the object is
implemented from the users of the object.

Architectures are *not* programs. They help people build a mental
model (typically expressed in words and diagrams), of what actually
runs in the network.  Their purpose is to both *prescribe* how the
network should be implemented (with the goal of guiding engineers in
the actual code that gets written) and *describe* how the network has
been implemented (with the goal of helping developers and operators
understand how the existing network behaves). As a consequence, one of
the main values of an architecture is to define terminology.

This section explains the role of a network architecture by
introducing two of the most widely referenced examples—the OSI
architecture and the Internet architecture.  Because we're just
getting started, the description is intentionally high level, and we
do not attempt to justify either architecture as "the right answer."
They are just two examples that illustrate how one might break the a
problem of building a network into manageable subsystems, setting the
stage for the rest of the book.


1.2.1 OSI Model
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The International Standards Organization (ISO) was one of the first
organizations to formally define a common way to connect computers.
Their architecture, called the *Open Systems Interconnection* (OSI)
architecture and illustrated in :numref:`Figure %s <fig-osi>`, defines
a partitioning of network functionality into seven layers, where one
or more components implement the functionality assigned to a given
layer. It is often referred to as the 7-layer model, and while there are
no OSI-based networks running today, the terminology it defined is
still widely used.

.. _fig-osi:
.. figure:: introduction/figures/osi.png
   :width: 600px
   :align: center

   The OSI 7-layer model.

Starting at the bottom and working up, the *physical* layer handles
the transmission of raw bits over a communications link. The *data
link* layer then collects a stream of bits into a larger aggregate
called a *frame*. Network adaptors, along with device drivers running
in the node operating system, typically implement the data link
level. This means that frames, not raw bits, are actually delivered to
end hosts. The *network* layer handles routing among nodes within a
switched network. At this layer, the unit of data exchanged among
nodes is typically called a *packet* rather than a frame, although
they are fundamentally the same thing. The lower three layers are
implemented on all network nodes, including switches within the
network and hosts connected to the edge of the network. The
*transport* layer then implements what we sometimes refer to as a
*process-to-process channel*, an *IPC (Interprocess Communication)*
mechanism. Here, the unit of data exchanged is also sometimes called a
*message*, *segment*, or *datagram* rather than a packet or a
frame. The transport layer and higher layers typically run only on the
end hosts and not on the intermediate switches.

Skipping ahead to the top (seventh) layer and working our way back
down, we find the *application* layer. Application layer protocols
include things like the Hypertext Transfer Protocol (HTTP), which is
the basis of the World Wide Web and is what enables web browsers to
request pages from web servers. Below that, the *presentation* layer
is concerned with the format of data exchanged between peers—for
example, whether an integer is 16, 32, or 64 bits long, or how an
image or video stream is formatted. Finally, the *session* layer
provides a name space that is used to tie together the potentially
different transport streams that are part of a single application. For
example, it might manage an audio stream and a video stream that are
being combined in a teleconferencing application.

The OSI model both defines how to modularize functionality into seven
layers, and suggests how one might assemble a network from a set of
*switches*—a special device that receives data on one communication
port and send it out on another port. This means a distributed
collection of switches can be interconnected to form a network, as
shown in :numref:`Figure %s <fig-network>`. This switched network
expands on the part of :numref:`Figure %s <fig-osi>` contained within
the cloud, where each switch implements the bottom three layers in the
OSI stack.

.. _fig-network:
.. figure:: introduction/figures/network.png
   :width: 400px
   :align: center

   Example switched network.

Note that the cloud icon in :numref:`Figures %s <fig-osi>` and
:numref:`%s <fig-network>` is commonly used to represent an abstract
network of some unspecified type, where we are not concerned with its
internal structure or what technology it uses. In the simplest case,
the network can be implemented by a single point-to-point link.


1.2.2 Internet Architecture
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The Internet architecture is more concrete than the OSI architecture
because it is organized around specific components, namely, the TCP
and IP protocols. (For now, think of "protocol" as a synonym for
"module"; we describe protocols in more detail in the next section.)
The Internet was being deployed as a way to extend its predecessor—the
ARPANET—when the OSI architecture was defined; the ARPANET had a major
influence on the OSI reference model, but the Internet was largely
viewed as a contemporary competitor.

.. _fig-internet:
.. figure:: introduction/figures/internet.png
   :width: 350px
   :align: center

   Abstract depiction of the Internet architecture. It's general shape
   is similar to an hourglass: wide at the top (representing many
   applications) and wide at the bottom (representing many network
   technologies), with a narrow waist in the middle (corresponding to
   IP).

The Internet architecture is often depicted by a diagram similar to
the one shown in :numref:`Figure %s <fig-internet>`. The diagram is
noteworthy because of its hourglass shape—wide at the top, narrow in
the middle, and wide at the bottom. This shape actually reflects an
important design philosophy of the Internet. IP serves as the
focal point for the architecture—it defines a common method for
exchanging packets among a wide collection of networks. Above IP there
can be arbitrarily many application programs (which the diagram
denotes as App\ :sub:`A` to App\ :sub:`Z`) and below IP the
architecture allows for arbitrarily many different network
technologies (which the diagram denotes as Net\ :sub:`1` to Net\
:sub:`N`). In practice, the applications can be anything from web
browsing to video streaming, while popular network technologies
include Ethernet, Wi-Fi, and 5G.

The intermediate layer sitting between IP and the applications, which
typically includes TCP and UDP but could include other protocols,
corresponds to the transport protocols that provide useful services to
application programs. As a rough analogy, you can think of these
protocols as similar to library programs; the Internet allows
applications to run directly on top of IP, but these protocols provide
useful functionality that makes the app developer's job easier. This
is the only layer of the two architectures that is perfectly aligned,
with "transport protocol" being a universal term.

While it is possible to roughly map the other layers between the two
architectures, understanding how and why they differ is more
instructive. At the top-end, the Internet does not partition
applications into sub-layers, corresponding to the OSI's layers 5, 6,
and 7.  This is because it treats applications as an orthogonal
concern, with each app free to adopt whatever modularization makes
sense. At the bottom-end, the Internet does not prescribe how the
underlying networks are partitioned into sub-layers, corresponding to
the OSI's layers 1, 2, and 3. As at the application layer, the
Internet architecture is agnostic as to how underlying network
technologies are organized. Deciding what details to specify and what
issues to treat as out-of-scope is an important judgement call every
architecture makes.

Where the Internet architecture is specific is in its definition of
the Internet layer, corresponding to IP in :numref:`Figure %s
<fig-internet>`.  IP supports universal connectivity between every
pair of edge devices: it assigns a global address to every device and
forwards packets to their destination based on this address. IP does
this *not* by implementing a single switched network—in the sense the
OSI network layer defines a switched network—but rather, by
interconnecting networks with switches. That is, IP supports an
*internetwork*, as shown in :numref:`Figure %s
<fig-internet-cloud>`. A switch that is connected to two or more
networks is commonly called a *router* or *gateway*, although it plays
much the same role as a switch—it forwards messages from one network
to another.

.. _fig-internet-cloud:
.. figure:: introduction/figures/internet-cloud.png
   :width: 500px
   :align: center

   Interconnection of networks, which switches (typically called
   routers) forwarding packets from one network to another.

Because IP depends on other networks, it cannot assume anything about
how they do their job, for example, whether they implement digital
circuits (as Telco networks typically do) or unreliable message
delivery (as wireless networks sometimes do). As a consequence, IP
establishes a minimal service model, known as *best-effort*. That is,
IP attempts to deliver every packet from source host to destination
host, but it makes no guarantees. Implicit in this definition, but
worth highlighting, is that IP is a *packet-swithced network*, and its
switches (i.e., routers) are *packet switches*: they receive full
packets on their input ports, store those packets in memory, and then
forward complete packets on an output port. Such networks are
sometimes called *store-and-forward* networks to distinguish them from
circuit-based networks that pass signals (either electrical or
optical) directly from input to output without buffering or delay. We
discuss the importance of this design in Section 1.4.

That the Internet is a *logical* network running on top of a
collection of *physical* networks is a powerful idea, with non-obvious
consequences. For example, because a minimal network consisting of
exactly one link connecting two switches still qualifies as a network,
it is possible to construct a switched network like the one shown in
:numref:`Figure %s <fig-network>` with IP playing exactly the same
role as the network layer in the OSI model. As another example, an
internet can itself be viewed as a kind of network, which means that
an internet can be built from a set of internets.  Thus, we can
recursively build arbitrarily large networks by interconnecting clouds
to form larger clouds.

We take a closer look at how IP achieves this goal in Chapter 6, but
for now the main take away is that we can define a *network*
recursively as consisting of two or more nodes connected by a physical
link, or as two or more networks connected by a node. In other words,
a network can be constructed from a nesting of networks, where at the
bottom level, the network is implemented by some physical medium.

.. sidebar:: IETF and Standardization

   *We have informally been talking about the Internet community, but
   it includes a standardization body, known as the
   Internet Engineering Task Force (IETF), which is responsible for
   the specification of many of the Internet protocols, such as TCP,
   UDP, IP, DNS, and BGP. The Internet architecture also embraces
   protocols defined by other organizations, including IEEE's 802.3
   ethernet and Wi-Fi standards, W3C's HTTP/HTML web specifications,
   3GPP's 4G and 5G cellular networks standards, and ITU-T's H.232
   video encoding standards, to name a few.*

   *In addition to defining architectures and specifying protocols,
   there are yet other organizations that support the larger goal of
   interoperability. One example is the IANA (Internet Assigned
   Numbers Authority), which as its name implies, is responsible for
   handing out the unique identifiers needed to make the protocols
   work. IANA, in turn, is a department within the ICANN (Internet
   Corporation for Assigned Names and Numbers), a non-profit
   organization that's responsible for the overall stewardship of the
   Internet.*

Finally, it is worth noting that the Internet had been operational for
well over a decade before anyone started publishing architectural
diagrams like the one shown in :numref:`Figure %s <fig-internet>`.
This is consistent with the Internet's philosophy of implementing and
evaluating new ideas before attempting to standardize them. The
hourglass design was not widely acknowledged until it was well
established as viable, in the same way individual protocols are not
standardized unless there is at least one (and preferably two)
representative implementations. This cultural assumption of the IETF
(see the sidebar) helps to ensure that the architecture’s protocols
can be efficiently implemented.  Perhaps the value the Internet
culture places on working software is best exemplified by a quote on
T-shirts commonly worn at IETF meetings:

   *We reject kings, presidents, and voting. We believe in rough
   consensus and running code.* **(David Clark)**

1.2.3 Separation of Concerns
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

System design principles—the collective wisdom that comes from years
of experience building systems—are often applied to architectural
decisions, and ultimately shape the resulting blueprint. We highlight
several design principles throughout the book, but we have already
seen one—the *separation of concerns*\ —applied to the Internet
architecture.  As discussed in the previous subsection, the Internet
is purposely agnostic about the application layer and the network
layer, focusing instead on how to assemble a collection of networks
into an internet (the IP layer), and how to deliver useful
process-to-process communication abstractions to applications (the
transport layer).

This isn't to say that applications and network technologies are
unimportant, but rather, that (1) it is best to not be overly
prescriptive since we can't know what applications and technologies
will emerge over time, and (2) it makes the job of architecting the
the system easier if we keep our focus narrow. In general, the
separation of concerns principle can be summarized as follows: When
faced with the design of a complex system, carve out independent
challenges and address them in isolation, without being overly
concerned about how you’re going to address the other issues.
Conflating too many issues and trying to address them all at once is
often tempting (it’s easy to convince yourself that doing so yields a
more optimized solution), but most of the time it’s a recipe for
failure.

This approach to dealing with complexity is so fundamental that we
leverage it as an organizing principle for the rest of this book,
which we break into three parts:

* **Foundation:** Part I introduces the base concepts and terminology,
  and scopes the other two parts. Chapter 2 introduces a
  representative set of applications and identifies how they differ
  and what they have in common. This sets the stage for understanding
  the edge software stack described in Part III.  Chapter 3 looks at
  the possible design space for switches and links, and then settles
  on a particular technology—Ethernet—that serves as an exemplar
  building block for discussing the challenges raised in Part II.

* **Inside the Network:** Part II describes how to support
  global-scale best-effort packet delivery. The chapters in Part II
  address the key challenges in building a packet switch network that
  interconnects tens of billions of edge devices. This substrate makes
  it possible for those devices to host the edge software stack
  described in Part III.

* **Edge of the Network:** Part III focuses on the edge software stack
  that supports applications. The chapters in Part III address the key
  challenges that arise when you try to “fill the gap” between what
  applications need and what the underlying packet delivery service
  described in Part II provides.

Parts II and III can be read in either order, knowing that the
“interface” between the two halves is a universal, best-effort packet
delivery service; Part II implements the service and Part III uses the
service.

Like any deconstruction of a complex system into separate parts, this
decomposition is not perfect. The inside-vs-edge separation is
important to the Internet’s success, but there are important
challenges that are not easily addressed by this particular framing.
We call attention to such “exceptions to the rule” when they arise,
and use them to illustrate that every system design requires judgement
and makes tradeoffs.
