.. index:: DSCP: DiffServ Code Point
.. index:: FIFO: First-In First-Out
.. index:: FQ/WFQ: Fair Queuing / Weighted Fair Queuing
.. index:: DRR: Deficit Round Robin

|Capacity|.2 Packet Schedulers
--------------------------------

Each router implements a packet scheduler that decides the order in
which queued packets are transmitted. This scheduler is closely
related to the *queuing discipline*, so much so, that it is not
uncommon to name the scheduler after the queuing discipline (as is the
case for the three examples given in this section). Whichever name we
choose, the mechanism is primarily concerned with deciding which
packet to transmit next on each link; the next section looks at other
queue-related issues.

|Capacity|.2.1 FIFO Queuing
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The idea of a FIFO scheduler (or FIFO queuing) is simple: the first
packet that arrives at a router is the first packet to be transmitted.
This is illustrated in :numref:`Figure %s(a) <fig-fifo>`, which shows
a FIFO with “slots” to hold up to eight packets. Given that the amount
of buffer space at each router is finite, if a packet arrives and the
queue (buffer space) is full, then the router discards that packet, as
shown in :numref:`Figure %s(b) <fig-fifo>`. This is done without
regard to which flow the packet belongs to or how important the packet
is. This is sometimes called *tail drop*, since packets that arrive at
the tail end of the FIFO are dropped.

.. _fig-fifo:
.. figure:: capacity/figures/f06-05-9780123850591.png
   :width: 400px
   :align: center

   FIFO queuing (a), and tail drop at a FIFO queue (b).

Note that tail drop and FIFO are two separable ideas. FIFO is a
*scheduling discipline*—it determines the order in which packets are
transmitted. Tail drop is a *drop policy*—it determines which packets
get dropped. Because FIFO and tail drop are the simplest instances of
scheduling discipline and drop policy, respectively, they are
sometimes viewed as a bundle—the vanilla queuing implementation.
Unfortunately, the bundle is often referred to simply as *FIFO
queuing*, when it should more precisely be called *FIFO with tail
drop*. The next section provides an example of other drop policies,
which uses a more complex algorithm than “Is there a free buffer?” to
decide when to drop packets. Such a drop policy may be used with FIFO,
or with one of the following more sophisticated schedulers.

|Capacity|.2.2 Priority Queuing
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A simple variation on FIFO queuing is priority queuing. The idea is to
mark each packet with a priority; the mark could be carried, for
example, in the IP header, as we discuss in a moment. The routers then
implement multiple FIFO queues, one for each priority class. The
router always transmits packets out of the highest-priority queue if
that queue is nonempty before moving on to the next priority queue.
Within each priority, packets are still managed in a FIFO manner.
This idea is a small departure from the best-effort delivery model,
but it does not go so far as to make guarantees to any particular
priority class. It just allows high-priority packets to cut to the
front of the line.

The problem with priority queuing is that the high-priority queue can
starve out all the other queues; that is, as long as there is at least
one high-priority packet in the high-priority queue, lower-priority
queues do not get served. For this to be viable, there needs to be
limits on how much high-priority traffic is inserted in the queue. It
should be immediately clear that we can’t allow users to set their own
packets to high priority in an uncontrolled way; we must either
prevent them from doing this altogether or provide some form of
“pushback” on users. One obvious way to do this is to use
economics—the network could charge more to deliver high-priority
packets than low-priority packets. However, there are significant
challenges to implementing such a scheme in a decentralized
environment such as the Internet.

Another approach is to enforce good behavior at a network-managed
ingress node, such as the first router that connects an ISP to a set
of its customers. End users can set the priority as desired, but the
ingress node *polices* the flow of packets to ensure they stay below
some target rate. Policing traffic is one aspect of differentiated services
introduced in the previous section. We'll see an example of how
policing is managed in a later section, but for now, the key is to
recognize the potential value of priority queues, and the
complications in using them.

|Capacity|.2.3 Fair Queuing
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The main problem with FIFO queuing is
that it does not discriminate between different traffic sources, or,
in the language introduced in the previous section, it does not
separate packets according to the flow to which they belong. This
means it is possible for a greedy flow to starve all the other
flows. While priority queuing can serve some flows in preference to
others, it still doesn't prevent starvation; high-priority flows can
starve low-priority flows, and within a single priority, a greedy flow
can starve out other flows at the same priority.

Fair queuing (FQ) is an algorithm that has been designed to address
this problem. FQ is designed to isolate flows, so that the performance
of one flow is not impacted by the traffic sent on a different
flow. We'll say more about the definition of a "flow" in a moment.

