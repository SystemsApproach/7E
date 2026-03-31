
|TLS|.5  Record Protocol
--------------------------------

The task of the TLS record protocol is to protect the data that is sent
over a TLS connection with both encryption and authentication.
While TLS supports a wide range of encryption and authentication
methods, the set of options has actually become narrower in version
1.3 as weaknesses of older methods became clear and new cryptographic
algorithms have emerged. All the algorithms in TLS 1.3 provide both
encryption and authentication in a single cipher suite, using the
technique known as authenticated encryption with additional data
(AEAD) discussed above.


In TLS, the cipher that provides authentication and encryption uses
two keys, one for each direction. It is nevertheless a symmetric key
cipher, since the same key is used for encryption and decryption. Similarly, two initialization
vectors are required.  Thus, regardless of the choice of cipher suite,
a TLS session requires effectively four keys to be agreed upon by the
end points. TLS derives all of them from a single shared secret that
was obtained during the handshake phase.

The step that derives the keys and initialization vectors from the
shared secret is called the "HMAC-based extract-and-expand key
derivation function (HKDF)". The goal is to produce enough keying
material for the record layer–two IVs and two symmetric keys of
appropriate length–and to do so in such a way that an attacker has no
better way of guessing them than a brute force attack. In other words,
we want the keys and IVs to be as close to random as possible. This is
a bit harder than it might first appear, because the shared secret
that is obtained via Diffie Hellman, which is our starting point, is
not itself completely random. The reason for this may not be obvious,
but the goal of the various Diffie Hellman algorithms is to generate a
shared secret, not that such secrets be randomly distributed.

There is some fairly serious mathematics underlying HKDF, but the
basic idea is called "extract and expand". The first step is to
"extract" the randomness from the shared secret. This is done by
calculating a HMAC (hash-based message authentication code, as described
in Chapter 3) over the shared secret. The resulting pseudorandom key
is input to the next stage, along with an additional source of
randomness: the hash of everything contained in the initial
handshake. Note that the handshake messages include two random
nonces. The "expand" step then applies the HMAC function using these
inputs and HMAC is reapplied as many times as needed to produce the
required amount of key and IV material.

When all the keys and IVs are available to client and server, the record
layer can now protect the underlying data with encryption and
authentication. The record layer also handles fragmentation and
reassembly–breaking the incoming stream of plaintext into chunks of up
to 2\ :sup:`14` bytes.

To encrypt one block for transmission, the record layer takes as input
the encryption key, a nonce (which we explain below), the plaintext to
be encrypted, and "additional data" to be authenticated but not
encrypted. This additional data is the header for the record layer,
indicating the type of data being encrypted (e.g., application data or
handshake data) and its length. The nonce is calculated by computing
the XOR of the IV and a sequence number that increments with every
block. The AEAD cipher then computes the ciphertext that will follow
the record header, and the resulting block is passed to the transport
layer (normally TCP) for transmission.

On the receiving side, the process runs in the other direction, with
the appropriate key, nonce, ciphertext and additional data (headers)
being passed to the AEAD decryption function. If authentication is
successful, the plaintext is recovered and can be passed up to the
application. If authentication does not succeed, the connection is
terminated and an alert is generated.
