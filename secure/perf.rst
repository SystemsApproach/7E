|TLS|.6  Performance Issues
--------------------------------

At the start of this chapter we presented a picture of how TLS has
been inserted as a layer between the application protocol—HTTP in most
cases—and TCP, the underlying transport protocol. It not a great
surprise that this layering comes with some performance impact. In
particular, there are quite a few round trip times required to execute
a simple request such as "fetch this web page". First we need to set
up the TCP connection, which entails a 3-way handshake. Then we need
to run the handshake protocol of TLS to secure the connection which
can add another two RTTs. Finally, we get to send our HTTP request and
wait for a response. So something as simple as retrieving a very short
web page might take four RTTs.

The solution to these multiple RTTs can partly be addressed by
improvements to TLS. The extra RTT due to TCP can be tackled by
replacing TCP with QUIC, a topic we return to in Chapter |Message|.

In our initial description of the TLS handshake, we described how
Diffie-Hellman is used to established a shared secret, but noted
that the option also exists to use a pre-shared key (PSK). While
out-of-band provisioning of a PSK is possible, a much more common use
of a PSK is to allow session resumption, thus removing the need to go
through another Diffie-Hellman exchange.

An important side-effect of using a pre-shared key is that it becomes
possible to start sending data earlier in the process. This operation
is referred to as "0-RTT Data" because it is possible to start sending
application data along with the handshake material without waiting for
the round trip time of the handshake to elapse. This is an important
step in improving the latency of HTTPS connection establishment and
thus the user experience when browsing the Web.

The idea of session resumption predates TLS 1.3 but it has evolved
somewhat to become more secure. In TLS 1.3, the server may create a
*session ticket* after the completion of the handshake process. The ticket
contains an opaque identifier of the session and a ticket lifetime (as
well as some other fields). The ticket is sent after the handshake
which means it is encrypted much like application data. More than one
ticket can be sent.

A ticket is effectively a label for a previously established
session, which has a shared secret already. When a client
connects to a server to which it was previously connected, it can look
at its stored tickets and, if there are any that have not expired, it
can include one in the first message of a handshake. Along
with the ticket, the client includes something called a "binding",
which is a HMAC calculated over the current handshake message using a
key derived from the *previous* handshake. The effect of this binding
is to tie the new handshake back to the old one, since only a client
that successfully completed the prior handshake can have the key
required to calculate the HMAC. Thus, while an attacker might snoop on
the ticket, it can't do much with it and any attempt to modify the new
handshake message will fail.

When the server sees that the client has sent a ticket, it validates
the binding, and if the HMAC calculation succeeds, then the server and
client now have agreement that they can use a shared secret
established in the prior session. They use a "resumption master
secret" that was calculated and stored in the prior session to derive
a new set of keys for this session. The keys of the new session are
different from those of the prior session to support forward secrecy
(i.e., even if an attacker manages to compromise the key for one
session, they don't gain the ability to decrypt data from other
sessions).

When the server sends its "Finished" message, it calculates the HMAC
over the handshake messages using the agreed-upon new key, and thus
authenticates itself to the client.

On its own, session resumption as just described may not seem that
interesting. It avoids the need for another Diffie-Hellman exchange
but is still requires a round trip time to establish the session. But
because the new session keys are known to both sides before the first
handshake message is sent, session resumption opens up the possibility
of sending "0-RTT data" along with the handshake. 0-RTT data can be
included along with the handshake messages, without waiting one RTT
for keys to be established. This is beneficial from a performance
perspective, especially for short-lived connections, but it comes with
some downsides in terms of security.

There are two main drawbacks to 0-RTT data. The first is that it is
prone to replay attacks in a way that other data transfers are not. If
an attacker can sit between a client and a server, they have the
opportunity to replay 0-RTT data. Exactly how much damage this does is
very much application dependent, so the TLS specifications dictate
that (a) 0-RTT data can only be sent when the application layer
explicitly requests it, i.e., it can't just be an optimization
provided by the socket layer (b) the application must know how to deal
with replays of data sent as 0-RTT, e.g., by only sending 0-RTT
data for operations that are idempotent. For example, sending the
initial GET request in HTTP would usually be an operation that is
idempotent.

The other drawback of 0-RTT data is that it depends on a potentially
long-lived secret. The resumption key used to encrypt 0-RTT data is
derived from shared key material that was established in an earlier
session. This material is retained by the server for some period of
time to allow for future resumptions. If that secret were somehow
compromised at some point in the future, an attacker who had recorded
the 0-RTT data would have the necessary information to decrypt
it. Thus, 0-RTT data lacks forward secrecy. For this reason, the
option exists to generate a new set of keys as part of the session
resumption handshake with a new Diffie-Hellman exchange. This means
that only the data sent in the first RTT lacks forward secrecy, and
the rest of the session is protected by the new ephemeral keys.


All of this work to reduce the setup time of TLS by a single RTT might
seem surprising, but in fact the history of HTTP and HTTPS over TCP is
full of issues with excessive setup times. The very first
implementations of HTTP were quite wasteful of TCP connections,
setting up a new connection for every object on a requested web
page. While this was addressed with persistent TCP connections, the
addition of TLS brought its own latency impact. The next step in the process
of reducing the latency of TLS session establishment involves
rethinking the choice of TCP as the underlying transport. We return to
this topic in Chapter |Message|, in which QUIC is discussed as an
alternative to TCP.

