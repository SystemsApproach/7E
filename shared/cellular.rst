5.4 Mobile Cellular Network
--------------------------------------------

The Mobile Cellular Network, which has a 40-year history that
parallels the Internet's, has undergone significant change. The first
two generations supported voice and then text, with 3G defining the
transition to broadband access (with data rates measured in hundreds
of kilobits per second). The industry has since transitioned to 4G
(with data rates typically measured in the few megabits per second)
and more recently to 5G (with the promise of a tenfold increase in
data rates).

Starting with 3G, the *3rd Generation Partnership Project (3GPP)* is
the standards body for the global cellular network. 3GPP established
ambitious goals for 5G:

- To support Massive Internet of Things, potentially including devices
  with ultra-low energy (10+ years of battery life), ultra-low
  complexity (10s of bits per second), and ultra-high density (1
  million nodes per square kilometer).

- To support Mission-Critical Control, potentially including
  ultra-high availability (greater than 99.999% or “five nines”),
  ultra-low latency (as low as 1 ms), and extreme mobility (up to 100
  km/h).

- To support Enhanced Mobile Broadband, potentially including extreme
  data rates (multi-Gbps peak, 100+ Mbps sustained) and extreme
  capacity (10 Tbps of aggregate throughput per square kilometer).

It's fair to say that many of these goals remain more ambition than
reality, but we expect them to carry over to 6G, which is now under
active discussion.

Note that specific frequency bands that are licensed for cellular
networks vary around the world, and are complicated by the fact that
network operators often simultaneously support both old/legacy
technologies and new/next-generation technologies, each of which
occupies a different frequency band. The high-level summary is that
traditional cellular technologies range from 700-2400 MHz, with new
mid-spectrum allocations now happening at 6 GHz, and millimeter-wave
(mmWave) allocations opening above 24 GHz. We'll see how these new
bands are being exploited for different purposes in Section 5.4.4.


5.4.1 Overview
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

:numref:`Figure %s <fig-cellular>` gives a high-level view of the
mobile cellular network, which consists of two main subsystems: the
*Radio Access Network (RAN)* and the *Mobile Core*.  The RAN manages
the radio resources (i.e., spectrum), making sure it is used
efficiently and meets the quality of service (QoS) requirements of
every user. It corresponds to a distributed collection of base
stations, which are cryptically called *gNodeB (gNB)*.

.. _fig-cellular:
.. figure:: shared/figures/cellular.png
    :width: 600px
    :align: center

    Mobile cellular networks consist of a Radio Access Network (RAN)
    and a Mobile Core.

The Mobile Core is a bundle of functionality that serves several
purposes:

-  Authenticates devices prior to attaching them to the network
-  Provides Internet (IP) connectivity for both data and voice services.
-  Ensures this connectivity fulfills the promised QoS requirements.
-  Tracks user mobility to ensure uninterrupted service.
-  Tracks subscriber usage for billing and charging.

Note that the word “Core” is a bit misleading; the Mobile Core runs
near the edge of the network, effectively providing a bridge between
the RAN in some geographic area and the greater Internet.
3GPP provides significant flexibility in how the Mobile Core is
geographically deployed, ranging from minimal deployments (the RAN and
the mobile core can be co-located) to areas that are hundreds of
kilometers wide. A common model is that an instantiation of the Mobile
Core serves a metropolitan area. The corresponding RAN would then span
several dozens (or even hundreds) of cell towers in that geographic
area.

Taking a closer look at :numref:`Figure %s <fig-cellular>`, a
*Backhaul Network* interconnects the base stations that implement the
RAN with the Mobile Core. This network is typically wired, may or may
not have the ring topology shown in the figure, and is often
constructed from commodity components found elsewhere in the Internet,
such as switched Ethernet.

There are two more architectural concepts to introduce. First,
:numref:`Figure %s <fig-cups>` redraws components from :numref:`Figure
%s <fig-cellular>` to highlight the fact that a base station has an
analog component (depicted by an antenna) and a digital component
(depicted by a processor pair). This section primarily focuses on the
latter, and assumes the building blocks outlined in Section 5.2 for
the over-the-air radio transmissions.

.. _fig-cups:
.. figure:: shared/figures/cups.png
    :width: 400px
    :align: center

    Mobile Core divided into a Control Plane and a User Plane, an
    architectural feature known as CUPS: Control and User Plane
    Separation.

The second concept, also depicted in :numref:`Figure %s <fig-cups>`,
is to partition the Mobile Core into a *Control Plane* and *User
Plane*. This is similar to the control/data plane split described in
Chapter 3. 3GPP has introduced a corresponding acronym—\ *CUPS,
Control and User Plane Separation*—to denote this idea. One motivation
for CUPS is to enable control plane resources and data plane resources
to be scaled independently of each other.

