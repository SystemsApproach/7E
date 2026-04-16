|CC|.1 Design Issues
------------------------------

Chapter |Capacity| outlines part of the design space for congestion
control, focusing what routers may (or may not) do to help manage
congestion; e.g., do they isolate flows, do they implement tail-drop
or some more sophisticated queue management mechanism, do they
drop packets silently or to they send explicit notifications.  There are
other questions that only the sources of those packets can answer. We
start by identifying what those questions are, and exploring the
options available to TCP (and other transport protocols running on
edge hosts) to address them.

|CC|.1.1 Load Control
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

At its core, congestion control is networking's version of a general
system problem: *load control*. No matter how sophisticated or
simple-minded a system's scheduler or resource allocation mechanism—the
algorithm that decides which user gets to use system resources
next—there is a complementary question of how many users are allowed
into the system in the first place. One answer is that there is no
load control mechanism; if you show up and want service, you get to
compete for it.

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
traffic, a dropped packet is retransmitted. So as congestion rises,
the number of retransmitted packets rises; in other words, the number
of packets sent into the network increases even if there is no real
increase in the offered load from users and applications. More packets
lead to more drops leading to more retransmissions and so on. You can
see how this leads to collapse.

A useful term in this context is *goodput*, which is distinguished
from throughput in the sense that only packets doing useful work are
counted towards goodput. So, for example, if a link is running at 100%
utilization, but 60% of the packets on that link are retransmitted due
to earlier losses, you could say the goodput is only 40%.

The solution is to somehow limit the load. On a multi-user computer
system there might be a centralized "gate" that decides to let new
users log in, but a centralized strategy is not practical on a network
the scale of the Internet. Each sender has to decide for itself
whether to send one packet, ten packets, or several gigabytes worth of
packets at any given time. In a network, the decision is ongoing
rather than a one-time gate, so it can be restated as deciding how
many bytes may be in transit in any given period of time. For ongoing
connections, you know your recent history. This is a good place to
start, but you need a heuristic to tell you if you should try to go
faster (because the network has unused capacity), or if there are
warning signs, that perhaps you should slow down. For new connections,
you need a heuristic to tell you have aggressively you can start
dumping packets into the network at the start.

As an aside, since we have already seen the idea of *flow control*, it
is important to distinguish between flow control and congestion
control. Flow control involves keeping a fast sender from overrunning
a slow receiver. Congestion control, by contrast, is intended to keep
a set of senders from sending too much data *into the network* because
of lack of resources at some location. These two concepts are often
confused; as we will see, they also share some mechanisms. (Note that
in principle congestion control algorithms are protocol independent,
but in practice, they have been integrated into TCP, and so we present
them in that context.)


|CC|.1.2 Signals from the Network
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The sender's decision to increase or decrease its sending rate is
based on different signals it receives or detects. There are several
types of signals, but the most important is the arrival of an ACK,
indicating that one of its packets has been received by the
destination. If the destination received the packet, it must have made
it through the network. By using ACKs to pace the transmission of
packets, TCP is said to be *self-clocking*.  On the other hand, a
timeout signals that a packet was lost, potentially implying that the
network is congested, and that TCP needs to reduce its sending
rate. Because using packet loss as a signal means congestion has
already occurred and we are reacting after the fact, we sometimes
refer to this approach as *control-based*, or alternatively,
*loss-based*.

Waiting packet loss to signal congestion, and then reacting to that
loss after the onset of congestion, is one approach. But it is
possible adopt a more proactive strategy, by watching for changes in
the measured throughput rate, and adjusting the sending rate *before*
congestion becomes severe enough to cause packet loss.  Such
algorithms are said to be *avoidance-based*, or alternatively, either
*delay-based* or *rate-based*. Delay and rate are directly related,
providing two ways of looking at the same information. The key is that
the sender observes how much data is successfully sent during some
period of time, where the time interval is either shrinking or
growing.

Both strategies depend on having an accurate timeout mechanism, which
in turn depends on the heuristic used to calculate round-trip times;
i.e., the timeout is set a bit larger than the estimated RTT.  The
next section looks at this issue, and the following two sections then
describe various loss-based and delay-based algorithms, respectively.

|CC|.1.3 Fairness and Stability
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

While we want good throughput and low delay, there are other
requirements. Two topics of particular importance when thinking about
congestion avoidance are *fairness* and *stability*. When the network
is congested, it's going to be necessary for some users or flows to
send less. It is clearly worth asking: which flows should send less?
Should all flows share the pain equally? And what happens if some
flows pay more attention to congestion signals than others? These
questions are at the heart of the fairness issue. Jain's *fairness
index* is one of the widely accepted ways to measure how fair a
network is. We briefly look at it here.

