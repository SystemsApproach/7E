5.4 Passive Optical Network
-----------------------------

PON (Passive Optical Network) is the technology most commonly used to
deliver fiber-based broadband to homes and businesses. PON adopts a
point-to-multipoint design, which means the network is structured as a
tree, with a single point starting in the ISP’s network and then
fanning out to reach on the order of a thousand homes. PON gets its
name from the fact that the splitters are passive: they forward
optical signals downstream and upstream without actively
storing and forwarding frames. In this way, they are the optical
equivalent of repeaters used in the classic Ethernet. Framing then
happens at the source in the ISP’s premises, in a device called an
*Optical Line Terminal* (OLT), and at the end-points in individual
homes, in a device called an *Optical Network Unit* (ONU).

:numref:`Figure %s <fig-pon>` shows an example PON, simplified to
depict just one
ONU and one OLT. In practice, a Central Office would include multiple
OLTs connecting to thousands of customer homes. For completeness,
:numref:`Figure %s <fig-pon>` also includes two other details about
how the PON is
connected to the ISP’s backbone (and hence, to the rest of the
Internet). The *Agg Switch* aggregates traffic from a set of OLTs, and
the *BNG* (Broadband Network Gateway) is a piece of Telco equipment
that, among many other things, meters Internet traffic for the sake of
billing. As its name implies, the BNG is effectively the gateway between
the access network (everything to the left of the BNG) and the Internet
(everything to the right of the BNG).

.. _fig-pon:
.. figure:: shared/figures/pon.png
   :width: 600px
   :align: center

   An example PON that connects OLTs in the Central Office
   to ONUs in homes and businesses.

Because the splitters are passive, PON has to implement some form of
multi-access protocol. The approach it adopts can be summarized as
follows. First, upstream and downstream traffic are transmitted on two
different optical wavelengths, so they are completely independent of
each other. Downstream traffic starts at the OLT and the signal is
propagated down every link in the PON. As a consequence, every frame
reaches every ONU. This device then looks at a unique identifier in the
individual frames sent over the wavelength, and either keeps the frame
(if the identifier is for it) or drops it (if not). Encryption is used
to keep ONUs from eavesdropping on their neighbors’ traffic.

Upstream traffic is then time-division multiplexed on the upstream
wavelength, with each ONU periodically getting a turn to transmit.
Because the ONUs are distributed over a fairly wide area (measured in
kilometers) and at different distances from the OLT, it is not practical
for them to transmit based on synchronized clocks. Instead,
the OLT transmits *grants* to the individual ONUs, giving them a time
interval during which they can transmit. In other words, the single OLT
is responsible for centrally implementing the round-robin sharing of the
shared PON. This includes the possibility that the OLT can grant each
ONU a different share of time, effectively implementing different levels
of service.

PON is similar to Ethernet in the sense that it defines a sharing
algorithm that has evolved over time to accommodate higher and higher
bandwidths. G-PON (Gigabit-PON) is the most widely deployed today,
supporting a bandwidth of 2.25-Gbps per wavelength. XGS-PON (10
Gigabit-PON) is just now starting to be deployed.


