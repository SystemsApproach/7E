.. _artifact-mpeg:
.. _artifact-jpeg:

|Stream|.2 Coding and Compression
----------------------------------

Multimedia data, comprised of audio, video, and still images, now makes
up the majority of traffic on the Internet. Part of what has made the
widespread transmission of multimedia across networks possible is
advances in compression technology. Because multimedia data is consumed
mostly by humans using their senses—vision and hearing—and processed by
the human brain, there are unique challenges to compressing it. You want
to try to keep the information that is most important to a human, while
getting rid of anything that doesn’t improve the human’s perception of
the visual or auditory experience. Hence, both computer science and the
study of human perception come into play. In this section, we’ll look at
some of the major efforts in representing and compressing multimedia
data.

The uses of compression are not limited to multimedia data of course—for
example, you may well have used a utility like ``zip``
to compress files before sending them over a network, or to uncompress a
data file after downloading. It turns out that the techniques used for
compressing data—which are typically *lossless*, because most people
don’t like to lose data from a file—also show up as part of the solution
for multimedia compression. In contrast, *lossy compression*, commonly
used for multimedia data, does not promise that the data received is
exactly the same as the data sent. As noted above, this is because
multimedia data often contains information that is of little utility to
the human who receives it. Our senses and brains can only perceive so
much detail. They are also very good at filling in missing pieces and
even correcting some errors in what we see or hear. And, lossy
algorithms typically achieve much better compression ratios than do
their lossless counterparts; they can be an order of magnitude better or
more.

To get a sense of how important compression has been to the spread of
networked multimedia, consider the following example. A high-definition
TV screen has something like 1080 × 1920 pixels, each of which has 24
bits of color information, so each frame is

.. centered:: 1080 × 1920 × 24 = 50 *Mb*

If you want to send 24 frames per second, that would be over
1 Gbps.  That’s more than most Internet users have access to even
today, and more by orders
of magnitude for most of the history of the Internet.  By
contrast, modern compression techniques can get a reasonably
high-quality HDTV signal down to the range of 10 Mbps, a two orders of
magnitude reduction and well within the reach of most broadband users.
Similar compression gains apply to lower quality video such as YouTube
clips—Web video could never have reached its current popularity
without compression to make all those videos fit within
the bandwidth of contemporary networks.

This section works through the problem in three steps: (1) how media
is represented in the first place (independent of compression); (2)
how still images are compressed, and (3) how video, which is
conceptually a sequence of still images, is compressed. We use the
familiar JPEG and MPEG standards to illustrate the ideas.

|Stream|.2.1 Image Representation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Given the ubiquitous use of digital imagery—this use was spawned by
the invention of graphical displays, not high-speed networks—the need
for standard representation formats and compression algorithms for
digital imagery data has become essential. In response to this need,
the ISO defined a digital image format known as *JPEG*, named after
the Joint Photographic Experts Group that designed it. (The “Joint” in
JPEG stands for a joint ISO/ITU effort.) JPEG is the most widely used
format for still images in use today. At the heart of the definition
of the format is a compression algorithm, which we describe in the
next subsection.  But we start with the simpler question of exactly
what makes up a digital image.

For starters, it's probably obvious that digital images are made up of
pixels (hence, the megapixels quoted in smartphone camera
advertisements). Each pixel represents one location in the
two-dimensional grid that makes up the image, and for color images
each pixel has some numerical value representing a color. There are
lots of ways to represent colors, referred to as *color spaces*; the
one most people are familiar with is RGB (red, green, blue). You can
think of color as being a three dimensional quantity—you can make any
color out of red, green, and blue light in different amounts. In a
three-dimensional space, there are lots of different, valid ways to
describe a given point (consider Cartesian and polar coordinates, for
example). Similarly, there are various ways to describe a color using
three quantities, and the most common alternative to RGB is YUV. The Y
is luminance, roughly the overall brightness of the pixel, and U and V
contain chrominance, or color information. Confoundingly, there are a
few different variants of the YUV color space as well. More on this in
a moment.

