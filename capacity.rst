.. SPDX-FileCopyrightText: 2026 Larry L. Peterson and Bruce S. Davie
.. SPDX-License-Identifier: CC-BY-4.0

.. include:: chapters.rst

Chapter |Capacity|: Allocating Capacity
========================================

We know from Chapter |Intro| that statistical multiplexing is the
foundation for sharing network resources in a best-effort network, but
it isn't the full story about how network resources are allocated.
This chapter fills in the remaining details, looking at how networks
deal with varying traffic loads at both per-node and network-wide
levels.

Each node has two resources that need attention: link bandwidth and
buffer capacity. (The third resource described in Section
|Intro|.4.3—internal forwarding capacity—could in principle be managed
dynamically, but in practice is not.) With respect to link bandwidth,
statistical multiplexing is the starting point, with packets scheduled
for transmission based on demand. But there are ways to augment
statistical multiplexing both to be more fair (i.e., to keep greedy
flows from starving well-behaved flows) and to ensure that important
packets don't get stuck behind unimportant packets.\ [#]_ Such algorithms,
known as *packet schedulers*, effectively allocate link capacity by
deciding which packet to send next.

.. [#] The idea that some packets are more important than others raises a
       set of issues to be discussed later, such as who decides which packets
       are important.


Packet scheduling is an aspect of buffer management—the scheduler
decides which packet to dequeue—but there are additional techniques
for managing packet queues. They are collectively known as *Active Queue
Management (AQM)*, and they generally decide which packet(s)
to drop as the buffer fills up. AQM plays an especially important role
in the Internet because dropping a packet is an indirect way for a
network node to "signal" an edge host—the entity responsible for
injecting the packet into the network—that something is amiss.

Ultimately, responsibility for managing congestion is shared between
routers inside the network (they detect the onset of congestion) and
edge hosts (they adjust their sending rates in response to
congestion).  This chapter focuses on the router side of this
interaction, and we cover the edge host side of this interaction
in Chapter |CC|. For the purpose of this chapter, all we need to know
about edge hosts is that they react to whatever signal the routers
send them by adapting the rate at which they inject packets into the
network.

These node-level mechanisms have to make local decisions in real time,
but there are also network-wide actions that dictate how to place that
traffic onto links and nodes in the first place. This network-wide
decision making process is known as *Traffic Engineering (TE)*, and it
comes into play when per-node capacity is chronically under-provisioned
due to long-term changes in the volume of traffic flowing between
various nodes. TE was originally an off-line process, requiring
administrator intervention, but it is increasingly being done as an
automated (but coarse-grained) part of a network's overall approach to
capacity management.

.. include:: capacity/design.rst
.. include:: capacity/scheduler.rst
.. include:: capacity/aqm.rst
.. include:: capacity/datacenter.rst
.. include:: capacity/traffic.rst
