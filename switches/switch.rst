3.2 Switch Design
---------------------------------

Packet switches are the device that make it possible to assemble
networks from individual communication links. There is a
straightforward way to build a switch: Buy a general-purpose processor
and equip it with multiple network interface cards. Such a device,
running suitable software, can receive packets on one of its
interfaces, decide the best way to forward each packet on towards its
destination, and then send packets out another of its interfaces.
This so called *software switch* is not too far removed from the
architecture of many commercial mid- to low-end network devices.
Implementations that deliver high-end performance typically take
advantage of additional hardware acceleration. We refer to these as
*hardware switches*, although both approaches obviously include a
combination of hardware and software.

This section gives an overview of both software-centric and
hardware-centric designs. Keep in mind that our goal is to understand
switches in enough depth so we can use them as the primary building
block for the best-effort message delivery system covered in Part II.

3.2.1 Control and Data Planes
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

At a high level, switch forwarding logic is straightforward. Every
switch maintains a table that maps addresses to output ports. For
every packet received on one of its input ports, the switch extracts
the destination address from the packet header, looks it up in the
table, and then enqueues the packet for transmission on the specified
output port. Looking back at the IP and ETH headers in Section 1.1.3
for example, this would mean a switch configured to forward IP packets
would use the 32-bit ``DestinationAddr`` found at an offset of 20
bytes from the beginning of the IP header as its lookup key, while a
switch configured to forward ETH packets would use the 48-bit
``DestinationAddr`` found at an offset of 6 bytes from the beginning
of the ETH header as its lookup key.

There is much more going on than this simple overview reveals, leading
switch designers to make a distinction between a switch's *control
plane* and its *data plane*.  The control plane determines how the
network should behave (i.e., it puts address-to-port mappings in the
lookup table), while the data plane is responsible for applying those
mappings to individual packets (i.e., it forwards packets based on
what is finds in the lookup table).

More specifically, the control plane runs a *routing algorithm* that
collects any information it might need to select the best route at a
given point in time, including alternative paths, their respective
costs, and any policy constraints. Chapter 4 looks at several widely
used routing algorithms, and while the exact collection of information
it collects is algorithm-specific, it is commonly referred to as the
*Routing Information Base (RIB)*.

The task of actually forwarding packets along those routes is the job
of the data plane. This is where the switch makes forwarding decisions
on a packet-by-packet basis. Data plane's lookup table is commonly
called the *Forwarding Information Base (FIB)*, and it is implemented
using an optimized data structure that supports fast lookup operations.

.. _fig-fib:
.. figure:: switches/figures/fib.png
    :width: 300px
    :align: center

    Control plane (and corresponding RIB) decoupled from the data
    plane (and the corresponding FIB).

As shown in :numref:`Figure %s <fig-fib>`, there is an interface
between the control and data planes, with the former periodically
loading new address-to-port mappings into the FIB.  There is no
controversy about the value of decoupling the network. It is a
well-established practice among switch vendors, although this
interface has historically been closed. Over the last several years, a
*Software-Defined Networking (SDN)* initiative has worked to define an
open interface for installing routes in the data plane, with the goal
of giving network owners (as opposed to switch vendors) more
control. We return to this topic at the end of this section.

One major takeaway from this discussion is that switches are highly
configurable. As our openning example implied, it is possible to
configure a switch to forward IP packets or Ethernet packets. This
helps explain the distinction between switches and routers: they are
just different configurations of the same forwarding device. To put it
in pragmatic terms, network administrators typically buy a single
forwarding box from a vendor and then configure it to be an Ethernet
switch (sometimes called an *L2 switch*, for Layer 2 in the OSI
architecture), an IP router (sometimes called an *L3 switch*, for
Layer 3 in the OSI architecture), or some combination of the two. For
this reason, these packet forwarding devices are sometimes called
*L2/L3 switches*. There are many other possible configuration options,
which we'll highlight throughout the book.

3.2.2 Software Switch
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
memory using DMA. The important take-away is that the packet is
buffered in main memory (this is the “store” half of
store-and-forward), with the CPU reading only the necessary header
fields into its internal registers for processing.

.. _fig-softswitch:
.. figure:: switches/figures/softswitch.png
   :width: 350px
   :align: center

   A general-purpose processor used as a software
   switch.

There are two potential bottlenecks with this approach, one or both of
which limits the aggregate packet forwarding capacity of the software
switch.

The first problem is that performance is limited by the fact that all
packets must pass into and out of main memory. Your mileage will vary
based on how much you are willing to pay for hardware, but as an
example, a machine limited by a 1333-MHz, 64-bit-wide memory bus can
transmit data at a peak rate of a little over 100 Gbps—enough to build a
switch with a handful of 10-Gbps Ethernet ports, but hardly enough for a
high-end switch in the core of the Internet.

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

