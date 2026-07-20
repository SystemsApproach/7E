.. index:: SNMP: Simple Network Management Protocol
.. index:: MIB: Management Information Base
.. index:: XML: Extensible Markup Language
.. index:: YAML: YAML Ain't Markup Language
.. index:: gNMI: gRPC Network Management Interface
.. index:: gNOI: gRPC Network Operations Interface
.. index:: YANG: Yet Another Next Generation

|Ops|.3 Network Configuration
-----------------------------------

We now turn our attention to the more general problem of configuring
all the switches and routers—plus the software they run—inside the
network. Our focus is on the protocols, interfaces, data models, and
open source tools that are commonly used.  Every network adopts its
own operational practices, so there is no single solution we can point
to. But there are some broad trends among the large cloud operators
that we use to illustrate the approaches to configuration
management. We use cloud operators as our exemplar because they are
pushing the boundaries of automation, which we expect to become more
widely adopted over time.  Keeping in mind this is a partial view of
the full landscape of operational practices, we conclude this section
with a brief introduction to another example toolset.


|Ops|.3.1 Configuration Interface
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

As outlined in Section |Ops|.1, the core problem is to define the set
of variables available for operators to ``GET`` and ``SET`` on these
devices, with the additional requirement that this dictionary of
variables should be uniform across devices (i.e., be vendor-agnostic).
We also need an over-the-wire protocol to remotely invoke these
operations, but that's the easy part, as we'll explain below.

The Internet has gone through a decades-long exercise defining such a
dictionary, resulting in the *Management Information Base (MIB)*,
which is used in conjunction with the *Simple Network Management
Protocol (SNMP)*; the latter is the protocol used to issue ``GET`` and
``SET`` commands for MIB-defined variables. SNMP has slightly
different names for these operations, specifically ``GetRequest`` and
``SetRequest``, plus other operations designed to simplify the process
of walking through an array of related variables, but the idea is
straightforward. In any case, our focus is on the variable dictionary.

.. admonition:: Further Reading

   J. Case, M. Fedor, M. Schoffstall, and J. Davin.  `Simple Network
   Management Protocol (SNMP)
   <https://www.rfc-editor.org/info/rfc1157>`__. RFC 1157, May 1990.

   K. McCloghrie and M. Rose. `Management Information Base for Network
   Management of TCP/IP-based Internets: MIB-II
   <https://www.rfc-editor.org/info/rfc1213>`__. RFC 1213, March 1991.

You can learn more about the basics of SNMP and the MIB from RFCs 1157
and 1213, respectively, and if you want to follow the history of
incremental refinements, there is a long list of follow-on RFCs. But
all of this work is based on an approach that pre-dates the
availability of modern modeling languages, of which YANG has become
the widely-accepted solution. YANG—which stands for *Yet Another Next
Generation*, a name chosen to poke fun at how often a do-over proves
necessary. YANG defines the structure of the data, and is general
enough to be used in conjunction with different over-the-wire message
formats, including XML, YAML, JSON, and Protocol Buffers (protobufs).

.. admonition:: Further Reading

   M. Bjorklund (Ed.). `YANG: A Data Modeling Language for the Network
   Configuration Protocol (NETCONF)
   <https://www.rfc-editor.org/info/rfc6020>`__. RFC 6020,
   October 2010.

   `YAML Ain't Markup Language (YAML) <https://yaml.org>`__.

.. sidebar:: Markup Languages

   *Because of HTML, markup languages are now part of the vernacular
   in networking, but that doesn't mean they are understood. The term
   originates in the practice of "marking up" a typed manuscript with
   annotations in the margins. From there, it's easy to see the
   connection to HTML, which is used to annotate data so that a web
   browser knows how to render it on a display. The next
   generalization was to markup (annotate) data for purposes other
   than to display it, for example, so that two programs could
   exchange data over HTTP as part of a e-commerce transaction. XML
   (Extensible Markup Language) serves that purpose.  For example, XML
   supports tagged data such as*
   ``<product_num>1234567</product_num>``
   *in addition to the familiar HTML tags.*

   *The complaint about XML was that it proved hard for humans to use
   directly, for example, when they are trying to "describe" some
   collection of data. No one wants to repeatedly type "<element_name>
   ... </element_name>" for every data item they enter. They do want
   the structured organization of data, and that's what YAML provides.
   Technically, this means YAML is not a markup language; hence it's
   name: YAML Ain't a Markup Language. YAML is actually closer to JSON
   (JavaScript Object Notation) than XML.*

   *Another piece of the puzzle is that all these markup languages
   need a companion language that is used to define the schema, or
   data model, for the information being represented. For XML, that
   companion language is XSD (XML Schema Definition). For YAML, YANG
   is commonly used (although YANG can also be used for other markup
   languages, including XML). There are technical differences between
   XSD and YANG—and what each is able to model—but those details are
   beyond the scope of this discussion.*

