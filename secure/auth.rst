|TLS|.4 Session Establishment
-----------------------------
So far we described how to encrypt messages, build message
authentication codes, and distribute keys. It might seem
as if all we have to do to make a protocol secure is append an
authentication code to every message and, if we want confidentiality,
encrypt the message.

There are two main reasons why it’s not that simple. First, there is
the problem of a *replay attack*: an adversary retransmitting a copy
of a message that was previously sent. One could imagine, for example,
that a message in which you place an order for some item on a website
could be replayed, appearing to the website as though you had ordered
more of the same. Even though it wasn’t the original incarnation of
the message, its authentication code would still be valid; after all,
the message was created by you, and it wasn’t modified. Clearly, we
need a solution that ensures *freshness*.

We also need to ensure *timeliness*. An adversary might merely
delay your message (by intercepting and later replaying it), so that
it is received at a time when it is no longer appropriate. For
example, an adversary could delay your order to buy stock from an
auspicious time to a time when you would not have wanted to
buy. Although this message would in a sense be fresh (it hasn't been
sent before), it wouldn’t be timely.

Freshness and timeliness may be considered aspects of
integrity. Ensuring them usually requires a nontrivial, back-and-forth
protocol. The TLS Handshake Protocol is such a protocol. There is,
however, one opportunity for replay attacks in TLS, when data is sent
before the handshake is complete. This is known as 0-RTT data because
it avoids the round-trip time associated with connection
establishement before sending data. It is an optional performance
enhancement that we cover in the next section.


.. TODO need to explain how these issues are addressed in TLS

When establishing a secure connection on the Web, it is typical to
authenticate the server, while client authentication is
optional. Session establishment includes both that initial
authentication and the sharing of a session
key, or more generally a set of secret key material, which the
participants can then use for encryption and mutual authentication
going forward. Even though the client may not be initially
authenticated, it is important that the server knows it remains
connected to the same client that established the connection. 

TLS defines a handshake protocol to handle the issues of
authentication and session key establishment. We now turn our attention to
the details of that protocol.

|TLS|.4.1 Handshake Protocol
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The TLS handshake protocol performs a set of functions to establish a
secure connection among a pair of participants.   It 
allows the client to authenticate the server, and, optionally, for
the client to be authenticated as well. It establishes a shared secret
key and allows them to negotiate at runtime
the set of cryptographic algorithms to use. It also allows for version
negotiation, so that, for example, a client running TLS version 1.2
can talk to a server that supports both version 1.2 and 1.3.

The handshake protocol needs to be resistant to man-in-the-middle
(MITM) attacks, which go beyond those we discussed in the last
section. At one point in its history, TLS version negotiation could be
subverted by a MITM in such a way that the client and server settled
on a lower version than necessary. Such a "downgrade attack" opens up
the risk that old vulnerabilities in the lower version, such as weak
cryptographic protocols, could be exploited.

TLS takes multiple precautions to increase its resistance against MITM
attacks. TLS 1.3 encrypts most of the handshake protocol, as early in
the process as possible. To facilitate this, the first step of the
handshake entails the establishment of a shared secret between the
client and the server. This is most commonly achieved using an
ephemeral Diffie-Hellman key exchange as described in the preceding
section. Pre-shared keys are also supported and have a role in restarting a
session quickly, as discussed below.

When we described Diffie-Hellman in chapter 4 we explained the original
algorithm that operates on groups of integers using modular
arithmetic. This is now known as Finite Field Diffie-Hellman. It is
also possible to use elliptic curves rather than modular arithmetic
and both options are available in modern TLS.

There are a number of ways the handshake protocol can play out, but
a typical set of operations is as follows:

1. The client sends a "client hello" message which specifies which
   Diffie-Hellman groups or elliptic curves it supports, along with an
   ephemeral Diffie-Hellman key for each group/curve. The hello also
   contains a nonce, the set of cipher suites that the client can use
   for subsequent encryption and authentication, and the TLS version
   the client supports.

2. The server replies with a "server hello" message indicating which
   Diffie-Hellman group or curve it has chosen and a corresponding
   ephemeral Diffie-Hellman public key. The hello also states which
   TLS version the server supports, the cipher suite it has chosen
   among those offered by the client, and a nonce.

Recall that a simple Diffie-Hellman key exchange is not secure against
MITM attacks, and the remaining steps in the handshake protect against
this. From the first two messages, the server and the client are able
to agree on a shared secret using one of several Diffie-Hellman
algorithms. A choice of groups or curves were provided in the client
hello, and one of them has been selected by the server. Similarly, one
of the offered cipher suites has been selected. With Diffie-Hellman
allowing them to obtain a shared secret, all subsequent messages
between client and server will be encrypted. But we still have to rule
out the MITM attack.

3. The server now sends one or more certificates. In the simplest
   case, there is a single certificate signed by a certificate
   authority (CA) that is trusted by the client.

4. The server sends a "certificate verify" message, which proves that
   the server has the private key that corresponds to the public key
   in the previously supplied certificate. The signature covers
   everything that has been sent in the handshake up to this point,
   which includes a pair of nonces, thus providing protection against
   replay attacks. And the signature along with the certificate is
   sufficient to prove to the client that it is talking to the
   intended server, not to some attacker in the middle, who would be
   unable to provide the signature.

5. The server sends a "handshake finished" message which contains a
   hash of everything sent so far, ensuring that nothing in the
   handshake was tampered with. This further protects against MITM attacks.

6. The client sends a similar "handshake finished" message.

At this point the client knows that it is talking to the intended
server, and both parties know that they have successfully completed the
handshake without any tampering of messages. The server in this case
does not know who the client is because there has been no client
authentication. TLS does support client authentication using client
certificates, but it is not the norm in today's Internet for clients
to authenticate in this way.

..   Something about compatibility with 1.2 middleboxes

Because public key cryptography is computationally more expensive
than symmetric key cryptography, public key
operations are limited to the handshake protocol. And when we said above that all
the messages after the first two are encrypted, this is done using
symmetric keys. The roles of public keys in TLS are (a) the
Diffie-Hellman key exchange (b) the use of certificates to
authenticate servers and, optionally, clients. All of that is limited
to the handshake protocol.

:numref:`Figure %s <fig-tls-hand>` shows the handshake protocol at a
high level.  When the client and server have each received a
"handshake finished" message from their respective peer, the handshake
is complete and application data can start to flow.

.. _fig-tls-hand:
.. figure:: secure/figures/TLS-handshake.png
   :width: 400px
   :align: center

   Handshake protocol to establish TLS session.

Encryption of data between client and server is performed by TLS’s
*record protocol*. Because the handshake protocol in TLS 1.3 requires
encryption after the first two messages, the record protocol actually
comes into play at step 3 above, even before we get to sending any
application data. We discuss the details of the record protocol below.


