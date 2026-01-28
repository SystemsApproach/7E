5.1  Design Issues
------------------------------

How to mediate access to a shared medium is a resource allocation
problem.  The communication medium is the resource, and some mechanism
needs to decide who to allocate it to next. The network problem is
somewhat unique in that it seems obvious that the decision-making
process needs to be decentralized; each node has to participate in the
decision. While literally true, in the sense that each node has to
"behave correctly", the actual decision mechanism could be
centralized.  This is what happens with a room full of people when one
is elected as the moderator; people raise their hand when they want to
speak, and the moderator decides who goes next. This is a fairly
common strategy for networks, and just as with people, the moderator
can announce a "scheduling plan" when several speakers have their hand
up, rather than having to interject itself between every speaker's
turn.

Another common solution in the human setting is to "go around the
room", giving person an opportunity to speak. If they decline, the
"speaking token" is passed to the next person. One of the earliest
Local Area Network technologies, generically known as *Token Ring*,
adopted a mediation algorithm inspired by this idea. It required the
nodes be arranged in a ring topology, providing a physical order on
the hosts. The *Cambridge Ring*, a contemporary of the original
Ethernet, is one of the earliest examples.  Later, *Fiber Distributed
Data Interface (FDDI)* was an international standard based on the same
approach. FDDI supported 100 Mbps speeds, but was eventually rendered
obsolete by Ethernet's continual bandwidth upgrades.

Finally, just as in a roomful of people, it also works to let each
node speak whenever it has something to say (assuming no one else is
currently transmitting), and then adopt some "backoff-and-retry"
algorithm should two or more nodes happen to speak at the same
time. Similar in spirit to statistical multiplexing, this is exactly
what the original Ethernet did, using an *exponential backoff*
algorithm: Each time a node tries to transmit but fails, it doubles
the amount of time it waits before trying again. To be more precise,
each node first delays either 0 or 51.2 μs, selected at random. If
this effort fails, it then waits 0, 51.2, 102.4, or 153.6 μs (selected
randomly) before trying again; this is k × 51.2 for k=0..3. After the
third collision, it waits *k × 51.2* for k = 0..2\ :sup:`3` - 1, again
selected at random. In general, the algorithm randomly selects a *k*
between 0 and 2\ :sup:`n` - 1 and waits k × 51.2 μs, where *n* is the
number of collisions experienced so far. The node gives up after a
given number of tries and reports a transmit error to the
host. Senders typically retry up to 16 times, although the backoff
algorithm caps *n* in the above formula at 10.

What's special about 51.2 μs is an obvious question, and while the
details are now mostly of historical interest, the fundamental problem
they were trying to solve is general. For the algorithm to work,
Ethernet had to know the worst-case RTT. Restrictions were placed on
the physical deployment (e.g., length of cable, how many repeaters
could be used) so it could establish an upper bound on the RTT. It was
also important that all nodes be able to detect when a collision had
happened, so there was a minimum frame size (46 bytes of data, plus
the Ethernet header). For our purposes, one big takeaway is that
multi-access networks have to be of a limited size, both in terms of
physical reach and in terms of how many nodes they can
support. Multi-access Ethernet works for hundreds of nodes, but as
with a room full of people, too many nodes (people) results in no one
getting the floor.

One generalization we can make about these examples is that they can
be characterized as either optimistic or conservative. If you assume a
lightly loaded network, you can optimistically transmit when the
shared link is idle and back off if contention is detected. Like the
original Ethernet, this is what Wi-Fi does. On the other hand, if you
are trying to run your network at high utilization, perhaps because
you are trying to maximize the number of nodes that can connect to a
limited amount of radio spectrum, then you will be more conservative
in your allocation strategy, and centrally allocate some "share of
link capacity" to each node. This is what 5G does. PON is closer in
spirit to 5G, but enough different to warrent it's own subsection.

There are two other design issues of note. The first is multi-access
networks allow all connected nodes to see (receive) any packet any of
the other hosts sends. Being able too receive every message has an
upside—it means the network can easily support *broadcast*, which
simplifies the design of some network applications—but it also has
security implications since nodes can snoop on each other's
traffic. This risk is compounded by the fact there there are often no
physical barriers, like a secured room or building, to keep bad actors
from gaining access to network traffic. Deciding what hosts are
allowed to legitimately connect to such a network, for example using
an authentication mechanism, is an important part of a comprehensive
solution.

The second issue is unique to wireless multi-access network: Wireless
nodes are inherently mobile, and this mobility has implications on
resource allocation. This is because even in a network that supports
mobility, some nodes remain teathered to the wired network, providing
the mobile nodes with wider connectivity. This is the role of a *base
station* or *access point*, as they are often called. The resource
allocation challenge is deciding which, among potentially multiple
base stations in some geographic area, serves a given mobile node, and
how does this assignment change over time as nodes move around. The
two wireless technologies have developed different approaches to
address this problem, each in keeping with their optimistic versus
conservative assumptions.



