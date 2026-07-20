.. index:: INT: Inband Network Telemetry
.. index:: NDT: Network Diagnostic Tool

|Ops|.4 Monitoring and Telemetry
--------------------------------------

There are many potential sources of telemetry data. This section looks
at some representative examples, with the caveat that we focus on
metrics and traces, but not logs (the second of the three pillars of
observability cited in Section |Ops|.1). This is because we are
primarily focused on operating a network that is relatively static
with respect to functionality: it forwards packets, with infrequent
changes to the protocols stack. Router and switch vendors make use of
logs to ensure that their implementations of OSPF, BGP, and so on, are
correct, but once in the hands of a network operator, metrics and
traces are the most common way to monitor whether or not the network
is operating properly and to diagnose problems. We revisit this
assumption in Section |Ops|.5, when we look at more rapidly evolving
network functionality.

|Ops|.4.1 OpenConfig Metrics
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A good place to start is the OpenConfig models described in the
previous section.  As we noted, the models include both configurable
variables (those with ``rw`` permissions) and variables that devices
use to report local metrics back to the central management system
(those with ``ro`` permissions). For example, the following code
snippet expands on the ``counter`` variable for Ethernet interfaces
shown in Section |Ops|.3.2:

.. literalinclude:: operations/code/counters.yang

The example shows variables that count packets and bytes being
forwarded or dropped. Note that the ``last-clear`` variable indicates
when the device last set the counters to zero, which can then be used
to calculate average transmission rates over various time intervals.

In addition to monitoring performance, gNMI and OpenConfig can also be
used to read variables that provide insight into other aspects of a
protocol's behavior. This can be especially important for monitoring
routing protocols, such as BGP.  For example, the following snippet of
YAML show what might be reported by BGP on a router. (As a reminder,
YANG is used to define the schema, and YAML is used to pass values
corresponding to a given schema.)

.. literalinclude:: operations/code/bgp.yaml

This example shows variables associated with a given BGP neighbor;
there are several things an operator needs to keep an eye on. Variable
``session-state`` tells us the current BGP session is ``ACTIVE``,
which is good, but seeing ``established-transitions`` increasing over
time might be a sign that the neighbor is repeatedly failing and
attempting to reconnect. Tracking changes in ``last-established`` is
another way to watch for potential problems. Variable
``messages.NOTIFICATIONS`` tells us how many clean session restarts
there have been; this value being less than
``established-transitions`` is an indication that the network path
between this router and its neighbor is flaky (i.e., the underlying
TCP connection failed). Finally, tracking changes in
``prefixes.received`` provides information about how stable the
neighbor is; a router that rapidly changes the number of prefixes it
is willing to advertise from one session to the next is likely
experiencing some kind of instability.

Notice from these examples that having a series of variable readings
is critical; it's often changes over time—as opposed to an
instantaneous reading—that signals worrisome behavior. It's also
important to recognize that the hard part is knowing what data to
record, and being able to establish "rules" or "thresholds" that
indicate when something is potential amiss, or worse, has failed and
requires corrective action. There is no magic formula; only experience
with the protocols and devices you are responsible for managing tells
you (a) what information is worth collecting, and (b) what values or
changes in values are worth acting on.


|Ops|.4.2 Traffic Analysis
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Counting the number of packets sent/received by a given interface is a
useful metric, but it does not provide any insight into what's
actually in the packets. Seeing the contents of packets being
exchanged between two end-points is helpful when you are
troubleshooting a problem, but more broadly, knowing how a given link
is being shared among multiple end-to-end flows can also be useful.
The most obvious use case for the latter is to be able to see when an
excessive amount of traffic belongs to just one flow, which could be a
sign of a Denial-of-Service attack.

A brute force approach to gaining visibility into the traffic being
carried by a network is to capture every packet being exchanged on a
link (typically the packet header and the first few bytes of the
payload is sufficient), and dump it into a file for analysis. This is
most often done on an end host rather than in a switch or router—due
to the volume of traffic being carried—and even then, it is only done
for the sake of troubleshooting a problem; continually collecting a
packet trace will eventually overflow the available backing store. The
most common tool for doing this is a program called ``tcpdump``,
although it can be configured to capture more than just TCP packets.
For example, the following command captures and displays all packets
exchanged with any remote web server at port ``443``:

.. code-block:: shell

   $ sudo tcpdump -i en0 -n tcp port 443

