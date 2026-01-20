3.1 Link Technology
---------------------------------

As introduced in Chapter 1, all network communication depends on
transmitting and receiving electromagnetic signals over some physical
medium, be it through the atmosphere, or over copper wires or optical
fibers. Each signal can be modeled as a sine function of some
amplitude, frequency, and phase. This happens within the frequency
bands depicted in :numref:`Figure %s <fig-spectrum>`.

.. _fig-spectrum:
.. figure:: switches/figures/spectrum.png
   :width: 650px
   :align: center

   Electromagnetic spectrum.

The first challenge of communication is to convert a digital signal
into the available analog signal, a process is known as *modulation*.
For example, *frequency modulation* corresponds to alternating between
a "high frequency" and a "low frequency". What makes modulation a hard
problem is that the receiver has to recover the intended digital value
from analog signal in the face of noise and attenuation.

This brings us to the second challenge, which is encoding digital data
(i.e., 1’s and 0’s) onto this digital signal. On the surface, this
encoding seems simple enough—the high signal encodes a 1 and the low
signal encodes a 0—but in practice the digital signal may have more
than two (high/low) settings. If there are four recoverable digital
signals, for example, then two bits could be coded in each. In
general, we think of the digital signal as carrying symbols rather
than bits, where each symbol is one or more bits in length.
:numref:`Figure %s <fig-signals>` summarizes the layers involved in
encoding binary data for transmission.

.. _fig-signals:
.. figure:: switches/figures/signals.png
   :width: 500px
   :align: center

   Binary data encoded in a digital signal, and in turn modulated over
   an analog signal.

It should be clear that the engineering required to transmit and
receive digital messages over a physical medium is non-trivial. The
same is true for the science—known as *Information Theory*\ —that
explains how efficiently that task can be performed. We consider both
Information Theory and modulation techniques out-of-scope for this
book, and refer the reader to authoritative sources on the topic.

That still leaves us with plenty of work to do, corresponding to what
is usually referred to as the link layer, or sometimes *Layer 2 (L2)*\
—terms originally coined by the OSI reference model presented in
Chapter 1.  A related term you will often see is *Medium Access
Control *(MAC)*, indicating that the focus is on controlling on how
the sending and receiving nodes access a physical medium.

This section introduces the problems addressed by the link layer, and
uses Ethernet as its representative example. The Ethernet has
undergone significant changes since it was first introduced in the
mid-1970s, not the least of which is that it is now possible to
transmit Ethernet packets over fiber optic links at bandwidths
approaching 1-Tbps, many orders of magnitude greater than the original
2.94-Mbps rate over coaxial cable. We focus on the modern version of
Ethernet in the next three subsections, and then fill in some of the
historical background in Section 2.1.4.

2.1.1 Encoding
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The first task is to encode the binary data that the source node wants
to send into the signals that the physical links are able to carry and
then to decode the signal back into the corresponding binary data at
the receiving node. Ignoring the details of modulation, and assuming
just two discrete signals (high and low) for the time being, the
obvious thing to do is to map the data value 1 onto the high signal
and the data value 0 onto the low signal. This is exactly the mapping
used by an encoding scheme called *non-return to zero* (NRZ).  For
example, :numref:`Figure %s <fig-nrz>` schematically depicts the
NRZ-encoded signal (bottom) that corresponds to the transmission of a
particular sequence of bits (top).

.. _fig-nrz:
.. figure:: switches/figures/f02-04-9780123850591.png
   :width: 400px
   :align: center

   NRZ encoding of a bit stream.

The problem with NRZ is that a sequence of several consecutive 1s means
that the signal stays high on the link for an extended period of time;
similarly, several consecutive 0s means that the signal stays low for a
long time. There are two fundamental problems caused by long strings of
1s or 0s. The first is that it leads to a situation known as *baseline
wander*. Specifically, the receiver keeps an average of the signal it
has seen so far and then uses this average to distinguish between low
and high signals. Whenever the signal is significantly lower than this
average, the receiver concludes that it has just seen a 0; likewise, a
signal that is significantly higher than the average is interpreted to
be a 1. The problem, of course, is that too many consecutive 1s or 0s
cause this average to change, making it more difficult to detect a
significant change in the signal.

