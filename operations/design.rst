|Ops|.1 Design Issues
------------------------------

Users who manage their home network (or a small enterprise network)
experience many of the same issues any operator faces:
deciding what devices to buy, making sure they are correctly
configured, and figuring out what went wrong when something doesn't
work. The main difference between a home network and an ISP or cloud
network is one of scale and the corresponding impact of
outages. Imagine that instead of managing a single
home router, your job is to make sure hundreds or possibly thousands
of network devices are properly configured. A manual approach,
requiring operators to log into each device and set parameters, would
not be practical. Even knowing what "properly configured" means is a
hard problem since the goal is to ensure that the network as a
whole—as opposed to any single device—works as expected.

One take away from this thought experiment is the importance of
automation. In an ideal world, the operator would be able to state
their high-level *intent* for the network's behavior, and a management
system would (a) translate that network-wide intent into a set of
per-device configuration parameters, and (b) remotely install those
parameters in each device. The devices would then periodically report
information about their status—such as how many packets they had
forwarded, the average queue length each output port, the number of
route updates they had processed, and so on—back to the management
system, with with the goal of detecting problematic behavior and
sending an *alert* to the operator whenever something requires their
attention.

Such an idealized management system is shown in :numref:`Figure %s
<fig-mgmt-system>`; we can imagine sophisticated possibilities for its
internal mechanisms. On the configuration side, operator intents could
be expressed as "natural text", with a Large Language Model (LLM)
translating the intent into a discrete set of configuration
settings. On the monitoring side, Machine Learning (ML) algorithms
could analyze the reported data, and raise an alert anytime a
statistical anomaly is detected. And in the limit, we could build a
*closed control loop*, where an alert triggers a new intent, bypassing
the human operator entirely.

.. _fig-mgmt-system:
.. figure:: operations/figures/mgmt-system.png
   :width: 500px
   :align: center

   Idealized management system, providing an intent-based interface
   to operators, and in turn, remotely configuring and collecting data
   from a set of network devices.

Realizing such an idealized design is an active area of research, but
that research depends on having the right subsystems and interfaces in
place.  Those building block components, which have long served human
operators and are now starting to serve automated agents, are the
focus of this chapter. And as our hypothetical design suggests, we can
think of the management system as having two parts: one responsible for
configuring network devices and the other responsible for monitoring
network devices. We consider each, in turn.

.. sidebar:: Is it SDN?

   The schematic for a Network Management System shown in
   :numref:`Figure %s <fig-mgmt-system>` looks a lot like the
   schematic for an SDN Controller presented in Section |Routing|.5.1.
   This is no accident. At a high level, both involve a centralized
   program being used to "oversee" a distributed set of devices.  The
   only difference is whether the centralized component is *controlling*
   the device (i.e., installing forwarding rules into the device's
   data plane) or *configuring* the device (i.e., giving the on-device
   control plane the list of BGP neighbors it should peer with).

   One of the papers that inspired the SDN movement, the 4D paper (see
   Further Reading below), specifically called out manageability in
   its top-level goals. SDN adopted the premise that it was possible
   to build a centralized control plane, and used it to install flow
   rules in a distributed data plane. That proved to be too
   disruptive, in part because it implied making real-time decisions
   about how to respond to link and switch failures. If the data plane
   reports a failure, the control plane needs to learn about this
   failure and provide a remedy (e.g., a new Match/Action flow rule)
   generally within milliseconds. In response, many switch and router
   vendors offered an "SDN Lite" solution, which involved support for
   a centralized *Management Plane*. As described in Section
   |Tech|.2.5, replacing a CLI with a programmatic interface was a key
   advancement. The centralized use of that interface has led to a
   clear win for SDN.

On the configuration side, automation require a programmatic approach;
expecting a human to manually enter configuration parameters into a
web form or command line interface simply does not scale. Manually
entering configuration settings is also notoriously error-prone. What
we need is a programmatic interface that supports ``GET`` and ``SET``
operations, but that implies knowing what set of parameters
(variables) each device supports.  Defining a schema for the set of
configurable variables supported by network elements is the central
problem of configuration management. To complicate matters, the schema
should be vendor-agnostic (i.e., work across similar devices from
multiple vendors), and support everything from low-level hardware
settings (e.g., setting the port speed on an interface card) to
high-level protocol settings (e.g., setting the AS number for BGP).

