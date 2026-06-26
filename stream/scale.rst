
|Stream|.5 Scaling Real-time Applications
-----------------------------------------

In Chapter |Apps| we observed that the success of DASH to deliver
steaming media at scale had a lot to do with its effective use of HTTP
as the delivery mechanism. The web has developed over the decades to
make HTTP delivery very efficient, particularly through the use of
CDNs to fan out popular content to large number of users. So it is
reasonable to wonder if something similar could be made to work for
real-time communications.

The main shortcoming of DASH for real-time communications is the lack
of control over latency. Running over TCP or QUIC, DASH streams
encounter variable latency, especially if packets are retransmitted to
recover from loss. This latency variation is smoothed out by buffering at the
receiver, so there is no easy way to get the latency down below a few
seconds as would be required for a video conference or other forms of
interactive session.

The traditional approach to real-time applications is to run them over
UDP rather than TCP and deal with packet losses in the application
rather than by retransmission. As we say in Section |Stream|.3, this
is how RTP typically operates. However, this doesn't tackle the
question of scale. How can a video stream be distributed out to
hundreds of thousands of viewers, as might be required for a live
sporting event, for example?

For a lot of video conference solutions, the answer has been to use a
"mixer" of some sort in a cloud data center. Participants send their
video and audio to the mixer, and a mixed video and audio stream is
sent back out to every recipient. That works reasonably well up to a
point, but it does mean that the same media stream is being sent out
many times from the cloud. Any single cloud site has significant
capacity, but for truly large scale events, a more efficient
replication method is required.

For several years, the standard answer to this problem was multicast
at the IP layer. Replication of packets was handled directly by the
routers, with receivers joining a multicast group to receive the
content, and senders directing their traffic to a multicast IP
address. This technology is probably still running in some enterprise
networks today, but multicast IP largely failed to launch in the
broader Internet. The core idea, however, of efficient replication at
intermediate nodes, lives on in overlay networks such as CDNs. And
there is an now an effort to standardize an analogous type of overlay
specifically for real-time streams. That effort is known as "Media
Over QUIC" (MoQ) but the name doesn't really tell you much about all
the moving parts. Let's take a look at the main features of MoQ.

There is a well-established concept in distributed systems known as
the *publish/subscribe* pattern. Publish/subscribe is a generic
approach to data distribution: data is published onto an abstract
channel, and subscribers to the channel receive the data. Notably, the
publishers and subscribers don't need to know about each other; they
only need to know about the channel. This turns out to be a good fit
for large-scale media distribution and it forms the basis of MoQ. We
can contrast it against the client-server model (which is used by DASH
and HTTP), in which the client asks the server for a specific piece of
content. In MoQ, the main abstraction is a *track*, which might, for
example, correspond to a particular live stream encoded at some
resolution. Publishers publish media to the track and subscribers
receive it.

Tracks are made up of *groups*, which in turn are made up of
*objects*. A group is independently decodable, that is, a receiver of
a group can decode it without having received any prior groups from
the track. A group would likely correspond to a group of pictures
(GOP) in a video stream, which includes an I frame and some number of
B and P frames as described in Section |Stream|.2. The fact that the
group can be decoded independently means a subscriber can join a
channel and immediately decode the media once a group is received.
Objects are the individual units of data transmitted by MoQ.

As its name implies, MoQ uses QUIC as the underlying transport. This
might seem at odds with the desire to minimize latency, since QUIC, as
we discussed in Section |Message|.3, uses retransmission of lost
packets to ensure reliable delivery. MoQ takes advantage of some of
the features of QUIC to get around this. In particular, recall that
the lightweight streams of QUIC allow for independent components of a
QUIC connection to proceed without being blocked by each other. So MoQ
uses streams to ensure that different parts of a media stream can be
sent independently of each other. One use of this is to discard the
higher resolution parts of a video if necessary to deal with
congestion. It is also possible to just terminate a stream if, for
example, the observed level of congestion suggests that sending this
part of the media would only exacerbate congestion, or the information
in the stream would arrive too late to be useful.  MoQ objects have an
associated *priority* that can be used to make decisions about which
streams to transmit and which to terminate.

The lightweight streams of QUIC operate at
an intermediate point in the design space between TCP and
UDP. Reliable delivery can be applied selectively to individual
streams, and congestion control can limit the total amount of traffic
in a QUIC connection when required. Lower priority streams can be
sacrificed to ensure that latency remains within bounds and more important
packets are not stuck waiting for less important ones to arrive.

The key to scalability in MoQ is the use of *relays*, a form of
overlay that handles replication of media. Just as we saw in Chapter
|Overlay| how CDNs can scale the delivery of HTTP traffic, MoQ relays
are designed to scale the delivery of MoQ traffic. They are central to
the establishment of the pub/sub model so that subscribers don't need
to know about publishers and *vice versa*.


.. _fig-relays:
.. figure:: stream/figures/relays.png
   :width: 500px
   :align: center

   Relays form an overlay for media distribution.

Relays form an overlay of nodes that can store, forward, and replicate
objects, as shown in :numref:`Figure %s <fig-relays>`. They don't need
to have any understanding of the content; objects are self-contained
pieces of data that carry enough metadata, which is visible to relays,
to enable them to be handled appropriately.  The metadata includes the
priority mentioned above, which allows the relay nodes to make
decisions about how to prioritize transmission of objects in the event
of congestion or other resource shortage. Metadata also enables
subscribers to determine what group and track an object belongs to.

There is a shared control plane among relays so that the relays in an
overlay can learn about the tracks that are available from other
relays in the network. :numref:`Figure %s <fig-relays>` shows a simple
example. A publisher announces the availability of some track to its
nearest relay node, Relay A. Subscriber 1 sends a subscription request
for this track to its nearest relay node, Relay B. Relay B forwards
the request to Relay A, which forwards it to the publisher, who now
starts publishing the data, sending it to Relay A. (There are a few
other possible sequences of message flows; this is just one example.)
Relay A passes on the data to Relay B, and Relay B sends it to the
subscriber. If Subscriber 2 later sends a subscription request to
Relay B, Relay B can immediately start sending data to Subscriber 2
without any additional flow of messages or content across the
backbone.

MoQ also embodies the idea of Application Layer Framing (ALF) that we
noted in Section |Stream|.3. Only the application knows what data it
needs to do its job, but it can frame the data in such a way that the
network can help the application function. MoQ lets the application
define objects that represent units of video that can be independently
decoded, and prioritize those objects based on their importance (e.g.,
lower priority for high frequency components of the video). The relay
nodes don’t have to understand the application at all—they just use
the metadata to relay objects in a way that benefits the
application. This makes MoQ relays more useful to the application than
a simple packet replication service such as IP multicast or the SFU
overlays we saw in Section |Overlay|.3.


The preceding discussion captures MoQ at a particular point in time,
since the specifications are still evolving and implementations are
trying to track the specifications. Nevertheless, this approach draws
on the success of existing technologies such as QUIC and DASH and has
the weight of major industry players behind it, so there is reason to
be optimistic for its future. The implementation of the relay function
in a large-scale deployment at Cloudflare is one example of the
industry investment in MoQ; you can read more about MoQ and its
implementation in their blog on the subject.

.. admonition:: Further Reading

   M. English and R. Dincer. `MoQ: Refactoring the Internet's
      real-time media stack
      <https://blog.cloudflare.com/moq/>`__. Cloudflare blog, August 2025.