The significance of this discussion is that the encoding and
transmission of color images (either still or moving) requires agreement
between the two ends on the color space. Otherwise, of course, you’d end
up with different colors being displayed by the receiver than were
captured by the sender. Hence, agreeing on a color space definition (and
perhaps a way to communicate which particular space is in use) is part
of the definition of any image or video format.

The JPEG format is tailored for photographic images. It starts by
transforming the RGB colors (which are what you usually get out of a
digital camera) to the YUV space. The reason for this has to do with
the way the eye perceives images. There are receptors in the eye for
brightness, and separate receptors for color. Because we’re very good
at perceiving variations in brightness, it makes sense to spend more
bits on transmitting brightness information. Since the Y component of
YUV is, roughly, the brightness of the pixel, we can compress that
component separately, and less aggressively, from the other two
(chrominance) components.

As noted above, YUV and RGB are alternative ways to describe a point in
a 3-dimensional space, and it’s possible to convert from one color space
to another using linear equations. For one YUV space that is commonly
used to represent digital images, the equations are:

::

   Y = 0.299R + 0.587G + 0.114B
   U = (B-Y) x 0.565
   V =  (R-Y) x 0.713

The exact values of the constants here are not important, as long as the
encoder and decoder agree on what they are. (The decoder will have to
apply the inverse transformations to recover the RGB components needed
to drive a display.) The constants are, however, carefully chosen based
on the human perception of color. You can see that Y, the luminance, is
a sum of the red, green, and blue components, while U and V are color
difference components. U represents the difference between the luminance
and blue, and V the difference between luminance and red. You may notice
that setting R, G, and B to their maximum values (which would be 255 for
8-bit representations) will also produce a value of Y=255 while U and V
in this case would be zero. That is, a fully white pixel is
(255,255,255) in RGB space and (255,0,0) in YUV space.

.. _fig-yuvsub:
.. figure:: stream/figures/f07-11-9780123850591.png
   :width: 500px
   :align: center

   Subsampling the U and V components of an image.

|Stream|.2.2 Image Compression
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Once the image has been transformed into YUV space, we are ready to
compress each of the three components separately. We want to be more
aggressive in compressing the U and V components, to which human eyes
are less sensitive. One way to compress the U and V components is to
*subsample* them. The basic idea of subsampling is to take a number of
adjacent pixels, calculate the average U or V value for that group of
pixels, and transmit that, rather than sending the value for every
pixel. :numref:`Figure %s <fig-yuvsub>` illustrates the point. The
luminance (Y) component is not subsampled, so the Y value of all the
pixels will be transmitted, as indicated by the 16 × 16 grid of pixels
on the left. In the case of U and V, we treat each group of four
adjacent pixels as a group, calculate the average of the U or V value
for that group, and transmit that. Hence, we end up with an 8 × 8 grid
of U and V values to transmit. Thus, in this example, for every four
pixels, we transmit six values (four Y and one each of U and V) rather
than the original 12 values (four each for all three components), for
a 50% reduction in information.

It’s worth noting that you could be either more or less aggressive in
the subsampling, with corresponding increases in compression and
decreases in quality. The subsampling approach shown here, in which
chrominance is subsampled by a factor of two in both horizontal and
vertical directions (and which goes by the identification 4:2:0),
happens to match the most commonly used approach.

.. _fig-jpeg:
.. figure:: stream/figures/f07-12-9780123850591.png
   :width: 550px
   :align: center

   Block diagram of JPEG compression.

