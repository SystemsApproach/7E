.. SPDX-FileCopyrightText: 2025 Systems Approach LLC
.. SPDX-License-Identifier: CC-BY-4.0

.. include:: chapters.rst

Preface
==========

As in the previous six editions, this book brings the Systems Approach
to networking. The Systems Approach is a methodology for designing,
implementing, and describing computer systems. It involves a specific
set of steps:

* Defining the problem or the need for a new component or system.
* Specifying the requirements, taking all stakeholders into account.
* Identifying the available building blocks to establish a tangible foundation.
* Exploring the design space and articulating the rationale for design decisions.
* Evaluating the effectiveness of the design with respect to the requirements.
* Extracting lessons learned, summarizing best practices and design principles.

We emphasize the approach because our goal is to both cover the
important concepts and practices in networking, and show—by
example—how computer systems are designed and built in practice. If
you work in the software industry (or hope to), mastering the latter
is likely to be a requirement for participating in the design process.
It is also becoming a requirement for anyone that wants to take
advantage of AI rather than be replaced by AI.  This is true whether
you work on low-level systems or high-level applications and services.

When applying the Systems Approach to networking in earlier editions,
we started each chapter with the problem to be solved in that chapter.
Chapter 1 framed the problem simply as “Building a Network”. Each
chapter tackled another problem ranging from connecting computers
directly to scaling networks to implementing applications to run over
networks. While the sequence of problems we tackled were roughly
ordered from the bottom up, our approach viewed layering as an
abstraction that is only partly useful in discussing networking. Many
issues, such as security and resource allocation, cut across the
traditional layers and need to be addressed with a system-wide
view. Another way of describing the Systems Approach is that it
requires looking at the whole end-to-end system and the interaction of
components within that system; you can’t just optimize a single
component (or layer) and declare success.

The 7th edition continues this approach, but after 30 years, we
believe it is time for a generational revision rather than an
incremental update. The following highlights the most important
changes.

New Organization
--------------------------

The most noticeable change is how the material is factored into
chapters. This is the first significant re-org of the book since the
2nd Edition (1999). See the Table of Contents for the details, but the
organizing principle revolves around the third bullet from above:
*identify the available building blocks.* The networking landscape is
much different than it was in 1995, and this needs to be reflected in
where the story begins and how the how the narrative flows.

The new organization is neither top-down nor bottom-up. Instead, we
started with the essential set of “big ideas” at the heart of computer
networking. There are many topics we could talk about—some bordering
on information theory, others bordering on cloud computing, and still
others involving policy and economics—but there is also a set of
topics that we can probably all agree represent the intellectual core
of the discipline (e.g., how to achieve scalable connectivity, how to
decentralize resource sharing, how to achieve high performance in the
face of large delays). For this set, our goal was to address each
topic in as self-contained way as possible, without leaning too
heavily into layering.

Of those "big ideas", the narrow waist of the Internet architecture
is at the center of how we organize the book. We divided the
topics (chapters) into two parts: Part II covers topics *inside the
network* (describing how to turn the underlying building blocks into a
best-effort packet delivery service of global scale) and Part III
covers topics *at the edge of the network* (describing the software
ecosystem that applications leverage to make effective use of a
best-effort packet delivery service). We had to pick an order for the
book, but Parts II and III can be read in either order. This is because
Part I sets the stage, defining the foundational concepts of
networking. This includes introducing the building block technologies
assumed by Part II and the target set of applications assumed by
Part III.

Role of Artifacts
-------------------------

The new organization is neither top-down nor bottom-up. Instead, we
started with the essential set of “big ideas” at the heart of computer
networking. There are many topics we could talk about—some bordering
on information theory, others bordering on cloud computing, and still
others involving policy and economics—but there is also a set of
topics that we can probably all agree represent the intellectual core
of the discipline (e.g., how to achieve scalable connectivity, how to
decentralize resource sharing, how to achieve high performance in the
face of large delays). For this set, our goal was to address each
topic in as self-contained way as possible, without leaning too
heavily into layering.

