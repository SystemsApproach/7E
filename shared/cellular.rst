5.4 Mobile Cellular Network
--------------------------------------------

The mobile celluar network, which has a 40-year history that
parallels the Internet's, has undergone significant change. The first
two generations supported voice and then text, with 3G defining the
transition to broadband access, supporting data rates measured in
hundreds of kilobits per second. The industry has recently
transitioned from 4G (with data rates typically measured in the few
megabits per second) to 5G, with the promise of a tenfold increase in
data rates. This section describes how 5G works, with a focus on the
hard problem of managing its scarce resource: radio spectrum.

5.4.1 Overview
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The mobile cellular network provides wireless connectivity to devices
that are (potentially) on the move. These devices, which are known as
*User Equipment (UE)*, have traditionally corresponded to mobile phones
and tablets, but increasingly include cars, drones, industrial and
agricultural machines, robots, home appliances, medical devices, and
so on. In some cases, the UEs may be devices that do not move, e.g.,
router interfaces using cellular connectivity to provide broadband
access to remote dwellings.

.. _fig-cellular:
.. figure:: shared/figures/cellular.png
    :width: 600px
    :align: center

    Mobile cellular networks consist of a Radio Access Network (RAN)
    and a Mobile Core.

As shown in :numref:`Figure %s <fig-cellular>`, the mobile cellular
network consists of two main subsystems: the *Radio Access Network
(RAN)* and the *Mobile Core*. The RAN manages the radio resources
(i.e., spectrum), making sure it is used efficiently and meets the
quality of service (QoS) requirements of every user. It corresponds
to a distributed collection of base stations. As noted above, these
are cryptically named *eNodeB* or *eNB* (which is short for *evolved
Node B*) in 4G. In 5G, base stations are known as *gNB*, where the
"g" stands for *next Generation*.

The Mobile Core is a bundle of functionality (conventionally packaged
as one or more devices) that serves several purposes.

-  Authenticates devices prior to attaching them to the network
-  Provides Internet (IP) connectivity for both data and voice services.
-  Ensures this connectivity fulfills the promised QoS requirements.
-  Tracks user mobility to ensure uninterrupted service.
-  Tracks subscriber usage for billing and charging.

For readers familiar with the Internet architecture and Wi-Fi as a
common access technology, some of these functions might look a bit
surprising. For example, Wi-Fi, like most of the Internet, normally
provides a best-effort service, whereas cellular networks often aim
to deliver some sort of QoS guarantee. Tracking subscribers for both
mobility and billing are also not the sort of things we tend to think
about in the Internet, but they are considered important functions for
cellular networks. The reasons for these differences are numerous,
including the typically large costs of acquiring cellular spectrum and
maintaining the infrastructure to use it such as radio towers. With
that large investment, there is a desire to recoup costs by charging
subscribers, which in turn leads to making some sort of service
guarantees to those subscribers to justify the cost. There is also a
need to maximize the efficiency of spectrum usage. Much of the
complexity of the mobile core follows from these requirements being
imposed by service providers. Even when we get to enterprises running
their own 5G networks, they still need to manage the usage of spectrum
to obtain the benefits of 5G over Wi-Fi, such as more predictable
control over latency and bandwidth.

Note that Mobile Core is another example of a generic term. In 4G it
was called the *Evolved Packet Core (EPC)* and in 5G it is called the
*5G Core (5GC)*. Moreover, even though the word “Core” is in its name,
the Mobile Core runs near the edge of the network, effectively providing
a bridge between the RAN in some geographic area and the greater
IP-based Internet. 3GPP provides significant flexibility in how the
Mobile Core is geographically deployed, ranging from minimal deployments
(the RAN and the mobile core can be co-located) to areas that are
hundreds of kilometers wide. A common model is that an instantiation
of the Mobile Core serves a metropolitan area. The corresponding RAN
would then span several dozens (or even hundreds) of cell towers in
that geographic area.

Taking a closer look at :numref:`Figure %s <fig-cellular>`, we see
that a *Backhaul Network* interconnects the base stations that
implement the RAN with the Mobile Core. This network is typically
wired, may or may not have the ring topology shown in the figure, and
is often constructed from commodity components found elsewhere in the
Internet, such as switched Ethernet. The backhaul network is obviously
a necessary part of the RAN, but it is an implementation choice and
not prescribed by the 3GPP standard.

