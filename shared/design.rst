5.1  Design Issues
------------------------------

Mediating access to a shared medium is a resource allocation
problem.  The communication medium is the resource, and some mechanism
needs to decide who to allocate it to next. In a network setting it
seems obvious that the decision-making process needs to be
decentralized; each node has to participate in the decision. While
literally true, in the sense that each node has to "behave correctly",
the actual decision-making mechanism could be centralized.  This is
what happens with a room full of people when one is elected as the
moderator; people raise their hand when they want to speak, and the
moderator decides who goes next. This is a fairly common strategy for
networks, and just as with people, the moderator may announce a
"schedule" (i.e., an order of speakers) when several people have their
hand up, rather than having to interject itself between every speaker.

Another common solution in the human setting is to "go around the
room", giving each person an opportunity to speak. If they decline,
the "speaking token" is passed to the next person. One of the earliest
Local Area Network technologies, generically known as *Token Ring*,
adopted a mediation algorithm inspired by this idea. It required the
nodes be arranged in a ring topology, providing a physical order on
which host is next. The *Cambridge Ring*, a contemporary of the
original Ethernet, is one of the earliest examples.  Later, *Fiber
Distributed Data Interface (FDDI)* was an international standard based
on the same approach. FDDI supported 100 Mbps speeds, which was
impressive at the time, but it was eventually rendered obsolete by
Ethernet's continued bandwidth improvements.

Finally, just as in a roomful of people, it also works to let each
node speak whenever it has something to say (assuming no other node is
currently transmitting), and then adopt some "backoff-and-retry"
algorithm should two or more nodes happen to speak at the same
time. Similar in spirit to statistical multiplexing, this is exactly
what the original Ethernet did, using an *exponential backoff*
algorithm. Each time a node tries to transmit but fails, it increases
the amount of time it waits before trying again. To be more precise,
after the first collision, each node first delays either 0 or 51.2
μs, selected at random. If this effort fails, it then waits 0, 51.2,
102.4, or 153.6 μs (selected randomly) before trying again; this is k
× 51.2 for k=0..3. After the third collision, it waits *k × 51.2* μs
for k = 0..7, again selected at random. You can probably see a pattern
here. In general, the algorithm randomly selects a *k* between 0 and
2\ :sup:`n` - 1 where *n* is the number of collisions experienced so
far, and waits *k × 51.2* μs before retrying. The node gives up after
a given number of tries and reports a transmit error to the
host. Senders typically retry up to 16 times, although the backoff
algorithm caps *n* in the above formula at 10.

What's special about 51.2 μs? It was the historical maximum RTT for an
Ethernet, enforced by limiting the physical length of Ethernet cables
and the maximum number of repeaters. While the details are now mostly
of historical interest, the fundamental problem is general. To be sure
of detecting collisions, an Ethernet sender needed to listen for one
RTT after it began sending, in case a distant sender started
transmitting at the last possible moment before detecting the line was
in use. This in turn led to requirements on the minimum frame size (46
bytes of data, plus the Ethernet header) to ensure collisions would be
detected. This maximum RTT came to be used as the basis of the backoff
interval calculation to increase the chances that two nodes that had
experienced collisions would not collide the next time they
transmitted.

For our purposes, the takeaway is that multi-access
networks have to be of a limited size, both in terms of physical reach
and in terms of how many nodes they can support. Multi-access Ethernet
works for hundreds of nodes, but, as with a room full of people, too
many nodes (people) results in no one getting the floor.

One generalization we can make about these examples is that they can
be characterized as either optimistic or conservative. If you assume a
lightly loaded network, you can optimistically transmit when the
shared link is idle and back off if contention is detected. Like the
original Ethernet, this is what Wi-Fi does. On the other hand, if you
are trying to run your network at high utilization, perhaps because
you are trying to maximize the number of nodes that can connect to a
limited amount of radio spectrum, then you will be more conservative
in your allocation strategy, and centrally allocate some "share of
link capacity" to each node. This is what 5G does. PON is more like 5G
than Wi-Fi, but enough different to warrant its own subsection.

There are three other design issues of note. The first is that
multi-access networks allow all connected nodes to see (receive) every
packet the other hosts send.\ [#]_ Being able too receive every message has
an upside—it means the network can easily support *broadcast*, which
simplifies the design of some network applications—but it also has
security implications since nodes can snoop on each other's
traffic. This risk is compounded by the fact there there are often no
physical barriers, like a secured room or building, to keep bad actors
from gaining access to network traffic. Deciding what hosts are
allowed to legitimately connect to such a network, for example using
an authentication mechanism, is an important part of a comprehensive
solution.

.. [#] This is not always the case on wireless networks, due to the
       hidden terminal problem discussed below.

The second issue is unique to wireless multi-access network: wireless
nodes are inherently mobile, and this mobility has implications on
resource allocation. This is because even in a network that supports
mobility, some nodes typically remain tethered to the wired network,
providing the mobile nodes with wider connectivity. This is the role
of a *base station* or *access point*, as they are often
called. Moreover, these base stations are typically deployed in a way
that provides overlapping coverage, with two or more potentially able
to serve a given mobile node. The resource allocation challenge is
deciding which, among potentially multiple base stations in some
geographic area, serves a given mobile node, and how does this
assignment change over time as nodes move around. The two main
wireless technologies have developed different approaches to address
this problem, each in keeping with their optimistic versus
conservative assumptions.

The third issue, which is unique to wireless networks, is that the
capacity of the network—i.e., how much bandwidth is available to be
allocated—is variable. It depends on several external factors that
interfere with the radio signal, and those factors continually change
over time. This means the allocation problem fundamentally has two
parts: (1) determine how many resources are available to allocate,
and (2) allocate those resources to the users with packets to
transmit. We explore this complication, and the current best practice
in adapting to observed conditions, in the next section.

