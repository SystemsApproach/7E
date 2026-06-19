|Overlay|.1 Design Issues
-----------------------------------------------

If an overlay is a new network implemented on top of an existing
network—presumably to provide some new functionality or
capability—then an obvious question to ask is: why didn't we just add
the new feature to the existing network? There are as many answers as
there are overlays, but they all boil down to one pragmatic
consideration: it is difficult to add new functionality to an
operational network. The challenge of getting everyone to agree to
such an upgrade, let alone dealing with the logistics of deploying the
upgrade across thousand of switches, is an extremely high barrier.
Demonstrating the value of a new feature is easier if you can run the
new network *over the top*, without having to modify any of the
existing network infrastructure.

Overlays are how networks evolve, as the Internet itself perfectly
illustrates. Decades ago, trying to augment the existing
circuit-switched network with a packet-switching capability would have
been an untenable technical problem, not to mention a significant
disruption of the business models of the incumbent telcos. The same is
true today, except now, Internet ISPs are the incumbents. History
teaches us that incumbents are cautious and change is slow, so much
so that entrenched technology tends to ossify over time. This results
in a phenomenon that Clayton Christensen famously called the
*Innovator's Dilemma*. Fortunately, networking gives us a workaround,
with overlays being the widely accepted as a way to introduce
disruptive technology. A National Academies report made this
observation about the state of the Internet over two decades ago:

  *The existing core IP network could be used simply as a data
  transport service, and disruptive technology could be implemented as
  an overlay in machines that sit between the core and the edge-user
  computers. This approach could allow new functionality to be
  deployed into a widespread user community without the cooperation of
  the major ISPs, with the likely sacrifice being primarily performance.
  Successful overlay functions might, if proven useful enough, be
  “pushed down” into the network infrastructure and made part of its
  core functionality.*

.. admonition:: Further Reading

   C. Christensen. `The Innovator's Dilemma
   <https://dl.acm.org/doi/book/10.5555/268729>`__.
   Harvard Business School Press,  June 1997.

   National Academies of Sciences, Engineering, and Medicine.
   `Looking Over the Fence at Networks: A Neighbor's View of
   Networking Research. <https://doi.org/10.17226/10183>`__.
   The National Academies Press, 2001.

Sometimes an overlay provides a path to incremental deployment of a
new capability. A famous example of this is IPv6, which started off
its deployment as an overlay on the IPv4 Internet but is now widely
implemented in the same core routers that provide IPv4 service.

While some functions do move into the core over time, supplementing or
replacing the original technology, that's not always the case. This
leads to a second question overlays force us to address: what is the
optimal *function placement?* That is, a does a particular function
(capability) belong inside the network or is it best delivered over
the top? In Chapter |Virt| we saw an example of VPNs (which you can
think of as providing a "nesting" function) which are, in some cases,
implemented on the same routers that provide the base Internet
service. In other cases, VPNs are implemented as an overlay using
encrypted tunnels as the logical links. This chapter looks at other
examples where experience has led us to conclude that the function
they support belongs on hosts connected to the edge of the
network. This outcome is predicted by the end-to-end argument we
discussed in Chapter |Intro|.

.. TODO -- We need to introduce e2e somewhere in Chapter 1.

This chapter looks at two examples, both of which have settled the
function placement question in favor of an overlay.  The first,
Content Distribution Networks (CDNs), support a *caching*
function. The idea is to add a storage capability to the network, and
unless we're going to connect disks to routers, this seems like a
obvious function to implement as an overlay. This doesn't make CDNs
any less part of the Internet's "critical infrastructure"—it's proven
to be an absolute requirement for scaling content delivery—but there
is little appetite for augmenting routers with storage. As we will see
in Section |Overlay|.2, however, CDNs do include a "request routing"
function, which has been proposed as a "content-based addressing"
extension to the Internet's forwarding mechanism. But the success of
CDNs in providing the same service as an overlay renders this issue
moot.

.. TODO -- some might see this as dismissive of content networking
   research, we could consider a sidebar (or not)

The second example, video conferencing, supports a *multicast*
function, whereby packets are delivered to multiple end-points instead
of a single destination. Multicast is a good example of a function
that has tried to find a home at multiple layers of the protocol
stack.  Originally, the MBone (multicast backbone) was an overlay that
enabled experimentation with IP multicast routing and forwarding. The
goal was to eventually add multicast support to the Internet's routers
(IPv4 addresses in the range ``224.0.0.0`` to ``239.255.255.255`` were
defined as multicast addresses), and while the function was
implemented in commercial routers, it never enjoyed widespread
deployment. Today, an over-the-top variant of multicast supports
familiar video conferencing apps, such as Zoom.

These two examples draw attention to yet another design question,
which the extent to which a given overlay is general-purpose—that is,
provides shared infrastructure used by multiple applications—or is
tightly bundled with a particular application. CDNs can serve as
general infrastructure, able to serve content on behalf of any web
site or application. (Note, however, that there are examples of
applications building their own private CDNs, with Netflix being one
well-known example.)  In contrast, video conferencing applications
typically instantiate a multicast overlay in support of a single
conference call; another call involving a different set of
participants gets its own overlay.  In practice, though, services such
as Zoom do maintain distributed infrastructure on which those per-call
overlays are created; one could view that infrastructure as another
kind of overlay.

A final question is where overlays find the distributed set of servers
to install their overlay nodes on. Today there is a ready answer:
clouds and hosting centers provide the means to acquire computing and
storage resources at hundreds of locations across the globe. But there
is an alternative, which is to depend on computing resources that you
(and many other volunteers like you) provide. These so called
*peer-to-peer networks* originally gained notoriety for their use
distributing content, but today they serve other purposes. *Onion
routing* is perhaps the most noteworthy, with Tor being a well-known
example. An onion routing overlay mimics IP forwarding, but with the
added feature that it is impossible to trace where a packet comes
from. This allows the sender to remain anonymous. Technically, any ISP
could offer onion routing, but of course, that would depend on users
trusting commercial providers to honor the expected anonymity. Hence,
the dependency on peer-provided resources.

.. TODO -- This story line lost the IPv6 angle. Not sure whether it
   belongs in the VPN paragraph or the the onion routing paragraph.

.. TODO -- With respect to video conferencing, three is a second
   function: timeliness. It would be good to include a forward
   reference to the next chapter.

.. TODO -- Could talk about other ways we can introduce functionality,
   such as programmable networks (and generally, SDN). Active Nets
   could fit here too. Maybe as a sidebar.



