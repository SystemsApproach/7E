|Message|.6 Argument Marshalling
----------------------------------

We conclude by looking at the issue of how RPC and RDMA deal with the
format of the payload carried in request and response messages. The
short answer is that data formatting is a fundamental aspect of RPC
(half of the two-part mechanism outlined in Section |Message|.2), and largely
ignored by RDMA. This is an explicit design choice. RPC makes no
assumptions about the process on the other end of a message exchange,
and so has to account for wide variation. The rest of this section
explains the implications. In contrast, RDMA was originally designed
for tightly-coupled parallel programs, where the same programming team
was responsible for both sides of a communication. In that scenario,
it is typically assumed that a common machine architecture and just
one compiler are being used throughout the program. The programmer is
responsible for dealing with any exceptions to that assumption.

Focusing then on RPC, the other half of the mechanism is a toolset
that transforms data from the representation used by the programs into
a form that is suitable for transmission over a network and *vice
versa*. This process is sometimes called *argument marshalling* or
*serialization*. This terminology comes from the RPC world, where the
client thinks it is invoking a procedure with a set of arguments, but
these arguments are then “brought together and ordered in an
appropriate and effective way” to form a network message.

.. _fig-marshal1:
.. figure:: message/figures/f07-01-9780123850591.png
   :width: 400px
   :align: center

   Argument marshalling involves serializing and unpacking application
   data.

One challenge is that computers represent data in different ways. For
example, while most modern computers represent floating-point numbers
in IEEE standard 754 format, some older machines still use their own
nonstandard format. Even for something as simple as integers,
different architectures use different sizes (e.g., 16-bit, 32-bit,
64-bit). To make matters worse, on some machines integers are
represented in *big-endian* form (the most significant bit of a
word—the "big end"—is in the byte with the lowest address), while on
other machines integers are represented in *little-endian* form (the
least significant bit—the "little end"—is in the byte with the lowest
address). Today, most architectures are little-endian, although some
support both representations (and so are called *bi-endian*). The
point is that you can never be sure how the host you are communicating
with stores integers. The big-endian and little-endian representations
of the integer 34,677,374 are given in :numref:`Figure %s
<fig-endian>`.

.. _fig-endian:
.. figure:: message/figures/f07-02-9780123850591.png
   :width: 500px
   :align: center

   Big-endian and little-endian byte order for the integer 34,677,374

Another reason that marshalling is difficult is that application
programs are written in different languages, and even when you are using
a single language there may be more than one compiler. For example,
compilers have a fair amount of latitude in how they lay out structures
(records) in memory, such as how much padding they put between the
fields that make up the structure. Thus, you could not simply transmit a
structure from one machine to another, even if both machines were of the
same architecture and the program was written in the same language,
because the compiler on the destination machine might align the fields
in the structure differently.

Although argument marshalling is not rocket science—it is a small
matter of bit twiddling—there are a surprising number of design
choices to address. This section gives a simple taxonomy for argument
marshalling, and concludes by showing how gRPC addresses the problem.

|Message|.6.1 Data Types
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The first question is what data types the system is going to support. In
general, we can classify the types supported by an argument marshalling
mechanism at three levels. Each level complicates the task faced by the
marshalling system.

At the lowest level, a marshalling system operates on some set of
*base types*. Typically, the base types include integers,
floating-point numbers, and characters. The system might also support
ordinal types and Booleans. The encoding process must be able to
convert each base type from one representation to another—for example,
convert an integer from big-endian to little-endian.

At the next level are *flat types*—structures and arrays. While flat
types might at first not appear to complicate argument marshalling, the
reality is that they do. The problem is that the compilers used to
compile application programs sometimes insert padding between the fields
that make up the structure so as to align these fields on word
boundaries. The marshalling system typically *packs* structures so that
they contain no padding.

At the highest level, the marshalling system might have to deal with
*complex types*—those types that are built using pointers. That is, the
data structure that one program wants to send to another might not be
contained in a single structure, but might instead involve pointers from
one structure to another. A tree is a good example of a complex type
that involves pointers. Clearly, the data encoder must prepare the data
structure for transmission over the network because pointers are
implemented by memory addresses, and just because a structure lives at a
certain memory address on one machine does not mean it will live at the
same address on another machine. In other words, the marshalling system
must *serialize* (flatten) complex data structures.

