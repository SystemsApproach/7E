|TLS|.1 Design Issues
-------------------------

As the World Wide Web became popular and
commercial enterprises began to take an interest in it, it became clear
that some level of security would be necessary for transactions on the
Web. The canonical example of this is making purchases by credit card,
where it is important that the credit card details are not leaked to
adversaries, the transaction is not modified in flight, and we expect
some assurance that we're connected to the party we want to transact
business with.

The first major requirement thus is likely to be
*confidentiality*. There are many ways that an adversary might gain
access to data in transit through a network. Confidentiality is the
ability to prevent data from being read by anyone other than its
intended recipient.

Closely related to the confidentiality of data is *traffic
confidentiality*. This is the idea that we don’t want an adversary to
be able to determine where or to whom we are sending traffic, or in
what quantity. This presents some distinct challenges from data
confidentiality; there is no need for network devices to see our data,
but they generally must look at packet headers, which contain
destination information, to determine where to send traffic.

The second major requirement is *integrity*, which is about having
confidence that the information we’re receiving is trustworthy, and
for example, has not been modified by some adversary while in
transit. Assuring integrity is multi-faceted, involving more than just
“in transit” adversaries.

For example, we need to be able to verify that an item of data was
sent by the entity that claimed to have sent it. This means we need to
*authenticate* the sender. In the example of e-commerce, this is what
allows us to know we are connected to, say, the website of the vendor
we wish to patronize and not handing over our credit card to some
impostor.

Authentication implies that we must have a concept of *identity*. That
is, we need a system by which communicating entities, often called
principals, can securely identify each other. This problem is harder
to solve than it might first appear. How can we know that a website
with a particular URL actually represents the business with whom we
wish to communicate? Or how does a banking system know that the person
behind a particular HTTP request is actually the account holder?


Integrity also requires messages be *fresh* and *timely*, which is
threatened by the possibility data is captured and then retransmitted
at some later time. This is known as a replay attack, where for
example, we want to protect against an attacker repeatedly adding an
item to a shopping cart. Thus, it is a common requirement to have some
form of replay prevention.

The final major requirement is *availability*, which is simply the
ability to access all the data we have the right to access. System
failures, either benign or caused by malicious actors, restrict
availability. Another common threat to availability is
denial-of-service (DoS) attacks.  Networks provide a means by which
data can be amplified by replication, allowing large volumes of
traffic to be sent to the target of a DoS attack; thus it has become
necessary to develop means to mitigate such attacks. While DoS
mitigation and prevention go beyond the scope of transport layer
security, designers of secure protocols need to keep an eye on ways in
which their protocol might fall victim to a DoS attack or serve as a
vector for amplifying DoS attacks.

In this chapter we examine how the Transport Layer Security protocols
have been developed to meet the requirements just outlined. While
cryptography provides us with some building blocks to meet these
requirements, which we discuss in the next section, there is more to
building a secure system than just adding some cryptographic
operations. 

The designers of SSL and TLS recognized that a requirement for secure
communication was not
specific to web transactions (i.e., those using HTTP) and thus they built
a general-purpose protocol that sits between an application protocol
such as HTTP and a transport protocol such as TCP. The reason for
calling this “transport layer security” is that, from the application’s
perspective, this protocol layer looks just like a normal transport
protocol except for the fact that it is secure. That is, the sender can
open connections and deliver bytes for transmission, and the secure
transport layer will get them to the receiver with the necessary
confidentiality, integrity, and authentication. By running the secure
transport layer on top of TCP, all of the normal features of TCP
(reliability, flow control, congestion control, etc.) are also provided
to the application. This arrangement of protocol layers is depicted in
:numref:`Figure %s <fig-tls-stack>`.

.. _fig-tls-stack:
.. figure:: secure/figures/f08-15-9780123850591.png
   :width: 300px
   :align: center

   Secure transport layer inserted between application and TCP layers.

When HTTP is used in this way, it is known as HTTPS (Secure HTTP). In
fact, HTTP itself is unchanged. It simply delivers data to and accepts
data from the SSL/TLS layer rather than TCP. For convenience, a default
TCP port has been assigned to HTTPS (443). That is, if you try to
connect to a server on TCP port 443, you will likely find yourself
talking to the SSL/TLS protocol, which will pass your data through to
HTTP provided all goes well with authentication and decryption. Although
standalone implementations of SSL/TLS are available, it is more common
for an implementation to be bundled with applications that need it,
primarily web browsers and servers.

The layered approach, inserting TLS between the application protocol
and the transport protocol, is not without drawbacks, particularly
when performance is considered. This eventually led to a rethinking
of the layering and a new transport protocol, QUIC, was developed with
the benefit of decades of experience with TLS and HTTP. We return to
this development later in this chapter and in Chapter |Message|.
