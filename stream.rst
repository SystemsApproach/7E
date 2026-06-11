.. SPDX-FileCopyrightText: 2019 Systems Approach LLC
.. SPDX-FileCopyrightText: 2025 Systems Approach LLC
.. SPDX-License-Identifier: CC-BY-4.0

.. include:: chapters.rst

Chapter |Stream|: Real-Time Streaming
=====================================

Audio and video have been streamed across the Internet for several decades. In
the early days there were concerns that specialized queueing and
resource reservations might be required to support these
latency-sensitive applications, but dramatically increasing
bandwidths of access links and the Internet's core has led to a state
where video now makes up a majority of the bytes transmitted over the
Internet. We touched on one technology that has enabled the rise of
video in Chapter |Apps|: DASH (dynamic adaptive streaming over
HTTP). However, DASH is only part of the story, since it works around
the issue of latency with a startup delay and substantial buffering at
the client. For use cases with tighter latency constraints, such as
video conferencing and live events, we need a few other tools to
manage the delivery of latency-sensitive media. This chapter examines
the additional challenges in delivering real-time media and some of
the technologies and protocols that have been developed to address
them.

Whether the media has real-time constraints or not, the first
challenge is to encode it in a way that facilitates its transmission
over packet networks. Video and audio coding have a rich history and
it is only because of advances in this field that today's networks are
able to carry high resolution video. So we begin our discussion with a
brief overview of the techniques used to encode and compress audio and
video streams prior to transmission.

Once we have encoded our media, we need to transport it over the
network, and here we find that the reliable delivery model of TCP,
which historically has been used by most Internet applications, isn't
a good fit when we have tight latency constraints. The real-time
transport protocol RTP is one alternative approach that has been
widely adopted.

There is another problem faced by real-time applications, which is
that the client-server model that is so common on the Web is a poor
fit to many applications that are peer-to-peer, such as phone
calls. This leads to the challenge known as session control.

Finally, as with everything on the Internet, if an application is
successful, we have to think about scaling issues. While DASH has been
effective at leveraging CDNs, a different approach is emerging for
real-time streaming. Our chapter concludes with a look at the emerging
standards and implementations for scaling real-time streams.

.. include:: stream/design.rst
.. include:: stream/coding.rst
.. include:: stream/transport.rst
.. include:: stream/session.rst
.. include:: stream/scale.rst
