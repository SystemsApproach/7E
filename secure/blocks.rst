|TLS|.2 Cryptographic Building Blocks
---------------------------------------

The foundational technology for providing confidentiality, integrity
and authentication is cryptography. In this section we introduce the
concepts of cryptography-based security step by step.  The first step
is the cryptographic algorithms—ciphers and cryptographic hashes. Such
algorithms are not a solution in themselves, but provide building
blocks from which a solution can be built. For example, cryptographic
algorithms are parameterized by *keys*, with the distribution of keys
a challenge that we tackle below. Once we have a set of cryptographic
algorithms and a way to distribute keys, we are in a position to build
protocols that enable secure communication between participants, such
as TLS.

|TLS|.2.1 Principles of Ciphers
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Encryption transforms a message in such a way that it becomes
unintelligible to any party that does not have the secret of how to
reverse the transformation. The sender applies an *encryption*
function to the original *plaintext* message, resulting in a
*ciphertext* message that is sent over the network, as shown in
:numref:`Figure %s <fig-genericCrypto>`. The receiver applies a
*decryption* function—the inverse of the encryption function—to
recover the original plaintext. The ciphertext transmitted across the
network is unintelligible to any eavesdropper, assuming the
eavesdropper doesn’t have the means to perform the decryption
function. The transformation represented by an encryption function and
its corresponding decryption function is called a *cipher*.

.. _fig-genericCrypto:
.. figure:: secure/figures/f08-01-9780123850591.png
   :width: 500px
   :align: center

   Secret-key encryption and decryption.

Cryptography has a long history
going back thousands of years. One of the leading cryptographers of
the 19th century, Auguste Kerckhoffs, stated in 1883 that
cryptographic system designs themselves should not be secret, but
should be parameterized by an easily changeable *key*; only the key
should need to be secret. Open designs and minimizing secrets are
among the widely
accepted principles of security.

One reason for open design is that, if you were to depend on the
cipher being kept secret, then you would have to retire the cipher
(not just the keys) when you believe it to be no longer secret. This
means potentially frequent changes of cipher, which is problematic
since it takes a lot of work to develop (and establish the security
of) a new cipher. In the pre-computer era, ciphers were often
implemented by specialized hardware such as the Enigma machine; these
could not be easily replaced, but were re-keyed frequently. Today's
algorithms, while implemented in software, are the result of lengthy
processes of development, testing, analysis and standardization; all
of this makes the algorithms expensive to replace.

Cryptography algorithms are occasionally found to be vulnerable after
years of use, so they are typically treated as pluggable modules in
end-to-end security solutions such as TLS.  The algorithm is a
selectable parameter. In the case of TLS, it is negotiated as part of
connection establishment. Thus the overall system does not become
obsolete just because one of its cryptography modules needs to be
deprecated.

One of the best ways to know that a cipher is secure is to use
it for a long time—the longer it goes unbroken, the better the chance
that is is secure. As with security in general, proving that a
cryptographic algorithm cannot be broken is a bit of a negative goal,
and thus hard to do with complete confidence.  Sometimes we may be
able to prove that breaking a cipher is as hard as solving some
well-studied mathematical problem such as factoring large numbers.
Even then there may be problems of implementation that will only get
discovered by the scrutiny of those looking at and using the
algorithm. And in many cases, we just have to rely on the fact that
no-one has yet found a viable way to break the cipher. Fortunately,
there are plenty of people who will try to break ciphers and who will
let it be widely known when they have succeeded.

Parameterizing a cipher with keys provides us with what is in effect a
very large family of ciphers; by switching keys, we are switching to
another cipher in the family. It is common to limit the amount of data
that a *cryptanalyst* (code-breaker) can access before the key
changes. This provides the attacker with less ability to break the
cipher, for reasons discussed below. It also limits the damage done if
the code is broken.

