.. SPDX-FileCopyrightText: 2019 Systems Approach LLC
.. SPDX-FileCopyrightText: 2025 Systems Approach LLC
.. SPDX-License-Identifier: Apache-2.0


Chapter 6: Federating Networks
========================================

Having looked at the operation of switches and the routing algorithms
that allow us to determine best paths through large networks, we are
well on our way to being able to build a network of global scale.
However, there is an another aspect of the Internet that has enabled
it to scale to connect the majority of the world's population: the
ability to interconnect independently built and operated networks.

This results in a global *internetwork* that has no central point of
control and yet manages to deliver a reasonably consistent service to
all those devices and the applications running on them. Now that the
Internet has been around for more than 50 years it is easy to lose
sight of the design decisions and challenges that had to be tackled to
achieve this remarkable feat. In this chapter we examine the problem
of *federating* networks: taking networks that are run by independent
operators using widely varying underlying technologies and connecting
them together to provide a useful, global packet delivery service.

Federating autonomous networks helps scale the approach to operations,
since each organization that manages a network can make its own
independent decisions about how to manage that network. It also gives
us a handle on how to scale routing to calculate paths across a global
network by breaking routing into two parts: routing among networks and
routing within individual networks. At the same time we need to find a
way to address billions of devices, a problem that turned out to be
bigger than first envisioned by the Internet's designers.

Compounding the issue of scale is the problem of heterogeneity. The
original Internet connected the ARPANET, a pioneering packet-switched
network, with some other early networks that used radio and satellite
technology. One of the early design decisions was that the other
networks would not be modified to "fit" into the Internet; the
Internet would have to be able to accommodate all these different
technologies. That has proven to be one of the pivotal decisions that
has allowed the Internet to succeed as it has accommodated new
technologies ranging from multi-gigabit optical fiber to 5G
wireless—technologies that were not even on the horizon at the time of
the Internet's design.

Calculating and sharing routes among the autonomously operated
networks making up the Internet has proven to be one of its greatest
challenges. Not only does the Internet's interdomain routing protocol,
BGP, now carry about one million pieces of routing information, but it
also bears the responsibility of allowing Internet Service Providers
and their customers to set and enforce business rules about whose
traffic is carried by whom. We explore these challenges in this
chapter. Finally we look at the ongoing work of securing the routing system
against attacks that can lead to traffic hijacking, black holes, and
other system-wide problems.

.. _reading_inet:
.. admonition:: Further Reading

   B. Leiner, et. al. `A Brief History of the Internet
   <https://dl.acm.org/doi/10.1145/1629607.1629613>`__.
   SIGCOMM CCR, October 2009.



.. include:: federation/design.rst
.. include:: federation/hetero.rst
.. include:: federation/scale.rst
.. include:: federation/routingbgp.rst
.. include:: federation/securebgp.rst
