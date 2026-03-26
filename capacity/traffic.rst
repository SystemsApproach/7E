|Capacity|.4  Traffic Engineering
-----------------------------------------

Another cloud-inspired use case is traffic engineering applied to the
wide-area links between datacenters. For example, Google has publicly
described their private backbone, called B4, which is built entirely
using bare-metal switches and SDN. Similarly, Microsoft has described
an approach to interconnecting their data centers called SWAN. A
central component of both B4 and SWAN is a
*Traffic Engineering (TE)* control program that provisions the network
according to the needs of various classes of applications.

The idea of traffic engineering for packet-switched networks is almost
as old as packet switching itself, with some ideas of traffic-aware
routing having been tried in the Arpanet. However, traffic engineering
only really became mainstream for the Internet backbone with the
advent of MPLS, which provides a set of tools to steer traffic to
balance load across different paths. However, a notable shortcoming of
MPLS-based TE is that path calculation, like traditional routing, is a
fully distributed process. Central planning tools are common but the
real-time management of MPLS paths remains fully distributed. This
means that it is near impossible to achieve any sort of global
optimization, as the path calculation algorithms–which kick in any
time a link changes status, or as traffic loads change–are making
local choices about what seems best.

Consider the example in :numref:`Figure %s <fig-te-example>`. Assume
that all links are of unit capacity and we are trying to find paths
for three unit flows of traffic. In the figure on the left, Flow A is
placed first and picks one of the two shortest paths available. Flow B
is placed next and takes the shortest remaining path, as the
single-hop path is already filled by Flow A. When placing Flow C last,
there is no choice but the long path. But a central algorithm that
looked at all three flows at once and tried to place them optimally
would end up with the much less wasteful set of paths shown on the
right hand side of the figure. While this is a contrived example,
sub-optimal outcomes as shown on the left are unavoidable when there
is no central view of traffic.

.. _fig-te-example:
.. figure:: capacity/figures/Slide53.png
    :width: 600px
    :align: center

    Example of non-optimal traffic engineering (left) and optimal
    placement (right).

B4 and SWAN recognize this shortcoming and move the path calculation to a
logically centralized SDN controller. When a link fails, for example,
the controller calculates a new mapping of traffic demands onto
available links, and programs the switches to forward traffic flows in
such a way that no link is overloaded.

Over many years of operation, these approaches have become more sophisticated. For
example, B4 evolved from treating all traffic equally to supporting a
range of traffic classes with different levels of tolerance to delay
and availability requirements. Examples of traffic classes
included: (1) copying user data (e.g., email, documents, audio/video)
to remote datacenters for availability; (2) accessing remote storage
by computations that run over distributed data sources; and (3)
pushing large-scale data to synchronize state across multiple
datacenters. In this example, user-data represents the lowest volume
on B4, is the most latency sensitive, and is of the highest
priority. By breaking traffic up into these classes with different
properties, and running a path calculation algorithm for each one, the
team was able to considerably improve the efficiency of the network,
while still meeting the requirements of the most demanding
applications.

Through a combination of centralizing the decision-making process,
programmatically rate-limiting traffic at the senders, and
differentiating classes of traffic, Google has been able to
drive their link utilizations to nearly 100%. This is two to three
times better than the 30-40% average utilization that WAN links are
typically provisioned for, which is necessary to allow those networks
to deal with both traffic bursts and link/switch failures. Microsoft's
reported experience with SWAN was similar. These hyperscale
experiences with SDN show both the value of
being able to customize the network and the power of centralized
control to change networking abstractions. A conversation with
Amin Vahdat, Jennifer Rexford, and David Clark is especially
insightful about the thought process in adopting SDN.

.. _reading_b4:
.. admonition:: Further Reading

   A. Vahdat, D. Clark, and J. Rexford. `A Purpose-built Global Network:
   Google's Move to SDN
   <https://queue.acm.org/detail.cfm?id=2856460>`__.
   ACM Queue, December 2015.