Although 3GPP specifies all the elements that implement the RAN and
Mobile Core in an open standard—including sub-layers we have not yet
introduced—network operators have historically bought proprietary
implementations of each subsystem from a single vendor. This lack of
an open source implementation contributes to the perceived
“opaqueness” of the mobile cellular network in general, and the RAN in
particular. And while it is true that base stations contain
sophisticated algorithms for scheduling transmission on the radio
spectrum—algorithms that are considered valuable intellectual property
of the equipment vendors—there is significant opportunity to open and
disaggregate both the RAN and the Mobile Core. This book gives a
recipe for how to do exactly that.

Before getting to those details, we have three more architectural
concepts to introduce. First, :numref:`Figure %s <fig-cups>` redraws
components from :numref:`Figure %s <fig-cellular>` to highlight the
fact that a base station has an analog component (depicted by an
antenna) and a digital component (depicted by a processor pair). This
book mostly focuses on the latter, but we introduce enough information
about the over-the-air radio transmission to appreciate its impact on
the overall architecture.

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

Finally, one of the key aspirational goals of 5G is the ability to
segregate traffic for different usage domains into isolated *network
slices*, each of which delivers a different level of service to a
collection of devices and applications. Thinking of a network slice as
a wireless version of a virtual network (see Chapter 9) is a fair
approximation.

.. _fig-slice:
.. figure:: shared/figures/slice.png
    :width: 500px
    :align: center

    Different usage domains (e.g., IoT and Video Streaming)
    instantiate distinct *network slices* to connect a set of devices
    with one or more applications.

For example, :numref:`Figure %s <fig-slice>` shows two slices, one
supporting IoT workloads and the other supporting multimedia streaming
traffic. As we'll see throughout the book, an important question is
how slicing is realized end-to-end, across the radio, the RAN, and the
Mobile Core. This is done through a combination of allocating distinct
resources to each slice and scheduling shared resources on behalf of a
set of slices.

5.4.2 Radio Access Network
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

We now describe the RAN by sketching the role each base station plays.
Keep in mind this is like describing the Internet by explaining
how a router works—not an unreasonable place to start, but it doesn't
fully do justice to the end-to-end story.

First, each base station establishes the wireless channel for a
subscriber's UE upon power-up or upon handover when the UE is active.
This channel is released when the UE remains idle for a predetermined
period of time. Using 3GPP terminology, this wireless channel is said
to provide a *radio bearer*. The term “bearer” has historically been
used in telecommunications (including early wireline technologies such
as ISDN) to denote a data channel, as opposed to a channel that carries
signaling information.

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

It is noteworthy that connectivity between the RAN and the Mobile Core
is IP-based. This was introduced as one of the main changes between 3G
and 4G. Prior to 4G, the internals of the cellular network were
circuit-based, which is not surprising given its origins as a voice
network. This also helps to explain why in Section 2.1 we
characterized the RAN Backhaul as an overlay running on top of some
Layer 2 technology.

.. _fig-tunnels:
.. figure:: shared/figures/tunnels.png
    :width: 500px
    :align: center

    Base Station to Mobile Core (and Base Station to Base
    Station) control plane tunneled over SCTP/IP and user plane
    tunneled over GTP/UDP/IP.

Fifth, each base station coordinates UE handovers with neighboring
base stations, using direct station-to-station links. Exactly like the
station-to-core connectivity shown in the previous figure, these links
are used to transfer both control plane (SCTP over IP) and user plane
(GTP over UDP/IP) packets. The decision as to when to do a handover is
based on the CQI values being reported by the radio on each of the
base stations within range of the UE, coupled with the 5QI value those
base stations know the RAN has promised to deliver to the UE.

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

The main takeaway is that the base station can be viewed as a
specialized forwarder. In the Internet-to-UE direction, it fragments
outgoing IP packets into physical layer segments and schedules them
for transmission over the available radio spectrum, and in the
UE-to-Internet direction it assembles physical layer segments into IP
packets and forwards them (over a GTP/UDP/IP tunnel) to the upstream
user plane of the Mobile Core. Also, based on observations of the
wireless channel quality and per-subscriber policies, it decides
whether to (a) forward outgoing packets directly to the UE, (b)
indirectly forward packets to the UE via a neighboring base station,
or (c) utilize multiple paths to reach the UE. The third case has the
option of either spreading the physical payloads across multiple base
stations or across multiple carrier frequencies of a single base
station (including Wi-Fi).

In other words, the RAN as a whole (i.e., not just a single base
station) not only supports handovers (an obvious requirement for
mobility), but also *link aggregation* and *load balancing*,
mechanisms that are similar to those found in other types of networks.
These functions imply a global decision-making process, whereby it's
possible to forward traffic to a different base station (or to
multiple base stations) in an effort to make efficient use of the
radio spectrum over a larger geographic area. We will revisit how such
RAN-wide (global) decisions can be made using SDN techniques in
Chapter 4.

