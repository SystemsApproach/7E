.. SPDX-FileCopyrightText: 2019 Systems Approach LLC
.. SPDX-FileCopyrightText: 2025 Systems Approach LLC
.. SPDX-License-Identifier: Apache-2.0

.. include:: chapters.rst

Chapter |CC|: Congestion Control
========================================

As introduced in Chapter |Intro|, a packet-switched network based on
statistical multiplexing risks congestion at various routers and
switches throughout the network. Chapter |Capacity| looks at the
router side of the problem, where the immediate concern is that
buffers fill up, and packets may eventually have to be dropped. We now
turn our attention to the role played by the edge hosts, which are
responsible for the volume of traffic flowing through those nodes.

The problem was not top-of-mind when the Internet protocol stack was
first deployed in the early 1980s.  By the end of the decade, however,
with the Internet gaining serious use in universities (but predating
the World Wide Web's invention by several years), the network began to
experience a phenomenon known as *congestion collapse*. A
solution—congestion control—was developed and deployed in the late
1980s and the immediate crisis was addressed. The Internet community
has been studying and refining its approach to congestion control ever
since. This chapter is about that journey.

The most famous early efforts to manage congestion were undertaken by
two researchers, Van Jacobson and Mike Karels. The resulting paper,
*Congestion Avoidance and Control*, published in 1988, is one of the
most cited papers in networking of all time. There are good reasons
for that. One is that congestion collapse really did threaten the
nascent Internet, and the work undertaken to address it was
foundational to the Internet's ultimate success. Without that work
it's unlikely we'd have the global Internet we have today.

Another reason for the citation impact of this work is that congestion
control has been an amazingly fruitful area of research for over three
decades. Congestion control, and resource allocation more broadly, are
wide open design spaces with plenty of room for innovation. Decades of
research and implementation have built on the early foundations, and
it seems fair to assume that new approaches or refinements to the
existing approaches will continue to appear for as long as the
Internet exists.

.. _reading_vj:
.. admonition:: Further Reading

   V. Jacobson. `Congestion Avoidance and Control
   <https://dl.acm.org/doi/10.1145/52324.52356>`__.
   ACM SIGCOMM '88 Symposium, August 1988.

.. include:: congestion/design.rst
.. include:: congestion/loss.rst
.. include:: congestion/delay.rst
.. include:: congestion/variants.rst
