13.4 Remote DMA
----------------------------

.. A helpful RDMA Tutorial Slide Deck
   https://www.doc.ic.ac.uk/~jgiceva/teaching/ssc18-rdma.pdf

RDMA is a remote variant of DMA, an architecture commonly used for
device/host communication over a computer's I/O bus. A network adaptor
(aka NIC) is an example device that uses DMA to pass frames between
the NIC and the host. For example, a host passes an address for an
empty memory buffer to the NIC, and the NIC is able to directly write
incoming frames (arriving from the network) into that buffer without
the host CPU being involved. In the other direction, the host passes
an address for a full memory buffer to the NIC, and the NIC is able to
transmit a frame onto the network by directly reading from that
buffer, again without the host CPU being involved.

As a communication abstraction, RDMA simply extends that model to
network-connected machines, as shown in :numref:`Figure %s <fig-rdma>`.
As a consequence, it becomes possible for two application
process to access (read and write) blocks of data from each others'
memory. Functionally, RDMA can be viewed as a special case of an RPC,
where only two "remote procedures" are supported: *Read( )* and
*Write( )*. That perspective glosses over a lot of details, but it's a
reasonable mental model for our purposes.

.. _fig-rdma:
.. figure:: message/figures/rdma.png
   :width: 450px
   :align: center

   RDMA supports read/write access to memory buffers on remote
   machines.

The history of RDMA traces back to early parallel computers used in
*High Performance Computing (HPC)*, which typically ran parallel
codes, such as weather forecasting, financial modeling, aerodynamic
design, atomic bomb simulations, and so on. Those machines had
internal interconnects that were eventually replaced by external
networks, but because of their focus on supporting low-latency
communication among parallel tasks, they largely avoided the more
ubiquitous Internet technology in favor of purpose-built networks.
Of these, Inifiniband—now a product of Nvidia—became the industry
standard, and RDMA became the end-to-end communication abstraction
running on top of Infiniband. Today, RDMA is a key enabling technology
for AI, which is why it is becoming mainstream. (The market for
weather forecasters, and the like, is fairly small.)

.. There is a second thread of history, having to do with wanting to
   replace internal memory buses with system-area networks (think
   FIberChannel), but it's not clear that adds anything important to
   the discussion. Maybe worth a footnote.

The case for why TCP/IP running over an Ethernet-based packet switched
network is poor match for HPC workloads essentially boils down to two
arguments. One is that TCP/IP are implemented in the OS kernel, which
means the OS has to get involved in sending and receiving every
packet. With RDMA, the NIC directly writes packets into and reads
packets out of application memory buffers. This avoids hundreds of
instructions and the throughput penalty of copying packets from one
buffer to another. The second argument is that Ethernet based networks
are best effort, meaning that potential for congestion can lead to
both queuing delays and packet loss. Infiniband has its own switches,
along with its own Infiniband NICs, which makes it possible to reserve
end-to-end capacity, thereby avoiding congestion-induced delay.

Debating either side of these two arguments is an interesting exercise,
but not one we'll pursue here. Instead, we'll focus on two outcomes.
One is that the RDMA programming interface, known as the *Verbs API*,
has become a *de facto* standard for HPC programs, and most
importantly, for AI workloads the run in cloud datacenters. The Verbs
API is low-level, and so is typically not directly used by application
programs. Such applications are generally written to higher level
interface, of which there are several options. They include *Message
Passing Interface (MPI)* , *Open SHMEM* (for "shared memory"), and
*Open Fabrics Enterprise Distribution (OFED)*.

The second outcome is that Ethernet continues to evolve, and in this
particular circumstance, it is adopting new elements that minimize
Inifiband's performance advantage. This effort is known as *Converged
Ethernet (CE)*, and it makes it possible to enjoy the best of both
worlds: the performance of Infiniband and the ubiquity of Ethernet.
Putting these two outcomes together, :numref:`Figure %s <fig-verbs>`
shows the central role RDMA (the abstraction) and Verbs (the API for
that abstraction) play to today's datacenter networks. This section
looks at the top half of this diagram (using RDMA) and the next
section looks at the bottom half of the diagram (optimizing RDMA).

.. _fig-verbs:
.. figure:: message/figures/verbs.png
   :width: 400px
   :align: center

   RDMA is invoked using the Verbs API. This API sits under multiple
   higher-layer APIs and is implemented by multiple lower-level
   technologies.

