|TLS|.3 Key Distribution
-------------------------

Given the central role played by keys in cryptography, we now turn our
attention to the problem of distributing keys to the
participants. Suppose, for example, that we want to validate that we
are connected to the web site of a particular company. How can we
reliably learn the public key of that company as a first step to validating signatures
produced with its private key?

Similarly, we are going to need session keys—short-lived, symmetrtic keys to protect the
confidentiality of data sent over a connection. Establishing a
short-lived session key is a different
problem from the distribution of long-lived public keys. We will
tackle each problem in turn.

In the following discussion, we use “Alice” and “Bob” to designate participants, as is
common in the cryptography literature. Although we
tend to refer to participants in anthropomorphic terms, we are more
frequently concerned with the communication between software or
hardware entities such as clients and servers that often have no
direct relationship with any particular person.

|TLS|.3.1 Predistribution of Public Keys
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The algorithms to generate a matched pair of public and private keys are
publicly known, and software that does it is widely available. So, if
Alice wants to use a public-key cipher, she can generate her own pair
of public and private keys, keep the private key hidden, and publicize
the public key. But, how can she publicize her public key—assert that it
belongs to her—in such a way that other participants can be sure it
really belongs to her? It is not as simple as putting it on a public
web page or dropping it in an email, because an adversary could
forge an equally plausible claim that key *x* belongs to Alice when *x*
really belongs to the adversary.

A complete scheme for certifying bindings between public keys and
identities—what key belongs to whom—is called a *Public Key
Infrastructure* (PKI). A PKI starts with the ability to verify
identities and bind them to keys out of band. By “out of band,” we mean
something outside the network and the computers that comprise it, such
as in the following. If Alice and Bob are individuals who know each
other, then they could get together in the same room and Alice could
give her public key to Bob directly, perhaps on a business card. If Bob
is an organization, Alice the individual could present conventional
identification, perhaps involving a photograph or fingerprints. If Alice
and Bob are computers owned by the same company, then a system
administrator could configure Bob with Alice’s public key.

Establishing keys out of band doesn’t sound like it would scale well,
but it suffices to bootstrap a PKI. Bob’s knowledge that Alice’s key is
*x* can be widely, scalably disseminated using a combination of digital
signatures and a concept of trust. For example, suppose that you have
received Bob’s public key out of band and that you know enough about Bob
to trust him on matters of keys and identities. Then Bob could send you
a message asserting that Alice’s key is *x* and—since you already know
Bob’s public key—you could authenticate the message as having come from
Bob. (Remember that to digitally sign the statement Bob would append a
cryptographic hash of it that has been encrypted using his private key.)
Since you trust Bob to tell the truth, you would now know that Alice’s
key is *x*, even though you had never met her or exchanged a single
message with her. 

Bob wouldn’t even have to
send you a message; he could simply create and publish a digitally
signed statement that Alice’s key is *x*. Such a digitally signed
statement of a public key binding is called a *public key certificate*,
or simply a certificate. Bob could send Alice a copy of the certificate,
or post it on a website. If and when someone needs to verify Alice’s
public key, they could do so by getting a copy of the certificate,
perhaps directly from Alice—as long as they trust Bob and know his
public key. You can see how starting from a very small number of keys
(in this case, just Bob’s) you could build up a large set of trusted
keys over time. Bob in this case is playing the role often referred to
as a *certificate authority* (CA), and much of today’s Internet
security depends on CAs. There are many commercial and non-profit CAs
in widespread use today. You may also see CA expanded as
*certification authority*—the two expansions are equivalent.

One thing to note about the above example is that we have to know two
things about Bob. First, we need to know his public key so that we can
verify that certain messages were originated by Bob. But we also have
to know that Bob is trustworthy enough to make statements about the
keys of others, which is where certificate authorities (rather than
random individuals) come into play.  We return to this topic below.

One of the major standards for certificates is known as X.509. This
standard leaves a lot of details open, but specifies a basic structure.
A certificate clearly must include:

-  The identity of the entity being certified

-  The public key of the entity being certified

-  The identity of the signer

-  The digital signature

-  A digital signature algorithm identifier (which cryptographic hash
   and which cipher)

In addition, certificates invariably contain an expiration date. We will
see how this feature is used below.

