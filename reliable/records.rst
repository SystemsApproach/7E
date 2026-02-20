11.7 Record Boundaries
-----------------------

Since TCP is a byte-stream protocol, the number of bytes written by the
sender are not necessarily the same as the number of bytes read by the
receiver. For example, the application might write 8 bytes, then
2 bytes, then 20 bytes to a TCP connection, while on the receiving side
the application reads 5 bytes at a time inside a loop that iterates 6
times. TCP does not interject record boundaries between the 8th and 9th
bytes, nor between the 10th and 11th bytes. This is in contrast to a
message-oriented protocol, such as UDP, in which the message that is
sent is exactly the same length as the message that is received.

Even though TCP is a byte-stream protocol, it has two different features
that can be used by the sender to insert record boundaries into this
byte stream, thereby informing the receiver how to break the stream of
bytes into records. (Being able to mark record boundaries is useful, for
example, in many database applications.) Both of these features were
originally included in TCP for completely different reasons; they have
only come to be used for this purpose over time.

The first mechanism is the urgent data feature, as implemented by the
``URG`` flag and the ``UrgPtr`` field in the TCP header. Originally, the
urgent data mechanism was designed to allow the sending application to
send *out-of-band* data to its peer. By “out of band” we mean data that
is separate from the normal flow of data (e.g., a command to interrupt
an operation already under way). This out-of-band data was identified in
the segment using the ``UrgPtr`` field and was to be delivered to the
receiving process as soon as it arrived, even if that meant delivering
it before data with an earlier sequence number. Over time, however, this
feature has not been used, so instead of signifying “urgent” data, it
has come to be used to signify “special” data, such as a record marker.
This use has developed because, as with the push operation, TCP on the
receiving side must inform the application that urgent data has arrived.
That is, the urgent data in itself is not important. It is the fact that
the sending process can effectively send a signal to the receiver that
is important.

The second mechanism for inserting end-of-record markers into a byte is
the *push* operation. Originally, this mechanism was designed to allow
the sending process to tell TCP that it should send (flush) whatever
bytes it had collected to its peer. The *push* operation can be used to
implement record boundaries because the specification says that TCP must
send whatever data it has buffered at the source when the application
says push, and, optionally, TCP at the destination notifies the
application whenever an incoming segment has the PUSH flag set. If the
receiving side supports this option (the socket interface does not),
then the push operation can be used to break the TCP stream into
records.

Of course, the application program is always free to insert record
boundaries without any assistance from TCP. For example, it can send a
field that indicates the length of a record that is to follow, or it
can insert its own record boundary markers into the data stream. The
fact that both HTTP and SMTP are ASCII text based application
protocols, and so use ``<CTLF>`` as a line delimintors, is no
accident. It is in part how they adapted to TCP offering a byte-stream
service.
