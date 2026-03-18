.. SPDX-FileCopyrightText: 2019 Systems Approach LLC
.. SPDX-FileCopyrightText: 2025 Systems Approach LLC
.. SPDX-License-Identifier: Apache-2.0


Chapter 4: Routing
========================================

As we saw in Chapter 3, you can build a network from a set of links
and switches, but once that network gets to any reasonable size, the
problem that becomes apparent is: how to get packets efficiently to
their correct destinations? When you try to open up a page on some
distant web site, how does the first packet finds its way from your
browser to the server that hosts that web site? This is the problem
that routing sets out to solve.

If you have ever used a mapping application to plot a route from A to
B over a network of roads and streets, you have used an algorithm
similar to what most networks use. You obviously don't want to go
around in circles—the path should be loop-free—and you normally want
to optimize some metric, such as travel time or distance. Similar
goals apply for routing across computer networks, but there are a
number of important differences.

There are several different approaches to routing in use today, with
different metrics of success. In the global Internet, routing among
service providers is largely handled by BGP (the Border Gateway
Protocol), which deals with the commercial relationships among
providers while also trying to find paths that are reasonably direct
and free of loops. We describe BGP, probably the most complex routing
system, in Chapter 7.

If we limit ourselves to looking at the network of a single service
provider or an enterprise, we normally see a routing protocol based on
one of two approaches: *link-state routing* or *distance-vector
routing*. We cover each of these in this chapter, along with a third
approach, known a the *spanning tree protocol*.  While the spanning
tree protocol is not typically referred to as a routing algorithm (for
historical reasons), it is a distributed algorithm for finding
loop-free paths in a network of Ethernet switches.


.. include:: routing/design.rst
.. include:: routing/spanningtree.rst
.. include:: routing/linkstate.rst
.. include:: routing/distancevector.rst
.. include:: routing/sdn.rst
