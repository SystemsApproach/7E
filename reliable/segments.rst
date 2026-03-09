11.2 Segments
-----------------------------

When we describe TCP as a byte-oriented protocol, we are describing
the abstraction that it offers to software running above it. This
means that the sender writes bytes into a TCP connection and the
receiver reads bytes out of the connection.  “Byte stream” describes
the service TCP offers to application processes.  TCP does not,
itself, transmit individual bytes over the Internet. Instead, TCP on
the source host buffers enough bytes from the sending process to fill
a reasonably sized packet and then sends this packet to its peer on
the destination host. TCP on the destination host then empties the
contents of the packet into a receive buffer, and the receiving
process reads from this buffer at its leisure.  This situation is
illustrated in :numref:`Figure %s <fig-tcp-stream>`, which, for
simplicity, shows data flowing in only one direction. Remember that,
in general, a single TCP connection supports byte streams flowing in
both directions.

.. _fig-tcp-stream:
.. figure:: reliable/figures/f05-03-9780123850591.png
   :width: 500px
   :align: center

   How TCP manages a byte stream.

The packets exchanged between TCP peers in :numref:`Figure %s
<fig-tcp-stream>` are called *segments*, since each one carries a
segment of the byte stream. Each TCP segment contains the header
schematically depicted in :numref:`Figure %s <fig-tcp-format>`. The
relevance of most of these fields will become apparent throughout this
section. For now, we simply introduce them.

.. _fig-tcp-format:
.. figure:: introduction/figures/tcphdr.png
   :width: 400px
   :align: center

   TCP header format.

The ``SrcPort`` and ``DstPort`` fields identify the source and
destination ports, respectively. These two fields, plus the source and
destination IP addresses, combine to uniquely identify each TCP
connection. That is, TCP’s demux key is given by the 4-tuple

.. code:: c

   (SrcPort, SrcIPAddr, DstPort, DstIPAddr)

Note that because TCP connections come and go, it is possible for a
connection between a particular pair of ports to be established, used to
send and receive data, and closed, and then at a later time for the same
pair of ports to be involved in a second connection. We sometimes refer
to this situation as two different *incarnations* of the same
connection.

The ``Acknowledgement``, ``SequenceNum``, and ``AdvertisedWindow``
fields are all involved in TCP’s sliding window algorithm. Because TCP
is a byte-oriented protocol, each byte of data has a sequence number.
The ``SequenceNum`` field contains the sequence number for the first
byte of data carried in that segment, and the ``Acknowledgement`` and
``AdvertisedWindow`` fields carry information about the flow of data
going in the other direction. To simplify our discussion, we ignore
the fact that data can flow in both directions, and we concentrate on
data that has a particular ``SequenceNum`` flowing in one direction
and ``Acknowledgement`` and ``AdvertisedWindow`` values flowing in the
opposite direction, as illustrated in :numref:`Figure %s
<fig-tcp-flow>`. The use of these three fields is described more fully
later in this chapter.

.. _fig-tcp-flow:
.. figure:: reliable/figures/f05-05-9780123850591.png
   :width: 500px
   :align: center

   Simplified illustration (showing only one direction)
   of the TCP process, with data flow in one direction and ACKs in
   the other.

The 6-bit ``Flags`` field is used to relay control information between
TCP peers. The possible flags include ``SYN``, ``FIN``, ``RESET``,
``PUSH``, ``URG``, and ``ACK``. The ``SYN`` (synchronize) and ``FIN``
(finish) flags are used when establishing and terminating a TCP
connection, respectively.  The ``ACK`` flag is set any time the
``Acknowledgement`` field is valid, implying that the receiver should
pay attention to it. The ``URG`` flag signifies that this segment
contains urgent data. When this flag is set, the ``UrgPtr`` field
indicates where the nonurgent data contained in this segment
begins. The urgent data is contained at the front of the segment body,
up to and including a value of ``UrgPtr`` bytes into the segment. The
``PUSH`` flag signifies that the sender invoked the push operation,
which indicates to the receiving side of TCP that it should notify the
receiving process of this fact.  Finally, the ``RESET`` flag signifies
that the receiver has become confused—for example, because it received
a segment it did not expect to receive—and so wants to abort the
connection. We discuss the usage of these flags later in this chapter.

Finally, the ``Checksum`` field is computed over the TCP header, the
TCP data, and extra fields known as a *pseudoheader*, which is simply
the source address, destination address, and length fields from the IP
header. Also, since the TCP header is of variable length (options can
be attached after the mandatory fields), a ``HdrLen`` field is
included that gives the length of the header in 32-bit words. This
field is also known as the ``Offset`` field, since it measures the
offset from the start of the packet to the start of the data.
