.. index:: RDMA: Remote Direct Memory Access
.. index:: MPI: Message Passing Interface
.. index:: NCCL: NVIDIA Collective Communications Library
.. index:: GPI: Global Address Space Programming Interface
.. index:: OFI: Open Fabrics Interface

|Message|.4 Remote DMA
----------------------------

.. A helpful RDMA Tutorial Slide Deck
   https://www.doc.ic.ac.uk/~jgiceva/teaching/ssc18-rdma.pdf

RDMA is a remote variant of DMA (direct memory access), an
architectural feature commonly used for device/host communication over
a computer's I/O bus. A network adaptor (i.e., NIC) is an example
device that uses DMA to pass frames between the NIC and the host. For
example, a host passes an address for an empty memory buffer to the
NIC, and the NIC is able to directly write incoming frames (arriving
from the network) into that buffer without the host CPU being
involved. In the other direction, the host passes an address for a
full memory buffer to the NIC, and the NIC is able to transmit a frame
onto the network by directly reading from that buffer, again without
the host CPU being involved.

As a communication abstraction, RDMA extends that model to
network-connected machines, as shown in :numref:`Figure %s
<fig-rdma>`.  As a consequence, it becomes possible for two
application processes to access (read and write) blocks of data from
each others' memory. Functionally, RDMA can be viewed as a special
case of an RPC, where only two "remote procedures" are supported:
*Read( )* and *Write( )*. But that characterization doesn't do justice to
how RDMA fundamentally changes the way applications interact with the
network. Section |Apps|.1.4 introduced this alternative programming
model; this section takes a closer look at the details.

.. _fig-rdma:
.. figure:: message/figures/rdma.png
   :width: 450px
   :align: center

   RDMA supports read/write access to memory buffers on remote
   machines.

