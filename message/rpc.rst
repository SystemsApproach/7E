.. _artifact-grpc:

|Message|.2 Remote Procedure Call
---------------------------------

RPC is a popular mechanism for structuring distributed systems, based
on the semantics of a local procedure call—the application program
makes a call into a procedure without regard for whether it is local
or remote and blocks until the call returns. An application developer
can be largely unaware of whether the procedure is local or remote,
simplifying his task considerably. This is why RPC is such a
compelling abstraction.

A complete RPC mechanism actually involves two major components:

1. A protocol that manages the messages sent between the client and
   the server processes and that deals with failure modes of the
   underlying network. This protocol is sometimes referred to as an
   "RPC Protocol", but we distinguish between the overall mechanism
   and the underlying protocol by calling the latter a message
   transaction protocol.

2. Programming language and compiler support to package the arguments
   into a request message on the client machine and then to translate
   this message back into the arguments on the server machine, and
   likewise with the return value. This piece of the RPC mechanism is
   sometimes called a *stub compiler*.

:numref:`Figure %s <fig-rpc>` schematically depicts what happens
when a client invokes a remote procedure. First, the client calls a
local stub for the procedure, passing it the arguments required by the
procedure.  This stub hides the fact that the procedure is remote by
translating the arguments into a request message and then invoking an
underlying message transaction protocol to send the request message to
the server machine. At the server, the transaction protocol delivers
the request message to the server stub, which translates it into the
arguments to the procedure and then calls the local procedure. After
the server procedure completes, it returns a reply message that it
hands off to the protocol for transmission back to the client. The
protocol on the client passes this message up to the client stub,
which translates it into a return value that it returns to the client
program.

.. _fig-rpc:
.. figure:: message/figures/rpc.png
   :width: 500px
   :align: center

   Complete RPC mechanism.

This section considers just the network-related aspects of an RPC
mechanism, focusing on the protocol that transmits messages between
client and server. The transformation of arguments into messages and
*vice versa* is covered in Section |Message|.6. It is also important to keep
in mind that the client and server programs are written in some
programming language, meaning that a given RPC mechanism might support
Python stubs, Java stubs, GoLang stubs, and so on, each of which
includes language-specific idioms for how procedures are invoked.

The first RPC mechanism is due to Andrew Birrell and Bruce Nelson at
Xerox PARC, as described in their 1984 paper. Many other RPC
mechanisms followed their lead, often implemented as the communication
substrate for early distributed systems built around personal
workstations and Ethernet-based local-area networks. During this era,
SunRPC became the *de facto* standard thanks to its wide distribution
with Sun workstations and the central role it played in Sun’s popular
Network File System (NFS). The IETF subsequently adopted it as a
standard Internet protocol under the name ONC RPC.

.. admonition:: Further Reading

    A. Birrell and B. Nelson. `Implementing Remote Procedure Calls
    <http://birrell.org/andrew/papers/ImplementingRPC.pdf>`__. ACM
    Transactions on Computer Systems, February 1984.

    R. Thurlow. `RPC: Remote Procedure Call Protocol Specification
    Version 2 <https://www.rfc-editor.org/rfc/rfc5531>`__. RFC 5531,
    May 2009.

The emergence of the cloud led to another seismic shift in RPC design,
in part corresponding to the wide-spread adoption of HTTP as discussed
in Chapter |Apps|. This is where we pick up the story by turning our
attention to gRPC, a popular open source RPC mechanism. gRPC is based
on an RPC mechanism that Google had been using internally to implement
cloud services in their datacenters.

.. sidebar:: RPC vs REST

   A false dichotomy...

Despite its origins in Google, gRPC does not stand for Google RPC. The
“g” stands for something different in each release. For version 1.10
it stood for “glamorous” and for 1.18 it stood for “goose”. According
to the official gRPC FAQ, it is now a recursive acronym: gRPC means
“gRPC Remote Procedure Call”. Nonetheless, gRPC is popular because it
makes available to everyone—as open source—a decade’s worth of
experience within Google using RPC to build scalable cloud services.

Before getting into the details, there is a major difference between
gRPC and other RPC mechanisms—such as SunRPC—that came before it.
The difference is that gRPC was explicitly designed for cloud services
rather than the simpler client/server paradigm that preceded it. As we
introduced in Chapter |Apps|, this results in an extra level of indirection.

