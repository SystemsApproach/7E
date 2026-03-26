.. SPDX-FileCopyrightText: 2019 Systems Approach LLC
.. SPDX-FileCopyrightText: 2025 Systems Approach LLC
.. SPDX-License-Identifier: Apache-2.0

.. include:: chapters.rst

Chapter |Capacity|: Allocating Capacity
========================================

We know from Chapter |Intro| that statstical multiplexing is the
foundation for sharing network resources in a best-effort network, but
statmux isn't the full story about how network resources are
allocated.  This chapter fills in the remaining details, looking at
ways to deal with time-varying traffic at both the per-node and
network-wide levels.

Each nodes has two resources that need attention: link bandwidth and
buffer capacity. (The third resource described in Section
|Intro|.4.3—internal forwarding capacity—could in principle be managed
dynamically, but in practice is not.) With respect to link bandwidth,
statistical multiplexing is the main factor, with packets scheduled
for transmision based on demand. There are no reservations that set
aside bandwidth for particular flows, but there are ways to augment
statistical multiplexing to be more fair; i.e., to keep greedy flows
from starving well-behaved flows. Such algorithms, known as *packet
schedulers*, effectively allocate link capacity.

Managing buffer capacity is, of course, related to packet scheduling
in that the scheduler decides which packet to dequeue for
transmission.  But there are other ways to manage transmission
queues. These mechanisms are collectively known as *Active Queue
Mangement (AQM)*, and they generally decide which packet(s) to drop
when the buffer is full (or nearly full). AQM plays an especially
important role in the Internet because dropping a packet is an
indirect way for a network node to "signal" an edge host—the entity
responsible for injecting the packet into the network—that something
is amiss.

Ultimately, responsibility for managing congestion is shared between
routers inside the network (they detect congestion) and edge hosts
(they adjust their sending rates, which is otherwise known as
controlling congestion). This chapter focuses on the router side of
this interaction, and we cover the edge host side of the this
interaction in Chapter |CC|. For the purpose of this chapter, all we
need to know about edge hosts is that they "see" whatever signal the
routers send them, and they adapt their sending rate in response.

The node-level mechanisms have to deal with traffic in real-time, but
there are also network-wide actions that dictate how to place that
traffic onto links and nodes in the first place. The problem is known
as *Traffic Engineering (TE)*, it is increasingly being done as a
continuous (but coarse-grain) part of a network's overall approach
to capacity management.

.. include:: capacity/design.rst
.. include:: capacity/scheduler.rst
.. include:: capacity/aqm.rst
.. include:: capacity/traffic.rst
