
|Stream|.5 Scaling Real-time Applications
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Back in Chapter |Apps| we observed that the success of DASH to deliver
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
rather than by retransmission. This is how RTP, described in Section
|Stream|.2, typically operates. However, this doesn't tackle the
question of scale. How can a video stream be distributed out to
hundreds of thousands of viewers, as might be required for a live
sporting event, for example?

For a lot of video conference solutions, the answer has been to use a
"mixer" of some sort in a cloud data center. Participants send their
video and audio to the mixer, and a mixed video and audio stream is
sent back out to every recipient. That works reasonably well up to a
point, but it does mean that the same media stream is being sent out
many times from the cloud. For large scale events, a more efficient
replication method would be ideal.

For a while, the standard answer to this problem was multicast at the
IP layer. Replication of packets was handled directly by the routers,
with receivers joining a multicast group to receive the content, and
senders directing their traffic to a multicast IP address. This
technology is probably still running in some enterprise networks today
but largely failed to launch in the broader Internet. The core idea,
however, of efficient replication at intermediate nodes, lives on in
overlay networks such as CDNs. And there is an ongoing effort to
standardize a type of overlay specifically for real-time streams. That
effort is known as "Media Over QUIC" (MOQ) but the name doesn't really
tell you much about all the moving parts. Let's take a look at the
main features of MOQ. 

There is a well-established concept in distributed systems known as
the *publish/subscribe* pattern. Publish/subscribe is a generic
approach to data distribution: data is published to some sort of
channel, and subscribers to the channel receive the data. Notably, the
publishers and subscribers don't need to know about each other; they
only need to know about the channel. This turns out to be a good fit
for large-scale media distribution and it forms the basis of MOQ. We
can contrast it against the client-server model (which is used by DASH
and HTTP), in which the client asks the server for a specific piece of
content. In MOQ, the main abstraction is a *track*, which might, for
example, correspond to a particular live stream encoded at some
resolution. Publishers publish media to the track and subscribers
receive it.

Tracks are made up of *groups*, which in turn are made up of
*objects*. A group is independently decodable, that is, a receiver of
a group can decode it without having received any prior groups from
the track. A group would likely correspond to a group of
pictures (GOP) in a video stream, which includes an I frame and some
number of B and P frames as described in Section |Stream|.1. The fact
that the group can be decoded independently means a subsscriber can
join a channel and immediately decode the
media once a group is received.
Objects are the individual units of data transmitted by MOQ.

As you probably guessed from the name, MOQ uses QUIC as the underlying
transport. This might seem at odds with the desire to minimize
latency, since QUIC, as we discussed in Section |Message|.3, uses
retransmission of lost packets to ensure reliable delivery. MOQ has a
few mechanisms for getting around this. In particular, it uses the
datagram option of QUIC in some cases to disable reliable delivery. It
also makes used of the lightweight streams of QUIC so that different
parts of a media stream can be sent independently of each other. One use of
this is to discard the higher resolution parts of a video if neceesary
to deal with congestion. 

The key to scalability in MOQ is the use of *relays*, a form of
overlay that handles replication of media. Just as we saw in Chapter
|Overlay| how CDNs can scale the delivery of HTTP traffic, MOQ relays
are designed to scale the delivery of MOQ traffic. They are central to
the establishment of the pub/sub model so that subscribers don't need
to know about publishers and *vice versa*.

.. TODO figure on relays

Relays form an overlay of nodes that can store, forward, and replicate
objects. They don't need to have any understanding of the content;
objects are self-contained pieces of data that carry enough metadata
to enable subscribers to determine what groups and tracks they belong
to. The metadata also allows the relay nodes to make decisions about
how to prioritise transmission of objects in the event of congestion.

There is a shared control plane among relays so that the relays in
an overlay can learn about the tracks that are available from other
relays in the network. 

.. TODO walk through a relay example similar to
   https://blog.cloudflare.com/moq/
   
   

