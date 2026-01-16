
2.3 Electronic Mail (Email)
----------------------------------------

Email is one of the oldest network applications. After all, what could
be more natural than wanting to send a message to the user at the other
end of a cross-country link you just managed to get running? It’s the
20th century’s version of “*Mr. Watson, come here… I want to see you.*”
Surprisingly, the pioneers of the ARPANET had not really envisioned
email as a key application when the network was created—remote access to
computing resources was the main design goal—but it turned out to be the
Internet’s original killer app.

As noted above, it is important (1) to distinguish the user interface
(i.e., your mail reader) from the underlying message transfer protocols
(such as SMTP or IMAP), and (2) to distinguish between this transfer
protocol and a companion standard (RFC 822 and MIME) that defines the
format of the messages being exchanged. We start by looking at the
message format.

2.3.1 Message Format
~~~~~~~~~~~~~~~~~~~~

RFC 822 defines messages to have two parts: a *header* and a *body*.
Both parts are represented in ASCII text. Originally, the body was
assumed to be simple text. This is still the case, although RFC 822 has
been augmented by MIME to allow the message body to carry all sorts of
data. This data is still represented as ASCII text, but because it may
be an encoded version of, say, a JPEG image, it’s not necessarily
readable by human users. More on MIME in a moment.

The message header is a series of ``<CRLF>``-terminated lines.
(``<CRLF>`` stands for carriage-return plus line-feed, which are a
pair of ASCII control characters often used to indicate the end of a
line of text.) The header is separated from the message body by a
blank line.  Each header line contains a type and value separated by a
colon. Many of these header lines are familiar to users, since they
are asked to fill them out when they compose an email message; for
example, the ``To:`` header identifies the message recipient, and the
``Subject:`` header says something about the purpose of the
message. Other headers are filled in by the underlying mail delivery
system. Examples include ``Date:`` (when the message was transmitted),
``From:`` (what user sent the message), and ``Received:`` (each mail
server that handled this message). There are, of course, many other
header lines; the interested reader is referred to RFC 822.

You might notice that this structure looks very similar to that of
HTTP messages. This is not a coincidence, as email was an obvious
application protocol that the designers of HTTP could use as a model. 

RFC 822 was extended in 1993 (and updated quite a few times since
then) to allow email messages to carry many different types of data:
audio, video, images, PDF documents, and so on. MIME consists of three
basic pieces. The first piece is a collection of header lines that
augment the original set defined by RFC 822. These header lines
describe, in various ways, the data being carried in the message
body. They include ``MIME-Version:`` (the version of MIME being used),
``Content-Description:`` (a human-readable description of what’s in
the message, analogous to the ``Subject:`` line), ``Content-Type:``
(the type of data contained in the message), and
``Content-Transfer-Encoding:`` (how the data in the message body is
encoded).

The second piece is definitions for a set of content types (and
subtypes). For example, MIME defines several different image types,
including ``image/gif`` and ``image/jpeg``, each with the obvious
meaning. As another example, ``text/plain`` refers to simple text you
might find in a vanilla 822-style message, while ``text/richtext``
denotes a message that contains “marked up” text (text using special
fonts, italics, etc.). As a third example, MIME defines an
``application`` type, where the subtypes correspond to the output of
different application programs (e.g., ``application/postscript`` and
``application/msword``).

MIME also defines a ``multipart`` type that says how a message carrying
more than one data type is structured. This is like a programming
language that defines both base types (e.g., integers and floats) and
compound types (e.g., structures and arrays). One possible ``multipart``
subtype is ``mixed``, which says that the message contains a set of
independent data pieces in a specified order. Each piece then has its
own header line that describes the type of that piece.

