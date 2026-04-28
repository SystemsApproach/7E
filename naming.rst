.. SPDX-FileCopyrightText: 2019 Systems Approach LLC
.. SPDX-FileCopyrightText: 2025 Systems Approach LLC
.. SPDX-License-Identifier: CC-BY-4.0


.. include:: chapters.rst

Chapter |Naming|:  Naming Systems
========================================

Naming systems are an essential component of any computer system,
since being able to name an object is a requirement for being able to
access or share it. The Internet provides multiple naming schemes—host
names of the form ``cicada.cs.princeton.edu`` and resource locators
of the form ``https://www.cs.princeton.edu/index.html``\ —are two
recognizable examples, but how we identify resources is so pervasive
that it is important to first understand the fundamentals before diving
into the details of specific mechanisms.

For starters, we need to establish a name for the different kinds of
names we'll encounter. Keeping in mind that we're talking about
references to objects—as opposed to the object itself—we can make
the following distinctions: a *name* identifies what object we want to
access (it's a human-readable string), an *address* identifies where
the object is located (it's a machine-readable data structure), and a
*route* identifies a way to reach or connect to the object (it's a
path through the network). The network then provides mechanisms
that can be used to map a name into an address and translate an
address into a route.

This particular definition traces back to an often-cited 1978 paper
written by John Shoch, but as will soon become obvious, precise
definitions about naming concepts can be elusive. A second influential
paper written by Jerry Saltzer in 1982 (republished as an RFC in
1993) comments on how such definitions leave much room for
interpretation. He then goes into significant depth describing the
underlying mechanisms. The terminology Saltzer introduced is still
relevant today, and informs the discussion in this chapter.

We have already seen various aspects of addresses and how they are
used by the routing process inside the network. (Human-readable names
are typically not used by a network's internal mechanisms.) In this
chapter we look at names and addresses—and yes, even routes—at the
edge of the network. This includes the mechanism that maps names used
at the edge of the network onto addresses used inside the network, but
also how other kinds of names, addresses, and routes are used by edge
hosts, entirely on top of the network. Naming Internet resources
requires a multi-faceted solution, which in turn provides the
foundation for all other network services and applications. The
centrality of naming makes it a good topic to begin our exploration of
the software ecosystem running on hosts at the edge of the network.

.. admonition:: Further Reading

   J. Shoch. `A Note on Inter-Network Naming, Addressing, and Routing.
   <https://www.rfc-editor.org/ien/ien19.txt>`__ Internet Experiment
   Note #19. January 1978.

   J. Saltzer. `On the Naming and Binding of Network Destinations.
   <https://datatracker.ietf.org/doc/html/rfc1498>`__  RFC 1498.
   August 1993.

.. include:: naming/design.rst
.. include:: naming/dns.rst
.. include:: naming/apps.rst
