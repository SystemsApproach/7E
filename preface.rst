.. SPDX-FileCopyrightText: 2026 Larry L. Peterson and Bruce S. Davie
.. SPDX-License-Identifier: CC-BY-4.0

.. include:: chapters.rst

Preface
==========

In the three decades that we have been writing textbooks, the Internet
has changed enough to warrant a fresh approach. This edition is a significant
refactoring of our networking textbook that aims to give readers the
necessary perspective to form a big-picture view of networking and the
skills to deal with the complexity of modern networks.

As with prior editions, we treat
networking as a set of system design problems and draw on existing
artifacts such as Ethernet, HTTP and IP to illustrate how these
challenges have been solved in the past. Our aim it not just to teach
now networks are built today, but to equip readers
with the perspective and systems thinking skills required to shape the networks
of the future.

Our initial motivation for a significant refactoring of our book
was the need to deal with the greater complexity of the Internet today
compared to the one we were describing in 1996, but other
factors come into play. Notably, there has actually been
something of a simplification of the lower layers of the network
stack, at least in terms of how many technologies are in widespread
use. Ethernet has “won” the competition among many different link
layers for wired networks, while there remain a handful of wireless
options (flavors of 802.11, 5G/6G, satellite links, etc.). Similarly,
the dominant upper layer protocol is HTTP, in contrast to the broader
ecosystem of application layer protocols in 1996. We see this
consolidation as something that can be leveraged to help readers
manage the complexity of modern networks.