Since a certificate creates a binding between an identity and a public
key, we should look more closely at what we mean by “identity.” For
example, a certificate that says, “This public key belongs to John
Smith,” may not be terribly useful if you can’t tell which of the
thousands of John Smiths is being identified. Thus, certificates must
use a well-defined name space for the identities being certified; for
example, certificates are often issued for DNS
domains.


Certificate Authorities
++++++++++++++++++++++++++++

Certificate authorities operate to establish *chains of trust*. If X
certifies that a certain public key belongs to Y, and then Y goes on
to certify that another public key belongs to Z, then there exists a
chain of certificates from X to Z, even though X and Z may have never
met. If you know X’s key—and you trust X and Y—then you can believe
the certificate that gives Z’s key.  In other words, all you need is a
chain of certificates, all signed by entities you trust, as long as it
leads back to an entity whose key you already know.

A *certificate authority* or *certification authority* (CA) is an entity
claimed (by someone) to be trustworthy for verifying identities and
issuing public key certificates. There are commercial CAs, governmental
CAs, and non-profit CAs. To use a CA, you must know its own key. You can
learn that CA’s key, however, if you can obtain a chain of CA-signed
certificates that starts with a CA whose key you already know. Then you
can believe any certificate signed by that new CA.

A common way to build such chains is to arrange them in a
tree-structured hierarchy, as shown in :numref:`Figure %s
<fig-pem-tree>`. If everyone has the public key of the root CA, then
any participant can provide a chain of certificates to another
participant and know that it will be sufficient to build a chain of
trust for that participant.

.. _fig-pem-tree:
.. figure:: secure/figures/f08-06-9780123850591.png
   :width: 600px
   :align: center

   Tree-structured certificate authority hierarchy.

There are some significant issues with building chains of trust. Most
importantly, even if you are certain that you have the public key of the
root CA, you need to be sure that every CA from the root on down is
doing its job properly. If just one CA in the chain is willing to issue
certificates to entities without verifying their identities, then what
looks like a valid chain of certificates becomes meaningless. For
example, a root CA might issue a certificate to a second-tier CA and
thoroughly verify that the name on the certificate matches the business
name of the CA, but that second-tier CA might be willing to sell
certificates to anyone who asks, without verifying their identity. This
problem gets worse the longer the chain of trust. X.509 certificates
provide the option of restricting the set of entities that the subject
of a certificate is, in turn, trusted to certify.

There can be more than one root to a certification tree, and this is
common in securing web transactions today, for example. Web browsers
such as Firefox and Chrome come pre-equipped with certificates for a
(reasonably large) set of CAs; in effect, the browser’s producer has
decided these CAs and their associated public keys can be trusted. A
user can also add CAs to those that their browser recognizes as
trusted (or remove CAs from the default list). In other words, for
most users, the browser manufacturer becomes the entity that they
trust to look after PKI for them. If you are curious, you can poke around in
the preferences settings for your browser and find the “view
certificates” option to see how many CAs your browser is configured to
trust.

.. sidebar on web of trust?


Certificate Revocation
+++++++++++++++++++++++++

One issue that arises with certificates is how to revoke, or undo, a
certificate. Why is this important? Suppose that you suspect that
someone has discovered your private key. There may be any number of
certificates in the universe that assert that you are the owner of the
public key corresponding to that private key. The person who discovered
your private key thus has everything required to impersonate you: valid
certificates and your private key. To solve this problem, it would be
nice to be able to revoke the certificates that bind your old,
compromised key to your identity, so that the impersonator will no
longer be able to persuade other people that he is you.

The basic solution to the problem is simple enough. Each CA can issue a
*certificate revocation list* (CRL), which is a digitally signed list of
certificates that have been revoked. The CRL is periodically updated and
made publicly available. Because it is digitally signed, it can just be
posted on a website. Now, when Alice receives a certificate for Bob that
she wants to verify, she will first consult the latest CRL issued by the
CA. As long as the certificate has not been revoked, it is valid. Note
that, if all certificates have unlimited life spans, the CRL would
always be getting longer, since you could never take a certificate off
the CRL for fear that some copy of the revoked certificate might be
used. For this reason, it is common to attach an expiration date to a
certificate when it is issued. Thus, we can limit the length of time
that a revoked certificate needs to stay on a CRL. As soon as its
original expiration date is passed, it can be removed from the CRL.

