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
addressed by routers and switches that implement the network packet
deliver service, but in exploring those challenges, we incude
descriptions of the following artifacts:

.. table::
   :align: center
   :widths: auto

   ======================   ===========
   Protocol                 Coverage
   ======================   ===========
   5G                       :ref:`Section 5.4 <artifact-5g>`
   802.1 (Spanning Tree)    :ref:`Section 4.2 <artifact-stp>`
   802.11 (Wi-Fi)           :ref:`Section 5.3 <artifact-80211>`
   802.1Q (VLAN)            :ref:`Section 9.2 <artifact-vlan>`
   ARP                      :ref:`Section 6.2.4 <artifact-arp>`
   BGP                      :ref:`Chapter 7 <artifact-bpg>`
   DHCP                     :ref:`Section 10.2.1 <artifact-dhcp>`
   ECMP                     :ref:`Section 4.5 <artifact_ecmp>`
   IPv4                     :ref:`Section 6.2 <artifact-ipv4>`
   IPv6                     :ref:`Section 6.3.3 <artifact-ipv6>`
   MPLS                     :ref:`Section 9.3.4 <artifact-mpls>`
   NAT                      :ref:`Section 6.3.2 <artifact-nat>`
   OSPF                     :ref:`Section 4.3 <artifact-ospf>`
   PON                      :ref:`Section 5.5 <artifact-pon>`
   RIP                      :ref:`Section 4.4 <artifact-rip>`
   YANG                     :ref:`Section 10.2.2 <artifact-yang>`
   ======================   ===========
