.. SPDX-FileCopyrightText: 2019 Systems Approach LLC
.. SPDX-FileCopyrightText: 2025 Systems Approach LLC
.. SPDX-License-Identifier: CC-BY-4.0


.. include:: chapters.rst

Chapter |Ops|:  Operating Networks
========================================

The focus of this book has been on the challenge of designing and
building a network, but the day you succeed in getting your network up
and running, you will discover that someone then needs to operate it.
And if you've done a good job in the design phase, that ops team will
be rewarded by getting to operate it for a long time, spanning
generations of hardware and software technologies.  For their sake,
your design should take the network's operational requirements into
account. This chapter looks at those requirements, and describes
mechanisms commonly built into networks to address them.

One of the biggest challenges in understanding network operations is
wading through the plethora of terminology that has been invented to
talk about it. Telcos—some of the earliest
organizations to operate data networks—have nearly a century of
know-how (and associated terminology) in operating networks. Many of
those practices have been adapted from earlier voice networks.
Today's cloud providers operate both massive datacenter networks that
connect tens of thousands of servers, and global backbone networks
that interconnect hundreds of datacenters. Their focus on feature
velocity—which takes advantage of agile software practices and
commodity hardware—has led to an entirely different approach (and
associated terminology) for operations. Yet another class of operator
is the system administrator responsible for buying, configuring, and
maintaining hardware that serves an enterprise or department. Their
approach to operations is heavily influenced by the vendors that sell
them servers, routers, and other network appliances. And finally there
is you, either managing your home network, or, like many people,
outsourcing that responsibility to their ISP.

In this chapter, we will try to avoid the use of arcane terminology,
and focus instead on the fundamental issues all of these operators
face. They all have the same goals: to ensure the network has enough
features and capacity to handle the demands of its current workload,
and then deploy additional resources and functionality when needed;
to decide how all the hardware and software components they deploy
should be configured, and then set their parameters accordingly; and
to detect when something goes wrong, troubleshoot the cause of that
failure, and then take corrective action.

.. include:: operations/design.rst
.. include:: operations/config.rst
.. include:: operations/telemetry.rst
.. include:: operations/velocity.rst
