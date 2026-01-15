3.1 Link Layer
---------------------------------

As introduced in Chapter 1, all network communication depends on
transmitting and receiving electromagnetic signals over some physical
medium, be it through the atmosphere, or over copper wires or optical
fibers. These signals are analog waves, and typically modeled as a
combination of sine functions of various amplitudes, frequencies, and
phases. The first challenge of communication is to map a digital
signal onto the available analog signal. The process is performed by a
Digital-to-Analog Converter (DAC). Every home stereo system includes a
DAC working in the audio range of the spectrum; for network
communication, we are typically working in the higher frequency bands
depicted in :numref:`Figure %s <fig-spectrum>`.

.. _fig-spectrum:
.. figure:: switches/figures/spectrum.png
   :width: 650px
   :align: center

   Electromagnetic spectrum.

Abstractly, then, we can picture network communication as a digital
signal being passed between nodes, where we can think of this signal
as corresponding to a “high frequency” and a “low frequency”, to give
just one example commonly known as *frequency modulation*. This brings
us to the second challenge, which is encoding digital data (i.e., 1’s
and 0’s) onto this digital signal. On the surface, this encoding seems
simple enough—the high signal encodes a 1 and the low signal encodes a
0—but in practice the digital signal may have more than two (high/low)
settings. If there are four recoverable digital signals, for example,
then two bits could be coded in each. In general, we think of the
digital signal as carrying symbols rather than bits, where each symbol
is one or more bits in length.

At this point, it should be clear that the engineering required to
transmit and receive digital messages over a physical medium is
non-trivial. The same is true for the science—known as *Information
Theory*\ —that explains how efficiently that task can be performed. We
consider Information Theory out-of-scope for this book, and refer the
reader to authoritative sources on the topic. For our purposes, we
pick up the story with the ability to send and receive symbols over a
link. At that point, we still have plenty of work to do, and it
corresponds to what is usually referred to as the link layer—a term
originally coined by the OSI reference model presented in Chapter 1.

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

2.1.2 Framing
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

2.1.3 Error Detection
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

2.1.4 Historical Background
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

*[Mention the Media Access Control (MAC) terminology. Refer back to
Aloha, and forward to WiFi.]*