When several flows share a particular link, we would like for each
flow to receive an equal share of the bandwidth. This definition
presumes that a *fair* share of bandwidth means an *equal* share of
bandwidth. But equal shares may not equate to fair shares.  Should we
also consider the length of the paths being compared? For example, as
illustrated in :numref:`Figure %s <fig-path-len>`, what is fair when
one four-hop flow is competing with three one-hop flows?

.. _fig-path-len:
.. figure:: congestion/figures/Slide10.png
   :width: 550px
   :align: center

   One four-hop flow competing with three one-hop flows.

Assuming that the most fair situation would be one in which all flows
receive the same bandwidth, networking researcher Raj Jain proposed a
metric that can be used to quantify the fairness of a
congestion-control mechanism. Jain’s fairness index is defined as
follows. Given a set of flow throughputs

.. math::

   (x_{1}, x_{2}, \ldots , x_{n})

(measured in consistent units such as bits/second), the following
function assigns a fairness index to the flows:

.. math::

   f(x_{1}, x_{2}, \ldots ,x_{n}) = \frac{( \sum_{i=1}^{n} x_{i}
   )^{2}} {n  \sum_{i=1}^{n} x_{i}^{2}}

The fairness index always results in a number between 0 and 1, with 1
representing greatest fairness. To understand the intuition behind this
metric, consider the case where all *n* flows receive a throughput of
1 unit of data per second. We can see that the fairness index in this
case is

.. math::

   \frac{n^2}{n \times n} = 1

Now, suppose one flow receives a throughput of :math:`1 + \Delta`.
Now the fairness index is

.. math::

   \frac{((n - 1) + 1 + \Delta)^2}{n(n - 1 + (1 + \Delta)^2)}
   = \frac{n^2 + 2n\Delta + \Delta^2}{n^2 + 2n\Delta + n\Delta^2}

Note that the denominator exceeds the numerator by :math:`(n-1)\Delta^2`.
Thus, whether the odd flow out was getting more or less than all the
other flows (positive or negative :math:`\Delta`), the fairness index has
now dropped below one. Another simple case to
consider is where only *k* of the *n* flows receive equal throughput,
and the remaining *n-k* users receive zero throughput, in which case the
fairness index drops to \ *k/n*.

Stability is another critical property for any sort of control system,
which is what congestion control is. Congestion is detected, some
action is taken to reduce the total amount of traffic, causing
congestion to ease, at which point it would seem reasonable to start
sending more traffic again, leading back to more congestion. You can
imagine that this sort of oscillation between congested and
uncongested states could go on forever, and would be quite detrimental
if the network is swinging from underutilized to collapsing.  We
really want it to find an equilibrium where the network is busy but
not so much so that congestion collapse occurs. Finding these stable
control loops has been one of the key challenges for congestion
control system designers over the decades. The quest for stability
features heavily in the early work of Jacobson and Karels and
stability remains a requirement that subsequent approaches have to
meet.

Finally, much of the theoretical work on congestion control has framed
the problem as *"a distributed algorithm to share network resources
among competing sources, where the goal is to choose source rate so as
to maximize aggregate source utility subject to capacity
constraints."* Formulating a congestion-control mechanism as an
algorithm to optimize an objective function is traceable to a paper by
Frank Kelly in 1997, and later extended by Sanjeewa Athuraliya and
Steven Low to take into account both traffic sources (TCP) and router
queuing techniques (AQM).

We do not pursue the mathematical formulation outlined in these papers
(and the large body of work that followed), but we do find it helpful
to recognize that there is an established connection between
optimizing a utility function and the pragmatic aspects of the
mechanisms described in this chapter. Congestion control is an area of
networking in which theory and practice have been productively linked
to explore the solution space and develop robust approaches to the
problem.

.. _reading_kelly_low:
.. admonition:: Further Reading

   R. Jain, D. Chiu, and W. Hawe. `A Quantitative Measure of Fairness
   and Discrimination for Resource Allocation in Shared Computer Systems
   <https://www.cse.wustl.edu/~jain/papers/ftp/fairness.pdf>`__.
   DEC Research Report TR-301, 1984.

   F. Kelly. `Charging and Rate Control for Elastic Traffic
   <http://www.statslab.cam.ac.uk/~frank/elastic.pdf>`__.
   European Transactions on Telecommunications, 8:33–37, 1997.

   S. Athuraliya and S. Low, `An Empirical Validation of a Duality
   Model of TCP and Active Queue Management Algorithms
   <https://ieeexplore.ieee.org/document/977445>`__.  Proceedings of the
   Winter Simulation Conference, 2001.


.. TODO -- Possibly include a sidebar on BSD/Linux as the reference
   implementation. Might also talk about Ware's fairness research in a
   sidebar.