The second problem is that frequent transitions from high to low and
*vice versa* are necessary to enable *clock recovery*. Intuitively, the
clock recovery problem is that both the encoding and decoding processes
are driven by a clock—every clock cycle the sender transmits a bit and
the receiver recovers a bit. The sender’s and the receiver’s clocks have
to be precisely synchronized in order for the receiver to recover the
same bits the sender transmits. If the receiver’s clock is even slightly
faster or slower than the sender’s clock, then it does not correctly
decode the signal. You could imagine sending the clock to the receiver
over a separate wire, but this is typically avoided because it makes the
cost of cabling twice as high. So, instead, the receiver derives the
clock from the received signal—the clock recovery process. Whenever the
signal changes, such as on a transition from 1 to 0 or from 0 to 1, then
the receiver knows it is at a clock cycle boundary, and it can
resynchronize itself. However, a long period of time without such a
transition leads to clock drift. Thus, clock recovery depends on having
lots of transitions in the signal, no matter what data is being sent.

One approach that addresses this problem, called *non-return to zero
inverted* (NRZI), has the sender make a transition from the current
signal to encode a 1 and stay at the current signal to encode
a 0. This solves the problem of consecutive 1s, but obviously does
nothing for consecutive 0s. NRZI is illustrated in :numref:`Figure %s
<fig-encode-all>`. An alternative, called *Manchester encoding*, does
a more explicit job of merging the clock with the signal by
transmitting the exclusive OR of the NRZ-encoded data and the
clock. (Think of the local clock as an internal signal that alternates
from low to high; a low/high pair is considered one clock cycle.) The
Manchester encoding is also illustrated in :numref:`Figure %s
<fig-encode-all>`. Observe that the Manchester encoding results in 0
being encoded as a low-to-high transition and 1 being encoded as a
high-to-low transition. Because both 0s and 1s result in a transition
to the signal, the clock can be effectively recovered at the
receiver.

.. _fig-encode-all:
.. figure:: switches/figures/f02-05-9780123850591.png
   :width: 400px
   :align: center

   Different encoding strategies.

The problem with the Manchester encoding scheme is that it doubles the
rate at which signal transitions are made on the link, which means that
the receiver has half the time to detect each pulse of the signal. The
rate at which the signal changes is called the link’s *baud rate*. In
the case of the Manchester encoding, the bit rate is half the baud rate,
so the encoding is considered only 50% efficient. Keep in mind that if
the receiver had been able to keep up with the faster baud rate required
by the Manchester encoding in :numref:`Figure %s <fig-encode-all>`, then
both NRZ and NRZI could have been able to transmit twice as many bits
in the same time period.

Note that bit rate isn’t necessarily less than or equal to the baud
rate, as the Manchester encoding suggests. If the modulation scheme is
able to utilize (and recognize) four different signals, as opposed to
just two (e.g., “high” and “low”), then it is possible to encode two bits
into each clock interval, resulting in a bit rate that is twice the baud
rate. Similarly, being able to modulate among eight different signals
means being able to transmit three bits per clock interval. In
general, it is important to keep in mind we have over-simplified
modulation, which is much more sophisticated than transmitting
"high" and "low" signals. It is not uncommon to vary a combination
of a signal's phase and amplitude, making it possible to encode
16 or even 64 different patterns (often called *symbols*) during each
clock interval. *QAM (Quadrature Amplitude Modulation)* is widely used
example of such a modulation scheme.

A more efficient alternative, called *4B/5B*, attempts to address the
inefficiency of the Manchester encoding without suffering from the
problem of having extended durations of high or low signals. The idea
of 4B/5B is to insert extra bits into the bit stream so as to break up
long sequences of 0s or 1s. Specifically, every 4 bits of actual data
are encoded in a 5-bit code that is then transmitted to the receiver;
hence, the name 4B/5B. The 5-bit codes are selected in such a way that
each one has no more than one leading 0 and no more than two trailing
0s. Thus, when sent back-to-back, no pair of 5-bit codes results in
more than three consecutive 0s being transmitted. The resulting 5-bit
codes are then transmitted using the NRZI encoding, which explains why
the code is only concerned about consecutive 0s—NRZI already solves
the problem of consecutive 1s. Note that the 4B/5B encoding results in
80% efficiency.