Once subsampling is done, we now have three grids of pixels to deal
with, and each one is dealt with separately. JPEG compression of each
component takes place in three phases, as illustrated in :numref:`Figure
%s <fig-jpeg>`. On the compression side, the image is fed through these
three phases one 8 × 8 block at a time. The first phase applies the
discrete cosine transform (DCT) to the block. If you think of the image
as a signal in the spatial domain, then DCT transforms this signal into
an equivalent signal in the *spatial frequency* domain. This is a
lossless operation but a necessary precursor to the next, lossy step.
After the DCT, the second phase applies a quantization to the resulting
signal and, in so doing, loses the least significant information
contained in that signal. The third phase encodes the final result, but
in so doing also adds an element of lossless compression to the lossy
compression achieved by the first two phases. Decompression follows
these same three phases, but in reverse order.

DCT Phase
++++++++++

DCT is a transformation closely related to the fast Fourier transform
(FFT). It takes an 8 × 8 matrix of pixel values as input and outputs an
8 × 8 matrix of frequency coefficients. You can think of the input
matrix as a 64-point signal that is defined in two spatial dimensions
(*x* and *y*); DCT breaks this signal into 64 spatial frequencies. To
get an intuitive feel for spatial frequency, imagine yourself moving
across a picture in, say, the *x* direction. You would see the value of
each pixel varying as some function of *x*. If this value changes slowly
with increasing *x*, then it has a low spatial frequency; if it changes
rapidly, it has a high spatial frequency. So the low frequencies
correspond to the gross features of the picture, while the high
frequencies correspond to fine detail. The idea behind the DCT is to
separate the gross features, which are essential to viewing the image,
from the fine detail, which is less essential and, in some cases, might
be barely perceived by the eye.

DCT, along with its inverse, which recovers the original pixels and
during decompression, are defined by the following formulas:

.. math::

   \begin{aligned}
   DCT(i,j) &=&  \frac{1}{\sqrt{2N}} C(i) C(j) \sum_{x=0}^{N-1}
    \sum_{y=0}^{N-1} pixel(x, y)
    \cos \left[ \frac{(2x+1)i \pi}{2N}\right]
    \cos \left[ \frac{(2y+1)j \pi}{2N}\right]\\
   \mathit{pixel}(x,y) &=&  \frac{1}{\sqrt{2N}} \sum_{i=0}^{N-1}
    \sum_{j=0}^{N-1} C(i) C(j) DCT(i, j)
    \cos \left[ \frac{(2x+1)i \pi}{2N}\right]
    \cos \left[ \frac{(2y+1)j \pi}{2N}\right]
   \end{aligned}

where :math:`C(x) = 1/\sqrt{2}` when :math:`x=0` and :math:`1` when
:math:`x>0`, and :math:`pixel(x,y)` is the grayscale value of the pixel
at position *(x,y)* in the 8 × 8 block being compressed; N = 8 in this case.

The first frequency coefficient, at location (0,0) in the output matrix,
is called the *DC coefficient*. Intuitively, we can see that the DC
coefficient is a measure of the average value of the 64 input pixels.
The other 63 elements of the output matrix are called the *AC
coefficients*. They add the higher-spatial-frequency information to this
average value. Thus, as you go from the first frequency coefficient
toward the 64th frequency coefficient, you are moving from low-frequency
information to high-frequency information, from the broad strokes of the
image to finer and finer detail. These higher-frequency coefficients are
increasingly unimportant to the perceived quality of the image. It is
the second phase of JPEG that decides which portion of which
coefficients to throw away.

Quantization Phase
++++++++++++++++++++++

The second phase of JPEG is where the compression becomes lossy. DCT
does not itself lose information; it just transforms the image into a
form that makes it easier to know what information to remove. (Although
not lossy, *per se*, there is of course some loss of precision during
the DCT phase because of the use of fixed-point arithmetic.)
Quantization is easy to understand—it’s simply a matter of dropping the
insignificant bits of the frequency coefficients.

To see how the quantization phase works, imagine that you want to
compress some whole numbers less than 100, such as 45, 98, 23, 66, and
7. If you decided that knowing these numbers truncated to the nearest
multiple of 10 is sufficient for your purposes, then you could divide
each number by the quantum 10 using integer arithmetic, yielding 4, 9,
2, 6, and 0. These numbers can each be encoded in 4 bits rather than the
7 bits needed to encode the original numbers.

