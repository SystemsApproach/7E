.. SPDX-FileCopyrightText: 2026 Larry L. Peterson and Bruce S. Davie
.. SPDX-License-Identifier: CC-BY-4.0

.. include:: chapters.rst

Chapter |Virt|: Virtualizing Networks
=======================================

The virtualization of computing resources is so deeply entrenched in
the design of modern computer systems that we often take it for
granted. The term "virtual" gets thrown around in many contexts, but
when it comes to computing, the definition is reasonably precise: a
virtualized resource provides the same interface to consumers of that
resource as a physical resource would, but it is done in a way that
decouples it from specific physical resources.

One of the easiest examples to understand is virtual memory. An
application has access to a potentially vast amount of virtual memory,
and can freely read and write to any address in that memory, without
worrying about the details of the underlying physical memory. The
physical memory is usually shared among many processes and users, and
is limited to orders of magnitude fewer bytes than the virtual memory.
But all of these details are hidden from the application or
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

One early and widely used approach to virtualization is the virtual LAN
(VLAN). Each VLAN is a separate broadcast domain from any other VLAN,
even if it runs on the same physical Ethernet as other
VLANs. By providing isolation among different tenants on shared
infrastructure, VLANs are analogous to virtual memory systems.

Virtual Private Networks (VPNs) provide something similar to VLANs but
with wide-area network capabilities. There are different
ways to deliver a VPN service, but they are commonly offered as a
private IP network to interconnect the sites of a large business with
multiple offices. In another use case, VPNs allow the devices of
remote workers to connect to the home office.

Network virtualization for datacenters is most closely analogous to
virtual machines on the computing side. The idea is to create
an abstraction of a complete network with features that
include switching, routing, firewalling, and NAT, among others. These
virtual networks resemble virtual machines in their completeness:
they allow applications that depend on a rich set
of capabilities to be deployed onto virtual networks without
modification, just as an unmodified operating system can be deployed
into a virtual machine as if it was running on a physical
machine. Network virtualization lies at the heart of many modern cloud
computing systems.



.. include:: virtual/design.rst
.. include:: virtual/vlan.rst
.. include:: virtual/vpn.rst
.. include:: virtual/datacenter.rst
.. include:: virtual/microseg.rst