In summary, depending on how complicated the type system is, the task
of argument marshalling usually involves converting the base types,
packing the structures, and linearizing the complex data structures,
all to form a contiguous message that can be transmitted over the
network. :numref:`Figure %s <fig-marshal2>` illustrates this task.

.. _fig-marshal2:
.. figure:: message/figures/f07-03-9780123850591.png
   :width: 400px
   :align: center

   Argument marshalling: converting, packing, and linearizing

|Message|.6.2 Conversion Strategy
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Once the type system is established, the next issue is what conversion
strategy the argument marshaller will use. There are two general
options: *canonical intermediate form* and *receiver-makes-right*. We
consider each, in turn.

The idea of canonical intermediate form is to settle on an external
representation for each type; the sending host translates from its
internal representation to this external representation before sending
data, and the receiver translates from this external representation into
its local representation when receiving data. To illustrate the idea,
consider integer data; other types are treated in a similar manner. You
might declare that the big-endian format will be used as the external
representation for integers. The sending host must translate each
integer it sends into big-endian form, and the receiving host must
translate big-endian integers into whatever representation it uses.
(This is what is done in the Internet for protocol headers.) Of course,
a given host might already use big-endian form, in which case no
conversion is necessary.

The alternative, receiver-makes-right, has the sender transmit data in
its own internal format; the sender does not convert the base types, but
usually has to pack and flatten more complex data structures. The
receiver is then responsible for translating the data from the sender’s
format into its own local format. The problem with this strategy is that
every host must be prepared to convert data from all other machine
architectures. In networking, this is known as an *N-by-N solution*:
Each of N machine architectures must be able to handle all N
architectures. In contrast, in a system that uses a canonical
intermediate form, each host needs to know only how to convert between
its own representation and a single other representation—the external
one.

Using a common external format is clearly the correct thing to do,
right? This has certainly been the conventional wisdom in the networking
community for over 30 years. The answer is not cut and dried, however.
It turns out that there are not that many different representations for
the various base classes, or, said another way, N is not that large. In
addition, the most common case is for two machines of the same type to
be communicating with each other. In this situation, it seems silly to
translate data from that architecture’s representation into some foreign
external representation, only to have to translate the data back into
the same architecture’s representation on the receiver.

A third option, although we know of no existing system that exploits it,
is to use receiver-makes-right if the sender knows that the destination
has the same architecture; the sender would use some canonical
intermediate form if the two machines use different architectures. How
would a sender learn the receiver’s architecture? It could learn this
information either from a name server or by first using a simple test
case to see if the appropriate result occurs.

|Message|.6.3 Tags
~~~~~~~~~~~~~~~~~~

The third issue in argument marshalling is how the receiver knows what
kind of data is contained in the message it receives. There are two
common approaches: *tagged* and *untagged* data. The tagged approach is
more intuitive, so we describe it first.

A tag is any additional information included in a message—beyond the
concrete representation of the base types—that helps the receiver
decode the message. There are several possible tags that might be
included in a message. For example, each data item might be augmented
with a *type* tag. A type tag indicates that the value that follows is
an integer, a floating-point number, or whatever. Another example is a
*length* tag.  Such a tag is used to indicate the number of elements
in an array or the size of an integer. A third example is an
*architecture* tag, which might be used in conjunction with the
receiver-makes-right strategy to specify the architecture on which the
data contained in the message was generated. :numref:`Figure %s
<fig-tags>` depicts how a simple 32-bit integer might be encoded in a
tagged message.

.. _fig-tags:
.. figure:: message/figures/f07-04-9780123850591.png
   :width: 400px
   :align: center

   A 32-bit integer encoded in a tagged message.

The alternative is not to use tags. How does the receiver know how to
decode the data in this case? It knows because it was programmed to
know. In other words, if you call a remote procedure that takes two
integers and a floating-point number as arguments, then there is no
reason for the remote procedure to inspect tags to know what it has
just received. It simply assumes that the message contains two
integers and a float and decodes it accordingly. Note that, while this
works for most cases, the one place it breaks down is when sending
variable-length arrays. In such a case, a length tag is commonly used
to indicate how long the array is.

