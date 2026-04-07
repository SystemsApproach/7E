.. SPDX-FileCopyrightText: 2019 Systems Approach LLC
.. SPDX-FileCopyrightText: 2025 Systems Approach LLC
.. SPDX-License-Identifier: CC-BY-4.0

.. include:: chapters.rst

Chapter |Virt|: Virtual Networks
====================================

Virtualization of computing resources is so deeply entrenched in the
design of modern computer systems that we often take it for
granted. The term "virtual" gets thrown around in many contexts, but
when it comes to computing, the definition is reasonably precise: a
virtualized resource provides the same interface to consumers of that
resource as a physical resource would do, but it is done in a way that
decouples it from specific physical resources.

One of the easiest examples to understand is virtual memory. An
application has access to a potentially vast amount of virtual memory,
and can freely read and write to any address in that memory, without
worrying about the details of the underlying physical memory. The
physical memory is usually shared among many processes and users and
is limited to a smaller number of bytes than the virtual memory, but
all of these details are hidden from the application or
programmer. With a mixture of operating system techniques and hardware
assistance to map from virtual addresses to physical addresses, the
illusion of a large, private memory space is presented seamlessly to
the application.

Virtualization of network resources has been done for almost as long as packet
switching, with the "virtual circuit" being an example of an
abstraction provided by some early packet-switched networks such as
X.25, Frame Relay, and ATM. But it turns out that there are many
different ways to virtualize networks, just as there are many ways to
virtualize computing resources. In this chapter we look at some of the
most common approaches to virtualizing networks.




.. include:: virtual/design.rst
.. include:: virtual/vlan.rst
.. include:: virtual/vpn.rst
.. include:: virtual/datacenter.rst