When a potential attacker receives a
piece of ciphertext, he may have more information at his disposal than
just the ciphertext itself. For example, he may know that the
plaintext was written in English, which means that the letter *e*
occurs more often in the plaintext that any other letter; the
frequency of many other letters and common letter combinations can
also be predicted. With simple ciphers, this information could greatly
simplify the task of determining the key. Similarly, the attacker may know
something about the likely contents of the message. For example, the
word “login” is likely to occur at the start of a remote login
session, and common headers appear at the start of HTTP messages. This may
enable a *known plaintext* attack, which has a much higher chance of
success than a *ciphertext only* attack. Even better is a *chosen
plaintext* attack, which may be enabled by feeding some information to
the sender that you know the sender is likely to transmit.

The best cryptographic algorithms, therefore, can prevent the attacker
from deducing the key even when the individual knows both the
plaintext and the ciphertext. In the ideal case, the attacker has no
choice but to try all the possible keys—exhaustive, “brute-force”
search. If keys have *n* bits, then there are 2\ :sup:`n` possible
values for a key (each of the *n* bits could be either a zero or a
one).  An attacker could be so lucky as to try the correct value
immediately, or so unlucky as to try every incorrect value before
finally trying the correct value of the key, having tried all 2\
:sup:`n` possible values; the average number of guesses to discover
the correct value is halfway between those extremes,

.. math:: 1/2 \times 2^n = 2^{n-1}

This can be made computationally impractical by choosing a
sufficiently large key space and by making the operation of checking a
key reasonably costly. Computing
speeds keep increasing, however, making formerly infeasible computations
feasible. Furthermore, such brute-force searches are easily
parallelized, meaning that an attacker can use general purpose GPUs
(GPGPUs) or other machines operating in parallel to speed up the
attack. For this reason key sizes are often chosen to be much larger
than what is required to make brute-force attacks expensive on the
hardware of today. We should also consider the
possibility that data might be captured for later analysis, or that
some data might be stored in archives for tens of years. At the same
time, increasing key size adds cost in terms of computation and
bandwidth required to distribute keys, so there is a tradeoff to be
made. 

Block Ciphers
++++++++++++++++++++

Most ciphers in use today are *block ciphers*; they are defined to
take as input a plaintext block of a certain fixed size, typically 128
bits. Using a block cipher to encrypt each block independently—known
as *electronic codebook (ECB) mode* encryption—has the weakness that a
given plaintext block value will always result in the same ciphertext
block (as long as the key remains constant). Hence, recurring block
values in the plaintext are recognizable as such in the ciphertext,
making it much easier for a cryptanalyst to break the cipher.

To prevent this, block ciphers are always augmented to make the
ciphertext for a block vary depending on context. Ways in which a
block cipher may be augmented are called *modes of operation*. A
common mode of operation is *cipher block chaining* (CBC), in which
each plaintext block is XORed with the previous block’s ciphertext
before being encrypted. The result is that each block’s ciphertext
depends in part on the preceding blocks (i.e., on its context). Since
the first plaintext block has no preceding block, it is XORed with a
random number. That random number, called an *initialization vector*
(IV), is included with the series of ciphertext blocks so that the
first ciphertext block can be decrypted. This mode is illustrated in
:numref:`Figure %s <fig-cbc>`. Another mode of operation is *counter
mode*, in which successive values of a counter (e.g., 1, 2, 3,
:math:`\ldots`) are incorporated into the encryption of successive
blocks of plaintext.

.. _fig-cbc:
.. figure:: secure/figures/f08-02-9780123850591.png
   :width: 500px
   :align: center

   Cipher Block Chaining.


Block size, like key size, is a design tradeoff. 64-bit blocks were
used for many years but as computer and network speeds increased and
storage costs dropped, 64-bit blocks became vulnerable to a type of
attack based on the *birthday problem* (the odds of 2 people in a
group of size *n* having the same birthday being surprisingly
high). If an attacker can manage to keep an encrypted session open
long enough to receive two identical 64-bit blocks, they can gain
useful information on the plaintext that produced the blocks. This was
known to be an issue in theory for a long time but the exploitation of
it was proven in 2016 leading to the deprecation of 64-bit blocks.

Block ciphers imply the padding of messages up to the next block
boundary, which wastes some network bandwidth, so there is a cost to
overly large blocks. For this reason most ciphers today have settled
on 128-bit blocks. Some details on how the birthday attacks were shown
to be an issue is available at the "Sweet32" website.