.. _tab-4b5b:
.. table:: 4B/5B encoding.
   :align: center
   :widths: auto

   +-------------------+------------+
   | 4-bit Data Symbol | 5-bit Code |
   +===================+============+
   | 0000              | 11110      |
   +-------------------+------------+
   | 0001              | 01001      |
   +-------------------+------------+
   | 0010              | 10100      |
   +-------------------+------------+
   | 0011              | 10101      |
   +-------------------+------------+
   | 0100              | 01010      |
   +-------------------+------------+
   | 0101              | 01011      |
   +-------------------+------------+
   | 0110              | 01110      |
   +-------------------+------------+
   | 0111              | 01111      |
   +-------------------+------------+
   | 1000              | 10010      |
   +-------------------+------------+
   | 1001              | 10011      |
   +-------------------+------------+
   | 1010              | 10110      |
   +-------------------+------------+
   | 1011              | 10111      |
   +-------------------+------------+
   | 1100              | 11010      |
   +-------------------+------------+
   | 1101              | 11011      |
   +-------------------+------------+
   | 1110              | 11100      |
   +-------------------+------------+
   | 1111              | 11101      |
   +-------------------+------------+

:numref:`Table %s <tab-4b5b>` gives the 5-bit codes that correspond to
each of the 16 possible 4-bit data symbols. Notice that since 5 bits
are enough to encode 32 different codes, and we are using only 16 of
these for data, there are 16 codes left over that we can use for other
purposes. Of these, code ``11111`` is used when the line is idle, code
``00000`` corresponds to when the line is dead, and ``00100`` is
interpreted to mean halt. Of the remaining 13 codes, 7 of them are not
valid because they violate the “one leading 0, two trailing 0s,” rule,
and the other 6 represent various control symbols. Some of the framing
protocols described later in this chapter make use of these control
symbols.

As for our exemplar link technology, Ethernet has been adaptive as its
bandwidth improved over time. It originally used Manchester encoding
when it rat at 10Mbps speeds, but switched to 4B/5B when it was
upgraded to run at 100Mbps. The jump to 1Gbps Ethernet (also called
1GE) was coupled with a change to 8B/10B encoding (8 bits of data
encoded in 10 digital signals), and more recently, 10GE and above uses
a 64B/66B encoding.


2.1.2 Framing
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

2.1.3 Error Detection
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Bit errors are sometimes introduced into frames during transmission.
This happens, for example, because of electrical interference or
thermal noise. Although errors are rare, especially on optical links,
some mechanism is needed to detect these errors so that corrective
action can be taken.

One of the most common techniques for detecting transmission errors is
a technique known as the *cyclic redundancy check* (CRC). Ethernet
uses a 32-bit CRC code (denoted CRC-32), which works by adding only 32
extra bits to the message. These few bits are enough to provide strong
protection against common bit errors in messages that are thousands of
bytes long. The theoretical foundation of the cyclic redundancy check
is rooted in a branch of mathematics called *finite fields*. While
this may sound daunting, the basic ideas is easy to understood.

To start, think of an (n+1)-bit message as being represented by an :math:`n`
degree polynomial, that is, a polynomial whose highest-order term is
:math:`x^{n}`. The message is represented by a polynomial by using the
value of each bit in the message as the coefficient for each term in
the polynomial, starting with the most significant bit to represent
the highest-order term. For example, an 8-bit message consisting of
the bits 10011010 corresponds to the polynomial

.. math::

   M(x) = (1 \times x^7) + (0 \times x^6) + (0 \times x^5) + (1 \times
   x^4 )+ (1 \times x^3) + (0 \times x^2) + (1 \times x^1) + (0 \times x^0)

.. math::

   M(x) = x^7 + x^4 + x^3 + x^1

Abstractly, then, we can thus think of a sender and a receiver as
exchanging polynomials with each other.

For the purposes of calculating a CRC, a sender and receiver have to
agree on a *divisor* polynomial :math:`C(x)`; a polynomial of degree
:math:`k`. For example, suppose :math:`C(x) = x^3 + x^2 + 1`.  In this
case, :math:`k=3`. The answer to the question “Where did :math:`C(x)`
come from?” is, in most practical cases, “You look it up in a book.”
In fact, the choice of :math:`C(x)` has a significant impact on what
types of errors can be reliably detected, as we discuss below. There
are a handful of divisor polynomials that are very good choices for
various environments, and the exact choice is normally made as part of
the protocol design. For example, the Ethernet standard uses a
well-known polynomial of degree 32.