Of those "big ideas", the narrow waist of the Internet architecture
plays an outsized role in how we organize the topics. We divided the
topics (chapters) into two parts: Part II covers topics *inside the
network* (describing how to turn the underlying building blocks into a
best-effort packet delivery service of global scale) and Part III
covers topics *at the edge of the network* (describing the software
ecosystem that applications leverage to make effective use of a
best-effort packet delivery service). We had to pick an order for the
book, but Parts II and III can be read in either order. This is because
Part I sets the stage, defining the foundational concepts of
networking. This includes introducing the building block technologies
assumed by Part II and the target set of applications assumed by
Part III.

Role of Artifacts
-------------------------

The new organization also changes how we balance “general concepts”
and “concrete artifacts”. On the one hand, every topic is framed in
terms of a general challenge any network must address. The concepts
are the main theme and the chapters are titled accordingly. On the
other hand, three key artifacts—IP, Ethernet, and HTTP—play an
oversized role in how we organize the material. The new organization
breaks the book into two parts, and introduces an anchor artifact for
each part.

Because networks are constantly evolving, it is important to be able
to think beyond today’s protocol standards and commercial
technologies; we refer to these as *artifacts*. At the same time,
staying purely in the “conceptual plane” is not satisfactory
either. The details are important, even if those details might
change. It’s not necessary to understand every protocol or standard we
encounter in its full glory, but seeing the details of select examples
helps you digest the next protocol you encounter. Seeing concrete
examples is also essential to understanding the general concepts in
depth, and satisfies the expectation that this book covers widely used
protocols in detail. For readers that want to jump directly to
descriptions of specific protocols, the introduction to each Part
includes an "artifact index" its chapters.

One insight driving the refactoring of the book is that some artifacts
are more important than others. They represent years of experience
trying different alternatives, with a consensus forming around a
particular way of doing things. But more than that, certain protocols
play an oversized role in shaping the course of how the network
evolves. Other problems are solved under the assumption that that
particular protocol defines a fixed point. We identify three such
protocols, and they have a prominent place in this book.

We have already talked about one—IP—which has played that role
throughout much of the Internet’s history. It didn’t pretend to be the
only viable networking technology, but instead positioned itself as a
logical network that can be overlaid on top of *any technology*\—those
known today, or yet to be invented. That approach worked so well that
for all practical purposes, “IP Internet” is now synonymous with
“global internet”, with IP providing a universal, best-effort packet
delivery service that makes it possible to communicate with every
connected device in the world. This “narrow waist” defines the
dividing line between Part II and Part III.

In our view, two other artifacts play a similar role today: the HTTP
protocol (which we introduce in Chapter 2), and Ethernet switches
(which we introduce in Chapter 3). We lead with these examples because
they provide an anchor for the rest of the chapters that follow.
Building a packet switched network is a tractable problem when you
start with a hardened building block like an Ethernet switch. Similarly,
how we build network applications—and the collection of sub-modules
that enable them—owes a great deal to HTTP (and the World Wide Web) as
the framework.


New Focus / New Topics
-------------------------

The reorganization is, in part, a response to the emergence of the
cloud as the “center of gravity” for the language we use to talk about
and describe networks. That language comes primarily from research
papers published by Google, Microsoft, Meta, and other cloud
providers. In contrast, the first six editions of our book were
heavily influenced by the language of RFCs, which were often written
by network vendors. Those RFCs still serve a purpose, but the
Internet’s continued evolution is no longer entirely gated by the
IETF.  The cloud providers are supplanting the router vendors as
thought leaders, and putting their stamp on today’s RFCs in the
process.

