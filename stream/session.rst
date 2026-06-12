.. _artifact-sip:

|Stream|.4 Session Control
--------------------------

Many applications, including most uses of the World Wide Web and
video-on-demand applications, fit well into the client-server model
that we discussed in Chapter |Apps|. However, applications for
human-to-human communication, such as VOIP or multi-party
conferencing, don't conform so well to that model. There is no
well-defined server for a client to connect to. Instead, we want to
establish a communication *session* among the participants. These
sorts of applications typically use something called *session
control*. This section explores the challenge of session control,
using SIP (Session Initiation Protocol) as an example. SIP is typically
paired with RTP to implement VOIP.

|Stream|.4.1 Session Initiation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

SIP bears a certain resemblance to
HTTP, being based on a similar request/response model. However, it is
designed with these different applications in mind and thus
provides different capabilities than HTTP. The capabilities
provided by SIP can be grouped into five categories:

-  User location: Determining the correct device with which to
   communicate to reach a particular user

-  User availability: Determining if the user is willing or able to take
   part in a particular communication session

-  User capabilities: Determining such items as the choice of media and
   coding scheme to use

-  Session setup: Establishing session parameters such as port numbers to
   be used by the communicating parties

-  Session management: A range of functions including transferring
   sessions (e.g., to implement “call forwarding”) and modifying session
   parameters

Most of these functions are straightforward, but the issue of location
bears further discussion. Since SIP is primarily used for
human-to-human communication, it is important to be able to locate
individual *users*, not just machines. And, unlike email, it’s not
good enough just to locate a server that the user will be checking on
at some later date and dump the message there—we need to know where
the user is right now if we want to be able to communicate with them
in real time. This is further complicated by the fact that a user
might choose to communicate using a range of different devices, such
as a desktop when in the office and a handheld device when
traveling. Multiple devices might be active at the same time and might
have widely different capabilities (e.g., an alphanumeric pager and a
PC-based video “phone”).  Ideally, it should be possible for other
users to be able to locate and communicate with the appropriate device
at any time. Furthermore, the user must be able to have control over
when, where, and from whom they receives calls.

To enable a user to exercise the appropriate level of control over
their calls, SIP introduces the notion of a proxy. A SIP proxy can be
thought of as a point of contact for a user to which initial requests
for communication with a given person are sent. Proxies also perform
functions on behalf of callers. We can see how proxies work best
through an example.

.. _fig-sipproxy:
.. figure:: stream/figures/f09-08-9780123850591.png
   :width: 600px
   :align: center

   Establishing communication through SIP proxies.

Consider the two users in :numref:`Figure %s <fig-sipproxy>`. The
first thing to notice is that each user has a name in the format
``user@domain``, very much like an email address. When user Bruce
wants to initiate a session with Larry, he sends his initial SIP
message to the local proxy for his domain, ``systemsapproach.org``.
Among other things, this initial message contains a *SIP URI*—these
are a form of uniform resource identifier which look like this:

::

   SIP:larry@princeton.edu

A SIP URI provides complete identification of a user, but (unlike a URL)
does not provide his location, since that may change over time. We will
see shortly how the location of a user can be determined.

Upon receiving the initial message from Bruce, the proxy looks at the
SIP URI and deduces that this message should be sent to the proxy. For
now, we assume that the proxy has access to some database that enables
it to obtain a mapping from the name to the IP address of one or more
devices at which Larry currently wishes to receive messages. The proxy
can therefore forward the message on to Larry’s chosen device(s).
Sending the message to more than one device is called *forking* and may
be done either in parallel or in series (e.g., send it to his mobile
phone if he doesn’t answer the phone at his desk).

The initial message from Bruce to Larry is likely to be a SIP ``invite``
message, which looks something like the following:

::

   INVITE sip:larry@princeton.edu SIP/2.0
   Via: SIP/2.0/UDP bsd-pc.systemsapproach.org;branch=z9hG4bK433yte4
   To: Larry <sip:larry@princeton.edu>
   From: Bruce <sip:bruce@systemsapproach.org>;tag=55123
   Call-ID: xy745jj210re3@bsd-pc.systemsapproach.org
   CSeq: 271828 INVITE
   Contact: <sip:bruce@bsd-pc.systemsapproach.org>
   Content-Type: application/sdp
   Content-Length: 142

