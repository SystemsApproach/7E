|Ops|.3 Monitoring and Telemetry
--------------------------------------

There are many potential sources of telemetry data. This section looks
at some representative examples; the collection is far from
comprehensive. Of particular note, we focus on quantitative metrics,
as opposed to logs and traces (the other three pillars of
observability cited in Section |Ops|.1). This is because we are
primarily focused on a rather narrow function: forwarding packets
through a network of switches and routers. Router and switch vendors
often make heavier use of logs and traces to ensure that their
implementation of BGP, say, is correct, but once in the hands of a
network operator, metrics are the most common way of monitoring
whether or not the network is operating properly.  We revisit this
topic in Section |Ops|.4, when we look at rapidly evolving network
functionality.
|Ops|.3.1 OpenConfig Variables
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A good place to start is the OpenConfig models described in the
previous section.  As we noted, the models include both configurable
variables (those with ``rw`` permissions) and variables that devices
use to report local data back to the central management system (those
with ``ro`` permissions). For example, the following code snippet
expands on the ``counter`` variable for Ethernet interfaces shown in
Section |Ops|.2.2:

.. literalinclude:: operations/code/counters.yang

Note that the ``last-clear`` variable indicates when the device last
set the counters to zero, which can then be used to calculate average
transmission rates over various time intervals.

In addition to monitoring performance, gNMI and OpenConfig can also be
used to read variables that provide insight into how stable a protocol
is. This can be especially important for monitoring routing protocols
like BGP.  For example, the following snippet of YAML show what might
be reported by BGP on a router. (As a reminder, YANG is used to define
the model schema, and YAML is used to actually pass values according
to a given schema.)

.. literalinclude:: operations/code/bgp.yaml

This example shows variables associated with a given BGP neighbor;
there are several things an operator needs to keep an eye on. Variable
``session-state`` tells us the current BGP session is ``ACTIVE``,
which is good, but seeing ``established-transitions`` increasing over
time might be a sign that the neighbor is repeatedly failing and
attempting to reconnect. Tracking changes in ``last-established`` is
another indicator of a potential problem.  Moreover, variable
``messages.NOTIFCATIONS`` tells us how many clean session restarts
there have been; that value being less than
``established-transitions`` is an indication that the network path
between this router and its neighbor is suspect (i.e., the underlying
TCP connection failed). Finally, tracking changes in
``prefixes.received`` can serve as a signal about how stable the
neighbor is; a router that rapidly changes the number of prefixes it
is willing to advertise from one session to the next is experiencing
some kind of instability.

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


|Ops|.3.2 Traffic Analysis
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Counting the number of packets sent/received by a given interface is a
useful metric, but it doesn't provide any insight into what's actually
in the packets. Seeing the contents of packets being exchanged between
two end-points is helpful when you are troubleshooting a problem, but
more broadly, just knowing how a given link is being shared among
multiple end-to-end flows can be useful. The most obvious use case for
the latter is to be able to see when an excessive amount of traffic
belongs to just one flow, which could be a sign of a Denial-of-Service
attack.

A brute force approach to gaining visibility into the traffic being
carried by a network is to capture every packet being exchanged on a
link (typically the packet header and the first few bytes of the
payload is sufficient), and dump it into a file for analysis. This is
most often done on an end host rather than in a switch or router—due
to the volume of traffic being carried—and even then, it is only done
for the sake of troubleshooting a problem; continually collecting a
packet trace will eventually overflow the available backing store. The
most common tool for doing this is a tool called ``tcpdump`` (although
it can be configured to capture more than just TCP packets), and the
output is a format known as ``pcap`` (for packet capture). Various
graphical tools, most notably *Wireshark* can then be used to parse
the packet headers and display their fields.

.. admonition:: Further Reading

    `Wireshark <https://www.wireshark.org/>`__.

Beyond troubleshoot, packet capture is not a practical approach to
analyzing traffic. At the flow level, simply deciding what flows are
currently active, and which flow a given packet belongs to, is an
expensive operation. This is true both in terms of the number of
processor cycles required to do a lookup, and in the amount of memory
required for all the data one might potentially collect.  These
concerns become significant when monitoring high-speed links, for
example, at 10 Gbps and above. Monitoring all the interfaces on just
one switch becomes even more problematic.

The solution is to instrument the forwarding path to just sample the
packets it processes. This means capturing and saving the header for
one out of every N packets. (We need the header to identify the
end-points, which typically includes TCP or UDP port numbers.) An open
standard, called *sFlow*, has been developed around the idea of
sampling. A switch needs to be instrumented to support sampling, and
an interface is needed to say what level of sampling to perform (e.g.,
1-in-10000 on a 100-Gbps link) and where to send the collected
headers. For the latter problem, OpenConfig defines an ``sflow``
model, although it is still more common for vendors to provide CLI
command to activate and configure sflow.

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
   planning.  Seems like it's missing a "takeway" similar to the last
   paragraph in the previous subsection.



|Ops|.3.3 Active Monitoring
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The preceding examples of telemetry data are all based on *passive
measurements*, a term that means we are counting/measuring traffic
that the network would be carrying whether we are monitoring it or
not. An alternative, called *active measurement*, is a strategy in
which we inject synthetic traffic into the network so we can observe
and measure how the network processes it.

For example, iperf, ping, traceroute.

Include off-line analysis based on widely collected data; e.g., MeasurementLab.

|Ops|.3.4 InBand Network Telemetry
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Finally, talk about inband network telemetry
