|Capacity|.4 Datacenter Networks
--------------------------------------

While RED and ECN have not enjoyed wide-spread adoption in the
Internet at large, and FIFO with tail drop continues to be the default
queuing discipline, the mechanisms described in the previous two
sections have proven useful in narrow settings. Datacenter networks
are a noteworthy example, and so we use them to illustrate how these
mechanisms are used in practice.

.. TODO -- Could rework this section to make Datacenter one example
   out of 2 or 3, including VOIP and perhaps access networks.

Datacenter networks have two properties that make them an ideal
candidate for enhanced resource management. One is that they are
self-contained, under the control of a single organization. Cloud
administrators can unilaterally decide that all routers implement a
particular queuing discipline, set the ECN bit when early signs of
congestion are detected, and enforce good behavior among all edge
hosts (the datacenter servers). The second reason is that datacenters
typically have sub-millisecond RTTs, so there is no worry that some
hosts will require 100ms, or longer, to react to an ECN.

If anything, datacenters have proven to be such fertile ground for the
mechanisms described in this chapter that there is no single correct
answer; different combinations are packaged in products by vendors and
deployed in datacenter fabrics by clouds providers. It is also an
active area of research, so there is not yet any consensus about
exactly what combination works best. But datacenters do illustrate how
multiple mechanisms described in this chapter are used in concert to
build a complete solution; that coordination is the focus of this
section.

First, we need a way for edge hosts and routers to exchange
information with each other. The ``ToS`` bits introduced in Section
|Capacity|.1.5, which has been replaced by an interpretation known as
*Differentiated Services Code Point (DSCP)*, plays this role. DSCP is
defined in RFC 2474, which was written in 1998, and predates modern
datacenters. But it prescribes a general mechanism that works across
many use cases, including datacenters.  Specifically, DSCP allows
networks to distinguish between different *classes* of traffic, where
each class corresponds to packets that should be treated the
same. Each class, in turn, is assigned a *behavior*, where the
recommended set of behaviors include:

* **Default Forwarding (DF):** Regular best-effort traffic.

* **Expedited Forwarding (EF):** Traffic that has low-loss,
  low-latency, low-jitter requirements.

* **Assured Forwarding (AF):** Traffic that has throughput
  requirements, which will be met as long as the sender is well
  behaved.

AF is further subdivided into four sub-classes, each of which has an
associated *drop preference* (low, medium, high), for a total of 12
possible AF-related values. Finally, there is an attempt to maintain
backward compatibility with the priority (precedence) specified in the
first three bits of the original ``ToS`` field, which continues to be
used for high-value network control traffic, such as BGP and OSPF.
All told, there are 2\ :sup:`6` = 64 possible DSCP settings, using the
first six bits of the ``ToS`` field; the other two bits are used by
ECN. The details of how a specific network might use these DSCP
settings are spelled out in multiple RFCs, each targeted at a
different use case—e.g., multimedia conferencing, multimedia
streaming, telephony, and so on—but it turns out that datacenter
networks have largely converged on a minimal subset that includes just
one or two AF classes, in addition to the high-priority network
control class and default best-effort class.

.. admonition:: Further Reading

   K. Nichols, S. Blake, F. Baker, and D. Black. `Definition of the
   Differentiated Services Field (DS Field) in the IPv4 and IPv6
   Headers <https://www.rfc-editor.org/info/rfc2474>`__. RFC 2474,
   December 1998.

.. TODO -- Verify that the above paragraph is correct. It suggests a
   thread that is of minimal value (except perhaps as a negative
   example). The Google Cloud docs suggest a larger number of AF classes.

Second, instead of probabilistically dropping packets, datacenters use
ECN to provide direct feedback. Given the low round-trip times, ECN
allows traffic sources to react quickly to queue buildup. But since we
have more than one class of service, we need a different way to decide
what packets to mark. One option is to use ECN in conjunction with a
variant of RED, known as *Weighted RED (WRED)*. WRED augments RED by
applying different thresholds (``MinThreshold``, ``MaxThreshold``) and
probability (``MaxP``) to each DSCP class. This means the router
more or less aggressively selects certain traffic for ECN marking, but
all within the same queue. A second option is to segregate different
DSCP classes into different queues, each of which is serviced in a
weighted round-robin fashion, that is, using the FQ scheduler
described in Section |Capacity|.2.3. Each of these queues then applies
ECN markings with independently selected thresholds.

While in principle the RED algorithm for calculating the average queue
length could be used in both cases, typically the instantaneous queue
length is used instead to trigger ECN marking. This works because (1)
the RTTs are uniform (meaning there is no need to account for drastic
variations), (2) the RTTs are short (meaning feedback happens
quickly), and (3) the switching fabric's capacity is calibrated to
require short queues (meaning any burst of queuing is enough to signal
congestion).  As it turns out, configuring RED to use the
instantaneous queue length is just a special case of the more general
algorithm: we set parameters ``MinThreshold`` and ``MaxThreshold``
equal to each other, and we set ``Weight=1`` for the average queue
length calculation.

Finally, the in-network mechanisms make assumptions about the edge
hosts being well-behaved, which includes applying the right DSCP
labels and limiting their sending rate). One general approach is for
the cloud to "police" senders; this acknowledges that a cloud hosts
VMs that are able to run arbitrary code. This policing action can be
implemented in the hypervisor that sits between the server and the
tenant VM; in the NIC that connects the server to the datacenter
fabric; and in the Top-of-Rack switch that is the first switch on that
fabric. The other general approach is to trust senders to behave
correctly; this is more likely to happen when the you are paying for
resource usage. As for exactly what constitutes good sender behavior,
we describe two examples in Part III: Chapter |CC| looks at TCP
congestion control, and Chapter |Message| looks at RDMA.