The other significant factor casting a shadow over any textbook or
educational endeavor is the rise of large language models
(LLMs).\ [#]_ When
large numbers of students turn to LLMs for the answer to any question
about networking, it raises a challenge for the textbook author to
provide something of value (beyond fodder for the training of the next
round of LLMs). Since day one, our books have aimed to provide a sense
of perspective and to explain architectural principles. We aim to
teach students about system design, not to list networking
technologies like an encyclopedia. We use existing technologies as
artifacts to illustrate design principles. We believe this approach
will continue to provide value in the new LLM-enabled environment.

.. [#] We prefer to avoid the term AI as being too broad and
   ill-defined to be useful; LLMs are the technology that has changed the
   way students consume information, for better or worse.

As with the previous edition of our book, we are developing this book
as an open source project.  The
source files are hosted on GitHub, and we solicit input from the
community. The book is licensed under a Creative Commons license (CC
BY 4.0 for the main content of the book). We welcome contributions from the
community as we have done for prior editions.


Fitting It All In Your Head
------------------------------

A challenge posed by the increased complexity of modern networks is
the difficulty of holding a mental model of the entire networking
stack in one’s head. We are reminded of a quote from Jon
Postel:

  *[There was] a space that we were exploring and, in the early days, we
  figured out this consistent path through the space: IP, TCP, and so
  on. What’s been happening over the last few years is that the IETF
  is filling the rest of the space with every alternative approach,
  not necessarily any better.*

Certainly we can’t hope to teach the next generation of
students about the details of every RFC published in the last 30
years. It would be unhelpful to try. This is what we mean by
giving a sense of perspective. Within that massive space of design
possibilities and protocols, there are certain topics that stand out
because they have out-sized importance in how networks operate, or that
teach a particular design principle.

One way we approach the magnitude of the task is to focus on the
fundamental problem of building a network. The opening paragraph of
our new edition is not greatly different from that of the first
edition:

  *Suppose you want to build a computer network, one that has the
  potential to grow to global scale and support applications as
  diverse as video conferencing, live and on-demand streaming, social
  media, e-commerce, automated manufacturing, and more. What available
  technologies would serve as the underlying building blocks, and what
  kind of software architecture would you design to integrate these
  building blocks into an effective communication service? Answering
  this question is the overriding goal of this book—to describe the
  available building materials and then to show how they can be used
  to construct a network from the ground up.*


The focus on building leads us to teach system design and
architectural principles rather than trying to be an encyclopedia of
networking technologies. We are not just trying to help students
understand the artifacts that exists today: we are trying to equip
them to shape the networks of the future.

One important change noted above is that the set of building blocks
has solidified. Ethernet should be taken as a given as the underlying
network technology. The Internet Protocol (IP) is clearly central to
networking to a greater extent than it was in 1996. And HTTP dominates
the application protocol space. The central role of these three
artifacts presents an opportunity to give the reader a reasonably
complete picture of today’s “default” network architecture: HTTP at
the top of the stack, providing application layer support; IP at the
narrow waist, serving as the demarcation point between what runs
inside the network and what runs at the edges; and Ethernet anchoring
the bottom of the stack, providing a ubiquitous link technology.

Neither Top-Down Nor Bottom-Up
--------------------------------

We have argued, since the first edition, that while layering can be a
useful tool to understand networks, strict layering is too reductive
to allow for a deep understanding of how a networking system
operates. So while our prior editions treated link layer issues early
in the book and application layer issues near the end, we did this not
to provide a “bottom-up” view but to allow us to tackle problems in an
orderly fashion. For example, it’s easier to explain how a reliable
transport protocol works when you have already understood how
unreliable datagram delivery works. But issues such as security and
resource allocation are not confined to a single layer, and so need
their own chapters.

We have embraced this non-layerist view more strongly in the new
edition. Many of the “big ideas in networking” that we believe we need
to cover don’t naturally align with particular layers. Many of the
chapters stand on their own, but of course we need some way to
organize the topics to help the reader develop their mental model of
networking. We don’t want readers to have to suspend disbelief if a
chapter depends on material that hasn’t yet been covered in the book.

One way we accomplish this is to adopt a "2-pass" strategy. In the
first pass (corresponding to Part I of the book) we introduce all the
foundational concepts need to understand the problem-specific examples
described later in the book. These include the basics of network
architectures, over-the-wire protocols, service interfaces,
encapsulation, and so on. This first pass also discusses network
applications (helping the student ground these concepts in something
that's likely to be familiar) and network hardware (helping the student
ground the concepts in something tangible). The second pass
(corresponding to Parts II and III of the book) then show how these
general concepts apply to specific subsystems, both *inside the
network* (Part II) and *at the edge of the network* (Part III).
This latter split helps to separate concerns, with IP's best-effort
packet delivery service defining the boundary between the two.

We've tried to minimize the dependency between Parts II and III—they
are designed to be read in either order—but it turns out there is a
rich set of interdependencies within each part. This is because of our
view that each part describes a multi-faceted ecosystem, with many
interrelated ideas with subtle dependencies. We include generous
cross-references to adjacent topics that readers can follow,
representing these interweaving threads across the software
stack. Following these threads is not a requirement for understanding
each topic, but it does help the reader appreciate that everything is
connected (just not always in a strictly top-down order).

One consequence of each chapter tackling a fundamental challenge in
networking, rather than the next layer in a prescribed stack, is that
we are forced to treat each challenge as an independent system design
problem, one that comes with its own set of requirements, assumptions,
and design choices. This means we begin each chapter with a focus on
architectural questions, reinforcing our sense that the book not only
covers the important concepts and practices in networking, but also
shows—by example—how computer systems are designed and built in
practice. We have decided to emphasize the design process (while
grounding the discussion with coverage of today’s artifacts) because
we believe that mastering the design process will be a requirement for
anyone who wants to take advantage of AI (however it is defined)
rather than be replaced by AI. We believe this will be true whether
you work on networking subsystems, networked applications, or
higher-level services.


Putting it All Together
------------------------------

To recap, our approach focuses on design principles rather than
encyclopedic coverage of specific technologies; we anchor the
discussion around three central artifacts (Ethernet, IP, and HTTP);
and we tackle topics that are central to the problem of building a
network in a non-layerist manner. This has led to the following
organization:

**Part I: Foundation**

Provides a big picture view of networking, explaining the key
challenges such as scale and performance. Introduces fundamental
concepts of networking such as protocols and encapsulation. Examines
some representative applications, with a focus on HTTP as the protocol
underpinning many modern apps. Explains the basics of packet switching
and statistical multiplexing with a focus on Ethernet as the canonical
example of modern packet-switched networks.

**Part II: Inside The Network**

Each chapter tackles one of the problems that needs to be solved to
build a global network of packet switches, starting with how to calculate and
distribute routes. Other chapters include: mediating shared access
(wireless networks); federating networks (internetworking and IP);
allocating resources; virtual networks; and network operations.

**Part III: The Edge of the Network**

We move our focus out to the systems, applications and protocols
running at the edges of the network. We start with a discussion of
naming, including both DNS and application level names. We cover
reliable delivery (TCP and QUIC); remote procedure call; congestion
control; secure channels (TLS); overlays and CDNs; and real-time
applications.

There are some recurring themes that show up repeatedly throughout the
book rather than forming their own chapter. Security is a notable
example. While the chapter on secure channels covers quite a few
security topics (encryption, authentication, public keys, certificate
hierarchies, etc.), it does so in the service of solving a problem:
how to secure the traffic between two endpoints (including a
discussion of what it means to “secure” a channel). Security is an
aspect of networking that should not be treated as something added at
the end, and so it shows up throughout the book, e.g., in wireless
networks, in the security of routing, in discussion of VPNs,
etc. Similarly, many topics have a role to play in datacenter
networking (SDN, congestion control, RPC, virtualization, etc.) and so
we tackle them as they arise rather than calling out datacenters in
their own chapter.

Another common theme is the symbiotic relationship between the
Internet and the cloud. It’s straightforward to explain how the cloud
is built on top of an Internet-based network substrate, but it’s
equally important to take into account—and help students
appreciate—how much the Internet has changed in response to the
existence of the cloud. This includes both the feature velocity that cloud
software enables (which impacts interoperability and operations) and
the global availability of computing resources (which impacts function
placement). It seems important that we shift the networking
mindset away from “fixed protocol stacks” and closer to “agile
software ecosystems”.

This relates to the earlier point about teaching students how to shape
the networks of the future, not just how they work today. The ready
availability of cloud-based resources and their central role in
networking means it is much more feasible for students to experiment
with new networking concepts that it was in an era where most
networking functionality lived in proprietary routers operated by
ISPs.

Given our focus on systems design principles,  we have added a
new design element to the book. These highlighted paragraphs
serve to draw attention to important principles of system design that
generalize beyond the specific artifact under discussion. For example,
the separation of policy and mechanism is a well-known systems
principle that happens to show up when we discuss queuing
algorithms. We take the opportunity to talk more broadly about this
principle at that point in the book and use a visual design element to
draw attention to it.

.. TODO finalize the design and show example here?

Textbooks are just one part of a networking course, but they can
provide a coherent perspective on the subject, helping readers to connect the
dots. A big picture view helps the student make sense of all the
details and see how various technologies fit to from the whole. It
enables them to know what questions to ask, not just telling them how
some bit of technology works today, but setting them up to shape the
networks of the future.

It is important that students don't perceive networking as a set of
fixed technologies that they need to learn, but see it as an evolving
field. Building the networks for data centers to train LLMs, for
example, might be where much of the excitement is today, but the
center of attention is sure to move again in the future just as it has
done in the past. Our job is to give them the understanding of system
design principles and the intellectual framework to manage complexity
that will set them up for the future of networking, whatever it may be.

For some more insight into our
thinking as we worked on 7E, you might find the following posts
informative:

 o `Fitting It All in Your Head <https://systemsapproach.org/2025/10/06/fitting-it-all-in-your-head/>`__. October 2025.

 o `No Skyhooks <https://systemsapproach.org/2026/01/26/no-skyhooks/>`__.  January 2026.

 o `Not Your Father's
 Internet <https://systemsapproach.org/2026/04/20/not-your-fathers-internet/>`__
 April 2026.

Larry and Bruce, June 2026