When a sender wishes to transmit a message :math:`M(x)` that is
n+1 bits long, what is actually sent is the (n+1)-bit message plus
:math:`k` bits. We call the complete transmitted message, including
the redundant bits, :math:`P(x)`. What we are going to do is contrive
to make the polynomial representing :math:`P(x)` exactly divisible by
:math:`C(x)`; we explain how this is achieved below. If :math:`P(x)`
is transmitted over a link and there are no errors introduced during
transmission, then the receiver should be able to divide :math:`P(x)`
by :math:`C(x)` exactly, leaving a remainder of zero. On the other
hand, if some error is introduced into :math:`P(x)` during
transmission, then in all likelihood the received polynomial will no
longer be exactly divisible by :math:`C(x)`, and thus the receiver
will obtain a nonzero remainder implying that an error has occurred.

It will help to understand the following if you know a little about
polynomial arithmetic; it is just slightly different from normal
integer arithmetic. We are dealing with a special class of polynomial
arithmetic here, where coefficients may be only one or zero, and
operations on the coefficients are performed using modulo 2
arithmetic. This is referred to as “polynomial arithmetic modulo 2.”
Since this is a networking book, not a mathematics text, let’s focus
on the key properties of this type of arithmetic for our purposes
(which we ask you to accept on faith):

- Any polynomial :math:`B(x)` can be divided by a divisor polynomial
  :math:`C(x)` if :math:`B(x)` is of higher degree than :math:`C(x)`.

- Any polynomial :math:`B(x)` can be divided once by a divisor
  polynomial :math:`C(x)` if :math:`B(x)` is of the same degree as :math:`C(x)`.

- The remainder obtained when :math:`B(x)` is divided by :math:`C(x)` is
  obtained by performing the exclusive OR (XOR) operation on each pair
  of matching coefficients.

For example, the polynomial :math:`x^3 + 1` can be divided by
:math:`x^3 + x^2 + 1` (because they are both of degree 3) and the
remainder would be :math:`0 \times x^3 + 1 \times x^2 + 0 \times x^1 +
0 \times x^0 = x^2` (obtained by XORing the coefficients of each
term). In terms of messages, we could say that 1001 can be divided by
1101 and leaves a remainder of 0100. You should be able to see that
the remainder is just the bitwise exclusive OR of the two messages.
And now that we know the basic rules for dividing polynomials, we are
able to do long division, which is necessary to deal with longer
messages. An example appears below.

Recall that we wanted to create a polynomial for transmission that is
derived from the original message :math:`M(x)`, is :math:`k` bits
longer than :math:`M(x)`, and is exactly divisible by :math:`C(x)`. We
can do this in the following way:

1. Multiply :math:`M(x)` by :math:`x^{k}`; that is, add :math:`k`
   zeros at the end of the message. Call this zero-extended message
   :math:`T(x)`.

2. Divide :math:`T(x)` by :math:`C(x)` and find the remainder.

3. Subtract the remainder from :math:`T(x)`.

It should be obvious that what is left at this point is a message that
is exactly divisible by :math:`C(x)`. We may also note that the
resulting message consists of :math:`M(x)` followed by the remainder
obtained in step 2, because when we subtracted the remainder (which
can be no more than :math:`k` bits long), we were just XORing it with
the :math:`k` zeros added in step 1. This part will become clearer
with an example.

Consider the message :math:`x^7 + x^4 + x^3 + x^1`, or 10011010.  We
begin by multiplying by :math:`x^3`, since our divisor polynomial is
of degree 3. This gives 10011010000.  We divide this by :math:`C(x)`,
which corresponds to 1101 in this case.  :numref:`Figure %s
<fig-crcalc>` shows the polynomial long-division operation.  Given the
rules of polynomial arithmetic described above, the long-division
operation proceeds much as it would if we were dividing
integers. Thus, in the first step of our example, we see that the
divisor 1101 divides once into the first four bits of the message
(1001), since they are of the same degree, and leaves a remainder of
100 (1101 XOR 1001). The next step is to bring down a digit from the
message polynomial until we get another polynomial with the same
degree as :math:`C(x)`, in this case 1001. We calculate the remainder
again (100) and continue until the calculation is complete. Note that
the “result” of the long division, which appears at the top of the
calculation, is not really of much interest—it is the remainder at the
end that matters.