.. _fig-quality:
.. figure:: shared/figures/quality.png
    :width: 300px
    :align: center

    Abstractly, measures of signal quality (CQI) and declarations
    of intended data delivery quality (5QI) are passed up and down
    the RAN stack.

Finally, in support of the six mechanisms just described, each base
station in a RAN has to collect two important pieces of information.
One is the signal-to-noise ratio that the base station observes when
communicating with each UE. This is called the *Channel Quality
Indicator (CQI)* and it is passed *up* from the radio. The other is
the quality of service the network wants to give a particular UE. This
is called the *5G QoS Identifier (5QI)* and it is passed *down* to the
radio. This high-level summary is shown in :numref:`Figure %s
<fig-quality>`. We explore how this information is used in a later
subsection.

5.4.3 Mobile Core
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

At the most basic level, the function of the Mobile Core is to provide
packet data network connectivity to mobile subscribers, i.e., connect
them to the Internet. (The mobile network assumes that multiple packet
data networks can exist, but in practice the Internet is the one that
matters). As we noted above, there is more to providing this
connectivity than meets the eye: the Mobile Core ensures that
subscribers are authenticated and aims to deliver the service
qualities to which they have subscribed. As subscribers may move
around among base station coverage areas, the Mobile Core needs to
keep track of their whereabouts at the granularity of the serving
base station. The reasons for this tracking are discussed further in
Chapter 5. It is this support for security, mobility, and QoS that
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
extracting other relevant fields. For example, the following is the
interpretation we assume in this book (where IMSIs are commonly
represented as an up to 15-digit decimal number):

* **MCC:** Mobile Country Code (3-digit decimal number).

* **MNC:** Mobile Network Code (2 or 3-digit decimal number).

* **ENT:** Enterprise Code (3-digit decimal number).

* **SUB:** Subscriber (6-digit decimal number).

The first two fields (*MCC*, *MNC*) are universally understood to
uniquely identify the MNO, while that last two fields are one example
of how an MNO might use additional hierarchical structure to uniquely
identify every device it serves. We are working towards delivering 5G
connectivity to enterprises (hence the *ENT* field), but other MNOs
might assign the last 9 digits using some other structure.

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

:numref:`Figure %s <fig-secure>` shows the
per-UE connection sequence. When a UE first becomes active, it
communicates with a nearby base station over a temporary
(unauthenticated) radio link (Step 1). The base station forwards the
request to the Core-CP over the existing SCTP connection, and the
Core-CP (assuming it recognizes the IMSI) initiates an authentication
protocol with the UE (Step 2). 3GPP identifies a set of options for
authentication and encryption, where the actual protocols used are an
implementation choice. For example, *Advanced Encryption Standard*
(AES) is one of the options for encryption. Note that this
authentication exchange is initially in the clear since the base
station to UE link is not yet secure.

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
different 5QI. This might be done on an application-by-application
basis, for example, based on policies present in the Core-CP for
packets that require special/different treatment.

.. _fig-per-hop:
.. figure:: shared/figures/per-hop.png
    :width: 500px
    :align: center

    Sequence of per-hop tunnels involved in an end-to-end User Plane
    channel.

In practice, these per-flow tunnels are often bundled into a single
inter-component tunnel, which makes it impossible to differentiate the
level of service given to any particular end-to-end UE channel. This
is a limitation of 4G that 5G has ambitions to correct as part of its
support for network slicing.

Support for mobility can now be understood as the process of
re-executing one or more of the steps shown in :numref:`Figure %s
<fig-secure>` as the UE moves throughout the RAN. The unauthenticated
link indicated by (1) allows the UE to be known to all base stations
within range. (We refer to these as *potential links* in later
chapters.) Based on the signal's measured CQI, the base stations
communicate directly with each other to make a handover decision.
Once made, the decision is then communicated to the Mobile Core,
re-triggering the setup functions indicated by (3), which in turn
re-builds the user plane tunnel between the base station and the
Core-UP shown in :numref:`Figure %s <fig-per-hop>`. One of the most
unique features of the cellular network is that the Mobile Core's user
plane buffers data while idle UEs are transiting to active state,
thereby avoiding dropped packets and subsequent end-to-end
retransmissions.

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

Note that this session-based approach can be traced to the mobile
cellular network's roots as a connection-oriented network. An
interesting thought experiment is whether the Mobile Core will
continue to evolve so as to better match the connectionless
assumptions of the Internet protocols that typically run on top of it.

