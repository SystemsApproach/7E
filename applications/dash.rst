2.4 Adaptive Streaming
-------------------------------


Efforts to stream video and audio data across the Internet go back at
least to the 1990s, with the MBONE (multicast backbone) being an
overlay network that was used to experiment with both multicast
routing and real-time streaming. Today video is one of the dominant
application classes on the Internet, making up a substantial fraction
of the total traffic over the network.

Before video can be streamed across the Internet, it needs to be
encoded, and video coding is another field with a long history. To
make efficient use of bandwidth and storage, video is usually
compressed, and organizations such as MPEG (the
Moving Picture Expert Group) produce standards that allow for
interoperability among devices that encode, store, and play video content.

.. We might package our MPEG section as a standalone supplement.

Most video coding schemes allow for a trade-off between the bandwidth
consumed and the quality of the images. Since the bandwidth that is
available on a network path typically varies over time, it is
desirable to pick a set of coding parameters that provides sufficient
quality without exceeding the available network bandwidth. There is
also no point delivering higher quality than an end system can
support. This tradeoff applies whether we are talking about real-time
video (e.g., video conferencing) or streaming of recorded content as
with video streaming services such as Netflix.  In this section, we
focus on the latter, which provides an interesting case
study in how the Web has come to dominate application design, even for
applications that didn't initially seem well suited to HTTP.

Early efforts to transmit video over HTTP recognized that HTTP was
becoming a ubiquitously supported protocol that could generally pass
through firewalls without incident and was increasingly supported by
caching infrastructure and CDNs. Beyond the simple idea that video is
just another sort of digital content that can be retrieved using HTTP,
two main innovations underpin the use of HTTP for video streaming:

 - The ability to request segments of a video (e.g. the first ten
   seconds, next ten seconds, etc.) rather than an entire
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

The same MPEG organization that specified the video format also
defined a standard way to represent this information. The standard,
called MPEG DASH (Dynamic Adaptive Streaming over HTTP), is necessary
so that a variety of clients (video players) can interoperate with
different backend video servers. DASH specifies a hierarchical
*manifest* for each video—officially known as an MPD (Media
Presentation Description) file. An MPD manifest is represented as an
XML document, where the following shows a truncated example:

.. code-block:: xml

   <MPD xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
           xmlns="urn:mpeg:dash:schema:mpd:2011"
           xsi:schemaLocation="urn:mpeg:dash:schema:mpd:2011"
           type="dynamic"
           minimumUpdatePeriod="PT2S"
           timeShiftBufferDepth="PT30M"
           availabilityStartTime="2011-12-25T12:30:00"
           minBufferTime="PT4S"
           profiles="urn:mpeg:dash:profile:isoff-live:2011">
      <BaseURL>http://server1.example.com/</BaseURL>
      <BaseURL>http://server2.example.com/</BaseURL>
      <Period id="1">
         <AdaptationSet contentType="video"
                 segmentAlignment="true"
                 bitstreamSwitching="true"
                 frameRate="24">
            <Representation id="0"
                    mimeType="video/mp4"
                    codecs="avc1.4d0020"
                    bandwidth="2000000"
                    width="1280"
                    height="960"
                    frameRate="24">
               <SegmentList duration="10">
                  <SegmentURL media="seg-1.mp4"/>
                  <SegmentURL media="seg-2.mp4"/>
                  <SegmentURL media="seg-3.mp4"/>
                  ...
               </SegmentList>
            </Representation>
            ...
         </AdaptationSet>
         ...
      </Period>
      ...
   </MPD>

As the snippet of XML shows, a manifest identifies a nested set of
elements: each video is broken into a set of *Periods*; each Period
contains a set of *Adaptation Sets*; an Adaptation Set contains a set
of *Representations* ; and finally, each Representation specifies a
set of *Segments* (each of which is an MPEG-compressed chunk of
video). If this seems like more layers than our "N × M chunks"
overview suggests, it is, but this corresponds a real-world
application of that idea.  For example, Periods typically last many
seconds (e.g., on the order of a minute), and define the boundary at
which the player is allowed to shift from one bandwidth to another.
Representations correspond to a given bandwidth, with the example
encoded at 2 Mbps. The Adaptation Set supports other degrees of
freedom, including the choice of audio streams (e.g., language, Dolby
mode).

Although not shown in the example manifest, DASH supports "trick play"
(skipping forward and backward in a video), and even an opportunity to
interject ads (between Periods). There is also support for templating,
so that full URLs need not be given for every segment.  (The example
does not include any templating directives, but it does illustrate how
a full URL is constructed from the ``BaseURL`` and the segment name.)
Finally, to allow for the possibility that the schema of the MPD file
may change over time, a URN (``urn:mpeg:dash:schema:mpd:2011``)
identifies the schema being used by this manifest. In other words,
standards often require a surprising amount of detail, in many cases
to accommodate the all the options different constituencies
require. That's the main lesson you should take away from this example, but
for a more complete introduction to DASH, we recommend Iraj Sodagar's
paper:

.. _reading_dash:
.. admonition:: Further Reading

   I. Sodagar. `The MPEG-DASH Standard for Multimedia Streaming Over
   the Internet
   <https://ieeexplore.ieee.org/document/6077864>`__. *IEEE
   MultiMedia, vol. 18, no. 4*, April 2011.

With this peek into the bookkeeping challenge of HTTP-based streaming,
let's pop back up to the big picture. To view a video, a player first
downloads its ``.mpd`` file, parses it, and then starts issuing HTTP
GET requests for a sequence of ``.mp4`` files, decoding and displaying
each at a smooth video frame rate But what is the right bandwidth to
select? For simplicity, let's first assume we have some way to measure
the amount of free capacity along a
path.  For example, a client can simply measure the rate at which data
has been arriving over some recent time interval. We can infer that
there is at least that much bandwidth available. If the quality that
is currently being delivered is acceptable, e.g. because it matches
the highest resolution of the display at the client device, then there
is no need to do anything. But if the client could take advantage of
higher quality, it can request that the next segment of video be sent
at that higher quality. The total amount of data being delivered per
second should now increase if there is no congestion. If it doesn't
increase as much as we expect, we might conclude that there is
congestion and so the client could now go back to asking for a lower
quality of video.

The situation is a bit more complicated than what we just described,
however, because the underlying transport protocol below HTTP is
TCP (or its modern sibling QUIC). TCP and QUIC have
congestion control mechanisms that try to match the rate at which data
is sent across the network to the available capacity. If congestion is
detected, TCP reduces its sending rate; in the absence of congestion,
it periodically tries to increase its sending rate to take advantage
of the available capacity. We'll explore these mechanisms in much more
detail in Chapter 14. For now, we can just accept that TCP is going to
deliver packets at a rate that varies over time depending on the available
network capacity.

TCP also provides reliable delivery of packets. That in itself is not
a bad thing, but reliability is achieved by retransmitting packets
that have been dropped in transit. Retransmission adds to the time
taken for a packet to arrive, since it takes time both to detect the
loss and to retransmit the lost packet. All of this means that the
rate at which packets arrive is inherently variable and that some
packets will take longer to arrive than others. Video frames, however,
need to be played out at a smooth rate, and so the client should not
find itself waiting for the next piece of data to arrive when it is
time to display the next video frame.  To handle the variability of
the rate at which packets arrive at the client, it is therefore
necessary for the client to maintain a local buffer of received
packets to smooth out the variability. This buffer, depicted in
:numref:`Figure %s <fig-buffer>`, plays an important role in adaptive
streaming as described below.

.. _fig-buffer:
.. figure:: applications/figures/buffer.png
   :width: 550px
   :align: center

   Playback buffer smooths out packet arrivals and provides
   information the player uses to adjust the video rate it requests.
   When the buffer runs too low, the player requests a lower rate.

To start playing a video, the client requests the first chunk of
video at some default quality, and then waits for packets to arrive. It
buffers a few packets in local storage and then starts to decode them
and play the video. A large buffer implies a longer start-up delay,
but also gives more insurance against the variable arrival rate of
packets. So tuning the initial size of the buffer is a tradeoff that system
designers can make based on expected network conditions.

Once a few packets have arrived, we now have a queue that is being filled with
packets as they arrive across the network, and is being
emptied of packets as they are decoded and played out on the screen.
The rate at which the queue is emptied is dependent on the quality of
the video: higher quality means more bits per second need to be
available for playback. The rate at which the queue fills up is
determined by the rate at which packets are arriving across the
network.

The client now uses a local algorithm to decide what quality it should request
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

As a reminder, this approach would not make sense for
real-time video such as video conferencing. A video or audio
conference depends on quite low latency interaction between
participants: you need to receive the data within a few hundred
milliseconds of it being sent or you will be unable to have a
conversation. Even at 300ms the delay becomes noticeable and
annoying. So this demands a different approach that doesn't rely on
TCP for congestion control and reliable delivery. We will
examine this problem space in Chapter 16.

Finally, it is worth noting that video streaming involves a complex
interplay between the application protocol and the underlying
transport protocol. They are, in effect, trying to co-mange the rate
at which packets are delivered. As an application developer, you would
undoubtedly prefer that the socket API for TCP told you everything you
need to know about TCP's behavior, but that's just not the case. The
API says nothing about timeliness, which in the Internet can be highly
variable, largely due to congestion. This leaves the application
developer with only one option: you need to first develop an
understanding of how the underlying network behaves.
