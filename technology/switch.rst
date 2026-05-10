|Tech|.2 Packet Switches
---------------------------------

Packet switches are devices that interconnect point-to-point links,
making it possible to assemble networks that connect hundreds or
thousands of hosts (rather than just two).  There is a straightforward
way to build a switch: buy a general-purpose processor and equip it
with multiple network interface cards. Such a device, running suitable
software, can receive packets on one of its interfaces, decide the
best way to forward each packet on towards its destination, and then
transmit packets on another of its interfaces.  This so called
*software switch* is not too far removed from the architecture of many
commercial mid- to low-end network devices.  Implementations that
deliver high-end performance typically take advantage of additional
hardware acceleration. We refer to these as *hardware switches*,
although both approaches obviously include a combination of hardware
and software.

This section gives an overview of both software-centric and
hardware-centric designs. It also describes fundamental concepts that
underlie both designs. Keep in mind that our goal is to understand
switches in enough depth so we can use them as the primary building
block for the best-effort message delivery system covered in Part II.

|Tech|.2.1 Control and Data Planes
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Switch forwarding logic is straightforward. Every switch maintains a
table that maps addresses to output ports. For every packet received
on one of its input ports, the switch extracts the destination address
from the packet header, looks it up in the table, and then enqueues
the packet for transmission on the specified output port. Looking back
at the IP and ETH headers in Section |Intro|.1.3, for example, this would
mean a switch configured to forward IP packets would use the 32-bit
``DestinationAddr`` found at an offset of 20 bytes from the beginning
of the IP header as its lookup key, while a switch configured to
forward ETH packets would use the 48-bit ``DestinationAddr`` found at
an offset of 6 bytes from the beginning of the ETH header as its
lookup key.

One important takeaway from this example is that many switches are highly
configurable—it is often possible to configure the same switch to forward IP
packets or Ethernet packets. This helps explain the distinction
between switches and routers: they are often just different configurations
of the same forwarding device. To put it in pragmatic terms, network
administrators typically buy a single forwarding box from a vendor and
then configure it to be an Ethernet switch (sometimes called an *L2
switch*, for Layer 2 in the OSI architecture), an IP router (sometimes
called an *L3 switch*, for Layer 3 in the OSI architecture), or some
combination of the two. For this reason, these packet forwarding
devices are sometimes called *L2/L3 switches*.

There are many other possible configuration options, but to understand
them, we need a more sophisticated model of a switch. For starters,
switches make a distinction between a their *control plane* and their
*data plane*.  The control plane determines how the network should
behave (i.e., it puts address-to-port mappings in the lookup table),
while the data plane is responsible for applying those mappings to
individual packets (i.e., it forwards packets based on what is finds
in the lookup table).

.. _fig-fib:
.. figure:: technology/figures/fib.png
    :width: 250px
    :align: center

    Packet switches are divided into two planes: the *Control Plane*
    runs a routing algorithm and maintains routing information in a
    RIB, and the *Data Plane* implements packet forwarding logic
    and maintains forwarding information in a FIB.

As shown in :numref:`Figure %s <fig-fib>`, the control plane runs a
*routing algorithm* that collects the information about the network's
topology so it can select the best route to each destination at a
given point in time. This includes available paths, their respective
costs, and any policy constraints. Chapter |Routing| looks at several widely
used routing algorithms, and while the exact set of information it
collects is algorithm-specific, it is commonly referred to as the
*Routing Information Base (RIB)*.

The task of actually forwarding packets along the routes selected by
the routing algorithm is the job of the data plane. This is where the
switch makes forwarding decisions on a packet-by-packet basis. The
data plane's lookup table is sometimes called the *Forwarding
Information Base (FIB)*, and it uses a data structure that has been
optimized to support fast lookup operations since it may have only
nanoseconds to make a decision. To further complicate matters, this
data structure needs to support multiple possible lookup keys; we have
already seen 32-bit IP address and 48-bit Ethernet address, but there
are others.  For now you can think of the FIB as being implemented by
multiple lookup tables—one per type of key—but we postpone an in-depth
description until subsection |Tech|.2.4.

:numref:`Figure %s <fig-fib>` also shows an interface between the
control and data planes, with the former periodically loading new
address-to-port mappings into the FIB.  There is no controversy about
the value of decoupling the two planes. It is a well-established
practice among switch vendors, although this interface has
historically been closed. Over the last several years, however, a
*Software-Defined Networking (SDN)* initiative has worked to define an
open interface for installing routes in the data plane, with the goal
of giving network owners (as opposed to switch vendors) more
control. The exact nature of this interface is related to the FIB data
structure, so we cover it in subsection |Tech|.2.4, after seeing how switches
are implemented in both software and hardware.