.. _tab-quant:
.. table::  Example JPEG Quantization Table.
   :widths: auto
   :align: center

   +---------+----+----+----+----+----+----+----+
   | Quantum |    |    |    |    |    |    |    |
   +=========+====+====+====+====+====+====+====+
   | 3       | 5  | 7  | 9  | 11 | 13 | 15 | 17 |
   +---------+----+----+----+----+----+----+----+
   | 5       | 7  | 9  | 11 | 13 | 15 | 17 | 19 |
   +---------+----+----+----+----+----+----+----+
   | 7       | 9  | 11 | 13 | 15 | 17 | 19 | 21 |
   +---------+----+----+----+----+----+----+----+
   | 9       | 11 | 13 | 15 | 17 | 19 | 21 | 23 |
   +---------+----+----+----+----+----+----+----+
   | 11      | 13 | 15 | 17 | 19 | 21 | 23 | 25 |
   +---------+----+----+----+----+----+----+----+
   | 13      | 15 | 17 | 19 | 21 | 23 | 25 | 27 |
   +---------+----+----+----+----+----+----+----+
   | 15      | 17 | 19 | 21 | 23 | 25 | 27 | 29 |
   +---------+----+----+----+----+----+----+----+
   | 17      | 19 | 21 | 23 | 25 | 27 | 29 | 31 |
   +---------+----+----+----+----+----+----+----+

Rather than using the same quantum for all 64 coefficients, JPEG uses
a quantization table that gives the quantum to use for each of the
coefficients, as specified in the formula given below. You can think
of this table (``Quantum``) as a parameter that can be set to control
how much information is lost and, correspondingly, how much
compression is achieved. In practice, the JPEG standard specifies a
set of quantization tables that have proven effective in compressing
digital images; an example quantization table is given in
:numref:`Table %s <tab-quant>`. In tables like this one, the low
coefficients have a quantum close to 1 (meaning that little
low-frequency information is lost) and the high coefficients have
larger values (meaning that more high-frequency information is
lost). Notice that as a result of such quantization tables many of the
high-frequency coefficients end up being set to 0 after quantization,
making them ripe for further compression in the third phase.

The basic quantization equation is

::

   QuantizedValue(i,j) = IntegerRound(DCT(i,j)/Quantum(i,j))

where

::

   IntegerRound(x) =
       Floor(x + 0.5) if x >= 0
       Floor(x - 0.5) if x < 0

Decompression is then simply defined as

::

   DCT(i,j) = QuantizedValue(i,j) x Quantum(i,j)

For example, if the DC coefficient (i.e., DCT(0,0)) for a particular
block was equal to 25, then the quantization of this value using
:numref:`Table %s <tab-quant>` would result in

::

   Floor(25/3+0.5) = 8

During decompression, this coefficient would then be restored as 8 × 3 =
24.

Encoding Phase
++++++++++++++++++++++

The final phase of JPEG encodes the quantized frequency coefficients
in a compact form. This results in additional compression, but this
compression is lossless. Starting with the DC coefficient in position
(0,0), the coefficients are processed in the zigzag sequence shown in
:numref:`Figure %s <fig-zigzag>`. Along this zigzag, a form of Run
Length Encoding (RLE) is used—RLE is applied to only the 0
coefficients, which is significant because many of the later
coefficients are 0. The individual coefficient values are then encoded
using a Huffman code. (RLE and Huffman codes are two examples of
*lossless* compression algorithms; see the sidebar for a summary.)

.. _fig-zigzag:
.. figure:: stream/figures/f07-13-9780123850591.png
   :width: 300px
   :align: center

   Zigzag traversal of quantized frequency coefficients.