In the client/server world, the client invokes a method on a specific
server process running on a specific server machine. One server
process is presumed to be enough to serve calls from all the client
processes that might call it. With cloud services, the client invokes
a method on a *service*, which, in order to support calls from
arbitrarily many clients at the same time, is implemented by a
scalable number of server processes, each potentially running on a
different server (or VM).  A *load balancer* then directs that
invocation to one of the many available server processes (containers)
that implement that service.

What we are interested in here is transport protocol at the core of
gRPC. Here again, there is a major departure from earlier RPC
mechanisms, not in terms of fundamental problems that need to be
addressed, but in terms of gRPC’s approach to addressing them. In
short, gRPC “outsources” many of the problems to other protocols,
leaving gRPC to essentially package those capabilities in an
easy-to-use form. Let’s look at the details.

First, gRPC runs on top of TCP instead of UDP, as was the case with
SunRPC and many of its peers. This means gRPC outsources the problem
of reliably transmitting request and reply messages of arbitrary
size. Second, gRPC actually runs on top of *Transport Layer Security*
(TLS)—a thin layer that secures and end-to-end TCP connection—which
means it also outsources responsibility for securing the communication
channel so adversaries can’t eavesdrop or hijack the message
exchange. Third, gRPC actually, actually runs on top of HTTP/2 (which
is itself layered on top of TCP and TLS), meaning gRPC outsources
two further problems: (1) efficiently encoding/compressing binary data
into a message, (2) multiplexing multiple concurrent remote procedure
calls onto a single TCP connection.

In other words, gRPC encodes the identifier for the remote method as a
URI, the request parameters to the remote method as content in the
HTTP message, and the return value from the remote method in the HTTP
response. The full gRPC stack is depicted in :numref:`Figure %s
<fig-grpc-stack>`, which also includes the language-specific elements.
(One strength of gRPC is the wide set of programming languages it
supports, with only a small subset shown in :numref:`Figure %s
<fig-grpc-stack>`.)

.. _fig-grpc-stack:
.. figure:: message/figures/grpc.png
   :width: 400px
   :align: center

   gRPC core stacked on top of HTTP, TLS, and TCP and
   supporting a collection of languages.

This all means we find ourselves in an interesting dependency loop:
RPC can be viewed as a message-oriented transport protocol used to
implement distributed applications, HTTP is an example of an
application-level protocol, and yet gRPC runs on top of HTTP rather
than the other way around.

It's not that uncommon to see layering produce this sort of convoluted
result.  Layering provides a convenient way for
humans to wrap their heads around complex systems, and outsourcing
problems to an existing layer can be see and an efficient want to
avoid duplication of effort. However, what we’re
really trying to do is solve a set of problems (e.g., reliably transfer
messages of arbitrary size, identify senders and recipients, match
requests messages with reply messages, and so on) and the way these
solutions get bundled into protocols, and those protocols then layered
on top of each other, is the consequence of incremental changes over
time. It's easy to imagine a different outcome if the set of available
protocols at any point in time had been slightly different.