|Tech|.2.2 Software Switch
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

:numref:`Figure %s <fig-softswitch>` shows a software switch built
using a general-purpose processor with four network interface cards
(NICs). The path for a typical packet that arrives on, say, NIC 1 and
is forwarded out on NIC 2 is straightforward: as NIC 1 receives the
packet it copies its bytes directly into the main memory over the I/O
bus (PCIe in this example) using a technique called *direct memory
access* (DMA). Once the packet is in memory, the CPU examines its
header to determine which interface the packet should be sent out on,
and instructs NIC 2 to transmit the packet, again directly out of main
memory using DMA. (Recall that because there may be contention, the
packet may be queued in the buffer waiting to be sent.) The important
take-away is that the packet is buffered in main memory (this is the
“store” half of store-and-forward), with the CPU reading only the
necessary header fields into its internal registers for processing.

.. _fig-softswitch:
.. figure:: technology/figures/softswitch.png
   :width: 350px
   :align: center

   A general-purpose processor used as a software
   switch.

There are two potential bottlenecks with this approach, one or both of
which limits the aggregate packet forwarding capacity of the software
switch. The first problem is that performance is limited by the fact
that all packets must pass into and out of main memory. Your mileage
will vary based on how much you are willing to pay for hardware, but
as an example, a machine limited by a 1333-MHz, 64-bit-wide memory bus
can transmit data at a peak rate of a little over 100 Gbps—enough to
build a switch with a handful of 10-Gbps Ethernet ports, but hardly
enough for a high-end switch in the core of the Internet.

Moreover, this upper bound assumes that moving data is the only problem.
This is a fair approximation for long packets but a bad one when packets
are short, which is the worst-case situation switch designers have to
plan for. With minimum-sized packets, the cost of processing each
packet—parsing its header and deciding which output link to transmit it
on—is likely to dominate, and potentially become a bottleneck. Suppose,
for example, that a processor can perform all the necessary processing
to switch 40 million packets each second. This is sometimes called the
packet per second (pps) rate. If the average packet is 64 bytes, this
would imply

.. centered:: Throughput = pps x BitsPerPacket

.. centered:: = 40 × 10\ :sup:`6` × 64 × 8

.. centered:: = 2048 × 10\ :sup:`7`

