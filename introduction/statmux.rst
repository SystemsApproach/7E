1.4 Resource Sharing
----------------------------------------------

Networks are a shared resource, carrying traffic between different
pairs of hosts, on behalf of many applications and users. This means
we need a strategy for *multiplexing* network resources—the links and
switches that make up the network—among those many traffic flows
traversing those resources. At an intuitive level, multiplexing can be
explained by analogy to a timesharing computer system, where a single
physical processor is shared (multiplexed) among multiple jobs, each
of which believes it has its own private processor. Similarly, data
being sent by multiple users can be multiplexed over the physical
links and switches that make up a network.

This section explores the options typically adopted by networks,
settling on *statsitical multiplexing* as the strategy we will employ
going forward. It's the approach that has proven the most
cost-effective for the general-purpose network we want to build.

1.4.1 Multiplexing Challenge
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

We start with a simple version of the problem. Consider the network
illustrated in :numref:`Figure %s <fig-mux>`, where the three hosts on
the left side of the network (senders S1-S3) are sending data to the
three hosts on the right (receivers R1-R3) by sharing a switched
network that contains only one physical link. (For simplicity, assume
that host S1 is sending data to host R1, and so on.) In this
situation, three flows of data—corresponding to the three pairs of
hosts—are multiplexed onto a single physical link by switch 1 and then
*demultiplexed* back into separate flows by switch 2. Note that we are
being intentionally vague about exactly what a “flow of data”
corresponds to. For the purposes of this discussion, assume that each
host on the left has a large supply of data that it wants to send to
its counterpart on the right.

.. _fig-mux:
.. figure:: introduction/figures/mux.png
   :width: 550px
   :align: center

   Multiplexing multiple logical flows over a single
   physical link.

There are several different methods for multiplexing multiple flows onto
one physical link. One common method is *synchronous time-division
multiplexing* (STDM). The idea of STDM is to divide time into
equal-sized quanta and, in a round-robin fashion, give each flow a
chance to send its data over the physical link. In other words, during
time quantum 1, data from S1 to R1 is transmitted; during time quantum
2, data from S2 to R2 is transmitted; in quantum 3, S3 sends data to R3.
At this point, the first flow (S1 to R1) gets to go again, and the
process repeats. Another method is *frequency-division multiplexing*
(FDM). The idea of FDM is to transmit each flow over the physical link
at a different frequency, much the same way that the signals for
different TV stations are transmitted at a different frequency over the
airwaves or on a coaxial cable TV link.

Although simple to understand, both STDM and FDM are limited in two
critical ways. First, if one of the flows (host pairs) does not have
any data to send, its share of the physical link—that is, its time
quantum or its frequency—remains idle, even if one of the other flows
has data to transmit. For example, S3 had to wait its turn behind S1
and S2 in the previous paragraph, even if S1 and S2 had nothing to
send. For computer communication, the amount of time that a link is
idle can be very large—for example, consider the amount of time you
spend reading a web page (leaving the link idle) compared to the time
you spend fetching the page. Second, both STDM and FDM are limited to
situations in which the maximum number of flows is fixed and known
ahead of time. It is not practical to resize the quantum or to add
additional quanta in the case of STDM or to add new frequencies in the
case of FDM.

1.4.2 Statistical Multiplexing
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Statistical multiplexing addresses these shortcomings. Although the
name is not all that helpful for understanding the concept,
statistical multiplexing is really quite simple, with two key ideas.
First, it is like STDM in that the physical link is shared over
time—first data from one flow is transmitted over the physical link,
then data from another flow is transmitted, and so on. Unlike STDM,
however, data is transmitted from each flow on demand rather than
during a predetermined time slot. Thus, if only one flow has data to
send, it gets to transmit that data without waiting for its quantum to
come around and thus without having to watch the quanta assigned to
the other flows go by unused. It is this avoidance of idle time that
gives packet switching its efficiency.

As defined so far, however, statistical multiplexing has no mechanism to
ensure that all the flows eventually get their turn to transmit over the
physical link. That is, once a flow begins sending data, we need some
way to limit the transmission, so that the other flows can have a turn.
To account for this need, statistical multiplexing defines an upper
bound on the size of the block of data that each flow is permitted to
transmit at a given time. This limited-size block of data is typically
referred to as a *packet*, to distinguish it from the arbitrarily large
*message* that an application program might want to transmit. Because a
packet-switched network limits the maximum size of packets, a host may
not be able to send a complete message in one packet. The source may
need to fragment the message into several packets, with the receiver
reassembling the packets back into the original message.

.. _fig-statmux:
.. figure:: introduction/figures/statmux.png
   :width: 500px
   :align: center

   A switch multiplexing packets from multiple sources
   onto one shared link.

In other words, each flow sends a sequence of packets over the
physical link, with a decision made on a packet-by-packet basis as to
which flow’s packet to send next. Notice that, if only one flow has
data to send, then it can send a sequence of packets back-to-back;
however, should more than one of the flows have data to send, then
their packets are interleaved on the link. :numref:`Figure %s
<fig-statmux>` depicts a switch multiplexing packets from multiple
sources onto a single shared link.

