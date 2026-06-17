|Overlay|.1 Design Issues
-----------------------------------------------

If an overlay is a new network implemented on top of an existing
network—presumably one that provides some new functionality or
capability—then an obvious question to ask is why didn't we just add
the new feature to the existing network? There are as many answers as
there are overlays, but they all boil down to one pragmatic
consideration: it is difficult to add new functionality to an
operational network. The challenge of getting everyone to agree to
such an upgrade, let alone dealing with the logistics of deploying the
upgrade, eventually leads to the ossification of the established
network.  Demonstrating the value of a new feature is easier if you
can run the new network *over the top*, without having to modify any
of the network's existing infrastructure.

Overlays are how networks evolve, as the Internet itself perfectly
illustrates.  Decades ago, trying to augment the existing
circuit-switched network with a packet-switching capability would have
been an untenable technical problem, not to mention how disruptive
such a change would have been to the business models of the incumbent
Telcos. The same is true today, except now, Internet ISPs are the
incumbents. Incumbents are cautious and change is slow, but overlays
are widely accepted as a way to introduce disruptive technology. This
observation was noted by a National Academies report about the state
of networking over two decades ago:

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

   National Academies of Sciences, Engineering, and Medicine.
   `Looking Over the Fence at Networks: A Neighbor's View of
   Networking Research. <https://doi.org/10.17226/10183>`__.
   The National Academies Press, 2001.

While some functions do move into the core over time, that's not
always the case. This leads to a second question overlays force us to
address: what is the optimal *function placement?* That is, a does a
particular function (capability) belong inside the network or is it
best delivered over the top? In Chapter |Virt| we saw an example of
VPNs (which you can think of as providing a "nesting" function) being
supported inside the network, implemented on the same routers that
provide the base Internet.  VPNs are a kind of overlay, but because
they reuse the same forwarding mechanism, they are easily incorporated
into the core. This chapter looks at three other examples, where with
20/20 hindsight, the right answer seems to be that the function they
support belongs on hosts connected to the edge of the network.

The first, Content Distribution Networks (CDNs), support a *caching*
function. The idea is to add a storage capability to the network, and
unless we're going to connect disks to routers, this seems like a good
candidate function to implement as an overlay. This doesn't make CDNs
any less part of the Internet's "critical infrastructure"—it's proven
to be an absolute requirement for scaling content delivery—but there
is little appetite for augmenting routers with storage. (As we will
see in Section |Overlay|.2, CDNs include a "request routing" function,
which has been proposed as a "content-based addressing" extension to
the Internet's forwarding mechanism, but the success of CDNs in
providing the same service as an overlay renders this issue mute.)

The second, video conferencing, supports a *multicast* function,
whereby packets are delivered to multiple end-points instead of a
single destination. Multicast is a good example of a function that has
tried to find a home at multiple layers of the protocol stack.
Originally, the MBone (multicast backbone) was an overlay that enabled
experimentation with IP multicast routing and forwarding. The goal was
to eventually add multicast support to the Internet's routers (IPv4
addresses in the range ``224.0.0.0`` to ``239.255.255.255`` were
treated as multicast addresses), and while the function was
implemented in commercial routers, it never enjoyed widespread
deployment. Today, an over-the-top variant of multicast supports
familiar video conferencing apps, such as Zoom.

The third is *onion routing*, which is a function that mimics IP
forwarding, with the added feature that it is impossible to trace
where a packet comes from. This allows the sender to remain anonymous.
Onion routing is not in enough demand to incentivize making it a
general part of the core. This is a common situation, meaning that
overlays can server niche markets. In the case of onion routing,
specifically, there is also the issue of trusting commercial providers
to honor the expected anonymity.

.. TODO -- This storyline lost the IPv6 angle. Not sure whether it
   belongs in the VPN paragraph or the the onion routing paragraph.

.. TODO -- With respect to video conferencing, three is a second
   function: timeliness. It would be good to include a forward
   reference to the next chapter.

.. TODO -- Could talk about other ways we can introduce functionality,
   such as programmable networks (and generally, SDN). Active Nets
   could fit here too. Maybe as a sidebar.