You can see from the very bottom of :numref:`Figure %s <fig-crcalc>` that the
remainder of the example calculation is 101. So we know that 10011010000
minus 101 would be exactly divisible by :math:`C(x)`, and this is what we
send. The minus operation in polynomial arithmetic is the logical XOR
operation, so we actually send 10011010101. As noted above, this turns
out to be just the original message with the remainder from the long
division calculation appended to it. The recipient divides the received
polynomial by :math:`C(x)` and, if the result is 0, concludes that there were
no errors. If the result is nonzero, it may be necessary to discard the
corrupted message; with some codes, it may be possible to *correct* a
small error (e.g., if the error affected only one bit). A code that
enables error correction is called an *error-correcting code* (ECC).

.. _fig-crcalc:
.. figure:: switches/figures/f02-15-9780123850591.png
   :width: 400px
   :align: center

   CRC calculation using polynomial long division.

We now return to the question of where the polynomial :math:`C(x)`
comes from. Intuitively, the idea is to select this polynomial so that
it is very unlikely to divide evenly into a message that has errors
introduced into it. If the transmitted message is :math:`P(x)`, we may
think of the introduction of errors as the addition of another
polynomial :math:`E(x)`, so the recipient sees :math:`P(x) +
E(x)`. The only way that an error could slip by undetected would be if
the received message could be evenly divided by :math:`C(x)`, and
since we know that :math:`P(x)` can be evenly divided by :math:`C(x)`,
this could only happen if :math:`E(x)` can be divided evenly by
:math:`C(x)`. The trick is to pick :math:`C(x)` so that this is very
unlikely for common types of errors.

One common type of error is a single-bit error, which can be expressed
as :math:`E(x) = x^i` when it affects bit position *i*. If we select
:math:`C(x)` such that the first and the last term (that is, the
:math:`x^k` and :math:`x^0` terms) are nonzero, then we already have a
two-term polynomial that cannot divide evenly into the one term
:math:`E(x)`. Such a :math:`C(x)` can, therefore, detect all
single-bit errors. In general, it is possible to prove that the
following types of errors can be detected by a :math:`C(x)` with the
stated properties:

- All single-bit errors, as long as the :math:`x^{k}` and
  :math:`x^{0}` terms have nonzero coefficients

- All double-bit errors, as long as :math:`C(x)` has a factor with at
  least three terms

- Any odd number of errors, as long as :math:`C(x)` contains the
  factor :math:`(x + 1)`

- Any “burst” error (i.e., sequence of consecutive errored bits) for
  which the length of the burst is less than :math:`k` bits (Most
  burst errors of length greater than :math:`k` bits can also be
  detected.)

Six versions of :math:`C(x)` are widely used in link-level
protocols. For example, Ethernet uses CRC-32, which is defined as
follows:

-  CRC-32 = :math:`x^{32} + x^{26} + x^{23} + x^{22} + x^{16} +
   x^{12} + x^{11} + x^{10} + x^8 + x^7 + x^5 + x^4 + x^2 + x + 1`

Finally, we note that the CRC algorithm, while seemingly complex, is
easily implemented in hardware using a :math:`k`\ -bit shift register
and XOR gates. The number of bits in the shift register equals the
degree of the generator polynomial (:math:`k`). :numref:`Figure %s
<fig-crc-hard>` shows the hardware that would be used for the
generator :math:`x^3 + x^2 + 1` from our previous example. The message
is shifted in from the left, beginning with the most significant bit
and ending with the string of :math:`k` zeros that is attached to the
message, just as in the long division example. When all the bits have
been shifted in and appropriately XORed, the register contains the
remainder—that is, the CRC (most significant bit on the right). The
position of the XOR gates is determined as follows: If the bits in the
shift register are labeled 0 through :math:`k-1`, left to right, then
put an XOR gate in front of bit :math:`n` if there is a term
:math:`x^n` in the generator polynomial.  Thus, we see an XOR gate in
front of positions 0 and 2 for the generator :math:`x^3 + x^2 + x^0`.

.. _fig-crc-hard:
.. figure:: switches/figures/f02-16-9780123850591.png
   :width: 350px
   :align: center

   CRC calculation using shift register.


2.1.4 Historical Background
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

*[Describe Ethernet as a multi-access network. Refer back to Aloha, and forward to WiFi.]*
