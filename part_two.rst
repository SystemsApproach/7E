.. SPDX-FileCopyrightText: 2025 Systems Approach LLC
.. SPDX-License-Identifier: CC-BY-4.0

.. role:: pop

:pop:`Part II: Inside the Network`
==============================================

Billions of devices distributed across the planet are connected to
today’s Internet, where any one of them can send and receive messages
with any of the other devices. The network that connects these devices
must itself also be highly distributed, and is built from millions of
packet switches. Part II describes how to build a global-scale
best-effort packet delivery service by interconnecting switches. The
chapters in Part II address the key challenges in assembling a packet
switch network that interconnects tens of billions of edge
devices. This substrate makes it possible for those devices to host
the edge software stack described in Part III.

The chapters in Part II are organized around the main challeges
addressed by the edge network stack, but in exploring those
challenges, we incude descriptions of the following artifacts:

.. table::
   :align: center
   :widths: auto

   ===========   ===========
   Protocol                  Coverage
   ===========   ===========
   5G                           Section 5.4
   802.11                    Section 5.3
   ARP                         :ref:`Section 6.2.5 <artifact-arp>`
   BGP                         :ref:`Section 6.4 <artifact-bpg>`
   OSPF                       Section 4.3
   IPv4                        :ref:`Section 6.2 <artifact-ipv4>`
   IPv6                        :ref:`Section 6.3.3 <artifact-ipv6>`
   RIP                          Section 4.4
   ===========   ===========