The command captures packets at the network interface (``en0`` in this
example), which means it needs to be executed with superuser
privilege (hence the ``sudo``). If you try this on your laptop you
will see a surprising amount of traffic, even if you are not actively
using your web browser. This points to how much background network
activity is always running. An alternative is to specify a particular
web server, and then refresh a page for that site in your browser; for
example:

.. code-block:: shell

   $ sudo tcpdump -i en0 -n host book.systemsapproach.org

In this case, you will see TCP and TLS packets, but not the HTTP
payloads they carry. This is because the end-to-end traffic is
encrypted by TLS, which points to a major limit in analyzing network
traffic: much of the packet payloads are not visible.

As a final step in this exploration, note that there are tools to help
capture and filter packets. *Wireshark* is a notable example, with
:numref:`Figure %s <fig-wireshark>` showing an example display.  In
this case, we have configured Wireshark to watch for traffic to and
from host ``example.com``, a web server that can be accessed using
HTTP rather than HTTPS. (This server is maintained by IANA for exactly
this purpose.)

.. _fig-wireshark:
.. figure:: operations/figures/wireshark.png
    :width: 700px
    :align: center

    Example packet trace using Wireshark. Because web site example.com
    is accessible using HTTP, the GET request and OK response are
    visible. The GET packet has been selected in the upper panel, with
    its contents summarized in the lower left panel.


.. admonition:: Further Reading

    `Wireshark <https://www.wireshark.org/>`__.

Beyond troubleshooting, packet capture is not a practical approach to
analyzing traffic. Simply deciding what flows are currently active,
and which flow a given packet belongs to, is an expensive operation.
This is true both in terms of the number of processor cycles required
to do a lookup, and in the amount of memory required for all the data
one might potentially collect.  These concerns become significant when
monitoring high-speed links, for example, at 10 Gbps and
above. Monitoring all the interfaces on a switch becomes even more
problematic.

The solution is to instrument the forwarding path to just sample the
packets it processes. This means capturing and saving the header for
one out of every N packets. (We need the header to identify the
end-points, which typically includes TCP or UDP port numbers.) An open
standard, called *sFlow*, has been developed around the idea of
sampling. A switch needs to be instrumented to support sampling, and
an interface is needed to say what level of sampling to perform (e.g.,
1-in-10000 on a 100-Gbps link) and where to send the collected
headers. For the latter problem, OpenConfig defines an ``sFlow``
model, although it is still more common for vendors to provide CLI
command to activate and configure sFlow.

The other big issue is how to analyze the collected samples. One
option is to send the samples to a network port where a process like
``sflowtool`` reads and displays them. This approach is not practical
for ongoing monitoring, but it is a reasonable development tool you
might use to determine what patterns you want to watch for. Another
option is to dump the samples into a time-series database that the
operator can query. Prometheus is a popular open source example;
it's often paired with Grafana, which can be used to build graphical
displays of the collected data.

.. admonition:: Further Reading

    `SFlow <https://sflow.org/>`__.

    `Prometheus <https://prometheus.io/>`__.

    `Grafana <https://grafana.com/>`__.

.. TODO -- What more to say about this? Could give some attack
   examples (e.g., spike in traffic to UDP port 53), or capacity
   planning.


.. admonition:: Systems Thinking Takeaway

   This discussion illustrates one of the key tradeoffs in monitoring
   a system: our attempts to monitor can themselves have an impact on
   the performance of the system we are monitoring. The "observer
   effect"—changing a system by the act of observing it—is well known
   in physics, and we see something similar in networks. Packet
   tracing is intrusive, and can easily impact the performance of the
   network data plane. Care must be taken to limit its duration (it
   should only activated to troubleshoot a problem) and the volume it
   captures (it may be sufficient to only sample traffic). Packet
   tracing also has serious implications for privacy, providing a
   motive for end-to-end encryption, which we discuss in Chapter
   |TLS|.


|Ops|.4.3 Active Monitoring
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The preceding examples of telemetry data are all based on *passive
measurements*, a term that means we are measuring traffic that the
network would be carrying whether we are monitoring it or not. An
alternative, called *active measurement*, is a strategy in which we
inject synthetic traffic into the network so we can observe and
measure how the network processes it.

Two widely-used active monitoring tools are ``ping`` and
``traceroute``. Even for end-users on their laptops, they provide
useful information, especially when you are trying to figure out why
some web site is unreachable. Despite their simplicity, they are also
an important part of any network operator's toolkit. Another, less
familiar tool is ``iperf``, which measures the achievable TCP
throughput rate to some remote server. As we'll see in Chapter |TCP|,
packet loss impacts the measured throughput rate, so ``iperf`` does
not necessarily reflect the maximum achievable rate—a fact your ISP
will remind you of should you report you are not seeing the bandwidth
you are paying for—but it is does provide useful data about your
effective bandwidth.