.. admonition:: Further Reading

   Sweet32. `Birthday attacks on 64-bit block ciphers in TLS and OpenVPN
   <https://sweet32.info>`__.

|TLS|.2.2 Secret-Key Ciphers
-----------------------------

In a secret-key cipher, both participants in a communication share the
same key.\ [#]_ In other words, if a message is encrypted using a particular
key, the same key is required for decrypting the message. If the
cipher illustrated in :numref:`Figure %s <fig-genericCrypto>` were a
secret-key cipher, then the encryption and decryption keys would be
identical. Secret-key ciphers are also known as symmetric-key ciphers
since the secret is shared with both participants. We’ll take a look
at the alternative, public-key ciphers, in which the keys are
asymmetrical, shortly.

.. [#] We use *participants* as a generic term for the endpoints of a
       communication channel. Depending on the layer of the network
       stack, a participant might correspond to a server, a process, a
       mailbox, or some other system abstraction. In the context of
       security, the communicating parties are often called
       *principals*, which in turn implies *identity*, and ultimately,
       an association with a human that can be held accountable. We
       use the term principal in place of participant when this full
       meaning is central to the discussion.

The U.S. National Institute of Standards and Technology (NIST) has
issued standards for a series of secret-key ciphers. *Data Encryption
Standard* (DES) was the first, and it survived for several decades
before being deprecated.

DES keys have 56 independent bits (although they have 64 bits
in total; the last bit of every byte is a parity bit). As noted above,
you would, on average, have to search half of the space of 2\
:sup:`56` possible keys to find the right one, giving 2\ :sup:`55` =
3.6 × 10\ :sup:`16` keys.  That may sound like a lot, but  by the late 1990s, it was
already possible to recover a DES key after a few hours. Consequently,
NIST updated the DES standard in 1999 to indicate that DES should only
be used for legacy systems. Importantly, DES was never shown to be
vulnerable to any attack other than brute force.

DES was initially replaced by *Triple DES* (3DES). A 3DES key has 168
(= 3 × 56) independent bits. Unfortunately, the computational cost of
launching a brute-force attack on 3DES was not, as expected, the cost of
searching a 2\ :sup:`168` key space, but rather a 2\ :sup:`112` key
space search due to a "meet in the middle" attack. At the same time, the
computational cost to perform encryption and decryption was still three
times higher than with single DES. This ultimately led to a process to
replace DES with newer algorithms.

3DES is now deprecated in favor of the *Advanced Encryption Standard*
(AES) issued by NIST. The cipher underlying AES (with a few
minor modifications) was originally named Rijndael (pronounced roughly
like “Rhine dahl”) based on the names of its inventors, Daemen and
Rijmen.  AES supports key lengths of 128, 192, or 256 bits, and the
block length is 128 bits. AES permits fast implementations in both
software and hardware, being somewhat more efficient than triple
DES. It doesn’t require much memory, which makes it suitable for small
mobile devices. AES has some mathematically strong security properties
and, as of the time of writing, has not suffered from any significant
practical attacks.

We won't go further into the details of secret-key ciphers, since it
really is a field for specialist cryptographers. The security expert
Bruce Schneier puts it this way:

  Anyone, from the most clueless amateur to the best cryptographer,
  can create an algorithm that he himself can’t break. It’s not even
  hard. What is hard is creating an algorithm that no one else can
  break, even after years of analysis. And the only way to prove that
  is to subject the algorithm to years of analysis by the best
  cryptographers around.

|TLS|.2.3 Public-Key Ciphers
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

An alternative to secret-key ciphers is public-key ciphers. Instead of
a single key shared by two participants, a public-key cipher uses a pair
of related keys, one for encryption and a different one for decryption.
The pair of keys is “owned” by just one participant. The owner keeps
one key of the pair secret. That key is the *private key*. The owner makes the second key
public; that key is called the *public key*.
Obviously, it must
not be possible to deduce the private key from the public key.

Anyone can get the public key and then use it to encrypt a message
before sending it to the owner of the keys. Only the owner has the
private key necessary to decrypt such a message. This scenario is depicted in
:numref:`Figure %s <fig-public>`.

.. _fig-public:
.. figure:: secure/figures/f08-03-9780123850591.png
   :width: 500px
   :align: center

   Public-key encryption.

Because it is somewhat unintuitive, we emphasize that the public
encryption key is useless for decrypting a message—you couldn’t even
decrypt a message that you yourself had just encrypted unless you had
the private decryption key. If we think of keys as defining a
communication channel between participants, then an important difference
between public-key and secret-key ciphers is the topology of the
channels. A key for a secret-key cipher provides a channel that is
two-way between two participants—each participant holds the same
(symmetric) key that either one can use to encrypt or decrypt messages
in either direction. A public/private key pair, in contrast, provides
a channel that is one way and many-to-one: from everyone who has the
public key to the unique owner of the private key, as illustrated in
:numref:`Figure %s <fig-public>`.


Public-key ciphers are used not just for encryption, but also for
authentication. The way this works is that the private key can be used
with the *encryption* algorithm to encrypt messages so that they can
then only be decrypted using the public key. This property clearly
wouldn’t be useful for confidentiality since anyone with the public
key could decrypt such a message. This property is, however, useful
for authentication since it tells the receiver of such a message that
it could only have been created by the owner of the keys (subject to
certain assumptions that we will get into later). This is illustrated
in :numref:`Figure %s <fig-pksign>`.

It should be clear from the figure that anyone with the public key can
decrypt the encrypted message, and, assuming that the result of the
decryption matches the expected result, it can be concluded that the
private key must have been used to perform the encryption. We return
to the details of how this operation is used to provide authentication
below.

To summarize, public-key ciphers may be used both for authentication
and to support confidential communications by encryption. In the
latter case, they are mostly used to *bootstrap* confidential
communications. Rather than encrypting the majority of messages sent
between participants, public-key ciphers are used to confidentially
distribute secret (symmetric) keys, leaving the rest of
confidentiality to secret-key ciphers. The symmetric key sent over
this confidential channel is called a *session key*. The reasons for
this two-step approach include the higher efficiency of secret-key
ciphers, and the need for reasonably frequent changing of encryption
keys as described above.

.. _fig-pksign:
.. figure:: secure/figures/f08-04-9780123850591.png
   :width: 500px
   :align: center

   Authentication using public keys.

The concept of public-key ciphers was first published in 1976 by
Diffie and Hellman and Diffie-Hellman key exchange is a part of TLS we
discuss below. Subsequently, however, documents have come to
light proving that Britain’s Communications-Electronics Security Group
had discovered public-key ciphers by 1970, and the U.S. National
Security Agency (NSA) claims to have discovered them in the mid-1960s.

The best-known public-key cipher is RSA, named after its inventors:
Rivest, Shamir, and Adleman. RSA relies on the high computational cost
of factoring large numbers. The problem of finding an efficient way to
factor numbers is one that mathematicians have worked on unsuccessfully
since long before RSA appeared in 1978, and RSA’s subsequent resistance
to cryptanalysis has further bolstered confidence in its security.
Unfortunately, RSA needs relatively large keys, at least 1024 bits, to
be secure. This is larger than keys for secret-key ciphers because it is
faster to break an RSA private key by factoring the large number on
which the pair of keys is based than by exhaustively searching the key
space.

Another public-key cipher is ElGamal. Like RSA, it relies on a
mathematical problem, the discrete logarithm problem, for which no
efficient solution has been found, and requires keys of at least 1024
bits. There is a variation of the discrete logarithm problem, arising
when the input is an elliptic curve, that is thought to be even more
difficult to compute; cryptographic schemes based on this problem are
referred to as *elliptic curve cryptography* (ECC). ECC can
provide equivalent security to RSA with considerably smaller
keys. 

Public-key ciphers are, unfortunately, several orders of magnitude
slower than secret-key ciphers. Consequently, secret-key ciphers are
used for the vast majority of encryption, while public-key ciphers are
reserved for use in authentication and session key establishment.

|TLS|.2.4  Message Authentication
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Encryption alone does not provide data integrity. For example, just
randomly modifying a ciphertext message could turn it into something
that decrypts into valid-looking plaintext, in which case the tampering
would be undetectable by the receiver. Nor does encryption alone provide
authentication. In a sense, integrity and authentication are
fundamentally inseparable. It is not much use to say that a message came from a
certain participant if the contents of the message have been modified
after that participant created it.

A *message authentication code* is a value, to be included in a transmitted message,
that can be used to verify simultaneously the authenticity and the data
integrity of a message. We will see later how such codes can be used in
protocols. For now, we focus on the algorithms that can generate and verify
authentication codes.

In Chapter |Tech| we saw how error-detecting codes are used to detect
errors in transmitted data. A similar concept applies to
authentication codes, with the added challenge that the corruption of the
message is likely to be deliberately performed by someone who wants
the corruption to go undetected. To support authentication, the
code includes some proof that whoever created it
knows a secret that is known only to the alleged sender
of the message; for example, the secret could be a key, and the proof
could be some value encrypted using the key. There is a mutual
dependency between the way the code is generated and how it is used as
proof of secret knowledge. The following discusses several workable
combinations.

For simplicity, let's assume initially that the original message need
not be confidential—that a transmitted message will consist of the
plaintext of the original message plus some additional code to support
authentication. Later we will consider the case where confidentiality
is also desired.

One common building block of message authentication is a
*cryptographic hash function*. Cryptographic hash algorithms are
treated as public knowledge, as with cipher algorithms. A
cryptographic hash function is a function that outputs sufficient
information about a message to expose any tampering. Just as a
checksum or error-detecting code exposes bit errors introduced by
noisy links or scratched disks, a cryptographic hash is designed to
expose deliberate corruption of messages by an adversary. The value it
outputs is called a *message digest* and, like an ordinary checksum,
is appended to the message. All the message digests produced by a
given hash have the same number of bits regardless of the length of
the original message. Since the space of possible input messages is
larger than the space of possible message digests, there will be many
different input messages that produce the same message digest, like
collisions in a hash table. An important property of cryptographic
hash functions is that such collisions may not be produced
deliberately under the control of the attacker. We will see why this
is so in a moment.

A message authentication code can be created by encrypting the message
digest with some key. That key could be the private key of an
asymmetric cipher, known only to the sender, or it could be a secret
key for a symmetric cipher that sender and receiver agreed to by some
other means. On receiving the message, the
receiver computes a digest of the plaintext part of the message and
compares that to the decrypted message digest. If they are equal, then
the receiver would conclude that the message is indeed from its alleged
sender (since it would have to have been encrypted with the right key)
and has not been tampered with.

Suppose that an adversary intercepts the message on its way to the
receiver and tries to modify the transmitted message in
some way. The message digest for this corrupted message would (with
very high likelihood) differ from that of the original message. And
the adversary lacks the necessary key to
encrypt the digest of the corrupted message. An adversary could,
however, obtain the plaintext original message and its encrypted digest
by eavesdropping. The adversary could then (since the hash function is
public knowledge) compute the digest of the original message and
generate alternative messages looking for one with the same message
digest. If she finds one, she could undetectably send the new message
with the old authentication code. Therefore, security requires that the hash
function have the *one-way* property: it must be computationally
infeasible for an adversary to find any plaintext message that has the
same digest as the original.

For a hash function to meet this requirement, its outputs must be
fairly randomly distributed. For example, if digests are 128 bits long
and truly randomly distributed, then you would need to try 2\ :sup:`127`
messages, on average, before finding a second message whose digest
matches that of a given message. If the outputs are not randomly
distributed—that is, if some outputs are much more likely than
others—then for some messages you could find another message with the
same digest much more easily than this, which would reduce the
security of the algorithm. So a random distribution of hash outputs is
an important property for these algorithms.

If you were instead just trying to find any *collision*—any two
messages that produce the same digest—then you would need to compute
the digests of only 2\ :sup:`64` messages, on average.  This
surprising fact is the basis of the “birthday attack” mentioned above.

There have been several common cryptographic hash algorithms over the
years, including Message Digest 5 (MD5) and the Secure Hash Algorithm
(SHA) family. Weaknesses of MD5 and earlier versions of SHA have been
known for some time, which led NIST to recommend a family of
algorithms known as SHA-3 in 2015.

As noted above, the encryption of the message digest can be performed
using either a secret-key cipher or a public-key cipher. If a
public-key cipher is used, the digest is encrypted using the sender’s
private key, and the receiver—or anyone else—could decrypt the digest
using the sender’s public key. If a secret-key cipher is used, the
sender and receiver have to agree on the secret key ahead of time
using some other means.


.. TODO We could reduce the verbiage here to just what is required for
   TLS but perhaps this is useful background

A digest encrypted with a public-key algorithm using the private key
of the sender is called a *digital signature* because it provides
nonrepudiation similar to that of a written signature. The receiver of
a message with a digital signature can prove to any third party that
the sender really sent that message, because the third party can use
the sender’s public key to check for herself. Secret-key encryption of
a digest does not have this property because only the two participants
know the key; furthermore, since both participants know the key, the
alleged receiver could have created the message herself. Any
public-key cipher can be used for digital signatures. NIST has
produced a series of *Digital Signature Standards* (DSS). The most
recent standard at the time of writing allows for the use of three
public-key ciphers, one based on RSA, another based on elliptic
curves, and a third called the *Edwards-Curve Digital Signature
Algorithm*.

A widely used alternative approach to encrypting a hash is to use a
hash function that takes a secret value (a key known only to the
sender and the receiver) as an input parameter in addition to the
message text. Such a function outputs a message authentication code
that is a function of both the secret key and the message
contents. The sender appends the calculated message authentication
code to the plaintext message. The receiver recomputes the
authentication code using the plaintext and the secret value and
compares that recomputed code to the code received in the message. The
most common approaches to generating these codes are called HMACs or
keyed-hash message authentication codes.

HMACs can use any hash function of the sort described above, but
also include the key as part of the material to be hashed, so that a
HMAC is a function of both the key and the input text. An approach to
calculating HMACs has been standardized by NIST and takes the
following form:

HMAC = H( (K⊕opad) || H((K⊕ipad) || text) )

H is the hash function, K is the key, and opad (output pad) and ipad
(input pad) are well-known strings that are XORed (⊕) with the key. ||
represents concatenation.

A deep explanation of this HMAC function is beyond the scope of this
book. However, this approach has been proved to be secure as long as
the underlying hash function H has the appropriate
collision-resistance properties outlined above. Note that the HMAC
takes a hash function *H* that is not keyed, and turns it into a keyed
hash by adding the key material to the text input to the hash. The
hash function *H* is applied twice. First the key (XORed
with a string, *ipad*) is prepended to the message, which is
then fed into the hash function. The output of this keyed hash is then
itself subjected to another keyed hash (again by XORing the key with
a string, *opad*, and prepending that to the output of the first keyed hash).
The two passes of the keyed-hash function are important to the proof
of security for this HMAC construction.

Up to this point, we have been assuming that the message wasn’t
confidential, so the original message could be transmitted as plaintext.
To add confidentiality to a message with an authentication code, it suffices
to encrypt the concatenation of the entire message including its
authentication code. Remember that, in
practice, confidentiality is implemented using secret-key ciphers
because they are so much faster than public-key ciphers. Furthermore, it
costs little to include the authenticator in the encryption, and it
increases security.

In recent years, the idea of using a single algorithm to support both
authentication and encryption has gained support for reasons of
performance and simplicity of implementation. This is referred to as
*authenticated encryption* or *authenticated encryption with
associated data*. The latter term allows for some data fields (e.g.,
packet headers) to be transmitted as plaintext—these are the
associated data—while the rest of the message is encrypted, and the
whole thing, headers included, is authenticated. We won't go into
details here, but there is now a set of integrated algorithms that
produce both ciphertext and authentication codes using a combination
of ciphers and hash functions.

If you want to get a deeper understanding of the principles of ciphers
and hash functions, among other cryptographic concepts, we recommend the following book.

.. admonition:: Further Reading

   A. Menezes, P. van Oorschot, and S. Vanstone. `Handbook of Applied
   Cryptography <https://cacr.uwaterloo.ca/hac/>`__. CRC Press, 1996.

Now that we have seen some of the building blocks for encryption and
authentication, we have the foundations for building some complete security
solutions. Before we get to those, however, we address the issue of how participants
obtain keys in the first place.

