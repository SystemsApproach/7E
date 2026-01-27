.. SPDX-FileCopyrightText: 2019 Systems Approach LLC
.. SPDX-FileCopyrightText: 2025 Systems Approach LLC
.. SPDX-License-Identifier: Apache-2.0


Chapter 5:  Mediating Shared Access
========================================

Chapter 1 highlights the challenge of multiplexing multiple users on a
shared network, but focuses on how that problem manifests on switches.
They buffer packets that need to be sent on a given output port, and
decide which of those packets to send next (typically the one at the
head of the queue). When connected to a point-to-point link, there is
no fear that someone else might try to use that link at the same time
you do. (There is a node at the other end of the link, but traffic
being transmitted in each direction is managed independently, without
interference.)

Another possibility is to connect multiple nodes directly to a shared
communication medium, a so called *multi-access network*.  Wireless
networks are an obvious example; the "shared medium" is the space
carrying radio signals. There are also wired examples, with the
original Ethernet being the most widely deployed. Just as with
multiple people in a room trying to talk at the same time, multiple
nodes simultaneously initiating a tranmission will interfere with
each other. Some strategy is needed to decide who's turn it is to
transmit (or speak) next.

This chapter looks at three approaches, as adopted by three different
network technologies. Wi-Fi and 5G are two wireless examples; *Passive
Optical Networks (PON)*\ —colloquially known as Fiber-to-the-Home—is
the third. Before looking at those three approaches in detail, we first take
a closer look at the problem and the design options for addressing those
problems.

.. include:: shared/design.rst
.. include:: shared/wifi.rst
.. include:: shared/cellular.rst
.. include:: shared/pon.rst