What’s important about this direction is that the data model, which
defines the semantics of the variables available to be read and
written, is available in a programmatic form; it’s not just text in a
standards document. But a modeling language is no better than the
models it defines, and this has proven problematic due to conflicting
incentives. Network operators that buy network hardware have a strong
incentive to drive the models for similar devices towards convergence,
so they are not locked into products from a single vendor.  Vendors,
on the other hand, have an equally strong incentive to emphasize the
uniqueness of their products. This results in a fragmented set of
models.  YANG makes the process of creating, using, and modifying
models programmable and hence, adaptable to an iterative process. The
only question is whether the industry can successfully iterate towards
convergence.

This is where an industry-wide standardization effort, called
*OpenConfig*, comes into play. OpenConfig is a group of network
operators trying to drive the industry towards a common set of
configuration models using YANG as its modeling language. OpenConfig
is officially agnostic as to the over-the-wire protocol used to access
on-device configuration and status variables, but gNMI (gRPC Network
Management Interface) is one approach it is actively pursuing. And as
you might guess from its name, gNMI uses gRPC (a request/response
protocol which runs on top of HTTP—see Chapter |Message|). Thus, gNMI
is intended as a standard management interface for network devices.\
[#]_

.. [#] For completeness, note that NETCONF is the transport protocol
       originally developed in conjunction with YANG, and it still
       enjoys wide adoption, particularly among ISPs.  OpenConfig also
       works with NETCONF, but our current assessment is that gNMI has
       the weight of the large cloud operators behind it as the future
       management protocol, and so we elect to focus on it throughout
       the rest of this chapter.

Returning to the data model, OpenConfig defines a hierarchy of object
types. For example, the YANG model for network interfaces looks like
this:

.. literalinclude:: operations/code/iface.yang

This is a base model that can be augmented, for example, to model an
Ethernet interface:

.. literalinclude:: operations/code/eth.yang

Other similar augmentations might be defined to support link
aggregation, IP address assignment, VLAN tags, and so on.

Each model in the OpenConfig hierarchy defines a combination of a
configuration state that can be both read and written by the client
(denoted ``rw`` in the examples) and feedback state that reports
device status (denoted ``ro`` in the examples, indicating it is
read-only from the client-side). This distinction between declarative
configuration state and runtime feedback state is a fundamental aspect
of any network device interface, where OpenConfig is explicitly
focused on generalizing the latter to include network telemetry data
the operator needs to track. (More on telemetry data in the next section.)

Having a meaningful set of models is necessary, but a full
configuration system includes other elements as well. In our case,
there are three important points to make about the relationship
between the OpenConfig models and the devices that need to respond to
requests for OpenConfig-defined variables. Think of this toolset as
being part of the operating system running on every switch or router.

The first is the availability of a YANG toolchain. :numref:`Figure %s
<fig-yang>` shows the steps involved in translating a set of
YANG-based OpenConfig models into the client-side and server-side gRPC
stubs used by gNMI. The toolchain supports multiple target programming
languages, where the client and server sides of the gRPC need not be
written in the same language. With respect to the overview of network
management shown in :numref:`Figure %s <fig-mgmt-system>`, the ``gNMI
Client`` stub runs as part of the Network Management System and an
instance of the the ``gNMI Server`` stub runs on each individual
switch, specifically as part of the Switch OS system running on the
switch's control processor. (See, for example, :ref:`Figure 40
<fig-nbi>` in Section |Tech|.2.5, where the server stub in
:numref:`Figure %s <fig-yang>` implements the switch's NBI.)

.. _fig-yang:
.. figure:: operations/figures/yang-tooling.png
    :width: 550px
    :align: center

    YANG toolchain used to generate gRPC-based runtime for gNMI.

Keep in mind that YANG is not tied to either gRPC or gNMI. The
toolchain is able to start with the very same OpenConfig models but
instead produce XML or JSON representations for the data being read
from or written to network devices using, for example, NETCONF. But in
our context, the target is gNMI.

The second point is that gNMI defines a specific set of gRPC methods to
operate on these models. The set is defined collectively as a ``Service``
in the following specification:

.. literalinclude:: operations/code/service.proto

This specification uses the Protocol Buffers (usually referred to as
protobufs) specification language. We take a closer look at protobufs
in Section |Message|.6, but understanding the essence of the spec is
straightforward.

The ``Capabilities`` method is used to retrieve the set of model
definitions supported by the device. The ``Get`` and ``Set`` methods
are used to read and write the corresponding variable defined in one
of those models. The ``Subscribe`` method is used to set up a stream
of telemetry updates from the device. The corresponding arguments and
return values (e.g., ``GetRequest``, ``GetResponse``) are defined by a
protobuf ``Message`` and include various fields from the YANG
models. A given field is identified with its fully qualified path name
in the data model tree.

The third point is that a given switch does not necessarily care about
the full range of OpenConfig models. This is because a given device
might support control plane protocols like BGP, or it might support an
SDN control plane like the one described in Section |Routing|.5.  For
example, the Switch OS on a datacenter switch might track the
following OpenConfig models: Interfaces, VLANs, QoS, and LACP (link
aggregation), in addition to a set of system and platform variables
(of which the switch’s fan speed is a favorite example).

We conclude this section by briefly turning our attention to a related
interface, called gNOI (Network Operations Interface).  The underlying
mechanism used by gNOI is exactly the same as for gNMI, and in the
larger scheme of things, there is little difference between a switch’s
configuration interface and its operations interface. Generally
speaking, persistent state is handled by gNMI (and a corresponding
YANG model is defined), whereas clearing or setting ephemeral state is
handled by gNOI. (By ephemeral state, we mean settings known on the
device, but without a requirement that any other entity remember the
values so they can be recovered upon restart.) It is also the case
that non-idempotent actions like reboot and ping tend to fall under
gNOI's domain.

As an illustrative example of how gNOI is used, the following is the
protobuf specification for the ``System`` service. In this example,
``Ping``, ``Traceroute``, ``Time``, ``SetPackage``, and ``Reboot`` are
gNOI methods the operator can invoke on a device. Of particular note,
``SetPackage`` is used to instruct the device to download and install
a specified software module. This method would typically be invoked to
upgrade the device, for example, by installing the latest version of BGP.

.. literalinclude:: operations/code/system.proto


|Ops|.3.2 Configuration-as-Code
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

As soon as you scale the network—for example, to the size of a
datacenter or a global backbone—you also need to scale operations; it
is not viable to heavily depend on manual intervention. Being able to
generate the configuration interface from a set of YANG models is an
important part of that, but the configuration settings, themselves,
still have to be entered.  If an operator has to do that by typing
individual values into a web form, then you still have a
problem. Moreover, it's not just that data entry is time
consuming. Every time a change needs to be made, there is an
opportunity to make a mistake.

The solution, which has its origins in cloud operations, is to treat
parameter settings as code; the practice is known as
*Configuration-as-Code*. Typically, this means parameters are
specified in YAML, and the set of YAML
files corresponding to a network's aggregate configuration is managed
in a code repository just like any other collection of C, Java, or
GoLang programs. This is not as big of stretch as it might sound: you
can think of YAML as a declarative programming language. These YAML
files then serve as the authoritative source of all parameter settings.

The following snippet of YAML code shows how one might configure an
Ethernet interface. This file corresponds to the YANG shown in the
previous section.

.. literalinclude:: operations/code/ethernet.yaml

The advantage of managing configuration state as code is that it can
be versioned just like other software modules, with a corresponding
set of version control and release management tools. There could be a
stable version that represents the currently deployed
parameters. Edits can be made, reviewed, and thoroughly tested, and
when there is confidence in its correctness, the changes rolled out to
the operational system. Most importantly, if there is a problem, it's
possible to roll back to an earlier, known-working version of the
configuration state. Testing that a configuration is correct is
clearly an important step in this process, and there are a variety of
tools available to help. Batfish and Minesweeper (described in a
2017 SIGCOMM paper) are popular examples.

.. TODO -- This is a good opportunity to cite some of the most
   notorious configuration failures. Maybe in a sidebar.

.. admonition:: Further Reading

   `Batfish: An open source network configuration analysis tool
   <https://batfish.org/>`__.

   R. Beckett, A. Gupta, R. Mahajan and D. Walker. `A General
   Approach to Network Configuration Verification
   <https://dl.acm.org/doi/abs/10.1145/3098822.3098834>`__.  ACM
   SIGCOMM '17 Symposium, August 2017.

Another aspect of treating configuration variables as code is that it
naturally plugs into a management pipeline, similar to the one
depicted in :numref:`Figure %s <fig-config-pipeline>`. The pipeline
takes input from three sources: an inventory repo, a config repo, and
an image repo. We briefly mentioned inventory in the previous section,
but you can think of it being implemented by a collection of YAML
files representing all the deployed devices. The config repo is
similar to what we've just described, with the exception that instead
of hard-coding some of the parameters, the YAML includes templates that
get filled in with device-specific information. Operators are
responsible for updating these first two repos. Finally, the image
repo holds the latest executable images for the software stack running
on each device (e.g., the latest release of OSPF or BGP). For now, we
assume an upstream provider, for example a vendor, populates the image
repo.  We'll look at how the pipeline extends to the left to account
for networks that also build their own software in Section |Ops|.5.

.. _fig-config-pipeline:
.. figure:: operations/figures/config-pipeline.png
   :width: 550px
   :align: center

   Simple configuration pipeline, with operator-supplied configuration
   and inventory specifications stored in their respective repositories,
   and executable images supplied by an upstream vendor.


|Ops|.3.3 Other Tools
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This chapter describes how to design a management system as a top-down
exercise, and while it is true that cloud operators have taken a
clean-slate approach to operations, many practices in use today look
the way they do because of how the operator did things yesterday.
(This is true of many systems: they evolve incrementally, as a result
of changing circumstances.) With respect to network operations, the
fact that many devices were originally managed via a device-specific
CLI means that a set of tools have evolved to take advantage of that
capability. Given this starting point, the challenge is how to
improve automation using a mechanism that was originally designed for
manual control.

One popular tool that helps address this problem is *Ansible*. It runs
over SSH, which unlike gRPC, is ubiquitously available on all devices.
The operator provides a set of *playbooks*, which are similar to
scripts, but are written as declarative statements in YAML.  The
operator also defines a set of *values* files (also YAML) to give the
parameter settings to be used by the playbooks. Each playbook
identifies a sequence of objectives that need to be satisfied, and
Ansible executes them in an attempt to bring the actual state of the
device into alignment with the specified state for the device.

Ansible is one piece of the puzzle, but not the whole solution. The
full set of issues introduced in Section |Ops|.1 still have to be
resolved. For example, we need to identify the authoritative source of
all variable settings. Ansible value files typically provide software
configuration parameters, but many operators already use an inventory
database for hardware configuration state; *NetBox* is a popular open
source example.  There is often overlap between the two, so operators
have to be consistent about which source is authoritative for each
variable.

As another example, we still need to establish what variables are
available to control, and since we're interacting with a
vendor-specific CLI, the vendor may define their own set of
variables. This raises the complication of dealing with multiple
vendors, no different than cloud operators faced when they pressed for
a vendor-neutral schema.  For this reason, OpenConfig remains an
option for those operators that care about vendor-neutrality, but we
need a way to map these vendor-neutral variables onto their
device-specific counterparts. *Napalm*, which stands for "Network
Automation and Programmability Abstraction Layer with Multivendor
support" is an open source tool that helps address this issue. It's a
library that can be installed on the device to provide a
vendor-neutral NBI. Ansible then interacts with this library rather
the native CLI. At this point, we've established rough equivalency
with the system described earlier in this section, although it ends up
being more bespoke than off-the-shelf. This makes it more cumbersome
to maintain and evolve.

.. TODO -- This last paragraph is a candidate Takeaway

A lesson illustrated by this example is that there is no single right
set of tools for any problem space. Instead, you typically start with
an initial set—likely influenced by previous experience with a similar
problem—evolve the tools to address the next most troublesome issue
you encounter, add new tools to the mix when you discover a gap in
your solution, and then iterate as new requirements come into focus.
Needing to scale the network and making the network more agile are
two requirements that often force operators to rethink their practices.

.. admonition:: Further Reading

   `Ansible <https://docs.ansible.com/>`__. A Radically Simple IT
   Automation System.

   `Netbox <https://netboxlabs.com/>`__. Network and Infrastructure
   Management Platform.

   `Napalm <https://napalm.readthedocs.io>`__. Network Automation and
   Programmability Abstraction Layer.