The third piece is a way to encode the various data types so they can be
shipped in an ASCII email message. The problem is that, for some data
types (a JPEG image, for example), any given 8-bit byte in the image
might contain one of 256 different values. Only a subset of these values
are valid ASCII characters. It is important that email messages contain
only ASCII, because they might pass through a number of intermediate
systems (gateways, as described below) that assume all email is ASCII
and would corrupt the message if it contained non-ASCII characters. To
address this issue, MIME uses a straightforward encoding of binary data
into the ASCII character set. The encoding is called ``base64``. The
idea is to map every three bytes of the original binary data into four
ASCII characters. This is done by grouping the binary data into 24-bit
units and breaking each such unit into four 6-bit pieces. Each 6-bit
piece maps onto one of 64 valid ASCII characters; for example, 0 maps
onto A, 1 maps onto B, and so on. If you look at a message that has been
encoded using the base64 encoding scheme, you’ll notice only the 52
upper- and lowercase letters, the 10 digits 0 through 9, and the special
characters + and /. These are the first 64 values in the ASCII character
set.

As one aside, so as to make reading mail as painless as possible for
those who still insist on using text-only mail readers, a MIME message
that consists of regular text only can be encoded using 7-bit ASCII.
There’s also a readable encoding for mostly ASCII data.

Putting this all together, a message that contains some plain text, a
JPEG image, and a PostScript file would look something like this:

::

   MIME-Version: 1.0
   Content-Type: multipart/mixed;
   boundary="-------417CA6E2DE4ABCAFBC5"
   From: Alice Smith <Alice@systemsapproach.org>
   To: Bob@cs.Princeton.edu
   Subject: promised material
   Date: Mon, 07 Sep 1998 19:45:19 -0400

   ---------417CA6E2DE4ABCAFBC5
   Content-Type: text/plain; charset=us-ascii
   Content-Transfer-Encoding: 7bit

   Bob,

   Here are the jpeg image and draft report I promised.

   --Alice

   ---------417CA6E2DE4ABCAFBC5
   Content-Type: image/jpeg
   Content-Transfer-Encoding: base64
   ... unreadable encoding of a jpeg figure
   ---------417CA6E2DE4ABCAFBC5
   Content-Type: application/postscript; name="draft.ps"
   Content-Transfer-Encoding: 7bit
   ... readable encoding of a PostScript document

In this example, the line in the message header says that this message
contains various pieces, each denoted by a character string that does
not appear in the data itself. Each piece then has its own
``Content-Type`` and ``Content-Transfer-Encoding`` lines.

2.3.2 Message Transfer
~~~~~~~~~~~~~~~~~~~~~~

For many years, the majority of email was moved from host to host using
only SMTP. While SMTP continues to play a central role, it is now just
one email protocol of several, Internet Message Access Protocol (IMAP)
and Post Office Protocol (POP) being two other important protocols for
retrieving mail messages. We’ll begin our discussion by looking at SMTP,
and move on to IMAP below.

To place SMTP in the right context, we need to identify the key components.
First, users interact with a *mail reader* when they compose, file,
search, and read their email. Countless mail readers are available, just
like there are many web browsers to choose from. In the early days of
the Internet, users typically logged into the machine on which their
*mailbox* resided, and the mail reader they invoked was a local
application program that extracted messages from the file system. Today,
of course, users remotely access their mailbox from their laptop or
smartphone; they do not first log into the host that stores their mail
(a mail server). A second mail transfer protocol, such as POP or IMAP,
is used to remotely download email from a mail server to the user’s
device. To further complicate matters, many users just use a web
browser to read their mail, which in some ways brings us full circle
back to the days of logging into a specific machine to read mail. 

Second, there is a *mail daemon* (or process) running on each host that
holds a mailbox. You can think of this process, also called a *message
transfer agent* (MTA), as playing the role of a post office: users (or
their mail readers) give the daemon messages they want to send to other
users, the daemon uses SMTP running over TCP to transmit the message to
a daemon running on another machine, and the daemon puts incoming
messages into the user’s mailbox (where that user’s mail reader can
later find them). Since SMTP is a protocol that anyone could implement,
in theory there could be many different implementations of the mail
daemon. It turns out, though, that there are only a few popular
implementations, with the old ``sendmail`` program from Berkeley Unix
and ``postfix`` being the most widespread.

