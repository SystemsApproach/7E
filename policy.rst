.. SPDX-FileCopyrightText: 2019 Systems Approach LLC
.. SPDX-FileCopyrightText: 2025 Systems Approach LLC
.. SPDX-License-Identifier: Apache-2.0


Chapter 7: Managing Policy
========================================

The previous chapter described the mechanisms that are used to
assemble a set of independent networks into a logical internet that
supports end-to-end packet delivery. This chapter looks at the
question of how policy is specified and enforced, govering how that
connectivity is actually used. This topic is intimately tied up with
the question of how routers learn what paths are available, and decide
which paths to use to deliver their traffic.

Calculating and sharing routes among autonomously operated networks
has proven to be one of the greatest challenge facing the Internet.
The protocol responsible for providing a solution, the Border Gateway
Protocol (BGP), now carries about one million pieces of routing
information, and bears the responsibility of allowing Internet Service
Providers and their customers to set and enforce business rules about
whose traffic is carried by whom. Moreover, trusting that information,
by providing defenses against attempts to hijack and "black hole"
traffic, critical aspect of the solution.  We explore these challenges
in this chapter.

Note that the routing algorithms described in Chapter 4 are not up to
the challenge. This is for two main reasons. The first is the issue of
scale. There are tens of thousands of autonomous organizations in the
Internet and they span the globe, so its not clear that the routing
protocols we have seen to date could scale to that level even if we
treated each organization as a simple point in a graph. But more
importantly, the problem definition is not "find the shortest path to
destination X". Instead, it is "find a path to X that matches the
policies of the providers who can deliver traffic to X". Ever since
the Internet transitioned from being a research network to a
commercially viable communication substrate in the 1990s, BGP has been
the protocol used to answer questions of that kind.

.. include:: policy/design.rst
.. include:: policy/routingbgp.rst
.. include:: policy/securebgp.rst