A programmatic interface implies that a program is already running on
the devices (that program responds to ``GET`` and ``SET`` requests),
which in turn requires that the device be connected to the network.
This points to a bootstrapping problem: how do we configure a device
so it is able to respond to configuration requests over the network.
This problem is often considered part of *resource provisioning*,
which is the set of steps needed to turn an "out of the box" piece of
hardware into a network-controllable element.\ [#]_ And just as with
ongoing maintenance, we'd like this initial setup process to be
automated, requiring as few manual steps as possible. (Another way of
saying this, is that the goal is for the person physically installing
the hardware to require as little technical training as possible.)
This goal is sometimes called *zero-touch provisioning* (a topic we
discussed in the VPN context in Chapter |Virt|).

.. [#] This discussion of provisioning implicitly assumes that
       hardware is being purchased and installed, but in general, an
       operator can also provision virtual resources. This is often
       done programmatically, for example, by invoking a "Create VM"
       operation on a cloud provider, or a "Create Circuit" operation
       on a broadband provider. We take up this broader perspective
       in Section |Ops|.4.

Managing networks via programmatic interfaces has proven to be one of
the famously hard problems in networking. One effort to address that
problem was covered in Section |Virt|.4: virtualize the network in
order to abstract away the large set of individual features across
thousands of devices. While that solution has been popular in
datacenter networks, it doesn't get around the fact that there are
still large networks of physical devices to be managed. Furthermore,
the wide variety of features and disparities among devices, even those
from the same vendor, have made it hard to come up with unifying
abstractions to manage the complexity of network operations.

Another aspect of the problem is *inventory management*, that is,
tracking both the physical resources (racks, servers, switches,
cabling) and virtual resources (IP ranges and addresses, VLANs) in a
network. As with configuration changes, this process frequently starts
using simple spreadsheets and text files, but as complexity grows, a
dedicated database for inventory facilitates greater automation.

In general, we can look at configuration management as an exercise in
*Lifecycle Management*, that is, the process of bringing up,
maintaining, and upgrading resources from day to day. In fact, the
practice is sometimes described in exactly those terms:

* **Day (-1):** Hardware configuration that is applied to a device
  (e.g., via a console) when it is first powered on. These
  configurations correspond to firmware (BIOS or similar) settings,
  and often need knowledge of how the device is physically connected
  to the network (e.g., the port being used).

* **Day 0:** Connectivity configuration required to establish
  communication between the device and the available network services
  (e.g., setting a device’s IP address and default router). While such
  information may be provided manually, this is an opportunity to
  auto-configure the device, in support of zero-touch provisioning.

* **Day 1:** Service-level configuration needed by the device,
  including parameters that allow the device to take advantage of
  other services (e.g., NTP, Syslog, SMTP, NFS), as well as setting
  the parameters this device needs to perform whatever service it
  provides. At the end of Day-1 operationalization, the device is
  considered up-and-running, and able to support user traffic.

* **Day 2..N:** On-going management in support of day-to-day
  operations, coupled with monitoring the network to detect failures
  and service degradation, with the goal of sustaining the
  service. This is often referred to simply as “Day 2 Operations”.


.. TODO -- Work Remote Device Management in somewhere, either here or
   in the Config Section. Maybe a footnote or sidebar is enough. Key
   is to point out that many servers have a "management port" and you
   can use IPMI or Redfish to remotely manage hardware devices in
   support of zero-touch provisioning. The idea is to send and receive
   out-of-band messages over the LAN in place of having video or
   serial console access to the device. Additionally, these may
   integrate with monitoring and other device health telemetry
   systems. (At the level of this chapter, this is a nit, but an
   interesting one highlighting how tricky mgmt can be.)

We now turn our attention to the building blocks we need to monitor an
operational network, a process often referred to as *telemetry* since
is involves "reading meters at a distance." Breaking the problem down
further, the first requirement is that devices themselves need to be
*instrumented*, which is to say, they need to record their activity
(such as number of packets sent or received) in local counters.
Layered on top of those raw meters and counters is a data collection
mechanism that periodically reads the per-device variables and records
their values in a time-series database. The collection mechanism can
be built around a combination of configuring devices to ``Stream``
their data to the collector, and configuring the collector to
periodically ``Poll`` the device.

We also need a query mechanism that allows operators to look at the
data. This includes both feeding data to dashboards that display it
graphically, and feeding analysis tools that help operators diagnose
faults, tune performance, do root-cause analysis, conduct security
audits, and provision additional capacity. As mentioned earlier in
this section, as these tools grow more and more sophisticated, there
is the potential that they auto-configure the system based on these
insights, thereby closing the loop between telemetry and
configuration.

As straightforward as this sounds, the challenge is knowing what data
to collect, and then being diligent about collecting it. In the same
way individual systems need to think about security, reliability,
scalability, availability, and so on (these are collectively known as
the *-ities*), system designers need to worry about *observability*\
—the property of a system that makes visible the facts about its
internal operation needed to make informed management and control
decisions. If you don't think to record certain facts as they happen,
then the data will not be there when you need it. As a general rule,
the answer is that there are three kinds of telemetry data: metrics,
logs, and traces.

Metrics are quantitative data about a system. These include common
performance metrics such as link bandwidth, CPU utilization, and
memory usage, but also binary results corresponding to "up" and
"down".  These values are produced and collected periodically (e.g.,
every few seconds), either by reading a counter, or by executing a
runtime test that returns a value. These metrics can be associated
with physical resources such as servers and switches, virtual
resources such as VMs or IP subnets, or even end-to-end protocols and
services.

Logs are the qualitative data that is generated whenever a noteworthy
event occurs. This information can be used to identify problematic
operating conditions (i.e., it may trigger an alert), but more
commonly, it is used to troubleshoot problems after they have been
detected. Various system components—all the way from the low-level OS
kernel to high-level services—write messages that adhere to a
well-defined format to the log. These messages include a timestamp,
which makes it possible for the logging stack to parse and correlate
messages from different components.

Traces are a record of causal relationships (e.g., Service A calls
Service B) resulting from user-initiated transactions or jobs. They
are a form of event logs, but provide more specialized information
about the context in which different events happen. Tracing is well
understood in a single program, but in a network setting, a trace is
inherently distributed across a graph of network-connected
processes. This makes the problem challenging, but also critically
important because it is often the case that the only way to understand
time-dependent phenomena—such as why a particular resource is
overloaded—is to understand how multiple independent workflows
interact with each other.

Finally, for each of these types of telemetry data, the central
challenge is to define a meaningful data model, so there is agreement
across the many components that go into an end-to-end solution.  This
data model (schema) is similar to one we need for configuration
management, except it is designed for *observed* behavior as opposed
to *configured* behavior. Note that we can treat some of the variables
read by the configuration system as telemetry data—that is, there are
some variables that serve both sides of the management system.



.. _reading-4D:

.. admonition:: Further Reading

   A. Greenberg et al. `A clean slate 4D approach to network control and management
   <https://dl.acm.org/doi/10.1145/1096536.1096541>`__.
   ACM SIGCOMM Computer Communication Review, October 2005.

