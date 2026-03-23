|TCP|.8 TCP Extensions
---------------------------------

We have mentioned at four different points in this chapter that there
are now extensions to TCP that help to mitigate some problem that TCP
faced as the underlying network got faster. These extensions are
designed to have as small an impact on TCP as possible. In particular,
they are realized as options that can be added to the TCP header. (We
glossed over this point earlier, but the reason why the TCP header has a
``HdrLen`` field is that the header can be of variable length; the
variable part of the TCP header contains the options that have been
added.) The significance of adding these extensions as options rather
than changing the core of the TCP header is that hosts can still
communicate using TCP even if they do not implement the options. Hosts
that do implement the optional extensions, however, can take advantage
of them. The two sides agree that they will use the options during TCP’s
connection establishment phase.

The first extension helps to improve TCP’s timeout mechanism. Instead of
measuring the RTT using a coarse-grained event, TCP can read the actual
system clock when it is about to send a segment, and put this time—think
of it as a 32-bit *timestamp*\ —in the segment’s header. The receiver then
echoes this timestamp back to the sender in its acknowledgment, and the
sender subtracts this timestamp from the current time to measure the
RTT. In essence, the timestamp option provides a convenient place for
TCP to store the record of when a segment was transmitted; it stores the
time in the segment itself. Note that the endpoints in the connection do
not need synchronized clocks, since the timestamp is written and read at
the same end of the connection.

The second extension addresses the problem of TCP’s 32-bit
``SequenceNum`` field wrapping around too soon on a high-speed network.
Rather than define a new 64-bit sequence number field, TCP uses the
32-bit timestamp just described to effectively extend the sequence
number space. In other words, TCP decides whether to accept or reject a
segment based on a 64-bit identifier that has the ``SequenceNum`` field
in the low-order 32 bits and the timestamp in the high-order 32 bits.
Since the timestamp is always increasing, it serves to distinguish
between two different incarnations of the same sequence number. Note
that the timestamp is being used in this setting only to protect against
wraparound; it is not treated as part of the sequence number for the
purpose of ordering or acknowledging data.

The third extension allows TCP to advertise a larger window, thereby
allowing it to fill larger delay × bandwidth pipes that are made
possible by high-speed networks. This extension involves an option that
defines a *scaling factor* for the advertised window. That is, rather
than interpreting the number that appears in the ``AdvertisedWindow``
field as indicating how many bytes the sender is allowed to have
unacknowledged, this option allows the two sides of TCP to agree that
the ``AdvertisedWindow`` field counts larger chunks (e.g., how many
16-byte units of data the sender can have unacknowledged). In other
words, the window scaling option specifies how many bits each side
should left-shift the ``AdvertisedWindow`` field before using its
contents to compute an effective window.

The fourth extension allows TCP to augment its cumulative acknowledgment
with selective acknowledgments of any additional segments that have been
received but aren’t contiguous with all previously received segments.
This is the *selective acknowledgment*, or *SACK*, option. When the SACK
option is used, the receiver continues to acknowledge segments
normally—the meaning of the ``Acknowledge`` field does not change—but it
also uses optional fields in the header to acknowledge any additional
blocks of received data. This allows the sender to retransmit just the
segments that are missing according to the selective acknowledgment.

Without SACK, there are only two reasonable strategies for a sender. The
pessimistic strategy responds to a timeout by retransmitting not just
the segment that timed out, but any segments transmitted subsequently.
In effect, the pessimistic strategy assumes the worst: that all those
segments were lost. The disadvantage of the pessimistic strategy is that
it may unnecessarily retransmit segments that were successfully received
the first time. The other strategy is the optimistic strategy, which
responds to a timeout by retransmitting only the segment that timed out.
In effect, the optimistic approach assumes the rosiest scenario: that
only the one segment has been lost. The disadvantage of the optimistic
strategy is that it is very slow, unnecessarily, when a series of
consecutive segments has been lost, as might happen when there is
congestion. It is slow because each segment’s loss is not discovered
until the sender receives an ACK for its retransmission of the previous
segment. So it consumes one RTT per segment until it has retransmitted
all the segments in the lost series. With the SACK option, a better
strategy is available to the sender: retransmit just the segments that
fill the gaps between the segments that have been selectively
acknowledged.

These extensions, by the way, are not the full story. We’ll see some
more extensions in the next chapter when we look at how TCP handles
congestion. The Internet Assigned Numbers Authority (IANA) keeps track
of all the options that are defined for TCP (and for many other Internet
protocols).

.. Should reference the updated TCP spec.