One way to conceptualize fair queuing is to use a *fluid
model*. We imagine that the outgoing link is a pipe and the flows
passing through it are fluids, so we can divide the available capacity
among the flows in whatever granularity suits us. While it's obviously
not possible to implement the fluid model perfectly when we are
dealing with packets rather than fluids, all implementations try to
approximate the fluid model in some way.

In the simplest version of fair queuing, each flow gets an equal
share of the outgoing link's capacity. If there is an outgoing link of
capacity *C*, and there are *n* active flows, then each flow will get
to send *C/n* bits per second at a minimum when all flows are active.

There is also a variant of fair queuing called *weighted* fair queuing
(WFQ) in which we allocate unequal shares of the link to each flow. In
this case, each flow has a weight
:math:`W_i` and it will receive a share of the outgoing link of at least

.. math::

   W_i/\sum (W_k) \times C

So we can think of WFQ as letting us allocate a share of the link
bandwidth to a flow.  We can be confident that, on average, the
flow will get that share in spite of what other flows do.

Almost all modern queuing algorithms are *work
conserving*. That means that as long as there is something in the
queue, it will be sent, even if the flow with data in the queue has
been getting more than its allocated share. The outgoing link never
goes idle unless there is no traffic at all to send.
One effect of work conserving queues is that a flow that shares
a link with flows that are not sending any data can use
the full link capacity. As soon as the other flows start
sending, however, they will start to use their share and the capacity
available to this flow will drop towards its allocated share.

The definition of a "flow" can be anything we choose. In the
Differentiated Services model, a flow is identified as the set of
packets with the same marking in the Differentiated Services Code
Point (DSCP), a field in the IP header. Another valid definition of a
flow would be all the packets flowing between a pair of endpoints for
a single application. For now, all we need to know is that there are
some number of flows and we have some way to tell them apart by
looking at their packets. It usually a matter of configuring a router
to specify how we want it to identify flows.

Let's turn our attention to the challenge of implementing a fair
queuing scheme, starting with a simple round-robin approach. We create
one queue per flow, and the router services these queues in
round-robin order, as illustrated in :numref:`Figure %s <fig-fq>`.
When a flow sends packets too quickly (faster than its allocated
share), then its queue fills up. That will increase the delay for that
flow, and possibly even cause packets to be dropped if the queue gets
long enough, but it has no effect on the other flows, which keep on
getting served once every cycle by the round-robin algorithm.

.. _fig-fq:
.. figure:: capacity/figures/f06-06-9780123850591.png
   :width: 350px
   :align: center

   Round-robin service of four flows at a router.


The first complication is that the
packets being processed at a router are not necessarily the same
length.  To truly allocate the bandwidth of the outgoing link in a
fair manner, it is necessary to take packet length into consideration.
For example, if a router is managing two flows, one with 1000-byte
packets and the other with 500-byte packets, then a simple round-robin
servicing of packets from each flow’s queue will give the first flow
two-thirds of the link’s bandwidth and the second flow only one-third
of its bandwidth.


What we really want is bit-by-bit round-robin, where the router
transmits a bit from flow 1, then a bit from flow 2, and so on. That
would get us pretty close to the ideal fluid model. Clearly,
it is not feasible to interleave the bits from different packets. The
original FQ algorithm therefore simulates this behavior by first determining when a
given packet would finish being transmitted if it were being sent using
bit-by-bit round-robin and then using this finishing time to sequence
the packets for transmission.

To see how we might approximate bit-by-bit round-robin,
consider the behavior of a single flow and imagine a clock that ticks
once each time one bit is transmitted from all of the active flows. (A
flow is active when it has data in the queue.) For this flow, let :math:`P_i`
denote the length of packet *i*, let :math:`S_i` denote the time when the
router starts to transmit packet *i*, and let :math:`F_i`
denote the time when the router finishes transmitting packet *i*. If
:math:`P_i` is expressed in terms of how many clock ticks it takes to transmit
packet *i* (keeping in mind that time advances 1 tick each time this
flow gets 1 bit’s worth of service), then it is easy to see that
:math:`F_i = S_i + P_i`.

When do we start transmitting packet *i*? The answer to this question
depends on whether packet *i* arrived before or after the router
finished transmitting packet *i-1* from this flow. If it was before,
then logically the first bit of packet *i* is transmitted immediately
after the last bit of packet *i-1*. On the other hand, it is possible
that the router finished transmitting packet *i-1* long before *i*
arrived, meaning that there was a period of time during which the queue
for this flow was empty, so the round-robin mechanism could not transmit
any packets from this flow. If we let :math:`A_i`
denote the time that packet *i* arrives at the router, then
:math:`S_i = \max(F_{i-1}, A_i)`. Thus, we can compute

.. math::

   F_i = \max(F_{i-1}, A_i) + P_i