.. _fig-mail:
.. figure:: applications/figures/f09-01-9780123850591.png
   :width: 600px
   :align: center

   Sequence of mail gateways store and forward email messages.

While it is certainly possible that the MTA on a sender’s machine
establishes an SMTP/TCP connection to the MTA on the recipient’s mail
server, in many cases the mail traverses one or more *mail gateways*
on its route from the sender’s host to the receiver’s host. Like the
end hosts, these gateways also run a message transfer agent
process. These intermediate nodes are called *gateways* since their
job is to store and forward email messages, much like an IP router
(which was at one time also referred to as a gateway) stores and forwards
IP datagrams. The key difference is that a mail gateway typically
buffers messages on disk and is willing to try retransmitting them to
the next machine for several days, while an IP router buffers
datagrams in memory and is only willing to retry transmitting them for
a fraction of a second. :numref:`Figure %s <fig-mail>` illustrates a
two-hop path from the sender to the receiver.

Why, you might ask, are mail gateways necessary? Why can’t the sender’s
host send the message to the receiver’s host? One reason is that the
recipient does not want to include the specific host on which he or she
reads email in his or her address. Another is scale: In large
organizations, it’s often the case that a number of different machines
hold the *mailboxes* for the organization. For example, mail delivered
to ``bob@cs.princeton.edu`` is first sent to a mail gateway in the CS
Department at Princeton (that is, to the host named
``cs.princeton.edu``), and then forwarded—involving a second
connection—to the specific machine on which Bob has a mailbox. The
forwarding gateway maintains a database that maps users into the machine
on which their mailbox resides; the sender need not be aware of this
specific name. (The list of header lines in the message will help you
trace the mail gateways that a given message traversed.) Yet another
reason, particularly true in the early days of email, is that the
machine that hosts any given user’s mailbox may not always be up or
reachable, in which case the mail gateway holds the message until it can
be delivered.

Independent of how many mail gateways are in the path, an independent
SMTP connection is used between each host to move the message closer to
the recipient. Each SMTP session involves a dialog between the two mail
daemons, with one acting as the client and the other acting as the
server. Multiple messages might be transferred between the two hosts
during a single session. Since RFC 822 defines messages using ASCII as
the base representation, it should come as no surprise to learn that
SMTP is also ASCII based. This means it is possible for a human at a
keyboard to pretend to be an SMTP client program.

SMTP is best understood by a simple example. The following is an
exchange between sending host ``cs.princeton.edu`` and receiving host
``systemsapproach.org`` . In this case, user Bob at Princeton is trying to send
mail to users Alice and Tom at Systems Approach. Extra blank lines have been added
to make the dialog more readable.

.. code-block:: shell

   HELO cs.princeton.edu
   250 Hello daemon@mail.cs.princeton.edu [128.12.169.24]

   MAIL FROM:<Bob@cs.princeton.edu>
   250 OK

   RCPT TO:<Alice@systemsapproach.org>
   250 OK

   RCPT TO:<Tom@systemsapproach.org>
   550 No such user here

   DATA
   354 Start mail input; end with <CRLF>.<CRLF>
   Blah blah blah...
   ...etc. etc. etc.
   <CRLF>.<CRLF>
   250 OK

   QUIT
   221 Closing connection

As you can see, SMTP involves a sequence of exchanges between the
client and the server. In each exchange, the client posts a command
(e.g., ``QUIT``) and the server responds with a code (e.g., ``250``,
``550``, ``354``, ``221``). The server also returns a human-readable
explanation for the code (e.g., ``No such user here``).  In this
particular example, the client first identifies itself to the server
with the ``HELO`` command. It gives its domain name as an
argument. The server verifies that this name corresponds to the IP
address being used by the TCP connection; you’ll notice the server
states this IP address back to the client. The client then asks the
server if it is willing to accept mail for two different users; the
server responds by saying “yes” to one and “no” to the other. Then the
client sends the message, which is terminated by a line with a single
period (“.”) on it. Finally, the client terminates the connection.