The first line identifies the type of function to be performed
(``invite``); the resource on which to perform it, the called party
(``sip:larry@princeton.edu`` ); and the protocol version (2.0). The
subsequent header lines probably look somewhat familiar because of
their resemblance to the header lines in an email message. SIP defines
a large number of header fields, only some of which we describe
here. Note that the ``Via:`` header in this example identifies the
device from which this message originated. The ``Content-Type:`` and
``Content-Length:`` headers describe the contents of the message
following the header, just as in a MIME-encoded email message. In this
case, the content is an SDP (Session Description Protocol)
message. That message would describe such things as the type of media
(audio, video, etc.) that Bruce would like to exchange with Larry and
other properties of the session such as codec types that he
supports.

Returning to the example, when the ``invite`` message arrives at the
proxy, not only does the proxy forward the message on toward
``princeton.edu``, but it also responds to the sender of the ``invite``.
Just as in HTTP, all responses have a response code, and the
organization of codes is similar to that for HTTP. In :numref:`Figure %s
<fig-sipeg>` we can see a sequence of SIP messages and responses.

.. _fig-sipeg:
.. figure:: stream/figures/f09-09-9780123850591.png
   :width: 650px
   :align: center

   Message flow for a basic SIP session.

The first response message in this figure is the provisional response
``100 trying``, which indicates that the message was received without
error by the caller’s proxy. Once the ``invite`` is delivered to Larry’s
phone, it alerts Larry and responds with a ``180 ringing`` message. The
arrival of this message at Bruce’s computer is a sign that it can
generate a “ringtone”. Assuming Larry is willing and able to communicate
with Bruce, he could pick up his phone, causing the message ``200 OK``
to be sent. Bruce’s computer responds with an ``ACK``, and media (e.g.,
an RTP-encapsulated audio stream) can now begin to flow between the two
parties. Note that at this point the parties know each others’
addresses, so the ``ACK`` can be sent directly, bypassing the proxies.
The proxies are now no longer involved in the call. Note that the media
will therefore typically take a different path through the network than
the original signalling messages. Furthermore, even if one or both of
the proxies were to crash at this point, the call could continue on
normally. Finally, when one party wishes to end the session, it sends a
``BYE`` message, which elicits a ``200 OK`` response under normal
circumstances.

There are a few details that we have glossed over. One is the
negotiation of session characteristics. Perhaps Bruce would have liked
to communicate using both audio and video but Larry’s phone only
supports audio. Thus, Larry’s phone would send an SDP message in its
``200 OK`` describing the properties of the session that will be
acceptable to Larry and the device, considering the options that were
proposed in Bruce’s ``invite``. In this way, mutually acceptable session
parameters are agreed to before the media flow starts.

The other big issue we have glossed over is that of locating the
correct device for Larry. First, Bruce’s computer had to send its
``invite`` to the ``systemsapproach.org`` proxy. This could have been
a configured piece of information in the computer, or it could have
been learned by DHCP. Then the ``systemsapproach.org`` proxy had to
find the ``princeton.edu`` proxy. This could be done using a special
sort of DNS lookup that would return the IP address of the SIP proxy
for the domain. (This is similar to how email servers can be found as
discussed in Chapter |Naming|.) Finally, the ``princeton.edu`` proxy
had to find a device on which Larry could be contacted. Typically, a
proxy server has access to a location database that can be populated
in several ways. Manual configuration is one option, but a more
flexible option is to use the *registration* capabilities of SIP.

A user can register with a location service by sending a SIP
``register`` message to the “registrar” for his domain. This message
creates a binding between an “address of record” and a “contact
address”. An “address of record” is likely to be a SIP URI that is the
well-known address for the user (e.g., ``sip:larry@princeton.edu``) and
the “contact address” will be the address at which the user can
currently be found (e.g., ``sip:larry@llp-ph.cs.princeton.edu``). This
is exactly the binding that was needed by the proxy ``princeton.edu`` in
our example.

Note that a user may register at several locations and that multiple
users may register at a single device. For example, one can imagine a
group of people walking into a conference room that is equipped with an
IP phone and all of them registering on it so that they can receive
calls on that phone.

SIP is a rich and flexible protocol that can support a wide range
of complex calling scenarios as well as applications that have little
to do with telephony. For example, SIP supports operations
that enable a call to be routed to a “music-on-hold” server or a
voicemail server. It is widely used for video conferencing, and can
also be used for instant messaging.
