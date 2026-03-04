4.5 Software-Defined Networking
-------------------------------

As discussed in Chapter 3, switches and routers have a control plane
and a data plane, with some interface between them to allow forwarding
rules to be installed in the data plane as the result of routing
calculations made in the control plane. For most of the history of the
Internet, these two planes ran inside each switch and router in the
Internet, and the interface was a strictly internal one. Furthermore,
because the interface existed inside a single box, there was no
pressing need to standardize it: those boxes were implemented by
equipment vendors and the interface between control and data planes
was proprietary and vendor-specific.

There were a few efforts to open up this interface so that the
implementation of the control plane and data plane could be undertaken by
indpendent teams. The approach that finally started to gain traction
around 2009 was the Software-Defined Networking (SDN) movement.

Not only did SDN define a standard interface between the data and
control planes, but it also advanced the idea of using a centalized
control plane to push forwarding rules to many devices implementing
the data plane. A conceptual picture of an SDN system is shown in
:numref:`Figure %s <fig-nos>`.



.. _fig-nos:
.. figure:: routing/figures/Slide05.png
    :width: 500px
    :align: center

    Network Operating System (NOS) hosting a set of control
    applications and providing a logically centralized point of
    control for an underlying network data plane.

Centralized control opens up the possibility to rethink how routing
works in a network. Rather than a fully distributed algorithm of the
sort described in the preceding sections, we now have the option of
using centralized algorithms. As a simple example, a controller could
gather information from all the switches to which is is connected
regarding the state of their links to other switches. With this
information in hand, it has all the information needed to run a
shortest path calculation from the perspective of any switch. Thus it
could calculate forwarding tables for every switch and push them down
to the forwarding plane as a set of flow rules.

Simply replacing the standardrd fully distributed version of OSPF with
a centralized equivalent may not seem much of a step forward. What is
more powerful is the capability to run *new* routing algorithms
centrally, without needing to solve the challenges of getting a
consistent view of the network at every node in a distributed
system.

Some famous examples of novel approaches to routing enabled by SDN
include the work at Google on B4 and Microsoft on SWAN. In both cases,
the aim was to pick paths through a network that were sufficient in
terms of capacity to support the expected  matrix of traffic applied to
the network. This problem of mapping a traffic matrix onto a set of
links is hard to solve efficiently in a fully distributed manner;
centralizing it makes the problem much easier. Thus one of the early
successes of SDN was to solve these *traffic engineering* problems in
the large backbones interconnecting hyperscale data
centers.

.. could include an example from our SDN book Section 2.3

These hyperscale
experiences with SDN show both the value of
being able to customize the network and the power of centralized
control to change networking abstractions. A conversation with
Amin Vahdat, Jennifer Rexford, and David Clark is especially
insightful about the thought process in adopting SDN.

.. _reading_b4:
.. admonition:: Further Reading

   A. Vahdat, D. Clark, and J. Rexford. `A Purpose-built Global Network:
   Google's Move to SDN
   <https://queue.acm.org/detail.cfm?id=2856460>`__.
   ACM Queue, December 2015.

The general idea that centralization is essential to improving the
practice of networking was made strongly by Scott Shenker in a 2011
presentation on SDN. The central thesis of Shenker’s talk is that the
practice of building and operating networks is in dire need of
abstractions to help manage complexity, and he spells out how SDN can
deliver those abstractions.

.. _reading_shenker:
.. admonition:: Further Reading

   S. Shenker. `The Future of Networking and the Past of Protocols
   <https://www.youtube.com/watch?v=YHeyuD89n1Y>`__.
   Open Networking Summit, October 2011.
