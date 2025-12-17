1.1  Requirements
---------------------------

This chapter establishes the ambitious goal of building a network that
scales to a global footprint and supports a wide range of
applications.  Scalability and generality are actually just two (of
many) requirements our design must address. This section expands on
these, and other requirements.  To place the requirements in their
proper context, it is important to recognize that there are multiple
stakeholders interested in the outcome.  They include:

* **Application Developers:** People that program application care
  about the communication services their applications need. This
  includes, for example, a guarantee that each message the application
  sends will be delivered without error within a certain amount of
  time or the ability to switch gracefully among different connections
  to the network as the user moves around.

* **Network Operators:** People that operate networks care that it is
  easy to administer and manage. This includes, for example, that
  faults can be easily isolated, new devices can be added to the
  network and configured correctly, and it is easy to account for
  usage.

* **Network Owners:** People that assemble networks for a particular
  deployment scenario, and likely to take budgetary considerations
  into account. These stakeholders care that network resources are
  being efficiently utilized and fairly allocated to different users.

It's possible to come up with other other stakeholders, but this list
is inclusive enough to stress our design. Importantly, instead of
adopting just one of these as our primary perspective, we approach the
problem of building a network from the perspective of the architect
that is responsible for crafting a design that takes all stakeholder
requirements into account.

With this in mind, that the network be *scalable* is our first
requirement.  Scalability does not necessarily mean picking a target
size, but rather, being aware of factors that might limit how large a
network can grow.  The original Internet architects were aware that
using 32-bit addresses for Internet-connected devices implied there
could be no more than :math:`2^{32}` (over 4 billion) devices.\ [#]_
That number seemed more than adequate at at the time, but of course,
we now know that it wasn't (and workarounds have been engineered).

.. [#] The Internet refers to connected devices as *hosts* since they
   "host user programs." The Mobile Cellular Network refers to
   connected devices as *User Equipment* or *UEs*. We use all
   three terms—devices, hosts, UEs—but most often say hosts
   because of the central role the software they run plays in the
   overall network.

There are other, less obvious factors that can limit network growth.
Restricting the number of stakeholders responsible for different
aspects of the network is an important one. For example, controlling
who may deploy applications tends to limit the user communities that
might benefit from the network; anyone should be able deploy a new
application that serves some community, no matter how niche.  As
another example, it would not be practical for a limited number of
operations teams to be responsible for fixing every outage reported
anywhere on the globe; anyone should be able to own, and then take
responsibility for operating, a piece of the global network. Avoiding
these kinds of limitations may seem obvious, but historically, many
networks have assumed tight control by a central authority rather than
the loose federation of cooperating parties, as is the case with the
Internet.

The second requirement is that the network be *general-purpose*, that
is, able to support a wide range of applications. In principle, this
means avoiding assumptions about how the network will be used. In
practice, this means that the lowest levels of the network—the base
that is shared by all applications—should be relatively minimal. It
should provide a service that is of value to all applications and
leave most decisions about functionality to higher layers (with the
application at the top of the software stack).  For example, it makes
no sense for the base service to provide absolute guarantees about
message delivery if not all applications need it. On the other hand,
many applications share common requirements about such things as
reliability and security, so it is helpful to provide optional
capabilities they can employ if they want to. Getting this right is a
balancing act, which we revisit at many points throughout the book.

The third requirement is that the network allow for *cost-effective
resource sharing*. Networks are built using finite resources; they do
not have infinite capacity and it is not generally practical to
provide dedicated connectivity between any two communicating entities.
This means networks are fundamentally a shared resource, making the
mechanism that allocates resources to different users one of the
central technical challenges we face. This mechanism should be fair
(for some definition of fairness), but also efficient. The latter
requirement means that we want to maximize the amount data the network
delivers on behalf of its users, for a given set of network resources.
We expand on this challenge in Section 1.5.

The fourth requirement is that the network be *high performance.*
Everyone wants the network to be fast, but performance is
multi-faceted.  Some applications case about response time; how long
it takes to send and receive a message. Others care about throughput;
how long it takes to download a large file. Still others care that
packet delivery is predictable, so that a video stream plays without
jarring pauses. All of these metrics depend on physical limitations,
such as the speed of light, but there is also significant work that a
network has to do in order to deliver on this potential. We expand
on this challenge in Section 1.6.

We highlight the these four requirements because they dominate the
design challenges discussed throughout the book. There are other
requirements, all of which mirror the requirements one would place on
any computer system. They include that the network be secure,
manageable, reliable, usable, available, evolvable, and so on. All of
these requirements play a role, and we address them throughout the
book. Keep in mind, though, that it is not possible to design any
system that treats all requirements as equally "the most important".
Tradeoffs are inevitable, and they change over time.  Security is a
great example.  Of course security is important, but it was of lower
priority during the early days of the Internet, when everyone was
focused getting it to work in the first place. As the Internet became
more and more successful, attracting more users and applications,
security moved up in importance. Today it is one of the topmost
concern. We address security in multiple places throughout the book,
with the most detailed discussion in Chapter 12. For a more
comprehensive look at network security, we recommend a companion book.

.. admonition:: Further Reading

   L. Peterson and B. Davie. `Network Security: A Systems Approach
      <https://security.systemsapproach.org>`__. 2025.
