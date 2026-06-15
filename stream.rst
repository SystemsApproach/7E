.. SPDX-FileCopyrightText: 2019 Systems Approach LLC
.. SPDX-FileCopyrightText: 2025 Systems Approach LLC
.. SPDX-License-Identifier: CC-BY-4.0

.. include:: chapters.rst

Chapter |Stream|: Real-Time Streaming
=====================================

Audio and video have been streamed across the Internet for several
decades. In the early days there were concerns that specialized
queuing and resource reservations might be required to support these
latency-sensitive applications, but dramatically increasing bandwidths
of access links and the Internet's core has led to a state where video
now makes up a majority of the bytes transmitted over the Internet.

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
disruptive than a few seconds of delay when watching a live sporting
event.

This chapter examines the additional challenges in delivering
real-time media and some of the technologies and protocols that have
been developed to address them.  Of particular note, the fact that we
are delivering *media* (i.e., video and audio), and not some other
type of data, plays a role in how we make sure delivery is timely. How
we represent this data, and how much we compress it so it consumes
fewer resources, is necessarily part of the story.

.. include:: stream/design.rst
.. include:: stream/coding.rst
.. include:: stream/transport.rst
.. include:: stream/session.rst
.. include:: stream/scale.rst
