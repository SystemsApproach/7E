.. SPDX-FileCopyrightText: 2025 Systems Approach LLC
.. SPDX-License-Identifier: CC-BY-4.0

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
This is true whether you work on low-level systems or high-level
applications and services.

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

The 7th edition will continue this approach, but after 30 years, we
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
how the topics are organized.

The new organization also changes how we balance “general concepts”
and “concrete artifacts”. On the one hand, every topic is framed in
terms of a general challenge any network must address. The concepts
are the main theme and the chapters are titled accordingly. On the
other hand, three key artifacts—IP, Ethernet, and HTTP—play an
oversized role in how we organize the material. The new organization
breaks the book into two parts, and introduces an anchor artifact for
each part. More on this below.

Role of Artifacts
-------------------------

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
protocols in detail. The set includes usual suspects (TCP, DNS, BGP,
SMTP, TLS), but also new examples, most notably QUIC and DASH.

One insight driving the refactoring of the book is that some artifacts
are more important than others. They represent years of experience
trying different alternatives, with a consensus forming around a
particular way of doing things. But more than that, certain protocols
play an oversized role in shaping the course of how the network
evolves. Other problems are solved under the assumption that that
particular protocol defines a fixed point. We identify three such
protocols, and they have a prominent place in this book.

IP has played that role throughout much of the Internet’s history. It
didn’t pretend to be the only viable networking technology, but
instead positioned itself as a logical network that can be overlaid on
top of *any technology*\—those known today, or yet to be invented. That
approach worked so well that for all practical purposes, “IP Internet”
is now synonymous with “global internet”, with IP providing a
universal, best-effort packet delivery service that makes it possible
to communicate with every connected device in the world. This “narrow
waist” defines the dividing line between Part One and Part Two.

In our view, two other artifacts play a similar role today: (1)
Ethernet switches (which we introduce in the first chapter of Part
One), and (2) the HTTP protocol (which we introduce in the first
chapter of Part Two). We lead with these examples because they provide
an anchor for the rest of the chapters that follow. Building a packet
switched network is a tractable problem when you start with a hardened
building block like an Ethernet switch. Similarly, how we build
network applications—and the collection of sub-modules that enable
them—owes a great deal to HTTP (and the World Wide Web) as the
framework.

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

