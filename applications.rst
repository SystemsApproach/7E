.. SPDX-FileCopyrightText: 2019 Systems Approach LLC
.. SPDX-FileCopyrightText: 2025 Systems Approach LLC
.. SPDX-License-Identifier: CC-BY-4.0

Chapter 2: Network Applications
==============================================

The success of the Internet can largely be attributed to the broad
range of applications it supports. Unlike earlier networks that
targeted specific use cases—most notably the telephone network’s
support for voice and the cable TV network’s support for disseminating
video—the Internet was designed to be application agnostic. It’s able
to deliver packets carrying keystrokes, voice, video, images,
financial transactions, and any other kind of information that can be
digitized. That generality is one of the driving motivations for the
design described in Chapter 1, and as we now know, that generality is
what allowed the Internet to grow to subsume those earlier
purpose-built networks.

Of course you still need to implement the applications, but unlike
earlier networks that assumed specialized edge devices—e.g.,
telephones or televisions—the Internet was designed to exploit
general-purpose computing at the edge, with the applications
implemented in software. This makes it possible for innovators to move
fast. But being software-based is not enough in and of itself. The
other half of the equation is to have access to a rich ecosystem of
existing software that you can build upon. Each application developer
should not have to start from scratch. To understand this ecosystem,
it is helpful to start with the applications themselves. That is the
goal of this chapter.

Not surprisingly, of all the applications that run on the Internet,
the World Wide Web—and more specifically, the HTTP standard that
defines it—is the place to start. This is for two reasons. One is
obvious: the web is the dominant application running on today’s
Internet. The second reason is less obvious but more profound: the web
has changed how we build applications; it has become more a framework
than an application. Related to this second point, this chapter also
describes two other applications: Email and Adaptive Streaming. Email
came before the web, and so illustrates the “old” (pre-HTTP) way to
build applications. Adaptive streaming, which underpins video
streaming services like Netflix, came after
HTTP, and so illustrates the “new” (post-HTTP) way to build
applications. But before getting to these examples, we first describe
the basic anatomy of an application.

.. include:: applications/anatomy.rst
.. include:: applications/web.rst
.. include:: applications/email.rst