:numref:`Figure %s <fig-verbs>` highlights the centrality of the RDMA
abstraction, but it is conceptual. To understand how RDMA works in
practice, it is helpful to redraw it with the additional detail shown
in :numref:`Figure %s <fig-libibverbs>`. Here, we see the application
loads two libraries: ``libibverbs`` and ``librdmacm``.  The
first—which you can read as "lib-ib-verbs", with the "ib" standing for
Infiniband—is effectively a low-level interface to an RDMA-capable
NIC. The second—which you can read as "lib-rdma-cm", with the "cm"
standing for "Communication Manager"—defines high-level wrapper
functions that make the Verbs API easier to use. The other thing to
note about the diagram is that ``libibverbs`` directly interacts with
the NIC, bypassing the OS. (Technically, the OS still polices access
to the NIC, but is not on the data path once access is granted to a
particular application.) The actual implementation of the RDMA logic
is in the NIC.

Much of the complexity in using RDMA is in setting up (managing) the
shared communication apparatus. Registering memory is a two-step
process: (1) create a *protection domain* that identifies the set of
nodes that are to collaboratively share access to each others' memory,
and (2) bind the addresses of a set of buffers that may be remotely
accessed by all those nodes. Once such a distributed pool of shared
memory is established, all other RDMA operations are invoked in the
context of that pool.

In addition to registering memory, setup also involves creating *event
queues* that the RDMA subsystem uses to deliver notifications that a
transaction has taken place. This is best understood in terms of the
familiar its DMA counterpart: a device driver registers with a device
so it can receive interrupts signaling a transaction completing. (See
the example code snippet below.)

All of this "connection management" overhead is conceptually simple,
but tediously detailed. We refer you to the respective manual pages
for the two libraries for more information.

.. admonition:: Further Reading

   RDMA Core Userspace Libraries and Daemons
   https://github.com/linux-rdma/rdma-core/blob/master/README.md

Once everything is set up, there are two usage patterns for RDMA,
which we broadly characterize as *shared memory* and *message
passing*.  Students of concurrent systems will recognize the duality
of these two approaches (i.e., a system constructed according to one
model has a direct counterpart in the other), as articulated by Lauer
and Needham in 1979.

.. admonition:: Further Reading

   H. Lauer and R. Needham. On the Duality of Operating System
   Structures. *Proceedings of the 7th ACM Symposium on Operating
   Systems Principles (SOSP).* December 1979.
   (https://dl.acm.org/doi/abs/10.1145/850657.850658)

Because we have seen many examples of message passing, we focus here
on the shared memory pattern, which the Verbs API refers to as
*one-sided* operations. The *two-sided* counterpart provides
operations that allow processes to send/receive messages in both
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

The two atomic operations are familiar to anyone that has written
programs that must synchronize concurrent processes. This is a common
activity in the kinds of parallel programs RDMA is designed to
support, but outside the scope of this book. Here, we focus on the
write operations, and in particular, *Write-with-Immedate*. The
following code snippet shows the client side writing a "Hello, World"
message to a remote address, along with an "immediate" ``42``. There's
nothing magical about the number 42 (it's just an example of a value
that get passed to the server), but in practice it might indicate
something about the state of the peer process or what the peer expects
the receiver to do with the message.

.. code-block:: c

   const char *msg = "Hello, World!";
   memset(ctx.buf, 0, BUFFER_SIZE);
   memcpy(ctx.buf, msg, strlen(msg) + 1);

   post_rdma_write(&ctx, ctx.buf, (uint32_t)(strlen(msg) + 1),
          remote_addr, remote_rkey, /*imm=*/42);
   poll_cq(ctx.cq, 1, NULL);

And correspondingly on the server side:

.. code-block:: c

    uint32_t imm;
    poll_cq(ctx.cq, 1, &imm);
    printf("[server] Message (via RDMA Write-with-Immediate, imm=%u): \"%s\"\n",
           imm, ctx.buf);

For both sides, ``ctx`` is the context set up before hand. It includes
a local buffer (``buf``) that holds the message being sent or
received, and a local "completion queue" (``cq``) where notifications
are delivered. In this example, both sides block on this queue: the
client waiting for notification its buffer has been successfully
written to the remote buffer, and the server waiting for notification
that a message has arrived in its local buffer.

Note that these are both clearly low-level primitives, upon which MPI
and the other libraries implement more powerful read/write and
send/receive operations. One could even implement RPC on top of such a
primitive. The point is that just such a primitive mechanism—assuming
the underlying NICs do their job—is the foundation for much of today's
AI workloads. We look at the NICs in the next section.
