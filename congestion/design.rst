|CC|.1 Design
------------------------------

Chapter |Capacity| outlines part of the design space for congestion
control, including the decision about how much routers do to help
manage congestion (e.g., do they isolate flows, do they implement
tail-drop or some more sophisticated queue management mechanism, do
they drop packets silently or to they send explicit notifications).
But there are other questions that only the sources of those packets
can answer. We start by identifying what those questions are, and
exploring the options available to TCP (and other transport protocols
running on edge hosts) to address them.

|CC|.1.1 Load Control
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

At its core, congestion control is networking's version of a general
system problem: *load control*. No matter how sophisticated or
simple-minded a system's resource alloction or scheduling
mechanism—the algorithm that decides which user gets to use system
resources next—there is a complementary question of how many users are
allowed into the system in the first place. One answer is none; if you
show up and want service, you get to compete for it.

This is what leads to congestion, and anyone who has driven on a
highway at rush hour has experienced it. There is a limited
resource—the space on the highway—and a set of cars, trucks, etc. that
compete for that resource. As rush hour gets underway, more traffic
arrives but the road keeps working as intended, just with more
vehicles on it. But there comes a point where the number of vehicles
becomes so large that everyone has to slow down (because there is no
longer enough space for everyone to keep a safe distance at the speed
limit) at which point the road actually becomes *less effective* at
moving vehicles. So, just at the point when you would be wanting more
capacity, there is actually less capacity to move traffic.

This is the situation depicted in :numref:`Figure %s <fig-collapse>`,
and while the right side of the load curve is labeled *congestion
collapse*, the general shape of the graph is the same in any system
with finite resources. If you've ever access a multi-user computer
system during peak load (e.g., just before an assignment is due),
you've likely suffered from the same experience. The same thing
happens when you try to log into a ticketing system for a popular
concert or a software site to download the latest release of a popular
game.

.. _fig-collapse:
.. figure:: congestion/figures/Slide1.png
   :width: 400px
   :align: center

   As load increases, throughput rises then falls at the point of
   congestion collapse.

The reason that congestion collapse occurred in the early Internet is
that dropped packets are not just discarded and forgotten. When the
end-to-end transport protocol is TCP, as it is for most Internet
traffic, a dropped packet will be retransmitted. So as congestion
rises, the number of retransmitted packets rises; in other words, the
number of packets sent into the network increases even if there is no
real increase in the offered load from users and applications. More
packets lead to more drops leading to more retransmissions and so
on. You can see how this leads to collapse.

A useful term in this context is *goodput*, which is distinguished
from throughput in the sense that only packets doing useful work are
counted towards goodput. So, for example, if a link is running at 100%
utilization, but 60% of the packets on that link are retransmitted due
to earlier losses, you could say the goodput was only 40%.

The solution is to somehow limit the load. On a multi-user computer
system there might be a centralized "gate" that decides to let new
users log in, but a centralized strategy is not practical on a network
the scale of the Internet. Each sender has to decide for itself
whether to send one packet, ten packets, or several gigabytes worth of
packets at any given time. In a network, the decision is ongoing
rather than a one-time gate, so it can be restated as deciding how
many bytes to transmit in a given period of time. For ongoing
conncetions, you know your recent history. This is a good place to
start, but you need a heuristic to tell you if you should try to go
faster, or if there are warning signs, that perhaps you should slow
down. For new connections, you need a heuristic to tell you have
aggresively you can start dumping packets into the network at the
start.

As an aside, since we have already seen the idea of *flow control*, it
is important to distinguish between flow control and congestion
control. Flow control involves keeping a fast sender from overrunning
a slow receiver. Congestion control, by contrast, is intended to keep
a set of senders from sending too much data *into the network* because
of lack of resources at some point. These two concepts are often
confused; as we will see, they also share some mechanisms.


|CC|.1.2 Signals from the Network
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The sender's decision to increase or decrease its sending rate is
based on different signals it receives or detects. There are several
types of signals, but the most important is an ACK that indicates one
of its packets has been received by the destination. If the
destination received the packet, it must have made it through the
network. By using ACKs to pace the transmission of packets, TCP is
said to be *self-clocking*.  On the other hand, a timeout signals that
a packet was lost, potentially implying that the network is congested,
and that TCP needs to reduce its sending rate. Because using packet
loss as a signal means congestion has already occurred and we are
reacting after the fact, we sometimes refer to this approach as
*control-based*, or alternatively, *loss-based*.