.. sidebar:: Other Compression Algorithms

   *When thinking about how to encode a piece of data in a set of
   bits, we might just as well think about how to encode the data in
   the smallest set of bits possible. For example, if a block of data
   is made up of the 26 symbols A through Z, and if all of these
   symbols have an equal chance of occurring in the data block you are
   encoding, then encoding each symbol in 5 bits is the best you can
   do (since 25 = 32 is the lowest power of 2 above 26). If, however,
   the symbol R occurs 50% of the time, then it would be a good idea
   to use fewer bits to encode the R than any of the other symbols. In
   general, if you know the relative probability that each symbol will
   occur in the data, then you can assign a different number of bits
   to each possible symbol in a way that minimizes the number of bits
   it takes to encode a given block of data. This is the essential
   idea of Huffman codes, one of the important early developments in
   data compression.*

   *Run length encoding (RLE) is a another compression technique.  The
   idea is to replace consecutive occurrences of a given symbol with
   only one copy of the symbol, plus a count of how many times that
   symbol occurs—hence, the name run length. For example, the string
   AAABBCDDDD would be encoded as 3A2B1C4D.*

   *Another straightforward idea is the dictionary-based approach, of
   which the Lempel-Ziv (LZ) compression algorithm is the best
   known. The idea is to build a dictionary (table) of variable-length
   strings (think of them as common phrases) that you expect to find
   in the data and then to replace each of these strings when it
   appears in the data with the corresponding index to the dictionary.*

In addition, because the DC coefficient contains a large percentage of
the information about the 8 × 8 block from the source image, and images
typically change slowly from block to block, each DC coefficient is
encoded as the difference from the previous DC coefficient. This is the
delta encoding approach described in the next section.

JPEG includes a number of variations that control how much compression
you achieve versus the fidelity of the image. This can be done, for
example, by using different quantization tables. These variations, plus
the fact that different images have different characteristics, make it
impossible to say with any precision the compression ratios that can be
achieved with JPEG. Ratios of 30:1 are common, and higher ratios are
certainly possible, but *artifacts* (noticeable distortion due to
compression) become more severe at higher ratios.

|Stream|.2.3 Video Compression
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

We now turn our attention to the MPEG format, named after the Moving
Picture Experts Group that defined it. To a first approximation, a
moving picture (i.e., video) is simply a succession of still images—also
called *frames* or *pictures*—displayed at some video rate. Each of
these frames can be compressed using the same DCT-based technique used
in JPEG. Stopping at this point would be a mistake, however, because it
fails to remove the interframe redundancy present in a video sequence.
For example, two successive frames of video will contain almost
identical information if there is not much motion in the scene, so it
would be unnecessary to send the same information twice. Even when there
is motion, there may be plenty of redundancy since a moving object may
not change from one frame to the next; in some cases, only its position
changes. MPEG takes this interframe redundancy into consideration. MPEG
also defines a mechanism for encoding an audio signal with the video,
but we consider only the video aspect of MPEG in this section.

Frame Types
++++++++++++

MPEG takes a sequence of video frames as input and compresses them into
three types of frames, called *I frames* (intrapicture), *P frames*
(predicted picture), and *B frames* (bidirectional predicted picture).
Each frame of input is compressed into one of these three frame types.
I frames can be thought of as reference frames; they are self-contained,
depending on neither earlier frames nor later frames. To a first
approximation, an I frame is simply the JPEG compressed version of the
corresponding frame in the video source. P and B frames are not
self-contained; they specify relative differences from some reference
frame. More specifically, a P frame specifies the differences from the
previous I frame, while a B frame gives an interpolation between the
previous and subsequent I or P frames.

.. _fig-mpeg:
.. figure:: stream/figures/f07-14-9780123850591.png
   :width: 500px
   :align: center

   Sequence of I, P, and B frames generated by MPEG.

