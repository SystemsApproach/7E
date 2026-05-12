2.3 Electronic Mail
----------------------------------------

Email is one of the oldest network applications. After all, what could
be more natural than wanting to send a message to the user at the other
end of a cross-country link you just managed to get running? It’s the
20th century’s version of “*Mr. Watson, come here… I want to see you.*”
Surprisingly, the pioneers of the ARPANET had not really envisioned
email as a key application when the network was created—remote access to
computing resources was the main design goal—but it turned out to be the
Internet’s original killer app.

As with the web, there are some important distinctions to
make. First, we should distinguish between the user interface (i.e.,
your mail reader) from the underlying message transfer protocols, such
as SMTP (Simple Mail Transfer Protocol) and IMAP (Internet Message
Access Protocol). Second, these transfer protocols are
distinct from the companion standards (RFC 822 [#]_ and MIME, the
Multipurpose Internet Mail Extensions) that define the format of the
messages being exchanged. We begin by looking at the message
format—including why there are two formatting standards—but before
doing that, the explanation for why there are multiple transfer
protocols warrants a quick look.

.. [#] RFC 822 is often used as shorthand for "the
       original, plain text email standard."  It has been updated
       several times, the
       most current version at the time of writing being RFC 5322.

The key abstraction in email is a *mailbox*, which provides a
rendezvous point for an email exchange: SMTP is used to send email to
a mailbox, and IMAP (and its predecessor, POP, the Post Office
Protocol) is used to read email from a mailbox.  The mailbox also
holds an archive of exchanged messages; IMAP is used to browse,
search, read, and manage that archive. As originally designed,
however, the mailbox abstraction was implemented as a file on a
multi-user machine.  This meant the reader was just a program that
directly accessed that file; there was no need for a protocol like
IMAP. As workstations and laptops became more common, users no longer
logged into a shared server, and so there was a need for a second
protocol to retrieve email from that server.

Today, the server process that implements the mailbox abstraction is
most likely implemented as a scalable cloud service. This is certainly
the case if you have a web-based email account such as Gmail.  Reading
web-based mail no longer requires IMAP. Instead you are effectively
using a RESTful API running over HTTPS to access your mailbox.\ [#]_
This suggests an interesting *"What if?"* question. If you were going
to start over with email using today's best practices, what would you
do differently?  One likely answer is that you would define a RESTful
API for email, and use HTTPS to both write messages to a mailbox and
read messages from a mailbox. In fact, the same could be said for many
applications that pre-date the web—their protocols are just
purpose-built Request/Reply protocols. This is a topic we address in
Chapter |Message|.

.. [#] IMAP is still supported to allow standard email clients to
       access Gmail mailboxes.

2.3.1 Message Format
~~~~~~~~~~~~~~~~~~~~

RFC 822 (and its successor RFC 5322) defines messages to have two
parts: a *header* and a *body*.  Both parts are represented in ASCII
text. Originally, the body was assumed to be simple text. This is
still the case, although the subsequent *Multipurpose Internet Mail
Extensions* (MIME) allow the message body to carry all sorts of
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
header lines; the interested reader is referred to RFC 5322.

You might notice that this structure looks very similar to that of
HTTP messages. This is not a coincidence, as email was an obvious
application protocol that the designers of HTTP could use as a model.


The MIME extensions were added in 1992 (and updated quite a few times since
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
might find in a plain RFC 822-style message, while ``text/richtext``
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

As an aside, to make reading mail as painless as possible for
those who may be using text-only mail readers, a MIME message
that consists of regular text only can be encoded using 7-bit ASCII.
There’s also a readable encoding for mostly ASCII data.

Putting this all together, a message that contains some plain text, a
JPEG image, and a PostScript file would look something like this:

::

   MIME-Version: 1.0
   Content-Type: multipart/mixed;
   boundary="-------417CA6E2DE4ABCAFBC5"
   From: Alice Smith <alice@systemsapproach.org>
   To: bob@Princeton.edu
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

As previewed in the introduction to this section, SMTP (*Simple
Mail Transfer Protocol*) is used to send email to one or more
recipient mailboxes. A mailbox, in turn, is an abstraction implemented
either by a server process running on some machine, or as a scalable
service running in the cloud. We'll assume the former in the following
discussion, but the implementation choice is completely independent
from the protocol (as should be the case for all protocols).

This process is called a *Message Transfer Agent (MTA)*, and it runs
on every host that holds a mailbox.\ [#]_ It implements both the
client side of the SMTP protocol (which gets executed when the MTA
needs to send a message somewhere else) and the server side of the
SMTP protocol (which runs continuously waiting for incoming TCP
connections). Note that while anyone can implement an MTA, there are
only a few popular implementations, with ``sendmail`` and ``postfix``
being two widely used open source examples.

.. [#] Not that we need more names for the same thing, but the MTA
       process is also called a *mail daemon*, dating back to its
       original implementation in Unix, where all long-running server
       processes were known as *daemon processes*.

.. _fig-mail:
.. figure:: applications/figures/smtp.png
   :width: 600px
   :align: center

   Sequence of MTAs store and forward email messages. Each MTA writes
   messages to disk (stable storage) so it can fulfill its promise to
   deliver a message once it has accepted it from an upstream MTA.

While it is possible that the MTA on a sender’s machine could
establish an SMTP/TCP connection to the MTA on the recipient’s mail
server, in many cases the mail traverses one or more intermediate MTA
(also called a *mail gateway*) on its route from the sender’s host to
the receiver’s host. The gateway MTA buffers messages on disk, and is
willing to try retransmitting them to the next machine for several
days.  :numref:`Figure %s <fig-mail>` illustrates a two-hop path from
the sender to the receiver.

Why are mail gateways necessary? Why can’t the sender’s host send the
message to the receiver’s host? One reason is that the recipient does
not want to include the specific host on which he or she reads email
in his or her address. A second is scale: in large organizations, it’s
often the case that a number of different machines hold the
mailboxes for the organization. For example, mail delivered to
``bob@princeton.edu`` might first be sent to a University gateway (that
is, to the host named ``princeton.edu``), and then forwarded—involving
a second connection—to the specific machine on which Bob has a mailbox
(perhaps on a department server). The forwarding gateway maintains a
database that maps users into the machine on which their mailbox
resides; the sender need not be aware of this specific name. (The list
of header lines in the message will help you trace the mail gateways
that a given message traversed.)

Independent of how many mail gateways are in the path, an independent
SMTP connection is used between each host to move the message closer
to the recipient. Each SMTP session involves a dialog between the two
MTA processes, with one acting as the client and the other acting as
the server. Multiple messages might be transferred between the two
hosts during a single session. Since RFC 822 defines messages using
ASCII as the base representation, it should come as no surprise to
learn that SMTP is also ASCII based. This means it is possible for a
human at a keyboard to pretend to be an SMTP client program.

SMTP is best understood by a simple example. The following is an
exchange between sending host ``princeton.edu`` and receiving host
``systemsapproach.org`` . In this case, user Bob at Princeton is
trying to send mail to users Alice and Tom at Systems Approach. Extra
blank lines have been added to make the dialog more readable.

.. code-block:: shell

   HELO cs.princeton.edu
   250 Hello daemon@mail.princeton.edu [128.12.169.24]

   MAIL FROM:<bob@princeton.edu>
   250 OK

   RCPT TO:<alice@systemsapproach.org>
   250 OK

   RCPT TO:<tom@systemsapproach.org>
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

There are, of course, many other commands and return codes. For
example, the server can respond to a client’s ``RCPT`` command with a
``251`` code, which indicates that the user does not have a mailbox on
this host, but that the server promises to forward the message onto
another MTA. In other words, the host is functioning as a mail
gateway.  As another example, the client can issue a ``VRFY``
operation to verify a user’s email address, but without actually
sending a message to the user.

The only other point of interest is the arguments to the ``MAIL`` and
``RCPT`` operations; for example, ``FROM:<bob@princeton.edu>`` and
``TO:<alice@systemsapproach.org>``, respectively. These look a lot
like RFC 822 header fields, and in some sense they are. What actually
happens is that the MTA process parses the message to extract the
information it needs to run SMTP. The information it extracts is said
to form an *envelope* for the message. The SMTP client uses this
envelope to parameterize its exchange with the SMTP server. One
historical note: the reason ``sendmail`` became so popular is that no
one wanted to reimplement this message parsing function. While today’s
email addresses look pretty tame (e.g., ``bob@princeton.edu``), this
was not always the case. In the days before everyone was connected to
the Internet, it was not uncommon to see email addresses of the form
``user%host@site!neighbor``.

2.3.3 Mail Reader
~~~~~~~~~~~~~~~~~

The final step is for the user to actually retrieve their messages
from the mailbox, read them, reply to them, and possibly save a copy
for future reference. The user performs all these actions by
interacting with a mail reader. As pointed out earlier, this reader
was originally just a program running on the same machine as the
user’s mailbox, in which case it could simply read and write the file
that implements the mailbox. This was the common case in the
pre-laptop era. Today, most often the user accesses their mailbox from
a remote machine using yet another protocol, such as IMAP (*Internet
Message Access Protocol*) or POP (*Post Office Protocol*). It is
beyond the scope of this book to discuss the user interface aspects of
the mail reader, but it is definitely within our scope to talk about
the access protocol. We consider IMAP, which has largely replaced POP.

IMAP is similar to SMTP in many ways. It is a client-server protocol
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