Waiting a packet loss as a congestion signal and then reacting to that
loss after the onset of congestion is one approach, but it's also
possible adopt a more proactive strategy, by trying to detect changes
in the measured throughput rate, and adjust the sending rate *before*
congestion becomes severe enough to cause packet loss.  Such
algorithms are said to be *avoidance-based*, or alternatively, either
*delay-based* or *rate-based*. Delay and rate are just two
representatoins of the same information, which observes how much
successfully data is sent over a period of time, where the time
interval is either shrinking or growing.

Both strategies depend on having an accuarate timeout mechanism, which
in turn depends on the heuristic used to calculate round-trip times;
the estimated RTT then serves as the basis for setting when the
timeout should fire. The next section looks at this issue, and the
following two then describe various loss-based and delay-baed
algorithms, respectively.


|CC|.1.3 Fairness and Stabililty
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

While we want good throughput and low delay, there are other
requirements. Two topics of particular importance when thinking about
congestion avoidance are *fairness* and *stability*. When the network
is congested, it's going to be necessary for some users or flows to
send less. It is clearly worth asking: which flows should send less?
Should all flows share the pain equally? And what happens if some
flows pay more attention to congestion signals than others? These
questions are at the heart of the fairness issue. Jain's *fairness
index* is one of the widely accepted ways to measure how fair a
network is. We dig into this topic in Chapter 3.

Stability is a critical property for any sort of control system, which
is what congestion control is. Congestion is detected, some action is
taken to reduce the total amount of traffic, causing congestion to
ease, at which point it would seem reasonable to start sending more
traffic again, leading back to more congestion. You can imagine that
this sort of oscillation between congested and uncongested states
could go on forever, and would be quite detrimental if the network is
swinging from underutilized to collapsing.  We really want it to find
an equilibrium where the network is busy but not so much so that
congestion collapse occurs. Finding these stable control loops has
been one of the key challenges for congestion control system designers
over the decades. The quest for stability features heavily in the
early work of Jacobson and Karels and stability remains a requirement
that subsequent approaches have to meet.

Once the initial congestion control algorithms of TCP were implemented
and deployed, researchers began to build mathematical models of TCP's
behavior. This enabled the relationship between packet loss rate,
round-trip time, and throughput to be established. The foundation was
laid in the paper by Mathis and colleagues, but there has been a body
of work that is ongoing as the congestion control algorithms
evolve. The idea that TCP would converge to a certain throughput given
stable conditions of RTT and loss also formed the basis for
*TCP-friendly rate control (TFRC)*. TFRC extends TCP-like congestion
control to applications that don't use TCP, based on the idea that
they can still share available capacity in a fair way with those that
do. We return to this topic in Section |CC|.5.

.. _reading_mathis_eqn:
.. admonition:: Further Reading

   M. Mathis, J. Semke, J. Mahdavi, and T. Ott. `The Macroscopic
   Behavior of the TCP Congestion Avoidance Algorithm
   <https://dl.acm.org/doi/abs/10.1145/263932.264023>`__.
   SIGCOMM CCR, 27(3), July 1997.

Finally, much of the theoretical work on congestion control has framed
the problem as *"a distributed algorithm to share network resources
among competing sources, where the goal is to choose source rate so as
to maximize aggregate source utility subject to capacity
constraints."* Formulating a congestion-control mechanism as an algorithm
to optimize an objective function is traceable to a paper by Frank
Kelly in 1997, and later extended by Sanjeewa Athuraliya and Steven
Low to take into account both traffic sources (TCP) and router queuing
techniques (AQM).

.. _reading_kelly_low:
.. admonition:: Further Reading

   F. Kelly. `Charging and Rate Control for Elastic Traffic
   <http://www.statslab.cam.ac.uk/~frank/elastic.pdf>`__.
   European Transactions on Telecommunications, 8:33–37, 1997.

   S. Athuraliya and S. Low, `An Empirical Validation of a Duality
   Model of TCP and Active Queue Management Algorithms
   <https://ieeexplore.ieee.org/document/977445>`__.  Proceedings of the
   Winter Simulation Conference, 2001.

This book does not pursue the mathematical formulation outlined in
these papers (and the large body of work that followed), but we do
find it helpful to recognize that there is an established connection
between optimizing a utility function and the pragmatic aspects of the
mechanisms described in this book. Congestion control is an area of
networking in which theory and practice have been productively linked
to explore the solution space and develop robust approaches to the
problem.

.. sidebar:: Reference Implementation

   About BSD and Linux as reference implementations