We conclude this overview of the Mobile Core by returning to the role
it plays in implementing a *global* mobile network. It is probably
already clear that each MNO implements a database of subscriber
information, allowing it to map an IMSI to a profile that records what
services (roaming, data plane, hot spot support) the subscriber is
paying for. This record also includes the international phone number
for the device. How this database is realized is an implementation
choice (of which we'll see an example in Chapter 5), but 3GPP defines
an interface by which one Mobile Core (running on behalf of one MNO)
queries another Mobile Core (running on behalf of some other MNO), to
map between the IMSI, the phone number, and the subscriber profile.

5.4.4  Scheduling Algorithm
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The transition from 4G to 5G introduces additional flexibility in
how the radio spectrum is scheduled, making it possible to adapt the
cellular network to a more diverse set of devices and application
domains.

Fundamentally, 5G defines a family of waveforms—unlike LTE, which
specified only one waveform—each optimized for a different band in the
radio spectrum.\ [#]_  The bands with carrier frequencies below 1 GHz are
designed to deliver mobile broadband and massive IoT services with a
primary focus on range. Carrier frequencies between 1-6 GHz are
designed to offer wider bandwidths, focusing on mobile broadband and
mission-critical applications. Carrier frequencies above 24 GHz
(mmWaves) are designed to provide super-wide bandwidths over short,
line-of-sight coverage.

.. [#] A waveform is defined by the frequency, amplitude, and phase-shift
   independent property (shape) of a signal. A sine wave is a simple
   example of a waveform.

These different waveforms affect the scheduling and subcarrier intervals
(i.e., the “size” of the resource elements described in the previous
section).

-  For frequency range 1 (410 MHz - 7125 MHz), 5G allows maximum 100 MHz
   bandwidths. In this case, there are three waveforms with subcarrier
   spacings of 15, 30 and 60 kHz. (We used 15 kHz in the example shown in
   :numref:`Figure %s <fig-sched-grid>`.) The corresponding to scheduling
   intervals of 0.5, 0.25, and 0.125 ms, respectively. (We used 0.5 ms in
   the example shown in :numref:`Figure %s <fig-sched-grid>`.)

-  For millimeter bands, also known as frequency range 2 (24.25 GHz -
   52.6 GHz), bandwidths may go from 50 MHz up to 400 MHz. There are
   two waveforms, with subcarrier spacings of 60 kHz and 120 kHz. Both
   have scheduling intervals of 0.125 ms.

These various configurations of subcarrier spacing and scheduling
intervals are sometimes called the *numerology* of the radio's air
interface.

This range of numerology is important because it adds another degree
of freedom to the scheduler. In addition to allocating radio resources
to users, it has the ability to dynamically adjust the size of the
resource by changing the waveform being used. With this additional
freedom, fixed-sized REs are no longer the primary unit of resource
allocation. We instead use more abstract terminology, and talk about
allocating *Resource Blocks* to subscribers, where the 5G scheduler
determines both the size and number of Resource Blocks allocated
during each time interval.

:numref:`Figure %s <fig-scheduler>` depicts the role of the scheduler
from this more abstract perspective. Just as with 4G, CQI
feedback from the receivers and the 5QI quality-of-service class
selected by the subscriber are the two key pieces of input to the
scheduler. Note that the set of 5QI values available in 5G is
considerably greater than its QCI counterpart in 4G,
reflecting the increasing differentiation among classes that 5G aims
to support. For 5G, each class includes the following attributes:

-  Resource Type: Guaranteed Bit Rate (GBR), Delay-Critical GBR, Non-GBR
-  Priority Level
-  Packet Delay Budget
-  Packet Error Rate
-  Maximum Data Burst
-  Averaging Window

Note that while the preceding discussion could be interpreted to imply a
one-to-one relationship between subscribers and a 5QI, it is more
accurate to say that each 5QI is associated with a class of traffic
(often corresponding to some type of application), where a given
subscriber might be sending and receiving traffic that belongs to
multiple classes at any given time.

.. We explore this idea in much more
.. depth in a later chapter.

.. Do we? Which chapter?

.. Might try to say more about QCI. Check out this reference:
   https://www.tech-invite.com/3m23/toc/tinv-3gpp-23-501_za.html#e-5-7-3


.. _fig-scheduler:
.. figure:: shared/figures/scheduler.png
    :width: 600px
    :align: center

    Scheduler allocates Resource Blocks to user data streams based on
    CQI feedback from receivers and the 5QI parameters associated with
    each class of service.