that is, a throughput of about 20 Gbps—fast, but substantially below
the range users are demanding from their switches today. Bear in mind
that this 20 Gbps would be shared by all users connected to the
switch. Thus, for example, a 16-port switch with this aggregate
throughput would only be able to cope with an average data rate of
about 1 Gbps on each port.\ [#]_

.. [#] These example performance numbers do not represent the absolute
       maximum throughput rate that highly tuned software running on a
       high-end server could achieve, but they are indicative of
       limits one ultimately faces in pursuing this approach.

One final consideration is important to understand when evaluating
switch implementations. The routing algorithms mentioned in Section
3.2.1 are *not* directly part of the per-packet forwarding decision.
They run periodically in the background, rather than for every packet
it forwards. The most costly routine the CPU is likely to execute on a
per-packet basis is a table lookup, for example, looking up an IP
address in an L3 forwarding table, or an Ethernet address in an L2
forwarding table.

3.2.3 Hardware Switch
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Throughout much of the Internet’s history, high-performance switches
have been specialized devices, built with Application-Specific
Integrated Circuits (ASICs). While it was possible to build low-end
switches using commodity servers running C programs, ASICs were
required to achieve the required throughput rates.

The problem with ASICs is that hardware takes a long time to design and
fabricate, meaning the delay for adding new features to a switch is
usually measured in years, not the days or weeks today’s software
industry is accustomed to. Ideally, we’d like to benefit from the
performance of ASICs and the agility of software.

Fortunately, recent advances in domain specific processors (and other
commodity components) have made this possible. Just as importantly, the
full architectural specification for switches that take advantage of
these new processors is now available online—the hardware equivalent of
*open source software*. This means anyone can build a high-performance
switch by pulling the blueprint off the web (see the Open Compute
Project, OCP, for examples) in the same way it is possible to build your
own PC. In both cases you still need software to run on the hardware,
but just as Linux is available to run on your home-built PC, there are
now open source L2 and L3 stacks available on GitHub to run on your
home-built switch. Alternatively, you can simply buy a pre-built switch
from a commodity switch manufacturer and then load your own software
onto it. The following describes these open *bare-metal switches*, so
called to contrast them with closed devices, in which hardware and
software are tightly bundled, that have
historically dominated the industry.

.. _fig-baremetal:
.. figure:: switches/figures/baremetal.png
   :width: 500px
   :align: center

   Bare-metal switch using a Network Processing
   Unit.

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

          Our use of the term NPU is a bit
          non-standard. Historically, NPU was the name given more
          narrowly-defined network processing chips used, for
          example, to implement intelligent firewalls or deep
          packet inspection. They were not as general-purpose as
          the NPUs we’re discussing here; nor were they as
          high-performance. It seems likely that the current
          approach will make purpose-built network processors
          obsolete, but in any case, we prefer the NPU nomenclature
          because it is consistent with the trend to build
          programmable domain-specific processors, including GPUs
          for graphics and TPUs (Tensor Processing Units) for AI.

The beauty of this new switch design is that a given bare-metal switch
can now be programmed to be an L2 switch, an L3 router, or a
combination of both, just by a matter of programming. The exact same
control plane software stack used in a software switch still runs on
the control CPU, but in addition, data plane “programs” are loaded
onto the NPU to reflect the forwarding decisions made by the control
plane software.  Exactly how one “programs” the NPU depends on the
chip vendor, of which there are currently several. In some cases, the
forwarding pipeline is fixed and the control processor merely loads
the forwarding table into the NPU (by fixed we mean the NPU only knows
how to process certain headers, like Ethernet and IP), but in other
cases, the forwarding pipeline is itself programmable. P4 is a new
programming language that can be used to program such NPU-based
forwarding pipelines. Among other things, P4 tries to hide many of the
differences in the underlying NPU instruction sets.

Internally, an NPU takes advantage of three technologies. First, a fast
SRAM-based memory buffers packets while they are being processed. SRAM
(Static Random Access Memory), is roughly an order of magnitude faster
than the DRAM (Dynamic Random Access Memory) that is used by main
memory. Second, a TCAM-based memory stores bit patterns to be matched in
the packets being processed. The “CAM” in TCAM stands for “Content
Addressable Memory,” which means that the key you want to look up in a
table can effectively be used as the address into the memory that
implements the table. The “T” stands for “Ternary” which is a fancy way
to say the key you want to look up can have wildcards in it (e.g, key
``10*1`` matches both ``1001`` and ``1011``). Finally, the processing
involved to forward each packet is implemented by a forwarding pipeline.
This pipeline is implemented by an ASIC, but when well-designed, the
pipeline’s forwarding behavior can be modified by changing the program
it runs. At a high level, this program is expressed as a collection of
*(Match, Action)* pairs: if you match such-and-such field in the header,
then execute this-or-that action.

The relevance of packet processing being implemented by a multi-stage
pipeline rather than a single-stage processor is that forwarding a
single packet likely involves looking at multiple header fields. Each
stage can be programmed to look at a different combination of fields. A
multi-stage pipeline adds a little end-to-end latency to each packet
(measured in nanoseconds), but also means that multiple packets can be
processed at the same time. For example, Stage 2 can be making a second
lookup on packet A while Stage 1 is doing an initial lookup on packet B,
and so on. This means the NPU as a whole is able to keep up with line
speeds. As of this writing, the state of the art is 25.6 Tbps.

Finally, :numref:`Figure %s <fig-baremetal>` includes other commodity
components that make this all practical. In particular, it is now
possible to buy pluggable *transceiver* modules that take care of all
the media access details—be it Gigabit Ethernet, 10-Gigabit Ethernet,
or SONET—as well as the optics. These transceivers all conform to
standardized form factors, such as SFP+, that can in turn be connected
to other components over a standardized bus (e.g., SFI). Again, the
key takeaway is that the networking industry is just now entering into
the same commoditized world that the computing industry has enjoyed
for the last two decades.

3.2.4 Flow Abstraction
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

*[This section will introduce the flow abstraction, and briefly talk
abouet OpenFlow and P4. It likely mentions SDN-style centralized
controllers, but that topic is better covered in Chapter 4.]*