One of the challenges in using tools like ``iperf`` is that you need a
willing server on the other end of the connection; ``ping`` and
``traceroute`` use other strategies to generate a response, although
ISPs often thwart them. In response to this problem, and so as to
collect data about a broad swath of the Internet, there are
collaborative efforts to deploy servers that are explicitly programmed
to respond to active measurement probes. One of these is
MeasurementLab (M-Lab), which has installed servers at richly
connected Internet exchange points and datacenters around the world.
M-Lab deploys its own active measurement tool—called the *Network
Diagnostic Tool (NDT)*\—which not only measures both uplink and
downlink performance, but also diagnoses the factors impacting the
results. M-Lab archives the results of any active probe requested by
any user, and the resulting dataset is then available to be queried by
anyone. This is useful for understanding the quality of broadband
connectivity worldwide (in addition to testing the quality of your own
connection).

.. admonition:: Further Reading

    `Measurement Lab <https://www.measurementlab.net/>`__.

|Ops|.4.4 In-Band Network Telemetry
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

We conclude this discussion of telemetry by returning to the weakest
link—that someone had the foresight to collect the data we want prior
to the realization that we need it. This concern is especially important
on hardware switches, which are not easily modified in the field.
Should we need to add a new counter to the data plane, we are likely
to need to wait until the next release of the switching chip. The
exception is when the forwarding pipeline is programmable, as is the
case with the Protocol Independent Switching Architecture (PISA)
mentioned in Section |Tech|.2. It turns out that one of the most
compelling aspects of programming the dataplane in a language like P4
is the ability to add instrumentation to the data plane, a technique
known as *In-band Network Telemetry (INT)*.

The idea of INT is to encode telemetry “instructions” into packet
header fields, and which then causes network switches to process them
as they flow through the forwarding pipeline. These instructions tell
an INT-capable device what state to collect, and then how to write
that state into the packet as it transits the network.\ [#]_ INT traffic
sources (e.g., applications, end-host networking stacks, hypervisors)
can embed the instructions either in normal data packets or in special
probe packets. Similarly, INT traffic sinks retrieve and report the
collected results of these instructions, allowing the traffic sinks to
monitor the exact data plane state that the packets observed
(experienced) while being forwarded.

.. [#] You read that correctly. Telemetry data is "written" to packet
       headers, as opposed to local memory, and then "read" from the
       header by the ultimate destination.

The idea is illustrated in :numref:`Figure %s <fig-int>`, which shows
an example packet traversing a path from source switch *S1* to sink
switch *S5* via transit switch *S2*. The INT metadata added by each
switch along the path both indicates what data is to be collected for the
packet, and records the corresponding data for each switch.

.. _fig-int:
.. figure:: operations/figures/int.png
    :width: 650px
    :align: center

    Illustration of Inband Network Telemetry (INT), with each packet
    collecting measurement data as it traverses the network.

INT is still in an early stage, but it has the potential to provide
qualitatively deeper insights into traffic patterns and the root
causes of network failures. For example, INT can be used to measure
and record queuing delay individual packets experience while
traversing a sequence of switches along an end-to-end path, with a
packet like the one shown in the figure reporting: *"I visited Switch
1 @780ns, Switch 2 @1.3µs, Switch 5 @2.4µs."* This information can be
used, for example, to detect *microbursts*—queuing delays measured
over millisecond or even sub-millisecond time scales—as reported by
Xiaoqi Chen and colleagues.  It is even possible to correlate this
information across packet flows that followed different routes, so as
to determine which flows shared buffer capacity at each switch.

.. _reading_int:
.. admonition:: Further Reading

   X. Chen et al. `Fine-grained queue measurement in the data plane
   <https://p4.org/p4/conquest>`__. ACM CoNEXT'19, December 2019.

Similarly, packets can report the decision making process that
directed their delivery, for example, with something like: *"In Switch
1, I followed rules 75 and 250; in Switch 2, I followed rules 3 and
80."* This opens the door to using INT to verify that the data plane
is faithfully executing the forwarding behavior the network operator
intended.

This brief overview of INT illustrates once again a potential
benefit of SDN and programmable pipelines: the ability to try out new
ideas that would have in the past been infeasible. With traditional
fixed-function ASICs doing the packet forwarding, you could never get
the chance to try an idea like INT to see if the benefits justify the
cost.
