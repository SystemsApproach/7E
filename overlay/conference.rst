|Overlay|.3 Teleconferencing
-----------------------------------------------

In the first edition of this book we described a multiparty video
conferencing application called NV (network video) which was probably
unfamiliar to most readers of a book published in 1996. NV ran on top
of an overlay network, the MBone, which provided the multicast
replication capability to make distribution of video among many
participants efficient. Of course, video conferencing applications
have been mainstream now for many years, so we no longer need to
explain the concept to our readers. And while there have been a lot of
technological improvements in video conferencing, they still rely on
overlays today to achieve scalable operation. The biggest change in
terms of the overlay is that rather than being a generic IP multicast
overlay, each video conferencing application has its own
application-specific overlay.

An overlay is an optimization in the sense that you can run a video
conferencing application without one. Indeed, if there are only two
participants in the conference, no optimization is needed. Each
participant can just send video and audio streams to the other
participant directly. There are still some issues to be solved,
including what to do if one or both participants is behind a firewall
or NAT, as we discussed in Section |Virt|.3.3. The broader issues that
multimedia applications face are the topic of Chapter |Stream|. But
optimizing the delivery of media to a large
number of conference participants is a job usually performed by an
overlay.

Consider the simple case of a 3-party video conference. You can
obviously treat this as 3 pair-wise connections: each participant
sends their video and audio streams to the other two. But every time
you add another participant to such a conference, each participant
needs to transmit another copy of the video. At some point, a
participant is likely to run out of link bandwidth to send all the
required video streams to all participants. In the absence of
multicast at the IP layer, some way to replicate the traffic upstream
from the participants is needed, and this is the role normally performed by an
application-specific overlay.

.. _fig-sfu:
.. figure:: overlay/figures/sfu.png
   :width: 600px
   :align: center

   Media Distribution Using Selective Forwarding Unit.

The typical solution to media replication is for each participant to
send their media to a replication device, which is likely to be
located in some cloud datacenter and operated by the company running
the conferencing service. One class of replication devices are
known as "selective forwarding units" (SFUs). In :numref:`Figure %s
<fig-sfu>` we've shown the very simple case where there is a single
SFU. All participants send their media to the SFU, and it sends a
set of media streams out to all the participants. The "selective" part
means that the SFU does not necessarily send every stream to every
participant. For example, a client on a low bandwidth link might
request the video from the active speaker only rather than from all
participants, while other clients could receive all streams and render
them appropriately on their screens. SFUs also forward media streams rather
than modifying them.

There is an alternative approach, in which a *Multipoint Control Unit*
(MCU) combines the incoming streams into a single outgoing stream,
such as by tiling four videos into a 2x2 grid. This is more
resource-intensive since it involves decoding and re-encoding the videos.

Of course, a single replication device is both a bottleneck from the
perspective of scaling, and a potential single point of failure. So
what we typically see in practice is a complete overlay network of
SFUs, with some SFUs capable of picking up the load from others in the
event of a failure, and SFUs arranged in a mesh so that replication
can be distributed amount the nodes in the mesh rather than all the
work falling on a single node. A simple example of this using three
SFUs in a tree structure is shown in :numref:`Figure %s
<fig-sfu-mesh>`.

.. _fig-sfu-mesh:
.. figure:: overlay/figures/sfu-mesh.png
   :width: 600px
   :align: center

   A mesh of SFUs provide scale and resilience.

As you can see, what we now have is an application-specific
overlay of SFUs. These devices create a virtual topology on top of the
topology of the Internet, with application-specific functionality—the
ability to forward media streams to multiple destinations—built into
the overlay. This is how most video conferencing solutions on today's
Internet work, using overlays to scale the performance of the
application without any assistance beyond basic packet transport from
the core of the Internet.

.. sidebar:: IP Multicast: A Case Study

    There's an opportunity to say a bit more about IP multicast,
    including what made it hard: the routing protocol, which provided
    a means to join a multicast group. Then link 6E section for more
    info. Also note where it is used today: last mile into the home
    (for live streams).