5.4.2 Radio Access Network
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

We now describe the RAN by sketching the role each base station plays,
but keep in mind that the collection of base stations in a RAN are
cooperating to manage the radio spectrum as a whole. Since their
coverage overlaps, they need to cooperate with each other to make
globally optimal decisions.

First, each base station establishes the wireless channel for a
subscriber's UE upon power-up or upon handover when the UE is active.
This channel is released when the UE remains idle for a predetermined
period of time. Using 3GPP terminology, this wireless channel is said
to provide a *radio bearer*. The term “bearer” has historically been
used in telecommunications (including early wireline technologies such
as ISDN) to denote a data channel, as opposed to a channel that
carries signaling information.

.. _fig-active-ue:
.. figure:: shared/figures/active-ue.png
    :width: 500px
    :align: center

    UE detects (and connects to) base station.

Second, each base station establishes “3GPP Control Plane”
connectivity between the UE and the corresponding Mobile Core Control
Plane component, and forwards signaling traffic between the two. This
signaling traffic enables UE authentication, registration, and
mobility tracking.

.. _fig-control-plane:
.. figure:: shared/figures/control-plane.png
    :width: 500px
    :align: center

    Base Station establishes control plane connectivity
    between each UE and the Mobile Core.

Third, for each active UE, the base station establishes one or more
tunnels to the corresponding Mobile Core User Plane component.
:numref:`Figure %s <fig-user-plane>` shows just two (one for voice and
one for data), and while in practice 4G was limited to just two, 5G
aspires to support many such tunnels as part of a generalized network
slicing mechanism.

.. _fig-user-plane:
.. figure:: shared/figures/user-plane.png
    :width: 500px
    :align: center

    Base station establishes one or more tunnels between each UE and
    the Mobile Core's User Plane (known in 3GPP terms as PDU session).

Fourth, the base station forwards both control and user plane packets
between the Mobile Core and the UE. These packets are tunneled over
SCTP/IP and GTP/UDP/IP, respectively. SCTP (Stream Control Transport
Protocol) is an alternative reliable transport to TCP, tailored to carry
signaling (control) information for telephony services. GTP (a nested
acronym corresponding to (General Packet Radio Service) Tunneling
Protocol) is a 3GPP-specific tunneling protocol designed to run over
UDP.

.. _fig-tunnels:
.. figure:: shared/figures/tunnels.png
    :width: 500px
    :align: center

    Base Station to Mobile Core (and Base Station to Base
    Station) control plane tunneled over SCTP/IP and user plane
    tunneled over GTP/UDP/IP.

Fifth, each base station coordinates UE handovers with neighboring
base stations, using direct station-to-station communication. Exactly
like the station-to-core connectivity shown in the previous figure,
this communication uses the same control plane (SCTP over IP) and user
plane (GTP over UDP/IP) packets. The decision as to when to do a
handover is part of the scheduling decision described in Section
5.4.4.

.. _fig-handover:
.. figure:: shared/figures/handover.png
    :width: 500px
    :align: center

    Base Stations cooperate to implement UE hand over.

Sixth, the base stations coordinate wireless multi-point transmission to
a UE from multiple base stations, which may or may not be part of a UE
handover from one base station to another.

.. _fig-link-aggregation:
.. figure:: shared/figures/link-aggregation.png
    :width: 500px
    :align: center

    Base Stations cooperate to implement multipath transmission (link
    aggregation) to UEs.

The main takeaway is that the base station does much more than
implement the radio interface.  In the downstream (Internet-to-UE)
direction, it fragments outgoing packets into physical layer segments
and schedules them for transmission over the available radio spectrum,
and in the upstream (UE-to-Internet) direction, it assembles physical
layer segments into IP packets and forwards them (over a GTP/UDP/IP
tunnel) to the upstream user plane of the Mobile Core. Also, based on
observations of the wireless channel quality and per-subscriber
policies, it decides whether to (a) forward outgoing packets directly
to the UE, (b) indirectly forward packets to the UE via a neighboring
base station, or (c) utilize multiple paths to reach the UE. The third
case has the option of either spreading the physical payloads across
multiple base stations or across multiple carrier frequencies of a
single base station.

In other words, the RAN as a whole (i.e., not just a single base
station) not only supports handovers (an obvious requirement for
mobility), but also *link aggregation* and *load balancing*.  These
functions imply a global decision-making process, whereby it's
possible to forward traffic to a different base station (or to
multiple base stations) in an effort to make efficient use of the
radio spectrum over a larger geographic area.

5.4.3 Mobile Core
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

