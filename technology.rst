.. SPDX-FileCopyrightText: 2019 Systems Approach LLC
.. SPDX-FileCopyrightText: 2025 Systems Approach LLC
.. SPDX-License-Identifier: Apache-2.0


Chapter 3:  Network Technology
==================================

We started Chapter 1 by asking one of the first questions any system architect must ask:

  *What available technologies will serve as the underlying building blocks?*

When the inventors of the Internet asked that question in the
mid-1970s, there weren’t many options. The ARPANET was an experimental
wide-area network, and the Ethernet had just been invented by
researchers at Xerox. At the time, large mainframe computers were the
norm, with Digital Equipment Corporation’s PDP-11 (a 16-bit
minicomputer that would inspire Intel’s x86 and Motorola’s 68000
microprocessors) being the most influential offering in the “small and
affordable” computer market.

To set a bit more historical context, the viability of packet
switching itself—as opposed to circuit switching technology used by
phone companies—was the ARPANET’s main innovation. The ARPANET, in
turn, was built on top of two existing technologies: (1) the ability
to lease 64kbps digital circuits from AT&T, and (2) a 16-bit processor
from Honeywell, a peer of the PDP-11 that was typically deployed in
industrial environments. The former provided the link technology and
the latter was programmed to forward packets.

Jump forward to the present day, and the technology landscape is very
different. Today’s phone companies emulate voice circuits on top of a
packet switch substrate that can be directly traced back to the
ARPANET. Hundreds of link technologies have been proposed (and
abandoned), with Ethernet continually adapting to serve as a
ubiquitous communication technology. And while low-end packet switches
are still implemented in software running on general-purpose
processors, there are now domain-specific chips optimized for packet
switching, analogous to the emergence of GPUs for graphics and TPUs
for AI.

This chapter gives an overview of these building block technologies,
which then serve as the basis for Part II. While this chapter focuses
on Ethernet, we caution that other technologies do exist. We discuss
some of them in the last section, but Ethernet is the example
technology that we describe in detail. If you understand Ethernet in
depth—both its links and switches—you can more easily digest any other
technology you encounter.

.. include:: technology/link.rst
.. include:: technology/switch.rst
.. include:: technology/circuits.rst
