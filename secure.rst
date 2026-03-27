.. SPDX-FileCopyrightText: 2019 Systems Approach LLC
.. SPDX-FileCopyrightText: 2025 Systems Approach LLC
.. SPDX-License-Identifier: CC-BY-4.0

.. include:: chapters.rst

Chapter |TLS|:  Secure Channels
====================================


Security has been a focus of system designers for as long as we have
had time-shared computers. If two users can share a computer, then it
is necessary to have protections in place to limit the impact one user
can have on another. For example, one user should not generally be
able to read the data of another user just because they run code on
the same system. Similarly, multi-user systems ensure that malicious
or poorly written code from one user cannot interfere with the
operation of another user's programs.

Computer networks are, like multi-user computers, shared resources,
and similar requirements apply. One network user should not be able to
interfere with another user's traffic, and in general, a user sending
data across a network wants that data to be protected from
unauthorized modification or eavesdropping.

The Internet architecture, however, was initially created with
essentially no security features. This was not because the inventors,
implementers and architects were unaware of security issues, but
rather because there were other, more pressing goals. As Vint Cerf,
the co-inventor of TCP/IP said: *"getting this thing to work at all
was non-trivial.”* David Clark, the architect of the Internet, has
said *"it’s not that we didn’t think about security…we thought we
could exclude [untrustworthy people].*

The Internet was created to allow users in one location to access
computing resources in another. Those systems had their own security
measures in place. For example, if you wanted to use the Internet to
log in to a remote computer, you would (usually) need to authenticate
yourself to that remote system (e.g., with a user name and password)
before gaining access to any resources on that system. When the
Internet interconnected only a handful of computers, it was feasible
to imagine that untrustworthy people might be excluded. That
possibility has long vanished as the Internet grew to interconnect
most of the world's population and billions of devices.

While it is common to decry the lack of security "built in" to the
Internet, there have been some notable successes in enabling secure
end-to-end communication over the Internet. Chief among these is the
development of Transport Layer Security (TLS) and its predecessor, the
Secure Socket Layer (SSL). As you might gather from this pair of
names, the design goal was to provide a layer of security to accompany
the reliable byte-stream abstraction provided by TCP and its
corresponding socket API. While initially motivated by the need to
support secure transactions on the World Wide Web, TLS was designed as
a general-purpose set of mechanisms to provide a secure byte-stream
abstraction to any higher layer protocol that needed it.

TLS provides mechanisms to authenticate end-points, to protect the
integrity of traffic, and to ensure its confidentiality. These
capabilities are as useful for remote procedure calls (a topic we
cover in the next chapter) as they are for web traffic.

One measure of the success of TLS is that the overwhelming majority of
web sites now use HTTPS (i.e., HTTP running over TLS) by default. In
this chapter we explore the various components that have been
assembled to deploy a secure byte-stream abstraction that is now the
default for most web traffic.

Network Security is a big enough field that we have an entire
companion book on it. In this chapter we will illustrate many of the
important principles of network security by focusing on the example
system of TLS.

.. admonition:: Further Reading

   Larry Peterson and Bruce Davie. `Network Security: A Systems
   Approach <https://security.systemsapproach.org>`__. Systems
   Approach, 2025.


.. include:: secure/design.rst
.. include:: secure/blocks.rst
.. include:: secure/key-dist.rst
.. include:: secure/auth.rst
.. include:: secure/proto.rst
.. include:: secure/perf.rst
