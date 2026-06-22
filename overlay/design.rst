|Overlay|.1 Design Issues
-----------------------------------------------

If an overlay is a new network implemented on top of an existing
network—presumably to provide some new functionality or
capability—then an obvious question to ask is: why didn't we just add
the new feature to the existing network? There are as many answers as
there are overlays, but they all boil down to one pragmatic
consideration: it is difficult to add new functionality to an
operational network. The challenge of getting everyone to agree to
such a change, let alone dealing with the logistics of deploying the
upgrade across thousands of switches, is an extremely high barrier.
Demonstrating the value of a new feature is easier if you can run the
new network *over the top*, without having to modify any of the
existing network infrastructure.

Overlays are a key way that networks evolve, as the Internet itself
perfectly illustrates. Decades ago, trying to augment the existing
circuit-switched network with a packet-switching capability would have
been an untenable technical problem, not to mention a significant
disruption of the business models of the incumbent telcos. The same is
true today, except now, Internet ISPs are the incumbents. History
teaches us that incumbents are cautious and change is slow, so much so
that entrenched technology tends to ossify over time. This results in
a phenomenon that Clayton Christensen famously called the *Innovator's
Dilemma*. Fortunately, networking gives us a workaround, with overlays
being the widely accepted as a way to introduce disruptive
technology. A National Academies report made this observation about
the Internet over two decades ago:

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

In some cases, an overlay is expected to be a temporary solution,
providing a path to incremental deployment of a new capability into
the network. A famous example of this is IPv6, which started off its
deployment as an overlay on the IPv4 Internet but is now widely
implemented in the same core routers that provide IPv4 service.

In other cases, an overlay is where certain functionality is best
provided. This raises a second question overlays force us to address:
what is the optimal *function placement?* That is, a does a particular
function (capability) belong inside the network or is it best
delivered over the top? In Chapter |Virt| we saw an example of VPNs
(which you can think of as providing a "nesting" function) which are,
in some cases, implemented on the same routers that provide the base
Internet service. In other cases, VPNs are implemented as an overlay
using encrypted tunnels as the logical links. This chapter looks at
other examples where experience has led us to conclude that the
function they support belongs on hosts connected to the edge of the
network. This outcome is predicted by the end-to-end argument we
discussed in Chapter |Intro|.

At the time the National Academies report was written, overlays were
the only viable way to introduce new capabilities into the Internet,
but starting in the late 2000s, various activities under the umbrella
of Software-Defined Networking (SDN)—including programmable forwarding
pipelines (Section |Tech|.2) and programmable control planes (Section
|Routing|.5)—made it easier to inject new features into the network
itself. That capability has not led to a wholesale reinvention of the
network's internals, but instead, has served to emphasize the
importance of this second question: deciding where functionality
belongs.  This chapter looks at three examples that have settled the
function placement question in favor of an overlay.

The first, Content Distribution Networks (CDNs), support a *caching*
function. The idea is to add a storage capability to the network, and
unless we're going to connect disks to routers, this seems like a
obvious function to implement as an overlay. This doesn't make CDNs
any less part of the Internet's "critical infrastructure"—it's proven
to be an absolute requirement for scaling content delivery—but there
is little appetite for augmenting routers with storage. As we will see
in Section |Overlay|.2, however, CDNs do include a "request routing"
function, which has been proposed as a "content-based addressing"
extension to the Internet's forwarding mechanism. But the success of
CDNs in providing the same service as an overlay has made this a tough
sell. The ease of deploying any new feature in an overlay means you
need a compelling argument for instead adding that feature to the
core.

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

Note that these two examples draw attention to yet another design
question, which is the extent to which a given overlay is
general-purpose—that is, provides shared infrastructure used by
multiple applications—or is tightly bundled with a particular
application. CDNs can serve as general infrastructure, able to serve
content on behalf of any web site or application. (Note, however, that
there are examples of applications building their own private CDNs,
with Netflix being one well-known example.)  In contrast, video
conferencing applications typically instantiate a multicast overlay in
support of a single conference call.; another call involving a
different set of participants gets its own overlay. But these overlays
are always in the service of the conferencing app; they are not
available for other purposes.

Our third example is *peer-to-peer networks*, originally developed by
file-sharing applications, such as BitTorrent.  This example brings
another design question to the forefront, which is how overlays find
the distributed set of servers they need to deploy their overlay on.
Today there is a ready answer: clouds and hosting centers provide the
means to acquire computing and storage resources at hundreds of
locations across the globe. But there is an alternative, which is to
depend on computing resources that you (and many other volunteers like
you) provide. Known as peer-to-peer networks, they originally gained
notoriety for their use sharing music, but today they serve a broader
role. For our purposes, they also illustrate how to design a
decentralized application that does not depend on a central authority,
that then has control over the service's users and their data.