In practice, certificate revocation has proven to be challenging. CRLs
can become very long now that certificates are in widespread use, so
retrieving them becomes costly. The time to retrieve a CRL may fall in
the critical path for opening a connection to a web site,
substantially increasing the time to load a page. A determined
attacker who has compromised a private key is motivated to disrupt the
distribution of the CRL to prolong the amount of time they can use the
compromised key. A number of proposals have been made to improve the
effectiveness of certificate revocation, such as using bit vectors or
other compact representations of the CRL to reduce its size, and the
development of the Online Certificate Status Protocol (OCSP) to
enable real-time checks on a certificate's status. At the time of
writing, there are some best practices for handling certificate
revocation but no comprehensive solution.

|TLS|.3.2 Distribution of Secret Keys
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

As discsused above, we want a short-lived, symmetric session key to
provide confidentiality and integrity protection for the traffic on a
connection between client and server. They must be short-lived to
limit the amount of information available to attackers and to limit
the damage caused when a key is compromised. And they are symmetric to
provide higher performance for the common-case data transfers; we
limit the use of public key operations to relatively infrequent tasks
such as session establishment. 

Distribution of these secret keys differs from public key distribution
for two obvious reasons:

-  While only one public key per entity is sufficient for authentication
   and confidentiality, there must be a secret key for each pair of
   entities who wish to communicate. If there are N entities, that means
   N(N-1)/2 keys.

-  Unlike public keys, secret keys must be kept secret.

In summary, there are a lot more keys to distribute, and you can’t use
certificates that everyone can read.

One common solution is to use public key operations to bootstrap the
communication. The most widely used approach relies on the
Diffie-Hellman key exchange protocol, which can establish a shared
secret between two principals without using any predistributed keys.


Diffie-Hellman on its own doesn’t authenticate the participants. Since
it is rarely useful to communicate securely without being sure whom
you’re communicating with, Diffie-Hellman is usually augmented in some
way to provide authentication. Diffie-Hellman is used in both the
Internet Key Exchange (IKE) protocol, a part of the IP Security
(IPsec) architecture, and in TLS. We start our discussion with the
basic principles underlying Diffie-Hellman and work our way through
the steps that are required to make it robust enough for use in
today's systems.

The original Diffie-Hellman protocol has two parameters, *p* and *g*, both of
which are public and may be used by all the users in a particular
system. Parameter *p* must be a prime number. The integers
:math:`\bmod p` (short for modulo *p*) are :math:`0` through *p-1*,
since :math:`x \bmod p` is the remainder after *x* is divided by *p*,
and form what mathematicians call a *group* under
multiplication. Parameter *g* (usually called a generator) must be a
*primitive root* of *p*: For every number *n* from 1 through *p-1*
there must be some value *k* such that :math:`n = g^k \bmod p`. For
example, if *p* were the prime number 5 (a real system would use a
much larger number), then we might choose 2 to be the generator *g*
since:

.. math::

   1 = 2^0 \bmod p

.. math::

   2 = 2^1 \bmod p

.. math::

   3 = 2^3 \bmod p

.. math::

   4 = 2^2 \bmod p

Suppose Alice and Bob want to agree on a shared secret key. Alice and
Bob, and everyone else, already know the values of *p* and *g*. Alice
generates a random private value *a* and Bob generates a random
private value \ *b*. Both *a* and *b* are drawn from the set of
integers :math:`\{1,\dots{}, p-1\}`. Alice and Bob derive their
corresponding public values—the values they will send to each other
unencrypted—as follows. Alice’s public value is

.. math::

   g^a \bmod p

and Bob’s public value is

.. math::

   g^b \bmod p

They then exchange their public values. Finally, Alice computes

.. math::

   g^{ab} \bmod p = (g^b \bmod p)^a \bmod p

and Bob computes

.. math::

   g^{ba} \bmod p = (g^a \bmod p)^b \bmod p.

