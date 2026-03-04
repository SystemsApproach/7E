13.1 Design Issues
----------------------

Any message transaction protocol faces many of the problems TCP
addressed in Chapter 11, starting with a precise definition of the
semantics we want to support. In this case, we're aided by the fact
that we're trying to emulate a local procedure call (in the case of
RPC) and a local read/write of a memory address (in the case of RDMA).

This is a good enough understanding to get started, with the caveat
that we talk about a generic "message transaction protocol" without
being specific as to whether it supports RPC or RDMA. This is not to
imply that there is a single protocol that supports both use
cases. It's conceivable there could be, but it turns out each has
adopted a different design. In this section, we are just looking
generally at the issues that require attention.

.. sidebar:: Story of VMTP

   Tell the story of VMTP (and using UDP as a counter argument).

The first problem is how to identify the target procedure/address. An
RPC mechanism needs a unique identifier for the remote procedure being
called, and an RDMA mechanism needs to provide the memory address for
the block of data to be read or written. In both cases, the mechanism
first establishes a name space (or address space) that both senders
and receivers understand, and then the request messages carry the
specific target identifier as one of its header fields. The reply
message then needs to include an identifier for the transaction, so
the response can be paired with the original caller. The TCP port
fields played this role for TCP—with the server being assigned a
well-known port—but this is just how TCP decided to identify
communication end points. Every end-to-end protocol needs to define an
analogous mechanism, tailored for the abstraction they provide; for
example, calling procedures and accessing memory.

The second problem is how to support reliable mesage delivey, and as
with TCP, we assume an imperfect network substrate. One option is to
"define the problem away" by choosing to run on top of a reliable
protocol like TCP. Another option is for the message transaction
protocol to implement its own reliable message delivery layer on top
of an unreliable substrate (e.g., UDP/IP). Such a protocol would
likely implement reliability using acknowledgments and timeouts,
similar to TCP. It would likely be optimized to have the response
message also acknowlege receipt of the request message (rather than
send a separate ACK). One complication is that the sender does not
know long it will take the receiver to produce the response; it may be
asking the receiver to execute a time-consuming computation. This
suggests a "keep alive" mechanism similar to the one implemented by
TCP may be necessary.

Note that there is an opportunity to fine-tune the definition of what
it means for the transaction protocol to be reliable. Typically, we
assume it supports a property known as *at-most-once semantics*. This
means that for every request message that the client sends, at most
one copy of that message is delivered to the server (and hence, the
remote procedure is executed only once or the remote memory location
is a accessed only once). On the other hand, if we assume the
operation being performed is *idempotent* (may be executed more than
once without any adverse side effects), then we do not need to protect
against duplicate messages being delivered. We still want to try to
execute the operation at least once—so reliable delivery is still a
goal—but we do not need to worry about duplicates.

The third challenge is to deal with request/response messsages that
are larger than the underlying network packets. Again, a message
transaction protocol could run on top of TCP and take advantage of
it's ability to reassmble segments, or it could implement its own
fragmentation/reassembly mechanism. Yet another option is to limit the
size of messages it is willing to deliver, which effectively moves
responsibility for larger blocks of data onto the application. As
limiting as this sounds, if the message transaction protocol knows
that 99% of the time applications deal with small chunks of data, it
could be a reasonable design choice.

A fourth consideration is how to deal with long network delays. From
the perspective of a protocol like TCP, this is usually viewed as the
challenge of keeping the pipe full, but for the message transaction
use case, we don't have an indefinite stream of data to send.
However, if the sender has to block waiting for a reply, it will have
lost the opportunity work on other tasks (presumably, tasks that do
not directly depend on the reply). Said another way, a message
transaction protocol needs to take concurrency into account.

This can been done in different ways. For example, a multi-threaded
process could afford to have one thread block waiting for a reply,
while others do productive work.  (The threads would still need to
synchronize if they ever got to the point that they needed to access
data in the reply message.) If a program is multi-threaded, then it is
possible that more than one thread will initiate its own message
transaction, meaning that multiple request/response transactions may
happen in parallel. The protocol would likely need to accommodate such
parallelism. The *stream id* in HTTP/2 (as described in Chapter 2)
supports exactly such a situation. Another design option is that
request messages do not block waiting for a reply, but instead, the
caller is allowed to proceed and will be notified when the reply
arrives. As with the multi-threaded design, the caller may eventually
have to block if it reaches a point where it needs to use the return
value in a computation.

A fifth design challenge concerns the format of the data being
exchanged in the request/response pair. Typically transport protocols
treat the payloads they carry as opaque blocks of data—leaving it to
the application whether they contain ASCII-encoded email messsages or
MPEG-encoded video—but we raise the point because of how message
transactions sometimes imply a strong link between the programming
environment and the messages being exchanged. For example, a caller
passing a data structure as an argument to a procedure or a remore
process reading a memory location that holds a data structure make
assumptions about how that data structure is layed out in memory.  As
a consequence, both RPC and RDMA make design choices about formatting.

Finally, to the extent a key motivations for using a message
transaction protocol in the first place is to support
latency-sensitive applications, then the design needs to aggresively
look for opportunities to optimize performance. Eliminating network
round-trips is an important part of that exercise, but as we will see
with our two two examples, there are other opportunities to exploit.

.. Maybe move latency to the top. QUIC gets better latency over
   wide-area and RDMA / RoCE gets better latency in datacenters.
   Maybe this is the "lead" in the intro -- unifying theme (at least
   one of them).