At the most basic level, the function of the Mobile Core is to provide
packet data network connectivity to mobile subscribers, i.e., connect
them to the Internet. As we noted above, there is more to providing
this connectivity than meets the eye: the Mobile Core ensures that
subscribers are authenticated and aims to deliver the service
qualities to which they have subscribed. As subscribers may move
around among base station coverage areas, the Mobile Core needs to
keep track of their whereabouts at the granularity of the serving base
station. It is this support for security, mobility, and QoS that
differentiates the cellular network from Wi-Fi.

We start with the security architecture, which is grounded in two
trust assumptions. First, each base station trusts that it is
connected to the Mobile Core by a secure private network, over which
it establishes the tunnels introduced in :numref:`Figure %s
<fig-tunnels>`: a GTP/UDP/IP tunnel to the Core's User Plane (Core-UP)
and a SCTP/IP tunnel to the Core's Control Plane (Core-CP). Second,
each UE has an operator-provided SIM card, which contains information
that uniquely identifies the subscriber and includes a secret key that
the UE uses to authenticate itself.

The identifier burned into each SIM card, called an *IMSI
(International Mobile Subscriber Identity)*, is a globally unique
identifier for every device connected to the global mobile network.
Each IMSI is a 64-bit, self-describing identifier, which is to say,
it includes a *Format* field that effectively serves as a mask for
extracting other relevant fields. For example, the following is a
straightforward interpretation, where IMSIs are commonly
represented as an up to 15-digit decimal number:

* **MCC:** Mobile Country Code (3-digit decimal number).

* **MNC:** Mobile Network Code (3-digit decimal number).

* **SUB:** Subscriber (9-digit decimal number).

The first two fields (*MCC*, *MNC*) are universally understood to
uniquely identify the MNO, while that last field is one example of how
an MNO might uniquely identify every device it serves; introducing
other structure into the SUB is also an option.

The *MCC/MNC* pair—which is also called the *Public Land Mobile
Network (PLMN)* identifier—plays a role in roaming: when a UE tries
to connect to a "foreign network" those fields are used to find the
"home network", where the rest of the IMSI leads to a subscriber
profile that says whether or not roaming is enabled for this
device. The following walks through what happens when a device
connects to its home network; more information about the global
ramifications is given at the end of the section.

.. _fig-secure:
.. figure:: shared/figures/secure.png
    :width: 600px
    :align: center

    Sequence of steps to establish secure Control and User Plane
    channels.

:numref:`Figure %s <fig-secure>` shows the per-UE connection
sequence. When a UE first becomes active, it communicates with a
nearby base station over a temporary (unauthenticated) radio link
(Step 1). The base station forwards the request to the Core-CP over
the existing SCTP connection, and the Core-CP (assuming it recognizes
the IMSI) initiates an authentication protocol with the UE (Step
2). 3GPP identifies a set of options for authentication and
encryption, where the actual protocols used are an implementation
choice. For example, *Advanced Encryption Standard* (AES) is one of
the options for encryption. Note that this authentication exchange is
initially in the clear since the base station to UE link is not yet
secure.

Once the UE and Core-CP are satisfied with each other's identity, the
Core-CP informs the other 5GC components of the parameters they will need
to service the UE (Step 3). This includes: (a) instructing the Core-UP
to initialize the user plane (e.g., assign an IP address to the UE and
set the appropriate 5QI); (b) instructing the base station to
establish an encrypted channel to the UE; and (c) giving the UE the
symmetric key it will need to use the encrypted channel with the base
station. The symmetric key is encrypted using the public key of the
UE (so only the UE can decrypt it, using its secret key). Once
complete, the UE can use the end-to-end user plane channel through the
Core-UP (Step 4).

There are three additional details of note about this process. First,
the secure control channel between the UE and the Core-CP set up
during Step 2 remains available, and is used by the Core-CP to send
additional control instructions to the UE during the course of the
session. In other words, unlike the Internet, the network is able to
"control" the communication settings in edge devices.

Second, the user plane channel established during Step 4 is referred
to as the *Default Data Radio Bearer*, but additional channels can be
established between the UE and Core-UP, each with a potentially
different QoS guarantee. This might be done on an
application-by-application basis, for example, based on policies
present in the Core-CP for packets that require special/different
treatment.

Support for mobility can now be understood as the process of
re-executing one or more of the steps shown in :numref:`Figure %s
<fig-secure>` as the UE moves throughout the RAN. The unauthenticated
link indicated by (1) allows the UE to be known to all base stations
within range. Based on the signal's measured CQI, the base stations
communicate directly with each other to make a handover decision.
Once made, the decision is then communicated to the Mobile Core,
re-triggering the setup functions indicated by (3), which in turn
re-builds the user plane tunnel between the base station and the
Core-UP. One of the most unique features of the cellular network is
that the Mobile Core's user plane buffers data while idle UEs are
transiting to active state, thereby avoiding dropped packets and
subsequent end-to-end retransmissions.

