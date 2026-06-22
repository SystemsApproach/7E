.. SPDX-FileCopyrightText: 2026 Larry L. Peterson and Bruce S. Davie
.. SPDX-License-Identifier: CC-BY-4.0

.. include:: chapters.rst

Chapter |Overlay|: Overlay Networks
=====================================

An overlay is a logical network implemented on top of some underlying
network. By this definition, the Internet started out as an overlay
network; a packet-switched network implemented on top of the circuits
provided by the old telephone network. Eventually those links from the
telephone network were replaced with point-to-point links directly
connecting the backbone routers. The Internet itself became the
foundational network on top of which new overlays could be built.

:numref:`Figure %s <fig-overlay-net>` depicts an overlay implemented
on top of an underlying network. Each node in the overlay processes
and forwards packets in an application-specific way. Because they run
arbitrary code, these overlay nodes are usually servers rather than
switches; these server are often co-located with a switch in the
underlying network. The links that connect the overlay nodes are
implemented as logical links (e.g., tunnels) through the underlying
network.

.. _fig-overlay-net:
.. figure:: overlay/figures/f09-19-9780123850591.png
   :width: 300px
   :align: center

   Overlay network layered on top of a physical network.

Overlays can be viewed as forming a network with some topology. The
example in :numref:`Figure %s <fig-overlay-net>` is a subgraph of the
underlying network, but another common configuration is for the
overlay to form a fully-connected graph. This is easy to do when the
underlying network is the Internet, since the Internet supports a
logical connection between every pair of hosts. We raise this point
because, while some overlays do include routing as part of their
"application logic", in other cases, the whole purpose of the overlay
is to give the application multiple points-of-presence. This
improves client-experienced latency (the application is likely to be
accessible at a nearby site) and aggregate application throughput
(many application sites can be accessed in parallel).

Overlays are an integral part of the software ecosystem running at the
edge of the Internet. Like transport protocols that run on individual
hosts in support of application programs, overlays run on a
distributed collection of edge hosts, also in support of applications.
They are a critical part of the Internet's approach to both scaling
applications and making them more resilient. This chapter looks at
three widely-used examples: Content Distribution Networks (CDNs), the
multicast overlays at the heart of video conferencing applications
such as Zoom, and peer-to-peer file sharing networks such as
BitTorrent. These three examples serve as case studies, helping to
illustrate a range of strategies available to applications.

.. include:: overlay/design.rst
.. include:: overlay/cdn.rst
.. include:: overlay/conference.rst
.. include:: overlay/p2p.rst
