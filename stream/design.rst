|Stream|.1 Design Issues
-----------------------------------------------
The idea of sending real-time media over packet networks goes back at
least to the 1980s, when researchers at Bellcore built an
``Etherphone'' that transmitted a 64kbps audio stream, captured using
something that looked exactly like a 1980s telephone handset, over an
Ethernet. Since the backbone of the Internet in those days was only
56kbps, it would be a few more years before this technology found its
way to the Internet. And only in the late 1990s did the bandwidths
available across the Internet start to make "voice over IP" feasible.

Today, of course, audio and video make up a large fraction of the
Internet's traffic. But there is more to the story than just Internet
link bandwidths catching up with the demands of multimedia
applications.

First of all, the coding and compression of audio and video content
presents an interesting study in how humans process information. There
is a *lot* of information in high-resolution video, for example, but a
much of it is either redundant or of limited value when perceived by a
human eye. An example of redundant content is the unchanging
background of a video. If we can send the background once and then say
"don't make any changes to the background" that requires a lot less
bandwidth than sending the same background image at 30 frames per second. The
hard part is algorithmically determining which part of the image isn't
changing, and that is one thing that modern video coding does.

Similarly, images contain both high and low spatial frequency
information, and the human eye is more sensitive to the lower
frequency information. Thus, we can allocate more bandwidth to
transmitting the information that has more value to the human eye.

Compression of video and audio, therefore, depends on an understanding
of human perception, and there have been decades of research on how to
optimize compression while maintaining high perceived quality. Compression
also has to take account of the fact that networks will occasionally
lose data; this means that you can't afford to completely remove
redundant information, or one lost packet might render the entirety of
a video useless to the recipient. 

When transporting real-time media across networks, it soon becomes
apparent that the reliable byte-stream model of TCP isn't suitable
because of the potentially unbounded delay of waiting for the
retransmission of lost packets. A single packet loss is often
preferable to a long delay. While this might lead one to assume that
UDP would be a better choice, that leaves a lot of work up to the
application. A simple example is conveying timing information from one
participant to another so that audio and video tracks can be
synchronized. It could be done by the application, but it is a common
enough problem that it's helpful to have a transport protocol that
takes care of this task. Identifying those useful common tasks and
building them into a real-time transport led to the development of RTP.

Since many real-time applications involve two or more humans conversing with
each other, many such applications are a poor fit to the client server
model of communication, in which the client makes a request to a
well-known server. The problem of how to bring participants together
in a *session* and how to communicate all the information (such as the chosen
coding standards) among participants is known as *session
control*. Solving these issues has give rise to a set of protocols
designed specifically for applications connecting sets of participants
rather than clients making requests of servers.

With real-time applications now commonplace on the Internet, you might
think that there are no further problems to solve. However, it turns
out that large-scale, real-time communications face something of a
scaling issue. While most of us have been on large video conferences
that seem to work well, the solution normally depends on some
high-capactiy cloud service to send the video out to the
participants. The problem of how to distribute real-time media to
large audiences in a scalable manner is gaining renewed attention and
we conclude the chapter by looking at an approach which aims to be as
scalable as today's delivery of entertainment video using DASH.

One final design question was how to ensure that latency-sensitive
traffic for applications like VOIP and video conferences could make it
across the network without encountering excessive queueing delays. We
discussed in Chapter |Capacity| a number of approaches to queueing
that have been developed so that some traffic can receiver lower delay
even when the overall load is high. However, for much of today's
Internet, those mechanisms are not deployed; the solution has simply
been to deploy enough capacity that delay is mostly acceptably low.