That is, a throughput of about 20 Gbps—fast, but substantially below
the range users are demanding from their switches today. Bear in mind
that this 20 Gbps would be shared by all users connected to the
switch. Thus, for example, a 16-port switch with this aggregate
throughput would only be able to cope with an average data rate of
about 1 Gbps on each port.\ [#]_

.. [#] These example performance numbers do not represent the absolute
       maximum throughput rate that highly tuned software running on a
       high-end server could achieve, but they are indicative of
       limits one ultimately faces in pursuing this approach.

Finally, note that the control plane (routing algorithm) and the data
plane (forwarding logic) are implemented as separate processes on the
same CPU. The routing algorithm is *not* directly part of the
per-packet forwarding decision, but there must be enough CPU capacity
for it to run periodically in the background without slowing down the
data plane. As for the data plane, the most costly routine the CPU is
likely to execute on a per-packet basis is a table lookup, for
example, looking up an IP address in an L3 forwarding table, or an
Ethernet address in an L2 forwarding table.

|Tech|.2.3 Hardware Switch
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Throughout much of the Internet’s history, high-performance switches
have been specialized devices, built with Application-Specific
Integrated Circuits (ASICs). Today, the availability of ASIC-based
switching chips, along with other commodity components and an open
architectural specification, makes it possible for anyone to build a
high-performance switch by pulling the blueprint off the web (see the
Open Compute Project, OCP, for examples) in the same way it is
possible to build your own PC. In both cases you still need software
to run on the hardware, but just as Linux is available to run on your
home-built PC, there are now open source L2 and L3 stacks available to
run on your home-built switch. Alternatively, you can simply buy a
pre-built switch from a commodity switch manufacturer and then load
your own software onto it. The following describes these open
*bare-metal switches*, so called to contrast them with closed devices,
in which hardware and software are tightly bundled, that have
historically dominated the industry.

.. _fig-baremetal:
.. figure:: technology/figures/baremetal.png
   :width: 500px
   :align: center

   Bare-metal switch using a Network Processing Unit.

:numref:`Figure %s <fig-baremetal>` is a simplified depiction of a
bare-metal switch. The key difference from the earlier implementation
on a general-purpose processor is the addition of a Network Processor
Unit (NPU), a domain-specific processor with an architecture and
instruction set that has been optimized for processing packet headers
(i.e., for implementing the data plane). NPUs are similar in spirit to
GPUs that have an architecture optimized for rendering computer
graphics, but in this case, the NPU is optimized for parsing packet
headers and making a forwarding decision. NPUs are able to process
packets (input, make a forwarding decision, and output) at rates
measured in Terabits per second (Tbps), easily fast enough to keep up
with 32x100-Gbps ports, or the 48x40-Gbps ports shown in the diagram.

.. sidebar:: Network Processing Units

          Our use of the term NPU is a bit non-standard. Historically,
          NPU was the name given more narrowly-defined network
          processing chips used, for example, to implement intelligent
          firewalls or deep packet inspection. They were not as
          general-purpose as the NPUs we’re discussing here; nor were
          they as high-performance. It seems likely that the current
          approach will make purpose-built network processors
          obsolete, but in any case, we prefer the NPU nomenclature
          because it is consistent with the trend to build
          programmable domain-specific processors, including GPUs for
          graphics and TPUs (Tensor Processing Units) for AI.

The beauty of this design is that a given bare-metal switch can now be
programmed to be an L2 switch, an L3 router, or a combination of both,
just by a matter of programming. The exact same control plane software
used in a software switch still runs on the control CPU, but in
addition, data plane “programs” are loaded onto the NPU to reflect the
forwarding decisions made by the control plane software.  Exactly how
one “programs” the NPU depends on the chip vendor. In some cases, the
forwarding pipeline is fixed and the control processor merely loads
the forwarding table into the NPU (by fixed we mean the NPU only knows
how to process certain headers, like Ethernet and IP), but in other
cases, the forwarding pipeline is itself programmable.

Internally, an NPU takes advantage of three technologies. First, a
fast SRAM-based memory buffers packets while they are being
processed. SRAM (Static Random Access Memory), is roughly an order of
magnitude faster than the DRAM (Dynamic Random Access Memory) that is
used by main memory. Second, a TCAM-based memory stores bit patterns
to be matched in the packets being processed. The “CAM” in TCAM stands
for “Content Addressable Memory,” which means that the key you want to
look up in a table can effectively be used as the address into the
memory that implements the table. The “T” stands for “Ternary” which
is a fancy way to say the key you want to look up can have wildcards
in it (e.g, key ``10*1`` matches both ``1001`` and ``1011``). Finally,
the processing involved to forward each packet is implemented by a
forwarding pipeline.  This pipeline is implemented by an ASIC, but
when well-designed, the pipeline’s forwarding behavior can be modified
by changing the program it runs. More on exactly what it means to
"program the forwarding pipeline" is given in the next subsection.

The relevance of packet processing being implemented by a multi-stage
pipeline rather than a single-stage processor is that forwarding a
single packet likely involves looking at multiple header fields; each
stage looks at a different combination of fields. A multi-stage
pipeline adds a little end-to-end latency to each packet (measured in
nanoseconds), but also means that multiple packets can be processed at
the same time. For example, Stage 2 can be making a second lookup on
packet A while Stage 1 is doing an initial lookup on packet B, and so
on. This means the NPU as a whole is able to keep up with line
speeds. As of this writing, the state of the art is 102.4 Tbps.

Finally, :numref:`Figure %s <fig-baremetal>` includes other commodity
components that make this all practical. In particular, it is now
possible to buy pluggable *transceiver* modules that take care of all
the media access details described in Section |Tech|.1, be it Gigabit
Ethernet, 10-Gigabit Ethernet, or some non-Ethernet technology such as
SONET (see Section |Tech|.3). These transceivers all conform to
standardized form factors, such as SFP+ (small form-factor pluggable),
that can in turn be connected to other components over a standardized
bus (e.g., SFI, the Serial Framing Interface). Again, the key takeaway
is that the networking industry is just now entering into the same
commoditized world that the computing industry has enjoyed for
decades.

|Tech|.2.4 Flow Rules
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Having seen a high-level schematic of a hardware switch, and knowing
that we need an interface by which the control plane installs
forwarding rules in the data plane, we are now ready to take a closer
look at this interface. At the heart of the interface is a *forwarding
abstraction:* a general-purpose way for the control plane to instruct
the data plane to forward packets in a particular way. But like any
good abstraction, it should **not** restrict how a given switch vendor
implements the data plane (e.g., the exact form of its forwarding
table or the process by which it forwards packets). This forwarding
abstraction should not assume (or favor) one data plane implementation
over another.

There have been two generations of this interface. The original
interface, called *OpenFlow*, was published in 2008. It introduced
*Flow Rules* as a simple but powerful way to specify forwarding
behavior. A flow rule is a *(Match, Action)* pair: any packet that
*Matches* the first part of the rule should have the associated
*Action* applied to it. A simple flow rule, for example, might specify
that any packet with destination address *D* be forwarded on output
port *i*. The original OpenFlow spec allowed the header fields shown
in :numref:`Figure %s <fig-headers>` to be included in the Match half
of the rule. So for example, a Match might specify a packet's ETH
header ``Type`` field equals ``0x800`` (indicating the frame carries
and IP packet) and its IP header ``DstAddr`` field be contained in
some subnet (e.g., ``192.12.69/24``). It is also possible to write
flow rules that match specific UDP or TCP port numbers.

.. _fig-headers:
.. figure:: technology/figures/headers.png
    :width: 600px
    :align: center

    Example header Fields Matched in Original OpenFlow Specification.

The Actions originally included *“forward packet to one or more
ports”* and *“drop packet,”* plus a *“send packet up to the control
plane”* escape hatch for any packet that requires further processing
by a *controller* (a term introduced to signify the process running in
the control plane responsible for controlling the switch). The Actions
also specify how various header fields should be modified. For
example, a packet received on a given input port would have this
switch's ETH address as the packet's destination, but a different
destination address would be inserted into the ETH header for the
selected output port, identifying the *next* hop the packet is to
visit along its route to the ultimate destination. The ETH source
address would also need to be changed accordingly.

There are two other noteworthy aspects of flow rules. First, as we saw
in Section |Tech|.2.3, hardware switches employ a forwarding pipeline that
makes it possible to do multiple lookups in parallel. :numref:`Figure
%s <fig-pipeline>` takes a closer look at this pipeline, this time
with the knowledge that it is being used to execute flow rules. The
idea is that the forwarding pipeline implements a series of flow
tables, each focused on a subset of the header fields that might be
involved in a given flow rule (e.g., one table matches the ETH header,
one matches the IP header, and so on). A given packet is processed by
multiple flow tables in sequence—i.e., a pipeline—to determine how it
is ultimately forwarded. A set of actions are accumulated as the
packet flows through the pipeline, and executed as a set in the last
stage, resulting in the packet being modified and enqueued for
transmission.

.. _fig-pipeline:
.. figure:: technology/figures/pipeline.png
    :width: 550px
    :align: center

    Simple Schematic of an OpenFlow Forwarding Pipeline.

Second, the pipeline shown in :numref:`Figure %s <fig-pipeline>` is
usually static, in the sense that each stage is hard coded to know
about exactly one subset of header fields. This means the pipeline as
a whole is limited to matching a fixed set of fields in the packet
headers (e.g., the fields shown in :numref:`Figure %s <fig-headers>`)
and perform a fixed set of actions; they are sometimes called
*fixed-function pipelines*. Most switching chips are designed this
way, although the set of flow tables has grown much larger than the
example shown in :numref:`Figure %s <fig-headers>` suggests.  (We'll
see many additional header fields that switches want to inspect in the
chapters of Part II.)

An alternative is to build a *programmable pipeline*, and pair it with
a programming language that can be used to dynamically program what
each stage in the pipeline does. This has happened over the last
several years, resulting in a *Protocol Independent Switching
Architecture (PISA)* and the *P4* programming language. The details of
both are beyond the scope of this introduction to switch design, but
we describe them in auxiliary material, and encourage readers to take
advantage of open source software to write P4 programs and run them on
emulated PISA hardware. Doing so will give you the power to implement
many of the ideas described in this book, and do so with performance
that competes with the largest vendors in the networking industry.

.. _reading_openflow:
.. admonition:: Further Reading

   N. McKeown, et. al. `OpenFlow: Enabling Innovation in Campus Networks
   <http://ccr.sigcomm.org/online/files/p69-v38n2n-mckeown.pdf>`__.
   SIGCOMM CCR, March 2008.

   L. Peterson, C. Cascone, B. O'Connor, T. Vachuska, and B. Davie.
   `Software-Defined Networks: A Systems Approach
   <https://sdn.systemsapproach.org>`__.  November 2021.

   `P4 Tutorials
   <https://github.com/p4lang/tutorials>`__. P4 Consortium, May 2019.