:numref:`Figure %s <fig-mpeg>` illustrates a sequence of seven video
frames that, after being compressed by MPEG, result in a sequence of
I, P, and B frames. The two I frames stand alone; each can be
decompressed at the receiver independently of any other frames. The
P frame depends on the preceding I frame; it can be decompressed at
the receiver only if the preceding I frame also arrives. Each of the
B frames depends on both the preceding I or P frame and the subsequent
I or P frame. Both of these reference frames must arrive at the
receiver before MPEG can decompress the B frame to reproduce the
original video frame.

Note that, because each B frame depends on a later frame in the
sequence, the compressed frames are not transmitted in sequential
order.  Instead, the sequence I B B P B B I shown in :numref:`Figure
%s <fig-mpeg>` is transmitted as I P B B I B B. Also, MPEG does not
define the ratio of I frames to P and B frames; this ratio may vary
depending on the required compression and picture quality. For
example, it is permissible to transmit only I frames. This would be
similar to using JPEG to compress the video.

In contrast to the preceding discussion of JPEG, the following focuses
on the *decoding* of an MPEG stream. It is a little easier to describe,
and it is the operation that is more often implemented in networking
systems today, since MPEG coding is so expensive that it is frequently
done offline (i.e., not in real time). For example, in a video-on-demand
system, the video would be encoded and stored on disk ahead of time.
When a viewer wanted to watch the video, the MPEG stream would then be
transmitted to the viewer’s machine, which would decode and display the
stream in real time.

Let’s look more closely at the three frame types. As mentioned above,
I frames are approximately equal to the JPEG compressed version of the
source frame. The main difference is that MPEG works in units of 16 × 16
macroblocks. For a color video represented in YUV, the U and V
components in each macroblock are subsampled into an 8 × 8 block, as we
discussed above in the context of JPEG. Each 2 × 2 subblock in the
macroblock is given by one U value and one V value—the average of the
four pixel values. The subblock still has four Y values. The
relationship between a frame and the corresponding macroblocks is given
in :numref:`Figure %s <fig-macroblock>`.

.. _fig-macroblock:
.. figure:: stream/figures/f07-15-9780123850591.png
   :width: 500px
   :align: center

   Each frame as a collection of macroblocks.

The P and B frames are also processed in units of macroblocks.
Intuitively, we can see that the information they carry for each
macroblock captures the motion in the video; that is, it shows in what
direction and how far the macroblock moved relative to the reference
frame(s). The following describes how a B frame is used to reconstruct a
frame during decompression; P frames are handled in a similar manner,
except that they depend on only one reference frame instead of two.

Before getting to the details of how a B frame is decompressed, we first
note that each macroblock in a B frame is not necessarily defined
relative to both an earlier and a later frame, as suggested above, but
may instead simply be specified relative to just one or the other. In
fact, a given macroblock in a B frame can use the same intracoding as is
used in an I frame. This flexibility exists because if the motion
picture is changing too rapidly then it sometimes makes sense to give
the intrapicture encoding rather than a forward- or backward-predicted
encoding. Thus, each macroblock in a B frame includes a type field that
indicates which encoding is used for that macroblock. In the following
discussion, however, we consider only the general case in which the
macroblock uses bidirectional predictive encoding.

In such a case, each macroblock in a B frame is represented with a
4-tuple: (1) a coordinate for the macroblock in the frame, (2) a
motion vector relative to the previous reference frame, (3) a motion
vector relative to the subsequent reference frame, and (4) a delta
(:math:`\delta`) for each pixel in the macroblock (i.e., how much each
pixel has changed relative to the two reference pixels). For each
pixel in the macroblock, the first task is to find the corresponding
reference pixel in the past and future reference frames. This is done
using the two motion vectors associated with the macroblock. Then, the
delta for the pixel is added to the average of these two reference
pixels. Stated more precisely, if we let F\ :sub:`p` and F\ :sub:`f`
denote the past and future reference frames, respectively, and the
past/future motion vectors are given by (x\ :sub:`p`, y\ :sub:`p`) and
(x\ :sub:`f`, y\ :sub:`f`), then the pixel at coordinate *(x,y)* in the current
frame (denoted F\ :sub:`c`) is computed as

