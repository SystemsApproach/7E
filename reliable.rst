.. SPDX-FileCopyrightText: 2019 Systems Approach LLC
.. SPDX-FileCopyrightText: 2025 Systems Approach LLC
.. SPDX-License-Identifier: CC-BY-4.0

.. include:: chapters.rst

.. _artifact-tcp:

Chapter |TCP|:  Reliable Byte Stream
====================================

In Part II we described how to build a best-effort packet delivery service,
but best-effort implies any number of things can go wrong.  Packets
may be dropped, reordered, duplicated, and/or delayed for an arbitrary
amount of time. In addition to these failure modes, packets are
limited to some finite size, typically on the order of a few thousand
bytes.

At the same time, applications like the ones we looked at in Chapter |Apps|
expect a higher level of service from the network. This often includes
requirements like the following: at most one copy of each message is
delivered; a sequence of messages should be delivered in the same
order they are sent; and there should be no limit on the size of
messages. Less obviously, many applications expect support for some
form of *end-to-end* synchronization between the sending and the
receiving processes. This might involve something as simple a blocking
the sender if the receiver is not able to keep up with incoming data,
or as sophisticated as blocking the sender until the receiver has
responded with a reply.

This chapter looks at the Internet's most widely used transport
protocol, TCP, and how it is designed to "fill the gap" between what
the network delivers and what applications require. TCP provides a
*reliable byte stream* abstraction as its answer to how to best fill
this gap. The byte-stream abstraction is easy to conceptualize: the
sender opens a pipe to the receiver, the sender writes a sequence of
bytes to the pipe, and the receiver reads that exact sequence of bytes
from the pipe.

Later chapters describe other approaches—offering different
inter-process communication abstractions—but supporting a reliable
byte stream between two communicating processes has proven to be an
extremely versatile strategy that serves a wide range of applications.
Email and the web are two examples with similar usage: they both use
TCP to transfer "files" from one machine to another. A remote terminal
program, such as SSH, is another style of application that uses TCP,
but it has a very different usage pattern: an SSH session may exist
for days, with only a slow drip of characters being sent in either
direction.

Note that TCP does not support *all* the functionality that
application processes might want from the network. For example, it
does not include security features like authentication or encryption.
It also makes no promises about jitter (variations in inter-packet
delays), which might be important to a real-time application.  TCP
also has inefficiencies that impact some usage scenarios. So the
edge-host software ecosystem is bigger than TCP, but TCP is the right
place to start exploring the options.

.. include:: reliable/design.rst
.. include:: reliable/segments.rst
.. include:: reliable/connections.rst
.. include:: reliable/window.rst
.. include:: reliable/trigger.rst
.. include:: reliable/retransmit.rst
.. include:: reliable/records.rst
.. include:: reliable/extensions.rst