The history of RDMA traces back to early parallel computers used in
*High Performance Computing (HPC)*, which typically ran parallel
applications, such as weather forecasting, financial modeling, aerodynamic
design, physics simulations, and so on. Those machines had
internal interconnects that were eventually replaced by external
networks, but because of their focus on supporting low-latency
communication among parallel tasks, they largely avoided the more
ubiquitous Internet technology in favor of purpose-built networks.  Of
these, InfiniBand became the industry
standard, and RDMA became the end-to-end communication abstraction
running on top of InfiniBand.\ [#]_ Today, RDMA is a key enabling
technology for many AI applications, which has pushed it further into the mainstream.

.. [#] There is a second thread to this history. InfiniBand was
   originally designed as a system-area bus, intended to replace
   internal I/O buses like PCI, and supporting network-attached
   storage. This explains why RDMA would be a natural fit as its
   communication primitive. Over time, however, InfiniBand gained
   traction as a "supercomputer interconnect", which is our focus in
   this section.

The claim that TCP/IP running over an Ethernet-based packet-switched network is a
poor match for HPC workloads was a hotly contested topic 20+ years ago.
The case essentially boiled down to two arguments. One is that TCP/IP
is implemented in the OS kernel, which means the OS has to get
involved in sending and receiving every packet. With RDMA, the NIC
directly writes packets into and reads packets out of application
memory buffers. This avoids hundreds of CPU instructions and the
throughput penalty of copying packets from one buffer to
another. However, this is really an implementation choice rather than
a fundamental feature of TCP/IP; it is certainly possible to offload
TCP to the NIC and move packets using DMA. The
second argument is that Ethernet-based networks are best effort,
meaning that the potential for congestion can lead to both queuing
delays and packet loss. We take up the case for whether this remains a
valid concern in the next section, but it does explain why InfiniBand
exists and remains popular.

|Message|.4.1 Components
~~~~~~~~~~~~~~~~~~~~~~~~~~

InfiniBand defines an entire network architecture, one that is roughly
parallel to the Internet architecture. It includes physical and link
layer technologies, a network layer based on packet switching, and a
transport layer that implements the RDMA abstractions (plus other
optional behaviors). We're not going to describe that architecture in
full detail, but it is instructive to understand the touchpoints it
shares with the Internet architecture. For the purpose of this
section, the main takeaway is that the transport layer—think of it as
InfiniBand's equivalent of TCP—runs in a network adaptor, or NIC,
rather than being implemented in software on the end host. InfiniBand
calls this NIC a *Host Channel Adaptor (HCA)*, but we'll use NIC and
HCA interchangeably.

Implementing the message transaction transport mechanism in hardware
is one of the professed advantages of the InfiniBand architecture, as
it avoids the overheads of executing TCP/IP in the OS kernel. The
other advantage is that by also using its own switches, InfiniBand is
able to avoid the congestion-induced delays inherent in a best-effort
technology like Ethernet.  In the next section we revisit these
assumptions, but for now, we focus on the key design decisions
underpinning RDMA.

First, the RDMA programming interface, known as the *Verbs API*, has
become a *de facto* standard for HPC programs, and most recently, for
AI workloads that run in cloud datacenters. The Verbs API is
low-level, and so is typically not directly used by application
programs. Such applications are generally written to a higher level
interface, of which there are several options. They include *Message
Passing Interface (MPI)* , *Global Address Space Programming Interface
(GPI)*, and *Open Fabrics Interface (OFI)*, all of which pre-date the
the last decade's pivot towards AI workloads. More recently, *NCCL*
(pronounced "nickel") has become the dominant API for AI software
running on GPUs. NCCL was created by NVIDIA (it is an acronym for
"NVIDIA Collective Communications Library"), but its implementation as
a software package running on top of the Verbs API is available as open
source.

Second, Ethernet continues to evolve, and in this particular
circumstance, offers an alternative to InfiniBand's "native" switches.
This effort is known as *Converged Ethernet (CE)*, and it makes it
possible to enjoy the best of both worlds: the performance of
InfiniBand and the ubiquity of Ethernet.  Putting these two outcomes
together, :numref:`Figure %s <fig-verbs>` shows the central role RDMA
(the abstraction) and Verbs (the API for that abstraction) play in
today's datacenter networks. The rest of this section looks at this
middle layer (RDMA-based message transactions) and the next section
looks at the bottom layer of the diagram (optimizing Ethernet for RDMA
workloads).

.. _fig-verbs:
.. figure:: message/figures/verbs.png
   :width: 350px
   :align: center

   RDMA is invoked using the Verbs API. This API sits under multiple
   higher-layer APIs and is implemented by multiple lower-level
   technologies.

|Message|.4.2 Programming Interface
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

:numref:`Figure %s <fig-verbs>` highlights the centrality of the RDMA
abstraction, but it is conceptual. To understand how RDMA works in
practice, it is helpful to redraw it with the additional detail shown
in :numref:`Figure %s <fig-libibverbs>`. Here, we see the application
loads two libraries: ``libibverbs`` and ``librdmacm``.  The
first—which you can read as "lib-ib-verbs", with the "ib" standing for
InfiniBand—is effectively a low-level interface to an RDMA-capable
device. The second—which you can read as "lib-rdma-cm", with the "cm"
standing for "Communication Manager"—defines optional wrapper
functions that make the Verbs API easier to use.  The other thing to
note about the diagram is that ``libibverbs`` directly interacts with
the NIC, bypassing the OS. (Technically, the OS still polices access
to the NIC, but is not on the data path once access is granted to a
particular application.) The actual implementation of the message
transaction protocol at the core of RDMA is in the NIC.

.. _fig-libibverbs:
.. figure:: message/figures/libibverbs.png
   :width: 200px
   :align: center

   Software components that implement RDMA, including user-level
   libraries that directly interact with the HCA (InfiniBand NIC).

Much of the complexity in using RDMA is in setting up (managing) the
shared state that the application processes need to communicate. This
involves three general steps. One is to create a *queue pair (QP)*,
which is a local handle for the communication end-point. The QP is
roughly analogous to a socket in that it is used to access the local
send and receive queues. A second is to create a *completion queue*
that the RDMA subsystem uses to deliver notifications that a send or
receive task has taken place. This is best understood in terms of its
familiar DMA counterpart: a device driver registers with a device so
it can receive interrupts signaling a transaction completing.  The
third step is to register the set of memory buffers that may be
remotely accessed on each host. Sharing information about these
buffers with your peers is typically part of the setup process.

All of this "communication management" overhead is conceptually
simple, but tediously detailed. We refer you to the respective manual
pages for the two libraries for more information. Also note that this
connection setup phase happens once for a long-running parallel
program, involving a relatively static set of peers. This means we are
not overly concerned about the overhead. A client browser would
definitely *not* want to go to this trouble for every web server it
connects to, so RDMA is not a candidate replacement for, say QUIC.

Once everything is set up, there are two usage patterns for RDMA,
which we broadly characterize as either *shared memory* or
*message passing*.  Students of concurrent systems will
recognize the duality of these two approaches (i.e., a system
constructed according to one model has a direct counterpart in the
other), as articulated by Lauer and Needham in 1979. They also happen
to match the patterns of the GPI and MPI libraries, respectively.

Because we have seen many examples of message passing, we focus here
on the shared memory pattern, which the Verbs API refers to as
*one-sided* operations. The *two-sided* counterpart provides
operations that allow processes to send and receive messages in both
directions. In the one-sided pattern, there are three categories of
operations:

 * **Read:** A block of memory is read from a remote host. The caller
   specifies the remote virtual address as well as a local memory
   address the data is to be copied to. The remote host is not
   notified that the read has happened.

 * **Write / Write-with-Immediate:** Similar to Read, but the data is
   written to the remote host. Write operations are performed with no
   notification to the remote host. Write-with-Immediate operations,
   however, do notify the remote host of the immediate value.

 * **Fetch-and-Add / Compare-and-Swap:** These are atomic operations,
   typically used to synchronize activity between hosts. The
   Fetch-and-Add operation atomically increments the value at a
   specified virtual address by a specified amount. The value prior to
   being incremented is returned to the caller. The Compare-and-Swap
   operation atomically compare the value at a specified virtual
   address with a specified value and if they are equal, a specified
   value will be stored at the address.

The two atomic operations are familiar to anyone who has written
programs that must synchronize concurrent processes. This is a common
activity in the kinds of parallel programs RDMA is designed to
support, but outside the scope of this book. Here, we focus on the
write operation. The following code snippet shows the client side
writing a "Hello, World" message to a remote memory address.

.. code-block:: c

   /* Register a memory region with the NIC */
   char buf[] = "Hello World";
   struct ibv_mr *mr = ibv_reg_mr(pd, buf, sizeof(buf), 0);

   /*  Post the RDMA write */
   ibv_wr_start(qpx);
   ibv_wr_rdma_write(qpx, remote_rkey, remote_addr);
   ibv_wr_set_sge(qpx, mr->lkey, (uintptr_t)buf, sizeof(buf));
   ibv_wr_complete(qpx);

The code snippet is surprisingly complicated, unless you are familiar
with device drivers, which is a reasonable comparison. Let's briefly
walk through the steps. We start with a call to ``ib _reg_mr`` to
register a region of application memory with the NIC; the buffer
holding the string ``"Hello World"`` in our case. In general, this
buffer can be reused for multiple sends once it's registered. The
actual write then spans four operations. The ``ibv_wr_start``
operation sets the context. In our example, we are assuming the QP has
already been created, corresponding to variable ``qpx``.  The
``ibv_wr_rdma_write`` operation specifies the target of the write,
which includes both the ``remote_addr`` and the ``remote_key`` needed
to write to that address. (Both parameters are established during the
Communication Management phase.) The third operation,
``ibv_wr_set_sge``, provides the local buffer that is the source of
data for the write.\ [#]_ Finally, the ``ibv_wr_complete`` operation
signals to the NIC that the message is ready to be sent.

.. [#] The "sge" in ``ibv_wr_set_sge`` operation denotes the concept
   of "Scatter-Gather", indicating that the message to be written
   might be scattered across multiple non-continuous memory
   buffers. (Our particular "Hello World" message is located in a
   single continuous buffer.) This is similar to the scatter/gather
   operations mentioned in Section |Apps|.1.4, but in that
   case work is "scattered" across multiple nodes, and in this case a
   message is "scattered" across multiple buffers on a single node.

What might be equally surprising about this example is how little we
know when the ``ibv_wr_complete`` returns, which is only that the
local NIC has accepted the message for transfer. We don't know that
the message has arrived at the remote server, or that it's been
successfully written into the remote buffer. Moreover, since this is a
one-way operation, the application on the remote server is not
explicitly notified when the write does in fact complete.

As a consequence of this design choice, the Verbs API provides other
mechanisms that application threads can use to synchronize.  On the
sender, for example, adding the following line to our "Hello
World" code fragment configures RDMA to deliver a signal to the
queue pair (``qpx``) when the write successfully completes.  In this
case, "completes" means that the message has been delivered over the
network and written into the destination buffer; it does not say
anything about whether the application has actually read it.

.. code-block:: c

   ibv_wr_set_flags(qpx, IBV_SEND_SIGNALED);

To see this completion signal (and others like it) senders typically
execute the following non-blocking "polling" operation:

.. code-block:: c

   ibv_poll_cq(cq, count, &wc)

This is where the *completion queue* mentioned earlier in this
section comes into play. That queue, denoted by variable ``cq`` in
this example, is where all send/receive completion events are
posted. It's created at the same time as the QP, and then linked to
the QP. It is not uncommon for a thread that needs to know that an
earlier send/receive has completed to execute a "spin loop" to watch
for such a notification:

.. code-block:: c

   struct ibv_wc wc;
   int n;
   do {
       n = ibv_poll_cq(cq, 1, &wc);
   } while (n == 0);

``ibv_poll_cq`` returns zero until an event arrives, at which point
the loop terminates. The thread would typically look at the ``wc``
(work completion) structure for more information about the event's
status (not shown in the example), but the important point is that
parallel programs that use this general approach to communication
prefer having a thread spin on a completion event to invoking a
blocking send operation.

The receiver has similar options. Briefly, the ``ibv_wr_rdma_write``
operation in the original example does not notify the destination when
the message has been written to a buffer. The receiver can, however,
execute a spin loop on the actual target memory buffer watching for
specific data values it might be expecting. A more likely
scenario—should the receiver need a completion signal to know that
it's safe to proceed—is for the sender to instead execute a
``ibv_wr_rdma_write_imm`` (write with immediate) operation. Doing so
causes a completion event to be delivered to the receiver's completion
queue when the message has been written to the destination buffer. The
receiver executes a spin loop using ``ibv_poll_cq``, just as we saw on
the sender.

.. takeaway::

   This simple one-way write example, and the subsequent variants, is
   just a small peek at the Verbs API, but it is enough of a peek to
   appreciate a fundamental insight about communication abstractions
   (and their APIs).  The insight is that "message transfer" and
   "process synchronization" are separable concerns. We often find it
   convenient to couple them—so, for example, a "receive" blocks until
   a request arrives (as is the case with Sockets), or a "send" blocks
   until a reply arrives (as is the case with RPC)—but this coupling
   is not a requirement. API design is about finding the right match
   between what the underlying communication mechanism is able to
   provide, and what the applications require to get their work done
   efficiently.

It is also worth noting that the Verbs API provides a collection of
low-level primitives, upon which MPI, NCCL, and the other libraries
implement more powerful read/write and send/receive operations.
Support for the Scatter/Gather communication pattern mentioned in
Section |Apps|.1.4 is one example.

.. admonition:: Further Reading

   `RDMA Core Userspace Libraries and Daemons
   <https://github.com/linux-rdma/rdma-core/blob/master/README.md>`__.

   H. Lauer and R. Needham. `On the Duality of Operating System
   Structures <https://dl.acm.org/doi/10.1145/850657.850658>`__.  7th
   ACM Symposium on Operating Systems Principles (SOSP), December 1979.


|Message|.4.3 Protocol Details
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

We conclude this overview of RDMA by looking at a few details of the
HCA, which is effectively where the message transaction protocol is
implemented. The description is relatively brief because RDMA uses
techniques we have already covered; only the details are protocol-specific.

The transport layer implemented by the HCA is responsible for in-order
packet delivery. It accomplishes this by implementing the sliding
window algorithm, with a 24-bit sequence number assigned to each
packet (rather than per-byte, as with TCP). It also fragments large
messages into packet-sized fragments on the sending side, and
reassembles those fragments into an application message on the
receiving side. Each fragment is a separate packet, with its own
sequence number. How we know that a sequence of packets needs to be
reassembled (because they are part of the same large message) will
become clear in a moment.

The transport header also includes a 24-bit QP to identify the local
end-point.  From a packet-format perspective, this field is similar to
the TCP port number in that it represents the destination of an RDMA
connection. But unlike TCP, which supports "well-known" ports, the QP
is a purely local identifier used to access the send and receive
queues (and other connnection state). In this way, a QP is more like a
socket identifier than a port.

The biggest difference from TCP is in the information specifically
related to the DMA aspect of the protocol. An "RDMA Extension Header"
contains information about the remote memory address that is to be
read or written. This information includes a 64-bit memory address, a
32-bit remote key (see the ``remote_rkey`` field in the code example),
and a 32-bit field indicating the length of the DMA transfer. (This is
how the receiver knows when a sequence of fragments needs to be
reassembled.)

Finally, note that we have been focused on what InfiniBand refers to
as its *Reliable Connection (RC)* class of service. There are other
options, including both unreliable connections and unreliable
datagrams. The set supports the full breadth of possible use cases,
but as our focus in this chapter is on RDMA, we describe only the RC
variant.