There are, of course, many other commands and return codes. For example,
the server can respond to a client’s ``RCPT`` command with a ``251``
code, which indicates that the user does not have a mailbox on this
host, but that the server promises to forward the message onto another
mail daemon. In other words, the host is functioning as a mail gateway.
As another example, the client can issue a ``VRFY`` operation to verify
a user’s email address, but without actually sending a message to the
user.

The only other point of interest is the arguments to the ``MAIL`` and
``RCPT`` operations; for example, ``FROM:<Bob@cs.princeton.edu>`` and
``TO:<Alice@systemsapproach.org>``, respectively. These look a lot like 822 header
fields, and in some sense they are. What actually happens is that the
mail daemon parses the message to extract the information it needs to
run SMTP. The information it extracts is said to form an *envelope* for
the message. The SMTP client uses this envelope to parameterize its
exchange with the SMTP server. One historical note: The reason
``sendmail`` became so popular is that no one wanted to reimplement this
message parsing function. While today’s email addresses look pretty tame
(e.g., ``Bob@cs.princeton.edu``), this was not always the case. In the
days before everyone was connected to the Internet, it was not uncommon
to see email addresses of the form ``user%host@site!neighbor``.

2.3.3 Mail Reader
~~~~~~~~~~~~~~~~~

The final step is for the user to actually retrieve their messages
from the mailbox, read them, reply to them, and possibly save a copy for
future reference. The user performs all these actions by interacting
with a mail reader. As pointed out earlier, this reader was originally
just a program running on the same machine as the user’s mailbox, in
which case it could simply read and write the file that implements the
mailbox. This was the common case in the pre-laptop era. Today, most
often the user accesses their mailbox from a remote machine using
yet another protocol, such as POP or IMAP. It is beyond the scope of
this book to discuss the user interface aspects of the mail reader, but
it is definitely within our scope to talk about the access protocol. We
consider IMAP, in particular.

IMAP is similar to SMTP in many ways. It is a client/server protocol
running over TCP, where the client (running on the user’s desktop
machine) issues commands in the form of ``<CRLF>``-terminated ASCII
text lines and the mail server (running on the machine that maintains
the user’s mailbox) responds in kind. The exchange begins with
authentication of the client and the client identifying the mailbox
the user wants to access. This can be represented by the simple state
transition diagram shown in :numref:`Figure %s <fig-imap>`. In this
diagram, ``LOGIN`` and ``LOGOUT`` are example commands that the client
can issue, while ``OK`` is one possible server response. Other common
commands include ``FETCH`` and ``EXPUNGE``, with the obvious
meanings. Additional server responses include ``NO`` (client does not
have permission to perform that operation) and ``BAD`` (command is ill
formed).

.. _fig-imap:
.. figure:: applications/figures/f09-02-9780123850591.png
   :width: 400px
   :align: center

   IMAP state transition diagram.

When the user asks to ``FETCH`` a message, the server returns it in
MIME format and the mail reader decodes it. In addition to the message
itself, IMAP also defines a set of message *attributes* that are
exchanged as part of other commands, independent of transferring the
message itself. Message attributes include information like the size
of the message and, more interestingly, various *flags* associated
with the message (e.g., ``Seen``, ``Answered``, ``Deleted``, and
``Recent``). These flags are used to keep the client and server
synchronized; that is, when the user deletes a message in the mail
reader, the client needs to report this fact to the mail server.
Later, should the user decide to expunge all deleted messages, the
client issues an ``EXPUNGE`` command to the server, which knows to
actually remove all earlier deleted messages from the mailbox.

Finally, note that when the user replies to a message, or sends a new
message, the mail reader does not forward the message from the client to
the mail server using IMAP, but it instead uses SMTP. This means that
the user’s mail server is effectively the first mail gateway traversed
along the path from the desktop to the recipient’s mailbox.

