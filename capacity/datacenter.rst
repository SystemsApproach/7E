|Capacity|.4 Datacenter Networks
--------------------------------------
   
While RED and EQN have not enjoyed wide-spread usage in the Internet
at large, and the default queuing discipline throughout the Internet
is still FIFO with tail drop, the mechanisms described in the previous
two sections have proven useful in certain settings. Datacenter
networks are the most noteworthy, and so we use them to illustrate how
these mechanisms can be used in practice.

Datacenter networks have two properties that make them an ideal
environment. One is that they are self-contained, under the control of
a single organization. Cloud adminstrators can unilaterally decide
that all routers implement a particular queuing discipline, set the
ECN bit when signs of congestion are detected, and enforce good
behavior among all edge hosts (the datacenter servers). The second
reason is that RTTs are (relatively) uniform within a datacenter;
there is no worry that some hosts will require 100ms, or longer, to
react to an ECN.

If anything, datacenters have proven to be such furtile ground for
these mechanisms that there is no single correct answer; different
combintations of mechanisms are sold by different hardware vendors and
used by different clouds. But there are a few universally agreed upon
point (and general lessons learnded) that we can call out.

First, the ``ToS`` bits introduced in Section |Capacity|.1.5 have been
replaced by an interpretation knowns as *Differentiated Services Code
Point (DSCP)*, as defined in RFC 2474. This RFC was written in 1998,
before modern datacenters, but the idea it advanced—known as
*Differentiated Services* or *DiffServ*\ —, as proven to be meet the
needs of datacenter networks.

Expand on DSCP codes and overview of DiffServ...

Second, instead of probablistically dropping packets, ECN is used to
provide direct feedback.

Third, RED is upgraded to WRED. Corresponding packet scheduler is WFQ
(DWRR), although Priority Queues are an option (especially for control
traffic).

Talk about how "abstract queues" (and DSCP settings) are mapped onto
hardware queue.

Finally, foreshadow edge hosts, which we run an ECN-aware transport,
either DCTCP or RDMA/RoCE. Include brief overview of policing options:
at ToR vs IPU vs Hypervisor.