.. math::

   F_c(x,y) = (F_p(x+x_p,y+y_p) + F_f(x+x_f,y+y_f))/2 + \delta(x,y)

where :math:`\delta` is the delta for the pixel as specified in the B frame.
These deltas are encoded in the same way as pixels in I frames; that is,
they are run through DCT and then quantized. Since the deltas are typically
small, most of the DCT coefficients are 0 after quantization; hence, they can
be effectively compressed.

It should be fairly clear from the preceding discussion how encoding
would be performed, with one exception. When generating a B or P frame
during compression, MPEG must decide where to place the macroblocks.
Recall that each macroblock in a P frame, for example, is defined
relative to a macroblock in an I frame, but that the macroblock in the
P frame need not be in the same part of the frame as the corresponding
macroblock in the I frame—the difference in position is given by the
motion vector. You would like to pick a motion vector that makes the
macroblock in the P frame as similar as possible to the corresponding
macroblock in the I frame, so that the deltas for that macroblock can be
as small as possible. This means that you need to figure out where
objects in the picture moved from one frame to the next. This is the
problem of *motion estimation*, and several techniques (heuristics) for
solving this problem are known. (We discuss papers that consider this
problem at the end of this chapter.) The difficulty of this problem is
one of the reasons why MPEG encoding takes longer than decoding on
equivalent hardware. MPEG does not specify any particular technique; it
only defines the format for encoding this information in B and P frames
and the algorithm for reconstructing the pixel during decompression, as
given above.

Effectiveness and Performance
+++++++++++++++++++++++++++++++++

MPEG typically achieves a compression ratio of 90:1, although ratios as
high as 150:1 are not unheard of. In terms of the individual frame
types, we can expect a compression ratio of approximately 30:1 for the
I frames (this is consistent with the ratios achieved using JPEG when
24-bit color is first reduced to 8-bit color), while P and B frame
compression ratios are typically three to five times smaller than the
rates for the I frame. Without first reducing the 24 bits of color to
8 bits, the achievable compression with MPEG is typically between 30:1
and 50:1.

MPEG involves a set of relatively expensive computations. Compression
can be done in an offline manner when preparing movies for a
video-on-demand service.  As video has become mainstream for real-time
communication, hardware that is well suited to video compression, such
as GPUs, has become standard on end systems. On the decompression
side, MPEG decoding is often done in software, with GPUs again playing
a role in many cases.

Video Encoding Standards
++++++++++++++++++++++++++++++

We conclude by noting that MPEG is an evolving standard of significant
complexity. This complexity comes from a desire to give the encoding
algorithm every possible degree of freedom in how it encodes a given
video stream, resulting in different video transmission rates. It also
comes from the evolution of the standard over time, with the Moving
Picture Experts Group working hard to retain backwards compatibility
(e.g., MPEG-1, MPEG-2, MPEG-4, MPEG-H). What we describe in this book
is the essential ideas underlying MPEG-based compression, but
certainly not all the intricacies involved in an international standard.

What’s more, MPEG is not the only standard available for encoding
video.  For example, the ITU-T has also defined the *H series* for
encoding real-time multimedia data. Generally, the H series includes
standards for video, audio, control, and multiplexing (e.g., mixing
audio, video, and data onto a single bit stream). Within the series,
H.261 and H.263 were the first- and second-generation video encoding
standards. In principle, both H.261 and H.263 look a lot like MPEG:
They use DCT, quantization, and interframe compression. The
differences between H.261/H.263 and MPEG are in the details.

A partnership between the ITU-T and the MPEG group lead to the joint
H.264/MPEG-4 standard, which is used for both Blu-ray Discs and by
many popular streaming sources, such as YouTube. The recent upgrade of
graphical displays to 4k comes with yet another round of
standardization, published as H.265/MPEG-H.

