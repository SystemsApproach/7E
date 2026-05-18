.. SPDX-FileCopyrightText: 2019 Systems Approach LLC
.. SPDX-FileCopyrightText: 2025 Systems Approach LLC
.. SPDX-License-Identifier: Apache-2.0

.. include:: chapters.rst

.. _artifact-bpg:

Chapter |BGP|: Interdomain Routing
========================================

The previous chapter described the mechanisms that are used to
assemble a set of independent networks into a logical internet. This
makes end-to-end packet delivery possible, but doesn't say anything
about how routes among the independent networks are calculated and
distributed. Whereas our discussion of routing in Chapter |Routing| focused on
finding paths with the lowest cost according to some metric, routing
among independent networks is largely about satisfying the policies
that govern how networks are used.  This chapter looks at this issue
by describing how routing policies are specified and enforced. The
answer is intimately tied up with the mechanism routers use to learn
what paths are available, and decide which paths to use to deliver
their traffic.

Calculating and sharing routes among autonomously operated networks
has proven to be one of the greatest challenges facing the Internet.
The protocol responsible for providing a solution, the Border Gateway
Protocol (BGP), now carries about one million pieces of routing
information, and bears the responsibility of allowing Internet Service
Providers and their customers to set and enforce business rules about
whose traffic is carried by whom. Moreover, establishing trust in that
information, by providing defenses against attempts to hijack and
"black hole" traffic, is a critical aspect of the solution.  We
explore these challenges in this chapter.

The routing algorithms described in Chapter |Routing| are not up to
the challenge, for two main reasons. The first is the issue of
scale. There are tens of thousands of autonomous organizations in the
Internet and they span the globe, so it's not clear that the routing
protocols we have seen to date could scale to that level even if we
treated each organization as a simple point in a graph. But more
importantly, the problem definition is not "find the shortest path to
destination X". "Shortest path" is not even well defined, since each
network can use independent metrics for their internal
routing. Instead, the problem is "find a path to X that matches the
policies of the providers who can deliver traffic to X". Ever since
the Internet transitioned from being a research network to a
commercially viable communication substrate in the 1990s, BGP has been
the protocol used to answer questions of that kind.

.. include:: policy/design.rst
.. include:: policy/routingbgp.rst
.. include:: policy/securebgp.rst