It is also worth noting that the untagged approach means that the
presentation formatting is truly end-to-end. It is not possible for some
intermediate agent to interpret the message unless the data is tagged.
Why would an intermediate agent need to interpret a message, you might
ask? Stranger things have happened, mostly resulting from *ad hoc*
solutions to unexpected problems that the system was not engineered to
handle. Poor network design is beyond the scope of this book.

|Message|.6.4 Stubs
~~~~~~~~~~~~~~~~~~~

A stub is the piece of code that implements argument marshalling. Stubs
are typically used to support RPC. On the client side, the stub marshals
the procedure arguments into a message that can be transmitted by means
of the RPC protocol. On the server side, the stub converts the message
back into a set of variables that can be used as arguments to call the
remote procedure. Stubs can either be interpreted or compiled.

In a compilation-based approach, each procedure has a customized client
and server stub. While it is possible to write stubs by hand, they are
typically generated by a stub compiler, based on a description of the
procedure’s interface. This situation is illustrated in :numref:`Figure
%s <fig-stubs>`. Since the stub is compiled, it is usually very efficient.
In an interpretation-based approach, the system provides generic client
and server stubs that have their parameters set by a description of the
procedure’s interface. Because it is easy to change this description,
interpreted stubs have the advantage of being flexible. Compiled stubs
are more common in practice.

.. _fig-stubs:
.. figure:: message/figures/f07-05-9780123850591.png
   :width: 500px
   :align: center

   Stub compiler takes interface description as input and outputs client
   and server stubs.

|Message|.6.5 ProtoBuf Example
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Protocol Buffers (Protobufs, for short) provide a language-neutral and
platform-neutral way of serializing structured data, commonly used
with gRPC. They use a tagged strategy with a canonical intermediate
form, where the stub on both sides is generated from a shared
``.proto`` file.  This specification uses a simple C-like syntax, as
the following example illustrates:

.. code-block:: c

   message Person {
       required string name = 1;
       required int32 id = 2;
       optional string email = 3;

       enum PhoneType {
           MOBILE = 0;
           HOME = 1;
           WORK = 2;
       }

       message PhoneNumber {
           required string number = 1;
           optional PhoneType type = 2 [default = HOME];
       }

       required PhoneNumber phone = 4;
   }

where ``message`` could roughly be interpreted as equivalent to
``typedef struct`` in C. The rest of the example is fairly intuitive,
except that every field is given a numeric identifier to ensure
uniqueness should the specification change over time, and each field can
be annotated as being either ``required`` or ``optional``.

The way Protobufs encode integers is novel. They use a technique called
*varints* (variable length integers) in which each 8-bit byte uses the
most significant bit to indicate whether there are more bytes in the
integer, and the lower seven bits to encode the two’s complement
representation of the next group of seven bits in the value. The least
significant group is first in the serialization.

This means a small integer (less than 128) can be encoded in a single
byte (e.g., the integer 2 is encoded as ``0000 0010``), while for an
integer bigger than 128, more bytes are needed. For example, 365 would
be encoded as

::

   1110 1101 0000 0010

To see this, first drop the most significant bit from each byte, as it
is there to tell us whether we’ve reached the end of the integer. In
this example, the ``1`` in the most significant bit of the first byte
indicates there is more than one byte in the varint:

::

   1110 1101 0000 0010
   → 110 1101  000 0010

Since varints store numbers with the least significant group first, you
next reverse the two groups of seven bits. Then you concatenate them to
get your final value:

::

   000 0010  110 1101
   →  000 0010 || 110 1101
   →  101101101
   →  256 + 64 + 32 + 8 + 4 + 1 = 365

For the larger message specification, you can think of the serialized
byte stream as a collection of key/value pairs, where the key (i.e.,
tag) has two sub-parts: the unique identifier for the field (i.e., those
extra numbers in the example ``.proto`` file) and the *wire type* of the
value (e.g., ``Varint`` is the one example wire type we have seen so
far). Other supported wire types include ``32-bit`` and ``64-bit`` (for
fixed-length integers), and ``length-delimited`` (for strings and
embedded messages). The latter tells you how many bytes long the
embedded message (structure) is, but it’s another ``message``
specification in the ``.proto`` file that tells you how to interpret
those bytes.
