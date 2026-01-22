2.4 Adaptive Streaming (DASH)
-------------------------------


Efforts to stream video and audio data across the Internet go back at
least to the 1990s, with the MBONE (multicast backbone) being an
overlay network that was used to experiment with both multicast
routing and real-time streaming.

Before video can be streamed across the Internet, it needs to be
encoded, and video coding schemes such as those developed by MPEG (the
Moving Picture Expert Group) allow for a
trade-off between the bandwidth consumed and the quality of the
image. Since the bandwidth that is available on a network path
typically varies over time, it is generally desirable to pick a set of
coding parameters that provides sufficient quality without exceeding
the available network bandwidth. There is also no point delivering
higher quality that an end system can support. This tradeoff applies whether we are
talking about real-time video (e.g., video conferencing) or streaming
of recorded content as with video streaming services such as Netflix.
In this section, we are going to focus on the latter, which provides
an interesting case study in how the Web has come to dominate
application design, even for applications that didn't initially seem
well suited to HTTP.

Early efforts to transmit video over HTTP recognized that HTTP was
becoming a ubiquitously supported protocol that could generally pass
through firewalls without incident and was increasingly supported by
caching infrastructure and CDNs. Beyond the simple idea that video is
just another sort of digital content that can be retrieved using HTTP,
two main innovations underpin the use of HTTP for video streaming:

 - The ability to request segments of a video (e.g. the first ten
   seconds, next  ten seconds, etc.) rather than an entire
   video file;

 - The idea of storing the video at multiple different quality levels,
   so that a client could request a different quality for each segment
   depending on its assessment of network conditions and its own
   ability to render different quality levels.

In effect, the receiver never asks the sender to stream the whole
movie, but instead it requests a sequence of short movie segments,
typically a few seconds long. Each segment is an opportunity to change
the quality level to match what the network is able to deliver.  In
other words, a movie is typically stored as a set of N × M chunks
(files): N quality levels for each of M segments.

To understand how this works, let's first assume that we have some way to measure
the amount of free capacity and level of congestion along a path.  For
example, a client can simply measure the rate at which data has been
arriving over some recent time interval. We can infer that there is at
least that much bandwidth available. If the quality that is currently
being delivered is acceptable, e.g. because it matches the highest
resolution of the display at the client device, then there is no need
to do anything. But if the client could take advantage of higher
quality, it can request that the next segment of video be sent at that
higher quality. The total amount of data being delivered per second
should now increase if there is no congestion. If it doesn't increase
as much as we expect, we might conclude that there is congestion and
so the client could now go back to asking for a lower quality of
video.

The situation is a bit more complicated than what we just described,
however, because the underlying transport protocol below HTTP is
either TCP or its modern sibling QUIC. Both these protocols have
congestion control mechanisms that try to match the rate at which data
is sent across the network to the available capacity. If congestion is
detected, TCP reduces its sending rate; in the absence of congestion,
it periodically tries to increase its sending rate to take advantage
of the available capacity. We'll explore these mechanisms in much more
detail in Chapter 14. For now, we can just accept that TCP is going to
deliver packets at a rate that varies over time depending on the available
network capacity.


What the client does in this case is to request the first chunk of
video at some default quality, and then wait for packets to arrive. It
buffers a few packets in local storage and then starts to decode them
and play the video. We now have a queue that is being filled with
packets at some rate as they arrive across the network, and is being
emptied at some rate as they are decoded and played out on the screen.
The rate at which the queue is emptied is dependent on the quality of
the video: higher quality means more bits per second need to be
available for playback. The rate at which the queue fill us is
determined by the rate at which packets are arriving across the
network.

The client now uses a local algorithm to decide what it should request
for each chunk of video. If the local buffer is being drained faster
than it is being filled, at some point the video playback would stall,
which looks bad to the end user. So the client tries to avoid this. If
the queue is starting to drain and there is a risk that it will become
empty, the client can request a lower quality of video, in the hope
that the reduced bandwidth can now be met by the network, and the
queue will start to fill again. If the queue is getting more full over
time, that can be a sign that a higher quality can be requested.

There is a lot of work that goes into tuning these algorithms to make
the appropriate tradeoff between perceived quality and bandwidth. For
example, constantly switching between two quality levels is likely to
be more annoying to the user than settling on a single lesser
quality. But ideally the quality chosen should be roughly as high as
the network can support subject to the limits of what the client can
display.

Another tradeoff is in how many quality levels to store on the
server. More levels means more storage, but also gives the client more
options for matching the quality to the available bandwidth and
different display sizes, from phones to large TV screens.

This general approach is called HTTP adaptive streaming, and the
predominant standard is MPEG DASH (Dynamic Adaptive Streaming over
HTTP). The standard ensures that clients and servers interoperate,
while allowing for innovation in the client algorithms to switch among
quality levels and also permitting the providers of streaming content
to make their own decisions about how many different quality levels to
support on their servers.
