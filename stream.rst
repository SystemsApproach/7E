.. SPDX-FileCopyrightText: 2026 Larry L. Peterson and Bruce S. Davie
.. SPDX-License-Identifier: CC-BY-4.0

.. include:: chapters.rst

Chapter |Stream|: Real-Time Streaming
=====================================

Audio and video have been streamed across the Internet for several
decades. In the early days there were concerns that specialized
queuing and resource reservations might be required to support these
latency-sensitive applications, but dramatically increasing bandwidths
of access links and the Internet's core has led to a state where video
now makes up a majority of the bytes transmitted over an Internet that
remains best-effort.

Streaming implies a rather broad class of applications, each with
slightly different requirements. For example, early success stories
had very loose latency requirements. If you wanted to watch a movie or
listen to a song, it was acceptable to wait a few seconds before the
stream started. This meant that video streaming services like Netflix
and YouTube, as well as music streaming services like Pandora and
Spotify, could use substantial buffering at the client to hide any
delays in packet delivery. This led to the development of adaptive
mechanisms, with the DASH protocol described in Chapter |Apps|
being a prime example.

But DASH doesn't work for every scenario. For use cases with tighter
latency constraints, such as video conferencing and live events, we
need additional tools to manage the delivery of latency-sensitive
media. This is why we include the "real-time" qualifier, although even
then, there is a spectrum of delay tolerances: a one-second delay when
trying to reply to a speaker in a video conference call is much more
disruptive than a few seconds of delay at the start of a live sporting
event.

This chapter examines the additional challenges in delivering
real-time media and some of the technologies and protocols that have
been developed to address them.  Of particular note, the fact that we
are delivering *media* (i.e., video and audio), and not some other
type of data, plays a role in how we make sure delivery is timely. How
we represent this data, and how much we compress it so it consumes
fewer resources, is necessarily part of the story.

.. admonition:: Further Reading

   B Braden, D. Clark, and S. Shenker. `Integrated Services in the
   Internet Architecture: An Overview
   <https://www.rfc-editor.org/info/rfc1633>`__. RFC 1633,
   June 1994.

   B Braden (Ed.). `Resource ReSerVation Protocol (RSVP) --
   Version 1 Functional Specification Internet Architecture: An
   Overview <https://www.rfc-editor.org/info/rfc2205>`__. RFC 2205,
   September 1997.

.. sidebar:: QoS Guarantees

   *The idea of supporting Quality-of-Service (QoS) guarantees has come
   up multiple times throughout this book, but with the exception of
   QoS support being a proclaimed feature of the Mobile Cellular
   Network in Section 5.4, we are yet to describe how one goes about
   reserving network bandwidth. Now that we're finally focused on
   real-time streaming, you would think the answer is imminent, but as
   the introduction to this chapter indicates, that is not the case.*

   *This is not for a lack of trying. Introducing reservations and QoS
   guarantees into the Internet was a significant focus of both
   academic research and protocol standardization throughout the
   1990s, resulting in the* **Integrated Services (IntServ)**
   *architecture, so called because it supported both best-effort
   traffic and reservations (see RFC 1633). As an architecture,
   IntServ included several moving parts. Fair Queuing packet
   schedulers (see Section 8.2) was one necessary component. A way to
   specify the reservation you wanted—codified in RSVP , the Resource
   ReSerVation Protocol—was another component.*

   *The reason RSVP and IntServ never took off is because it never
   became necessary to support the applications people cared
   about. Being able to make hard guarantees is a requirement only
   when (a) resources are scare, and (b) people are willing to pay a
   premium to guarantee a certain allocation. So far, bandwidth
   improvements have kept both requirements at bay.*

.. include:: stream/design.rst
.. include:: stream/coding.rst
.. include:: stream/transport.rst
.. include:: stream/session.rst
.. include:: stream/scale.rst
