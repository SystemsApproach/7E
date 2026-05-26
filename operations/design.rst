|Ops|.1 Design Issues
------------------------------

Users that manage their home network (or small enterprise network)
have experience with many of the same issues any operator faces:
deciding what devices to buy, making sure they are correctly
configured, and figuring out what went wrong when something doesn't
work. The main difference between a home network and an ISP or cloud
network is one of scale. Imagine that instead of managing a single
home router, your job is to make sure hundreds or possibly thousands
of network devices are properly configured. A manual approach,
requiring operators to log into each device and set parameters, would
not be practical. Even knowing what "properly configured" means is a
hard problem since the goal is to ensure that the network as a whole,
as opposed to isolated devices, works as expected.

One take away from this thought experiment is the importance of
automation. In an ideal world, the operator would be able to state
their high-level *intent* for the network's behavior, and a management
system would (a) translate that network-wide intent into a set of
per-device configuration parameters, and (b) remotely install those
parameters in each device. The devices would then periodically report
information about their status—such as how many packets they had
forwarded, the average queue lenth each output port, the number of
routes updates they had processed, and so on—back to the tool, with
with the goal of detching unexpected behavior and sending an *alert*
to the operator whenever something requires their attention.

Such an idealized mangement system is shown in :numref:`Figure %s
<fig-mgmt-system>`, where we can fantisize a bit more about the
sophistication of its internal mechanisms. On the configuration side,
opertor intents could be expressed as "natural text", with a Large
Language Model (LLM) translating the intent into a discrete set of
configuration settings. On the data collection side, Machine
Learning (ML) algorithms could analyze the reported data, raising an
alert when a statistical anomoly is detected. And in the limit, we
could build a *closed control loop*, where an analyics alert triggers
a new intent, bypassing the human operator entirely.

.. _fig-mgmt-system:
.. figure:: operations/figures/mgmt-system.png
   :width: 400px
   :align: center

   Idealized management sytem, prividing an intent-based interface
   to operators, and in turn, remotely configuring and collecting data
   from a set of network devices.

Realizing such an idealized design is an active area of research, but
that research depends on having the right subystems and interfaces in
place.  Those building block components, which have long served human
operators, and are now starting to serve automated agents, are the
focus of this chapter.

**[The following borrows from the Terminology section of the Ops book;
need to reframe as requirements.]**

Provisioning... Adding capacity (either physical or virtual resources)
to a system, usually in response to changes in workload, including
the initial deployment.

Zero-Touch Provisioning... Usually implies adding new hardware without
requiring a human to configure it (beyond physically connecting the
device). This implies the new component auto-configures itself, which
means the term can also be applied to virtual resources (e.g., virtual
machines, services) to indicate that no manual configuration step is
needed to instantiate the resource.

Remote Device Management... A standard (e.g., IPMI, Redfish) that
defines a way to remotely manage hardware devices in support of
zero-touch provisioning. The idea is to send and receive out-of-band
messages over the LAN in place of having video or serial console
access to the device. Additionally, these may integrate with
monitoring and other device health telemetry systems.

Inventory Management... Planning and tracking both the physical
(racks, servers, switches, cabling) and virtual (IP ranges and
addresses, VLANs) resources is a sub-step of the provisioning
process. This process frequently starts using simple spreadsheets and
text files, but as complexity grows, a dedicated database for
inventory facilitates greater automation.

Lifecycle Management... Upgrading and replacing functionality (e.g.,
new services, new features to existing services) over time.

Monitoring & Telemetry... Collecting data from system components
to aid in management decisions. This includes diagnosing faults,
tuning performance, doing root cause analysis, performing security
audits, and provisioning additional capacity.

Analytics... A program (often using statistical models) that produces
additional insights (value) from raw data. It can be used to close a
control loop (i.e., auto-reconfigure a system based on these
insights), but could also be targeted at a human operator who
subsequently takes some action.