Imagine, for example, a different version of history in which the
Internet had started with an RPC mechanism as ubiquitous as TCP—see
the sidebar on VTMP in the previous section for an example. HTTP then
might have been implemented on top of that RPC layer. Google might
have spent their time improving the RPC protocol rather than inventing
one of their own in gRPC. What happened instead is that the web became
the Internet’s killer app, which meant that its application protocol
(HTTP) became universally supported by the rest of the Internet’s
infrastructure: Firewalls, Load Balancers, Encryption, Authentication,
Compression, and so on. Because all of these network elements have
been designed to work well with HTTP, HTTP has effectively become the
Internet’s universal request/reply transport protocol.\ [#]_

.. [#] If you are
   wondering why we would describe an application protocol as a transport
   protocol, just remember that layering is an abstraction to help
   us understand and modularize networks, not a set of precise rules about what
   happens in any given module.

There is, however, a possibility of a different set of layers
replacing or at least coexisting with gRPC/HTTP/TCP. As we discuss
below, QUIC has been developed to be a more suitable transport for
HTTP, and part of what it offers is a better match to the
request/reply semantics of both HTTP and RPC.

Returning to the unique characteristics of gRPC, the biggest value it
brings to the table is to incorporate *streaming* into the RPC
mechanism, which is to say, gRPC supports four different request/reply
patterns:

1. Simple RPC: The client sends a single request message and the server
   responds with a single reply message. The client blocks until the
   reply is returned.

2. Server Streaming RPC: The client sends a single request message and
   the server responds with a stream of reply messages. The client
   completes (unblocks) once it has all the server’s responses.

3. Client Streaming RPC: The client sends a stream of requests to the
   server, and the server sends back a single response, typically (but
   not necessarily) after it has received all the client’s requests.

4. Bidirectional Streaming RPC: The call is initiated by the client, but
   after that, the client and server can read and write requests and
   responses in any order; the streams are completely independent.

This extra freedom in how the client and server interact means the gRPC
transport protocol needs to send additional metadata and control
messages—in addition to the actual request and reply messages—between
the two peers. Examples include ``Error`` and ``Status`` codes (to
indicate success or why something failed), ``Timeouts`` (to indicate how
long a client is willing to wait for a response), ``PING`` (a keep-alive
notice to indicate that one side or the other is still running), ``EOS``
(end-of-stream notice to indicate that there are no more requests or
responses), and ``GOAWAY`` (a notice from servers to clients to indicate
that they will no longer accept any new streams).

Unlike many other protocols in this book, where we show the protocol’s
header format, the way this control information gets passed between
the two sides is largely dictated by the underlying transport
protocol, in this case HTTP/2. For example, HTTP already includes a
set of header fields and reply codes that gRPC takes advantage of.

With HTTP's basic operation in mind, the following is fairly
straightforward. A simple RPC request (with no streaming) might
include the following HTTP message from the client to the server:

.. code-block:: html

   HEADERS (flags = END_HEADERS)
   :method = POST
   :scheme = http
   :path = /google.pubsub.v2.PublisherService/CreateTopic
   :authority = pubsub.googleapis.com
   grpc-timeout = 1S
   content-type = application/grpc+proto
   grpc-encoding = gzip
   authorization = Bearer y235.wef315yfh138vh31hv93hv8h3v
   DATA (flags = END_STREAM)
   <Length-Prefixed Message>

leading to the following response message from the server back to the
client:

.. code-block:: html

   HEADERS (flags = END_HEADERS)
   :status = 200
   grpc-encoding = gzip
   content-type = application/grpc+proto
   DATA
   <Length-Prefixed Message>
   HEADERS (flags = END_STREAM, END_HEADERS)
   grpc-status = 0 # OK
   trace-proto-bin = jher831yy13JHy3hc

In this example, ``HEADERS`` and ``DATA`` are two standard HTTP
control messages, which effectively delineate between “the message’s
header” and “the message’s payload.” Specifically, each line following
``HEADERS`` (but before ``DATA``) is an ``attribute = value`` pair
that makes up the header (think of each line as analogous to a header
field); those pairs that start with colon (e.g., ``:status = 200``)
are part of the HTTP standard (e.g., status ``200`` indicates
success); and those pairs that do not start with a colon are
gRPC-specific customizations (e.g., ``grpc-encoding = gzip`` indicates
that the data in the message that follows has been compressed using
``gzip``, and ``grpc-timeout = 1S`` indicates that the client has set
a one second timeout).

There is one final piece to explain. The header line

.. code-block:: html

   content-type = application/grpc+proto

indicates that the message body (as demarcated by the ``DATA`` line)
is meaningful only to the application program (i.e., the server
method) that this client is requesting service from. More
specifically, the ``+proto`` string specifies that the recipient will
be able to interpret the bits in the message according to a *Protocol
Buffer* (abbreviated ``proto``) interface specification. Protocol
Buffers are gRPC’s way of specifying how the parameters being passed
to the server are encoded into a message, which is in turn used to
generate the stubs that sit between the underlying RPC mechanism and
the actual functions being called (see :numref:`Figure %s
<fig-rpc>`). This is how gRPC tackles the *argument marshalling*
problem, to which we return in Section |Message|.6.

The bottom line is that complex mechanisms like RPC, once packaged as
a monolithic bundle of software (as with SunRPC), are nowadays built by
assembling an assortment of smaller pieces, each of which solves a
narrow problem. gRPC is both an example of that approach, and a tool
that enables further adoption of the approach.  The microservices
architecture mentioned Chapter |Apps| applies the “built from small parts”
strategy to entire cloud applications (e.g., the services provided by Uber, Lyft, Netflix,
Yelp, Spotify, etc.), where gRPC is often the communication mechanism used
by those small pieces to exchange messages with each other.