Now we move on to the situation in which there is more than one flow,
and we find that there is a catch to determining :math:`A_i`.
We can’t just read the wall clock when the packet arrives. As noted
above, we want time to advance by one tick each time all the active
flows get one bit of service under bit-by-bit round-robin, so we need a
clock that advances more slowly when there are more flows. Specifically,
the clock must advance by one tick when *n* bits are transmitted if
there are *n* active flows. This clock will be used to calculate
:math:`A_i`.

Now, for every flow, we calculate :math:`F_i` for each packet that arrives
using the above formula. We then treat all the :math:`F_i` as timestamps,
and the next packet to transmit is always the packet
that has the lowest timestamp—the packet that, based on the above
reasoning, should finish transmission before all others.


Without going into further detail, you can start to see the problems
of implementing FQ accurately. We have to do a sorting operation on
the timestamps of all packets that could be candidates for the next
transmission every time we want to send a packet, and this sorting
operation grows in cost with the number of active flows. Impementing
FQ at high speed proved challenging enough to inspire a lot of work on
creating good approximate implementations of fair queuing that don't
have the same computational complexity.

One well known and widely implemented approximation to WFQ is *deficit
round robin* (DRR). DRR takes care of the problem of variable packet
sizes that affects simple round robin using a *deficit counter* for
each queue. Initially this counter is set to zero for each queue. Then
in each round of the algorithm, a *quantum* is added to the counter
for each queue. The quantum is a configurable number of bytes that
should be in the range of an average packet size. It can be the same
for all queues for fair queuing, or it can be set to different values
per queue to implement weighted fair queuing. For simplicity here we
use the same quantum of 500 bytes for all queues.

We illustrate the process in :numref:`Figure %s
<fig-drr1>` and :numref:`Figure %s
<fig-drr2>`. There are four queues each holding some number of packets
of different sizes. There is a pointer indicating which queue is next
due for service in the rotation. As the pointer reaches a queue, we
add the quantum to the deficit counter, which in this case was zero,
so it goes up to 500. The interpretation of this counter is that the
queue may send up to that many bytes but no more. So the 300-byte
packet is sent, but there is not enough "credit" to send the next
500-byte packet. So the deficit counter is set to 500-300 = 200,
representing the difference between what the flow was entitled to send
and what it actually sent. The pointer moves on to the next queue, and
the 400-byte packet is sent and its deficit will be set to 500-400 = 100.



.. _fig-drr1:
.. figure:: capacity/figures/DRR1.png
   :width: 550px
   :align: center

   Serving the first queue in deficit round robin.


.. _fig-drr2:
.. figure:: capacity/figures/DRR2.png
   :width: 550px
   :align: center

   Serving the second queue in deficit round robin.

Finally, after all four queues have been serviced, the pointer comes
back to the first queue again, and its counter is again incremented by
the quantum of 500, bringing it to 700. This means that the next *two*
packets can be sent, since their total size is only 600 bytes. If
there are more packets in the queue, its deficit would now be set
to 100. If the queue has now become empty, the deficit counter is set
to zero again. It is important that a queue with no packets
in it does not keep accumulating credit; otherwise an idle flow could
build up an unlimited amount of credit and then starve out the other
flows at some point in the future.

Note that the effect of the deficit scheme is to even out the impact
of variable-sized packets. On average, every flow is getting to send
500 bytes per round as long as it has packets to send, so fairness is
enforced among flows on average. Further details of the algorithm and
proofs of its fairness are included in the paper by Shreedhar and Varghese.

.. _reading_drr:
.. admonition:: Further Reading

     M. Shreedhar and G. Varghese. `Efficient fair queuing using deficit
     round robin <https://dl.acm.org/doi/10.1145/217391.217453>`__.
     ACM SIGCOMM '95 Symposium, August 1995.

It is possible to combine DRR with a priority queuing scheme of the
sort described above. You can have one queue that always gets served
as long as it has packets in it, and then a set of DRR queues that are
served using the DRR algorithm whenever they have packets and the
priority queue is empty. This approach has been implemented in some
commercial routers to provide one low-latency queue and a set of DRR
queues that share the remaining bandwidth in a weighted fair manner.

.. admonition:: Systems Thinking Takeaway

   The preceding discussion of queue management illustrates an
   important system design principle known as *separating policy and
   mechanism*. The idea is to view each mechanism as an opaque box
   that provides a multifaceted service that can be controlled by a
   set of knobs. A policy specifies a particular setting of those
   knobs but does not know (or care) about how the policy is
   implemented.  In this case, the mechanism in question is the
   queuing discipline, and the policy is a particular set of scheduler
   settings (such as weights and priorities) to be applied to traffic
   classes. Similarly, there are various mechanims to classify packets
   in to classes, such as flow-based or DSCP-based, and it a matter of
   policy to set the classifiers to divide traffic into particular
   classes to be scheduled.