Alice and Bob now have :math:`g^{ab} \bmod p` (which is equal to
:math:`g^{ba} \bmod p)` as their shared secret key.

Any eavesdropper would know *p, g*, and the two public values
:math:`g^a \bmod p` and :math:`g^b \bmod p`.
If only the eavesdropper could determine *a* or *b*, she could easily
compute the resulting key. Determining *a* or *b* from that information
is, however, computationally infeasible for suitably large *p,a,* and
*b*; it is known as the *discrete logarithm problem*.

Let's look at an example using small numbers to illustrate the
calculation. Let *p = 5* and *g = 2*, and suppose Alice picks
the random number *a = 3* and Bob picks the random number *b = 4*.
Then Alice sends Bob the public value

.. math::

   2^3 \bmod 5 = 3

and Bob sends Alice the public value

.. math::

   2^4 \bmod 5 = 1

Alice is then able to compute

.. math::

   g^{ab} \bmod p = (2^b \bmod 5)^3 \bmod 5 = (1)^3 \bmod 5 = 1

by substituting Bob’s public value for :math:`(2^b \bmod 5)`. Similarly,
Bob is able to compute

.. math::

   g^{ba} \bmod p = (g^a \bmod 5)^4 \bmod 5 = (3)^4 \bmod 5 = 1.

by substituting Alice’s public value for :math:`(2^a \bmod 5)`.
Both Alice and Bob now agree that the secret key is :math:`1`.

As described above, Diffie-Hellman’s lack of authentication makes it
vulnerable to various attacks. One such attack is the
*man-in-the-middle attack*. Suppose Mallory is an adversary with the
ability to intercept messages. Mallory already knows *p* and *g* since
they are public, and she generates random private values :math:`c` and
:math:`d` to use with Alice and Bob, respectively. When Alice and Bob
send their public values to each other, Mallory intercepts them and
sends her own public values, as in :numref:`Figure %s
<fig-manInTheMiddle>`. The result is that Alice and Bob each end up
unknowingly sharing a key with Mallory instead of each other.

.. _fig-manInTheMIddle:
.. figure:: secure/figures/f08-12-9780123850591.png
   :width: 300px
   :align: center

   A man-in-the-middle attack.

It is possible to address the immediate problem of man-in-the-middle
attacks by authenticating one or both participants. We can do this
using certificates that are similar to public key certificates but
instead certify the Diffie-Hellman public parameters of an entity. For
example, such a certificate would state that Alice’s Diffie-Hellman
parameters are *p, g*, and :math:`g^a \bmod p` (note that the value of
*a* would still be known only to Alice). Such a certificate would
assure Bob that the other participant in Diffie-Hellman is Alice—or
else the other participant won’t be able to compute the secret key,
because she won’t know *a*.

The approach we have just described, known as *fixed* Diffie-Hellman,
is not widely recommended in practice because it lacks *forward
secrecy*. If the long-lived private key of Alice were to be
compromised at some point, past sessions that had been recorded by an
attacker would then be potentially at risk. For this reason, another
approach known as *ephemeral* Diffie-Hellman is widely used, notably
in TLS.

Like the fixed variant, ephemeral Diffie-Hellman relies on at
least one participant having a certificate issued by a CA, but in this
case it certifies that Alice is associated with a given public key
(e.g., an RSA key). Alice then generates an ephemeral value of *a*
rather than a fixed one, and uses her private key to sign the Diffie
Hellman parameters: *p, g*, and :math:`g^a \bmod p`. By providing the
certificate and the signed value, Alice is able to show Bob that the
message has really come from her and to authenticate the Diffie-Hellman
parameters, while still keeping *a* secret. Unlike fixed
Diffie-Hellman, this approach provides forward secrecy:
even if the long-lived private key of Alice were to be compromised,
past sessions that had been recorded by an attacker will still be
secure, since they used ephemeral keys that changed with every
session. Note that while the word "ephemeral" strictly implies only
that *a* is a short-lived value, it is widely used in protocol
specifications to apply to cases where authentication is also
performed using a public key as we have described it here.  To avoid
confusion, the original form of Diffie-Hellman that lacks
authentication is often referred to as "anonymous" mode.

Finally, we note that if both participants have been issued
certificates, they can authenticate each other. If just one has a
certificate, then just that one can be authenticated. One-way
authentication is commonly used on the web; for example, when one
participant is a web server and the other is an arbitrary client, the
client can authenticate the web server and establish a secret key for
confidentiality before sending a credit card number to the web server.
