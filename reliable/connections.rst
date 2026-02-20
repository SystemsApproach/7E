11.3 Connection Establishment
-------------------------------------

A TCP connection begins with a client (caller) doing an active open to a
server (callee). Assuming that the server had earlier done a passive
open, the two sides engage in an exchange of messages to establish the
connection. (Recall from Chapter 2 that a party wanting to initiate a
connection performs an active open, while a party willing to accept a
connection does a passive open.\ [#]_) Only after this connection
establishment phase is over do the two sides begin sending data.
Likewise, as soon as a participant is done sending data, it closes one
direction of the connection, which causes TCP to initiate a round of
connection termination messages. Notice that, while connection setup is
an asymmetric activity (one side does a passive open and the other side
does an active open), connection teardown is symmetric (each side has to
close the connection independently). Therefore, it is possible for one
side to have done a close, meaning that it can no longer send data, but
for the other side to keep the other half of the bidirectional
connection open and to continue sending data.

.. [#] TCP actually allows connection setup to be symmetric, with both
       sides trying to open the connection at the same time, but the
       common case is for one side to do an active open and the other
       side to do a passive open.

11.3.1 Three-Way Handshake
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The algorithm TCP uses to establish and terminate a connection is
called a *three-way handshake*. We first describe the basic algorithm
and then show how it is used by TCP. The three-way handshake involves
the exchange of three messages between the client and the server, as
illustrated by the timeline given in :numref:`Figure %s <fig-twh-timeline>`.

.. _fig-twh-timeline:
.. figure:: reliable/figures/f05-06-9780123850591.png
   :width: 400px
   :align: center

   Timeline for three-way handshake algorithm.

The idea is that two parties want to agree on a set of parameters,
which, in the case of opening a TCP connection, are the starting
sequence numbers the two sides plan to use for their respective byte
streams. In general, the parameters might be any facts that each side
wants the other to know about. First, the client (the active
participant) sends a segment to the server (the passive participant)
stating the initial sequence number it plans to use (``Flags`` =
``SYN``, ``SequenceNum`` = x). The server then responds with a single
segment that both acknowledges the client’s sequence number (``Flags =
ACK, Ack = x + 1``) and states its own beginning sequence number
(``Flags = SYN, SequenceNum = y``). That is, both the ``SYN`` and
``ACK`` bits are set in the ``Flags`` field of this second message.
Finally, the client responds with a third segment that acknowledges
the server’s sequence number (``Flags = ACK, Ack = y + 1``). The
reason why each side acknowledges a sequence number that is one larger
than the one sent is that the ``Acknowledgement`` field actually
identifies the “next sequence number expected,” thereby implicitly
acknowledging all earlier sequence numbers. Although not shown in this
timeline, a timer is scheduled for each of the first two segments, and
if the expected response is not received the segment is retransmitted.

You may be asking yourself why the client and server have to exchange
starting sequence numbers with each other at connection setup time. It
would be simpler if each side simply started at some “well-known”
sequence number, such as 0. In fact, the TCP specification requires that
each side of a connection select an initial starting sequence number at
random. The reason for this is to protect against two incarnations of
the same connection reusing the same sequence numbers too soon—that is,
while there is still a chance that a segment from an earlier incarnation
of a connection might interfere with a later incarnation of the
connection.

11.3.2 State-Transition Diagram
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

TCP is complex enough that its specification includes a state-transition
diagram. A copy of this diagram is given in :numref:`Figure %s <fig-tcp-std>`.
This diagram shows only the states involved in opening a connection
(everything above ESTABLISHED) and in closing a connection (everything
below ESTABLISHED). Everything that goes on while a connection is
open—that is, the operation of the sliding window algorithm—is hidden in
the ESTABLISHED state.

.. _fig-tcp-std:
.. figure:: reliable/figures/f05-07-9780123850591.png
   :width: 600px
   :align: center

   TCP state-transition diagram.

TCP’s state-transition diagram is fairly easy to understand. Each box
denotes a state that one end of a TCP connection can find itself in. All
connections start in the CLOSED state. As the connection progresses, the
connection moves from state to state according to the arcs. Each arc is
labeled with a tag of the form *event/action*. Thus, if a connection is
in the LISTEN state and a SYN segment arrives (i.e., a segment with the
``SYN`` flag set), the connection makes a transition to the SYN_RCVD
state and takes the action of replying with an ``ACK+SYN`` segment.

Notice that two kinds of events trigger a state transition: (1) a
segment arrives from the peer (e.g., the event on the arc from LISTEN
to SYN_RCVD), or (2) the local application process invokes an
operation on TCP (e.g., the *active open* event on the arc from CLOSED
to SYN_SENT).  In other words, TCP’s state-transition diagram
effectively defines the *semantics* of both its peer-to-peer interface
and its service interface. The *syntax* of these two interfaces is
given by the segment format (as illustrated in :numref:`Figure %s
<fig-tcp-format>`) and by some application programming interface, such
as the socket API, respectively.

Now let’s trace the typical transitions taken through the diagram in
:numref:`Figure %s <fig-tcp-std>`. Keep in mind that at each end of the
connection, TCP makes different transitions from state to state. When
opening a connection, the server first invokes a passive open operation
on TCP, which causes TCP to move to the LISTEN state. At some later
time, the client does an active open, which causes its end of the
connection to send a SYN segment to the server and to move to the
SYN_SENT state. When the SYN segment arrives at the server, it moves to
the SYN_RCVD state and responds with a SYN+ACK segment. The arrival of
this segment causes the client to move to the ESTABLISHED state and to
send an ACK back to the server. When this ACK arrives, the server
finally moves to the ESTABLISHED state. In other words, we have just
traced the three-way handshake.

There are three things to notice about the connection establishment half
of the state-transition diagram. First, if the client’s ACK to the
server is lost, corresponding to the third leg of the three-way
handshake, then the connection still functions correctly. This is
because the client side is already in the ESTABLISHED state, so the
local application process can start sending data to the other end. Each
of these data segments will have the ``ACK`` flag set, and the correct
value in the ``Acknowledgement`` field, so the server will move to the
ESTABLISHED state when the first data segment arrives. This is actually
an important point about TCP—every segment reports what sequence number
the sender is expecting to see next, even if this repeats the same
sequence number contained in one or more previous segments.

The second thing to notice about the state-transition diagram is that
there is a funny transition out of the LISTEN state whenever the local
process invokes a *send* operation on TCP. That is, it is possible for a
passive participant to identify both ends of the connection (i.e.,
itself and the remote participant that it is willing to have connect to
it), and then for it to change its mind about waiting for the other side
and instead actively establish the connection. To the best of our
knowledge, this is a feature of TCP that no application process actually
takes advantage of.

The final thing to notice about the diagram is the arcs that are not
shown. Specifically, most of the states that involve sending a segment
to the other side also schedule a timeout that eventually causes the
segment to be resent if the expected response does not happen. These
retransmissions are not depicted in the state-transition diagram. If
after several tries the expected response does not arrive, TCP gives up
and returns to the CLOSED state.

Turning our attention now to the process of terminating a connection,
the important thing to keep in mind is that the application process on
both sides of the connection must independently close its half of the
connection. If only one side closes the connection, then this means it
has no more data to send, but it is still available to receive data from
the other side. This complicates the state-transition diagram because it
must account for the possibility that the two sides invoke the *close*
operator at the same time, as well as the possibility that first one
side invokes close and then, at some later time, the other side invokes
close. Thus, on any one side there are three combinations of transitions
that get a connection from the ESTABLISHED state to the CLOSED state:

-  This side closes first:
   ESTABLISHED :math:`\rightarrow`
   FIN_WAIT_1 :math:`\rightarrow` FIN_WAIT_2 :math:`\rightarrow` TIME_WAIT :math:`\rightarrow` CLOSED.

-  The other side closes first:
   ESTABLISHED :math:`\rightarrow`
   CLOSE_WAIT :math:`\rightarrow` LAST_ACK :math:`\rightarrow` CLOSED.

-  Both sides close at the same time:
   ESTABLISHED :math:`\rightarrow`
   FIN_WAIT_1 :math:`\rightarrow` CLOSING :math:`\rightarrow` TIME_WAIT :math:`\rightarrow` CLOSED.

There is actually a fourth, although rare, sequence of transitions that
leads to the CLOSED state; it follows the arc from FIN_WAIT_1 to
TIME_WAIT. We leave it as an exercise for you to figure out what
combination of circumstances leads to this fourth possibility.

The main thing to recognize about connection teardown is that a
connection in the TIME_WAIT state cannot move to the CLOSED state until
it has waited for two times the maximum amount of time an IP datagram
might live in the Internet (i.e., 120 seconds). The reason for this is
that, while the local side of the connection has sent an ACK in response
to the other side’s FIN segment, it does not know that the ACK was
successfully delivered. As a consequence, the other side might
retransmit its FIN segment, and this second FIN segment might be delayed
in the network. If the connection were allowed to move directly to the
CLOSED state, then another pair of application processes might come
along and open the same connection (i.e., use the same pair of port
numbers), and the delayed FIN segment from the earlier incarnation of
the connection would immediately initiate the termination of the later
incarnation of that connection.