This focus brings new topics to the forefront. These include:

 o Applications implemented as scalable cloud services: Sections
 |Apps|.1 and |Apps|.2.

 o QUIC as an alternative to TCP: Section |Message|.3.

 o Datacenter switching fabrics: Sections |Routing|.5,
 |Capacity|.4, and |CC|.5.

 o Traffic engineering for datacenter backbones: Section |Capacity|.5.

 o Datacenter networking in support for AI workloads: Sections
 |Message|.4 and |Message|.5.

 o SDN and programmable switches: Sections |Tech|.2 and |Routing|.5.

Of particular note, because the book is organized around the
fundamental problems of networking, a topic like "Datacenter Networks"
appears in multiple places (rather than a single chapter). This is
because datacenters face the same set of problems as any network; they
have just tailored the solutions to their specific circumstances.

Layering Revisited
--------------------------

Earlier editions of this book emphasized our attitude towards
layering, which is that it can be a helpful tool to managing
complexity, but layers are not axiomatic. The new organization is even
less "layerist" than earlier editions. While is true that Parts II and
III roughly divide the topics into two broad layers (which as noted
above, can be read in either order), the topics within each Part are
largely orthogonal; each covers a self-contained challenge.

This does not mean that there are no dependencies. By their nature,
each Part describes a multi-faceted ecosystem, with many interrelated
ideas with subtle dependencies. We include generous cross-references
to adjacent topics that readers can follow, representing these
interweaving threads across the software stack. Following these
threads is not a requirement for understanding each topic, but it does
help the reader appreciate that everything is connected (just not
always in a strictly top-down order).

.. TODO -- Previous paragraph is rough, but a first attempt at calling
   attention to what I believe is a feature we should highlight.


Emphasis on Perspective
--------------------------------

Perspective—explaining the “why” of network design—is a cornerstone of
the Systems Approach, and will continue to be so. But we have an
opportunity to expand on this theme by giving our long-term
perspective on how systems evolve, and to discuss how both technical
decisions and other factors played a role in that evolution. We call
out these “big picture” lessons when they arise.

Part of this perspective is our judgement on what constitutes the
right building blocks to start with. This is why updating the book to
reflect today’s reality, rather than assuming the state of the world
in 1995 (as we did in the 1st Edition) is so important. (Subsequent
editions added and removed topics, but the overall organization
remained largely rooted in 1995.) While there is arguably more
complexity in the Internet today than in 1995, some things are
simpler. For example, there are fewer competing link layers, so we can
focus most of our energy on Ethernet. On the flip side, more attention
needs to be paid to the symbiotic relationship between the Internet
and the cloud, with the Internet providing the communication substrate
that the cloud runs on, and the cloud providing the computing
substrate that enables ever more powerful wide-area applications. Our
approach to network applications emphasizes this relationship.

Supplemental Content
--------------------------------

In the process of refactoring our coverage of networking, some topics
receive less attention than in past editions, and some emerging topics
cannot be covered in as much detail as we would like. To account for
these limits, the book includes references to a wealth of supplemental
material. This includes links to relevant subsections for the 6th
Edition (which continues to be available online), as well as links to
a series of monographs we have written since publishing 6E in 2020.
These monographs cover what we believe to be among the most important
topics over the last few years, including SDN, 5G, Congestion Control,
Operational Practices, and Security (with a book on ML/AI in the works).
We reference these books for students who want to go deeper on these
important topics, which allows us to focus on more introductory
material (and keep the length of this book in check).

Another source of supplemental information is our bi-weekly
newsletter, where among other things, we sometimes talk about
book-related issues that we're wrestling with. For insight into our
thinking as we worked on 7E, you might find the following posts
informative:

 o `Fitting It All in Your Head <https://systemsapproach.org/2025/10/06/fitting-it-all-in-your-head/>`__. October 2025.

 o `No Skyhooks <https://systemsapproach.org/2026/01/26/no-skyhooks/>`__.  January 2026

 o `Not Your Father's
 Internet <https://systemsapproach.org/2026/04/20/not-your-fathers-internet/>`__
 April 2026.

.. TODO -- We could include posts that discuss various topics, but I
   limited this list to "supplemental to helping you decipher the Preface."
 
