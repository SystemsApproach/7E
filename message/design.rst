|Message|.1 Design Issues
-------------------------

A message transaction protocol faces many of the same problems as TCP,
starting with a precise definition of the
semantics we want to support. In this case, we're aided by the fact
that we're trying to emulate a local procedure call (in the case of
RPC) and a local read/write of a memory address (in the case of RDMA).

This is a good enough understanding to get started, with the caveat
that for now we will talk about a generic "message transaction protocol" without
being specific as to whether it supports RPC or RDMA. This is not to
imply that there is a single protocol that supports both use
cases. It's conceivable there could be, but it turns out each has
adopted a different design. In this section, we look generally at the
issues that require attention.

A key motivation for using a message transaction protocol is often to support
latency-sensitive applications. In these cases, the design needs to aggressively
look for opportunities to optimize performance. Eliminating network
round-trips is an important part of that exercise, but as we will see
with throughout this chapter, there are other opportunities to reduce
delay. As with TCP, performance is an important design goal, but the
emphasis is on the latency to complete transactions rather than the
filling of the transport pipe.

The next problem is how to identify the target procedure/address. An
RPC mechanism needs a unique identifier for the remote procedure being
called, and an RDMA mechanism needs to provide the memory address for
the block of data to be read or written. In both cases, the mechanism
first establishes a name space (or address space) that both senders
and receivers understand, and then the request messages carry the
specific target identifier as one of their header fields. The reply
message then needs to include an identifier for the transaction, so
the response can be paired with the original request. The TCP port
fields played this role for TCP—with the server being assigned a
well-known port—but this is just how TCP identifies communication end
points. Every end-to-end protocol needs to define an analogous
mechanism, tailored for the abstraction it provides; for example,
calling procedures and accessing memory.

A third problem is how to support reliable message delivery, and as
with TCP, we assume an imperfect network substrate. One option is to
"define the problem away" by choosing to run on top of a reliable
protocol like TCP. That, however, might run counter to the goals of minimizing
latency. Another option is for the message transaction
protocol to implement its own reliable message delivery layer on top
of an unreliable substrate, either directly on IP (as a peer of TCP)
or as a user-level process running on top of UDP. Such a protocol
would implement reliability using acknowledgments and timeouts,
similar to TCP. It would also likely be optimized to have the response
message implicitly acknowledge receipt of the request message (rather
than send a separate ACK). One complication is that the sender does
not know long it will take the receiver to produce the response; it
may be asking the receiver to execute a time-consuming
computation. This suggests a "keep alive" mechanism similar to the one
implemented by TCP may be necessary.

Note that there is an opportunity to fine-tune the definition of what
it means for the transaction protocol to be reliable. Typically, we
assume a transaction-based protocol supports a property known as
*at-most-once semantics*. This means that for every request message
that the client sends, at most one copy of that message is delivered
to the server (and hence, the remote procedure is executed only once
or the remote memory location is a accessed only once). On the other
hand, if we assume the operation being performed is *idempotent* (may
be executed more than once without any adverse side effects), then we
do not need to protect against duplicate messages being delivered. We
still want to try to execute the operation at least once—so reliable
delivery is still a goal—but we do not need to worry about duplicates.

The fourth challenge is how to deal with request/response messages
that are larger than the underlying network packets. Again, a message
transaction protocol could run on top of TCP and take advantage of
its ability to reassemble segments, or it could implement its own
fragmentation/reassembly mechanism. Yet another option is to limit the
size of messages the protocol is willing to deliver, which effectively
moves responsibility for larger blocks of data onto the application.
As limiting as this sounds, if the message transaction protocol knows
that 99% of the time applications deal with small chunks of data, it
could be a reasonable design choice.

A fifth consideration is how to deal with long network delays. From
the perspective of a protocol like TCP, this is usually viewed as the
challenge of keeping the pipe full, but for the message transaction
use case, we don't have an indefinite stream of data to send.
However, if the sender has to block waiting for a reply, it will have
lost the opportunity work on other tasks (presumably, tasks that do
not directly depend on the reply). Just as importantly, that reply not
only signals that the destination has received the message (as with
TCP), but that the application has acted upon it. Said another way, a
message transaction protocol needs to take concurrency into account.

This can been done in different ways. For example, a multi-threaded
process could afford to have one thread block waiting for a reply,
while others do productive work.  (The threads would still need to
synchronize if they ever got to the point that they needed to access
data in the reply message.) But if a program is multi-threaded, then
it is possible that more than one thread will initiate its own message
transaction, meaning that multiple request/response transactions may
happen in parallel. The protocol would need to accommodate such
parallelism. The *stream id* in HTTP/2 (as described in Chapter |Apps|)
supports exactly such a situation. Another design option is that
request messages do not block waiting for a reply, but instead, the
caller is allowed to proceed and will be notified when the reply
arrives. As with the multi-threaded design, the caller may eventually
have to block if it reaches a point where it needs to use the return
value in a computation. This implies the message protocol needs to
signal to the application that the reply has arrived.

A final design challenge concerns the format of the data being
exchanged in the request/response pair. Most transport protocols
treat the payloads they carry as opaque blocks of data—leaving it to
the application whether they contain ASCII-encoded email messages or
MPEG-encoded video—but we raise the point because of how message
transactions sometimes imply a strong link between the programming
environment and the messages being exchanged. For example, a caller
passing a data structure as an argument to a procedure (or a remote
process reading a memory location that holds a data structure) makes
assumptions about how that data structure is laid out in memory.  As
a consequence, both RPC and RDMA make design choices about formatting.


