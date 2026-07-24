|Capacity|.4 Service Differentiation
--------------------------------------

While RED and ECN have not enjoyed wide-spread adoption in the
Internet at large, and FIFO with tail drop continues to be the default
queuing discipline, the mechanisms described in the previous two
sections have proven useful in several specific settings. We describe
here some of the scenarios where offering different levels of service
to different classes of traffic has proven both tractable and useful.

The possibility of sending latency-sensitive traffic over
packet-switched networks was known from at least the 1980s, but
increases in bandwidth in the 1990s drove greater interest in sending
voice and then video traffic over the Internet. Voice over IP (VOIP)
started to emerge as a real possibility and exposed the need to
control the latency experienced by a subset of the traffic passing
through routers and across networks. This interest in controlling the
latency of selected traffic led to a flurry of related research, and
to a set of activities at the IETF to standardize quality of service
mechanisms. The most successful of these was the Differentiated
Services architecture, also known as DiffServ.

One major contribution of DiffServ was to redefine the ``ToS`` field
of the IP header to allow common behaviors, such as low-latency
queuing, to be requested by setting the *Differentiated Services Code
Point (DSCP)* to a well-known value. In addition, it standardized a
handful of *per-hop behaviors (PHBs)* that could be implemented using
common queuing mechanisms such as those described in Section
|Capacity|.2. With these two features in place, it became reasonably
straightforward to configure networks to support applications such as
VOIP. A device sending VOIP traffic marks the voice packets with the
well-known DSCP value; routers are configured to recognize that value
and place the corresponding packets into a queue that will deliver low
latency, such as a priority queue.

The commonly used set of per-hop behaviors include:

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
first six bits of the ``ToS`` field; the other two bits are available
to be used by ECN. The details of how a specific network might use
these DSCP settings are spelled out in multiple RFCs, each targeted at
a different use case—e.g., multimedia conferencing, multimedia
streaming, VOIP, and so on.


The definition of the Differentiated Services field, defining six
bits of the old ``ToS`` byte, is in RFC 2474, while the companion RFC
2575 spells out the larger architectural picture for DiffServ. 

.. admonition:: Further Reading

   K. Nichols, S. Blake, F. Baker, and D. Black. `Definition of the
   Differentiated Services Field (DS Field) in the IPv4 and IPv6
   Headers <https://www.rfc-editor.org/info/rfc2474>`__. RFC 2474,
   December 1998.

   S. Blake et al. `An Architecture for Differentiated Services
   <https://www.rfc-editor.org/info/rfc2475>`__. RFC 2475,
   December 1998.

There are some remaining challenges, such as making sure that this
capability is not abused by endpoints that don't require low latency,
since an excessive amount of traffic in a priority queue still won't
receive low latency. There may be certain trusted endpoints, such as
the dedicated IP phones that started appearing in the early days of
VOIP. There are also ways to "police" traffic so that packets marked
with certain DSCP values are either limited in total bandwidth or
completely disallowed on some interfaces. These challenges are
manageable, especially in the enterprise networks where this
technology was particularly popular. While ever-increasing backbone
speeds limited the utility of DiffServ in the core of the Internet,
the capabilities remain available for managing bottleneck links in
enterprises and in the last mile of residential networks. They also
find application in modern datacenters.

Two properties make datacenter networks ideal candidates for enhanced
resource management. One is that they are self-contained, under the
control of a single organization. Cloud administrators can, for
example, unilaterally decide on a single set of DSCP values and PHBs
to be used. They can arrange that all routers implement a particular
queuing discipline, set the ECN bit when early signs of congestion are
detected, and enforce good behavior among all edge hosts (the
datacenter servers). The second simplifying factor in datacenters is
that they typically have sub-millisecond RTTs, so there reason to
expect that most hosts will react in a timely manner to a
congestion markings.

If anything, datacenters have proven to be such fertile ground for the
mechanisms described in this chapter that there is no single correct
answer; different combinations are packaged in products by vendors and
deployed in datacenter fabrics by cloud providers. It is also an
active area of research, so there is not yet any consensus about
exactly what combination works best. But datacenters do illustrate how
multiple mechanisms described in this chapter are used in concert to
build a complete solution; that coordination is the focus of this
section. It appears that datacenter networks have largely converged on
a minimal subset that includes just one or two AF classes, in addition
to the high-priority network control class and default best-effort
class.


In addition to DiffServ traffic differentiation, datacenters use ECN
in a manner that is slightly different than that described in the
previous section. Instead of probabilistically dropping packets, datacenters use
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
labels and limiting their sending rate. One general approach is for
the cloud to police senders; this acknowledges that a cloud hosts
VMs that are able to run arbitrary code. This policing action can be
implemented in the hypervisor that sits between the server and the
tenant VM; in the NIC that connects the server to the datacenter
fabric; and/or in the Top-of-Rack switch that is the first switch on that
fabric. The other general approach is to trust senders to behave
correctly; this is more likely to happen when the you are paying for
resource usage. As for exactly what constitutes good sender behavior,
we describe two examples in Part III: Chapter |CC| looks at TCP
congestion control, and Chapter |Message| looks at RDMA.