The decision as to which packet to send next on a shared link can be
made in a number of different ways. For example, in a network consisting
of switches interconnected by links such as the one in :numref:`Figure
%s <fig-mux>`, the decision would be made by the switch that transmits
packets onto the shared link. (As we will see later, not all
packet-switched networks actually involve switches, and they may use
other mechanisms to determine whose packet goes onto the link next.)
Each switch in a packet-switched network makes this decision
independently, on a packet-by-packet basis.

One of the issues that faces a network designer is how to make this
decision in a fair manner. For example, a switch could be designed to
service packets on a first-in, first-out (FIFO) basis. Another
approach would be to transmit the packets from each of the different
flows that are currently sending data through the switch in a
round-robin manner. This might be done to ensure that certain flows
receive a particular share of the link’s bandwidth or that they never
have their packets delayed in the switch for more than a certain
length of time. A network that attempts to allocate bandwidth to
particular flows is sometimes said to support *quality of service*
(QoS).

Also, notice in :numref:`Figure %s <fig-statmux>` that since the
switch has to multiplex three incoming packet streams onto one
outgoing link, it is possible that the switch will receive packets
faster than the shared link can accommodate. In this case, the switch
is forced to buffer these packets in its memory. Should a switch
receive packets faster than it can send them for an extended period of
time, then the switch will eventually run out of buffer space, and
some packets will have to be dropped. When a switch is operating in
this state, it is said to be *congested*.

Finally, even though this discussion focuses on a simple network
topology involving a single switch, statistical multiplexing works
across an arbitrarily large network. This is because each multiplexing
decision—i.e., deciding which packet to forward next and which packet
to drop if the buffer is full—is made on purely local basis. It
doesn't matter if a given packet has already traversed a dozen
upstream switches, or just now entered the network.  No global
coordination is required. This might not always result in a fair
decision since packets that have already been forwarded around the
globe intuitively have more "at risk" should they encounter
congestion, but that's a problem for another day (as we'll see in
Chapter 14). It turns out this issue is a consequence of our
architectural decision to separate in-the-network versus
edge-of-network concerns.

.. _key-stat-mux:
.. admonition:: Key Takeaway

  The bottom line is that statistical multiplexing defines a
  cost-effective way for multiple users (e.g., host-to-host flows of
  data) to share network resources (links and nodes) in a fine-grained
  manner. It defines the packet as the granularity with which the
  links of the network are allocated to different flows, with each
  switch able to schedule the use of the physical links it is
  connected to on a per-packet basis. Fairly allocating link capacity
  to different flows and dealing with congestion when it occurs are
  the key challenges of statistical multiplexing.

1.4.3  Managing Capacity
~~~~~~~~~~~~~~~~~~~~~~~~~~

Our high-level description of statistical multiplexing is defined in
term of links and switches, but we need to be more precise about
individual resources and their capacities. We also need to manage that
capacity, which includes acquiring more resources when necessary.
Adding (and removing) capacity is often often referred to as *resource
provisioning*, and while it sometimes requires installing new hardware
(or pulling new cables), it is increasingly accomplished by activating
physical infrastructure that is already in place.

Provisioning resources and scheduling resources go hand-in-hand; they
both assign resources to some user (or class of users), but they
differ in the time scale they operate on. Provisioning new resources
can take anywhere from seconds or minutes to days or weeks; the former
when those resources just need to be "turned on" and the latter when
new hardware needs to be procured and deployed. Scheduling resources
happens on much shorter time scales, typically measured in
microseconds or less. For example, statistical multiplexing
effectively schedules link resources on a packet-by-packet basis.

We revisit both provisioning and scheduling throughout the book,
but for now, we summarize the key resources:

* **Link Capacity:** Every link as a fixed bandwidth that limits the
  rate at which it can transmit packets. This capacity is measured in
  bits-per-second (bps). Sometimes new cables need to be laid, but
  more often than not, additional bandwidth can be "turned
  on". Deciding how to do this is known as *traffic engineering*.

* **Forwarding Capacity:** Switches are devices that move data from
  an input port to an output port. This requires an internal forwarding
  mechanism. This capacity is measured in packets-per-second
  (pps), with a given switch engineered to support some fixed
  forwarding rate. (See Chapter 2 for more details.) A new switch has
  to be deployed when additional forwarding capacity is needed.

* **Buffer Capacity:** Switches store packets in a buffer, where they
  wait their turn to be transmitted. This buffer is implemented in a
  finite amount of memory, with capacity is measured in bytes.  Memory
  is easily added to an existing switch, but too much memory for
  buffering results in potentially long buffering delays. This can be
  problematic, and so buffer capacity is often defined as a function
  of how many bytes can be in transit across the network (a concept
  explained in the next section).