In other words, the mobile cellular network maintains the *UE session*
in the face of mobility (corresponding to the control and data
channels depicted by (2) and (4) in :numref:`Figure %s <fig-secure>`,
respectively), but it is able to do so only when the same Mobile Core
serves the UE (i.e., only the base station changes). This would
typically be the case for a UE moving within a metropolitan area.
Moving between metro areas—and hence, between Mobile Cores—is
indistinguishable from power cycling a UE. The UE is assigned a new IP
address and no attempt is made to buffer and subsequently deliver
in-flight data. Independent of mobility, but relevant to this
discussion, any UE that becomes inactive for a period of time also
loses its session, with a new session established and a new IP address
assigned when the UE becomes active again.

5.4.4  Scheduling Transmission
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Responsibility for scheduling transmission is centered at the base
stations. It depends on two key inputs. The first is feedback sent
from every node back to the base station, reporting how much
interference it is experiencing. 5G specifies a *Channel Quality
Indicator (CQI)* for this purpose. In practice, the receiver sends a
CQI status report to the base station periodically (e.g., every
millisecond). These CQI messages report the observed signal-to-noise
ratio, which impacts the receiver's ability to recover the data bits.

The second input to the scheduling decision is a *QoS Class Identifier
(QCI)*, which indicates the quality-of-service each class of traffic
is to receive. The richness of the QCI is a major difference between
4G and 5G. In 4G, the QCI value assigned to each class indicates
whether the traffic has a *Guaranteed Bit Rate (GBR)* or not
*(non-GBR)*, plus the class's relative priority within those two
categories.  In 5G, the QCI value (renamed 5QI) reflects increasing
differentiation among classes that 5G aims to support. Each class
includes the following attributes:

- Resource Type: Guaranteed Bit Rate (GBR), Delay-Critical GBR, Non-GBR
- Priority Level
- Packet Delay Budget
- Packet Error Rate
- Maximum Data Burst
- Averaging Window

The other big difference for 5G (differentiating it from both 4G and
Wi-Fi) is that 5G tailors its scheduling algorithm according to the
use case it is trying to satisfy. Specifically, 5G bands below 1 GHz
are designed to deliver mobile broadband and massive IoT services with
a primary focus on range. Frequencies between 1-6 GHz are designed to
offer wider bandwidths, focusing on mobile broadband and
mission-critical applications. Frequencies above 24 GHz (mmWaves) are
designed to provide super-wide bandwidths over short, line-of-sight
coverage.

Moreover, for each of these, 5G dynamically adapts the numerology to
match that target use case. (Recall that we used 15 kHz frequency
spacing and 0.5ms scheduling intervals in the example shown in
:numref:`Figure %s <fig-sched-grid>`.)

-  For the frequency range 410 MHz - 7125 MHz, 5G allows maximum 100
   MHz bandwidths. In this case, there are three options with subcarrier
   spacings of 15, 30 and 60 kHz. These correspond to scheduling
   intervals of 0.5, 0.25, and 0.125 ms, respectively.

-  For millimeter bands, corresponding to the frequency range 24.25 GHz -
   52.6 GHz, bandwidths may go from 50 MHz up to 400 MHz. There are
   two options, with subcarrier spacings of 60 kHz and 120 kHz. Both
   have scheduling intervals of 0.125 ms.

.. _fig-scheduler:
.. figure:: shared/figures/scheduler.png
    :width: 600px
    :align: center

    Scheduler allocates Resource Blocks to user data streams based on
    CQI feedback from receivers and the 5QI parameters associated with
    each class of service.

:numref:`Figure %s <fig-scheduler>` depicts the role of the scheduler
from this more abstract perspective. The CQI feedback from the
receivers and the 5QI quality-of-service class selected by the
subscriber are the two key pieces of input to the scheduler.  Note
that while the preceding discussion could be interpreted to imply a
one-to-one relationship between subscribers and a 5QI, it is more
accurate to say that each 5QI is associated with a class of traffic
(often corresponding to some type of application), where a given
subscriber might be sending and receiving traffic that belongs to
multiple classes at any given time). Finally, note that one possible
decision the scheduler might make is to "hand-off" the user to another
base station, as outlined in Section 5.4.2. In this way, a collection
of base stations effectively collaborate to allocate spectrum across
a set of radio cells.

.. See https://www.youtube.com/watch?v=GEQgEDcRcZI 5G for numerology.

.. See
   https://documentation.meraki.com/Wireless/Design_and_Configure/Architecture_and_Best_Practices/Wi-Fi_6_(802.11ax)_Technical_Guide
   for wifi numerology
