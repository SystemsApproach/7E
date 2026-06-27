.. index:: IRR: Internet Routing Registry
.. index:: RIR: Regional Internet Registry
.. index:: ROA: Route Origin Authorization
.. index:: ROV: Route Origin Validation
.. index:: RPKI: Resource Public Key Infrastructure
.. index:: NIST: National Institute of Standards and Technology

|BGP|.3 Securing Route Information
--------------------------------------

The division of the Internet into autonomous systems allows individual
ASes the freedom to run their networks as they please, as long as they
provide the IP service model and exchange routing information with
other ASes. However, an important question arises around trust. How
can one AS trust the routing advertisements from another AS,
especially one with which it has no direct relationship? This problem
has proven to be a long-lasting challenge for the smooth running of
the Internet.

In most respects, a router is just a special-purpose computer with
some high-speed interfaces and specialized software to perform tasks
such as route computation and advertisement. So routers need to be
protected like end-systems, e.g., with strong passwords and
multi-factor authentication, using access control lists and firewalls,
etc.  That is only a starting point, however, because the actual
routing protocols themselves introduce opportunities for attack. BGP
is vulnerable to a wide range of attacks, and as the one routing
protocol that crosses the boundaries of a single administrative
domain, deserves our attention when we think about security of routing.

There are multiple levels to the problem of securing inter-domain
routing.  When you make a secure, encrypted connection to your bank,
you rely on Transport Layer Security (TLS) to keep your data private
(using encryption) and to authenticate the connection to the bank. We
cover the details of TLS in Chapter |TLS|. And if you are confident that you
are really connected to your bank, you probably trust them to show you
accurate information about your account (mostly—banks do make mistakes
on occasions). But a secure connection to a BGP speaker (a router
running BGP) doesn't imply that every route advertisement provided by
that speaker is reliable. In fact both honest mistakes and deliberate
configuration decisions can and have resulted in false advertisements
being made in BGP.

|BGP|.3.1 Authentication and Integrity
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Since BGP runs over a TCP connection, it was recommended for many
years that the TCP connection be authenticated and integrity-protected
using MD5, a cryptographic hash. That algorithm is now known to be
vulnerable to attack and is deprecated in favor of a more general TCP
authentication option using more modern cryptography, described in
RFC 5925.

.. admonition:: Further Reading

   J. Touch, A. Mankin, and R. Bonica. `The TCP Authentication Option
   <https://www.rfc-editor.org/info/rfc5925>`__. RFC 5925,
   June 2010.

Given that TLS is now quite ubiquitous on the World Wide Web, it may
seem surprising that BGP does not use it for authentication.
This comes down to a combination
of factors including history, inertia, and the different operational
model of running BGP versus connecting to a remote website. For
example, BGP sessions often run between directly connected routers at
a peering point or Internet exchange points (IXPs) which allows for a
simple TTL-based method to prevent spoofing. Privacy of BGP updates is
considerably less important than authenticity. And as we shall see,
there is a lot more to establishing the authenticity of a BGP
advertisement than just authenticating the messages from a peer.

When BGP was being developed in the 1980s and 1990s, TLS was still far
in the future, and packet encryption and decryption operations were
generally quite computationally expensive. So it made sense that the
initial focus was on authenticating messages, first using MD5 and then
with updated algorithms, rather than providing the
greater protection of encryption that TLS offers.

Whatever authentication mechanism is used, the
effect is only to verify that the information came from the intended
peer and has not been modified in transit. The much more challenging
part of Internet routing security is in the validation of the routing
information itself. When a peer announces a path to a
certain prefix, how do we know that they really have this path?

An overview of current best practice recommendations for securing BGP
can be found in RFC 7454. As we discuss below, there remains plenty of
room for improvement when it comes to ensuring the validity of path
advertisements.

.. _reading_BGPTLS:
.. admonition::  Further Reading


   J. Durand, I. Pepelnjak and G. Doering. `BGP Operations and
   Security <https://www.rfc-editor.org/info/rfc7454>`__. RFC 7454,
   February 2015.

|BGP|.3.2 Correctness of Routing Information
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

When a BGP speaker announces a path to a particular prefix, how do we
know that they really have such a path? Do we know that they will use
the path if asked to do so? Are they even authorized to use the path?
The short answer to all these questions is that we don't know, but
there has been a multi-decade quest to build mechanisms that enable
greater confidence in the correctness of such announcements. The
Further Reading below includes two papers (from 2014 and 2021) that
show how long the process has been.

Let's start with a simple and well-studied example. In 2008, ISPs in
Pakistan were ordered by the government to block access to YouTube for
users in the country. One ISP (Pakistan Telecom) chose to do this by
advertising a route to a prefix that was within the range allocated to
YouTube. In effect, the ISP announced "I have a good path to YouTube"
so that it could then redirect traffic that would try to follow that
path. So this was a false advertisement created with the goal of
preventing access to part of the Internet.

The problem that arose is a consequence of one of the rules of
Internet routing: when you have two overlapping prefixes in the
routing table, the *more specific* (i.e. longer) prefix is
preferred. Unfortunately, the false YouTube path announcement made in
Pakistan was for a longer prefix than the true path that was
advertised elsewhere.  This turned into a problem well beyond the
boundaries of Pakistan when the ISP advertised the route upstream to a
larger ISP.  The upstream ISP now saw the more specific route as a
distinct piece of routing information from the true, less specific
route, and so it re-advertised this (false) path to its
peers. Repeated application of this decision to accept the more
specific path and re-advertise it caused much of the Internet to view
the small ISP in Pakistan as a true path to YouTube. Within minutes a
large percentage of the Internet was sending YouTube request traffic
to Pakistan, causing a global outage for YouTube. Resolution was
achieved by manual intervention at multiple ISPs to stop the global
advertisement of the false path.

There are many other forms of attack possible on BGP, but they mostly
take the form of a route being advertised and then propagated when it
should not be. There is a relatively simple measure that should have
prevented the incident described above: the provider AS immediately
upstream from Pakistan Telecom should not have accepted the
advertisement that said "I have a route to YouTube". How would it know
not to accept this? After all, BGP needs to be dynamic, so a newly
advertised prefix is often going to be correct. One solution to
this problem is the use of Internet Routing Registries (IRRs), which serve as
databases mapping address prefixes to the ASes that are authorized to
advertise them. In the prior example, since YouTube is not a customer
of Pakistan Telecom, the IRR would show that the YouTube prefix should
not be advertised by this AS. The responsibility to filter out the
false announcement falls on the *upstream* ISP, who would need to
periodically query one or more IRRs in order to maintain an up-to-date
set of filters to apply to its downstream peers.

There are numerous issues with the IRR approach, including
that this sort of filtering gets much more difficult the closer you
get to the "core" of the Internet. It's one thing to filter prefixes
from an ISP that serves a modest number of customers in a single
country; it's another to filter prefixes coming from a large peer with
global presence. Some obviously bad routes can be filtered but it's
very hard to get a complete picture sufficient to rule out anything
incorrect that could be advertised. The set of rules that need to be
configured on a BGP router for an ISP that carries hundreds of
thousands of routes can also get very large.

There is also a question of whether all the information provided by an
IRR can be trusted. We discuss approaches to building trust in the
information provided by an IRR below.

Furthermore, as noted in the article "*Why Is It
Taking So Long to Secure Internet Routing?*", the incentives for
prefix filtering are not well aligned. The cost of filtering falls
on the AS that is immediately upstream of the misbehaving ISP, while
the benefit accrues to some distant entity (YouTube in our example)
who avoids the impact to their traffic thanks to the work of a
provider with whom they have no relationship.

A more sophisticated approach relies on the use of cryptographically
signed statements authorizing a particular AS to advertise paths to a
particular prefix. The technology behind this is referred to as RPKI:
Resource Public Key Infrastructure.  RPKI builds on the concepts of
cryptographic signatures and certificate hierarchies that also
underpin TLS and security of the Web.

RPKI provides a means by which entities involved in routing, such as
the operator of an AS, can make assertions about information that is
related to the advertisement of routes. These assertions take various
forms depending on which part of the problem they aim to solve. We
describe three different uses of the RPKI in the following sections.


.. admonition::  Further Reading

   S. Goldberg. `Why Is It Taking So Long to Secure Internet
   Routing? <https://dl.acm.org/doi/pdf/10.1145/2668152.2668966/>`__
   ACM Queue, August 2014.

   C. Testart and D. Clark. `A Data-Driven Approach to
   Understanding the State of Internet Routing Security
   <https://faculty.cc.gatech.edu/~ctestart8/publications/RoutingSecTPRC.pdf>`__. TPRC
   48, February 2021.

|BGP|.3.3 Route Origin Validation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The first use of RPKI is to allow an AS to prove that it is authorized
to originate routing advertisements for specific address prefixes. A
Route Origin Authorization (ROA) contains a certificate (which we
explain below),
an AS number, and a set of prefixes that the AS is authorized to
advertise. The ROA is cryptographically signed by an entity that is
itself trusted to provide this authorization, generally the organization to
which this address prefix has been allocated. For example, an ISP
typically is allocated a certain set of prefixes and may originate
routing advertisements for those prefixes. An ROA allows the ISP to
assert that it has the authority to make such an announcement, and for
BGP speakers elsewhere in the Internet to validate that assertion. The
challenge lies in how we establish trust in the ISPs to make these
announcements so that they can be validated anywhere in the Internet.

Address allocation, since the introduction of CIDR and IPv6, has been
a hierarchical process.  Regional Internet Registries (RIRs) are at
the top of the hierarchy for address allocation. Their position in the
hierarchy of address allocation makes them a logical place to
establish "roots of trust", known as trust anchors. There are five
RIRs globally (ARIN, RIPE, APNIC, AFRINIC and LACNIC) and each has a
root certificate in the RPKI. We will see how certificates work in
more detail in Chapter |TLS|. For our purposes in this chapter, it is
sufficient to understand that if I know the public key of some entity
such as an ISP or an RIR, then I can check the validity of something
signed by that entity. And a certificate is just a cryptographically
signed statement providing some information. For example, an RIR can sign a
certificate that asserts that a certain ISP has a particular public
key or a certain allocated set of addresses.

The chain of trust for address allocation starts from an RIR, the root
of trust. An RIR allocates a chunk of address space to an ISP, and the
ISP can sub-allocate from that chunk to each of its customers. There
may be additional layers in this hierarchy. A hierarchy of
certificates is created to follow this hierarchy of address
allocation. At each level in the hierarchy, a certificate is issued
that says "this entity has been allocated the following block of
address prefixes, and has the following public key." By repeating this
process all the way down the hierarchy we can build "chains of trust"
from the root to the leaves.



.. _fig-rpki:
.. figure:: policy/figures/rpki.png
   :width: 600px
   :align: center

   Chain of trust for RPKI

:numref:`Figure %s <fig-rpki>` shows how the certificates are arranged
for a simple example of an ISP *A* with customer *C*. There is a chain
of trust from the root certificate to the customer. The certificate
that ISP *A* issues, on the far right of the picture, says that some
address prefix has been allocated to customer *C*, and includes the
public key of customer C. This certificate is signed by ISP *A* using
the private key of *A*. So if we can trust *A*, we learn two things
about *C*: its public key and the set of addresses allocated to the
holder of that public key. Note that we don't learn who *C* is; we
just learn the public key of the entity that is authorized to
originate routing advertisements for some prefix or prefixes.

One level higher in the chain, the Regional Internet Registry (RIR) has
issued a certificate that states ISP *A* has authority to allocate
addresses out of some prefix. The certificate also tells us the
public key of *A*. The prefix that *A* has allocated to *C*
must be a subprefix within the allocation made by the RIR.
By following the chain back to the root certificate, it is possible to
establish that *C* is legitimately able to advertise the prefix
allocated to it by *A*. This chain works as long as we trust the root
certificate, and so we have to take care to distribute root
certificates in some secure manner to bootstrap the process.

At this point we have created a set of bindings between public keys,
which are held by entities such as Internet Registries, ISPs, and end
customers, and IP address prefixes allocated to those entities. The
next step is to create a Route Origin Authorization (ROA), which is a
cryptographically signed object that associates a prefix with an AS
that is authorized to originate routing advertisements for that
prefix.

In our example above, *C* creates an ROA which it signs
with its private key. The ROA contains the AS number of *C* and the
prefix or prefixes that it wishes to advertise. Anyone who looks at
the ROA and the resource certificate chain that leads from the root CA to *C*
can validate that it has been signed with the private key belonging to
C; they can also check that C is authorized to advertise the prefixes contained in the
ROA. Because the ROA also contains the AS number for *C*, we now know
that we should trust advertisements of this prefix if they originate
from the stated AS. Furthermore, an ROA may limit the maximum length of the prefix to
protect against bogus advertisements of more specific routes to a
sub-prefix (as in the YouTube example above).

.. _fig-roa:
.. figure:: policy/figures/ROA-trust.png
   :width: 600px
   :align: center

   An ROA has a chain of trust back to the RPKI root

Rather than being passed around in real time with routing advertisements,
the RPKI certificates and ROAs are stored in repositories, which are typically
operated by the RIRs. Address allocations happen at a relatively long
timescale, and certificates can be issued at the same time. Thus it is
feasible to fetch the entire contents of the RPKI repository to build up a
complete picture of the chains of certificates and signed ROAs that have been
issued. With this information, ISPs use validation tools to determine *in advance* which
ASes could originate routing advertisements for which prefixes. When a
router that participates in BGP receives a new announcement, it can
check its validity against the local validation tool. The
repositories now become an essential part of our routing
infrastructure and must themselves be secured and protected against
Denial of Service (DoS) attacks.

There is a well-established set of software tools to automate the
process of leveraging the RPKI for popular operating systems and
commercial routing platforms. Notably, the routers running BGP do not
perform cryptographic operations in real time when processing route
advertisements; all the cryptographic operations happen in advance on
servers that are external to the routers themselves.  The external validator
systems answer queries about the validity of BGP advertisements based on information
they have downloaded from the RPKI repository.

With the RPKI in place it is now possible to perform Route Origin
Validation (ROV). That is, if a given AS claims to be the originator of a
certain prefix, that claim can be checked against the information in
the RPKI. So, for example, if Pakistan Telecom were now to claim to be the
origin AS for a subprefix of YouTube, that could immediately be
detected as false information and discarded by any router receiving
such an advertisement, not just the neighbors of the offending ISP.

.. _fig-rpki-sys:
.. figure:: policy/figures/RPKI-system.png
   :width: 600px
   :align: center

   Each AS maintains a local cache of the RPKI repository, and BGP
   speakers query the local cache, allowing them to validate BGP
   advertisements.

Some of the practical aspects of ROV are shown in :numref:`Figure %s
<fig-rpki-sys>`. An AS performing route origin validation maintains a
local cache of the RPKI repository, which is fetched using rsync or
the RPKI Repository Delta Protocol (RRDP). A BGP speaker in the AS
retrieves the set of valid ROAs by querying the local cache, using
another protocol called the RPKI-to-router (RTR) protocol. This
protocol allows the router to receive periodic updates to the set of
valid ROAs from the local cache. With this information in hand, the
router is able to check the validity of the originating AS in BGP
advertisements that it receives from other ASes.

While there are many forms of attack or misconfiguration that would
not be caught by ROV (particularly an AS falsely advertising a path that
doesn't actually exist to a valid originating AS) it does prevent a large number of issues,
especially those caused by misconfiguration. To more fully combat the
advertisement of false information in BGP, it is necessary to adopt
some sort of path validation, as discussed below.

The adoption of ROAs is tracked by
NIST (the National Institute of Standards and Technology in the
U.S.)—see the Further Reading section. At the time of writing, the
NIST RPKI monitor indicates that of the one-million-plus routes
advertised globally in BGP, about 56% are covered by valid ROAs. Less than 2%
are detected as invalid (the ROV check fails) while the remaining 42%
do not contain ROA information.  Looking at the deployment over time
we can see a steady increase in valid ROA and a corresponding decrease
in the "not found" group—the advertisements with no ROA. While 56% is
a long way from 100%, this level of penetration is a significant
accomplishment—especially given the historical difficulty of making
changes to Internet routing and the "core" of the Internet.

Finally, note that there needs to be a process to revoke certificates,
for cases such as the re-allocation of an address prefix from one
provider to another. Revocation can be handled by the RPKI
repositories using "certificate revocation lists" (CRLs) which are
distributed using the same mechanisms as certificates.


.. _reading_rpki:
.. admonition::  Further Reading


   NIST. `RPKI Monitor <https://rpki-monitor.antd.nist.gov/ROV/>`__.



|BGP|.3.4 Path Validation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


Route origin validation only tackles part of the problem with BGP
security. Even if the originating AS can be shown to be valid, what do
we know about the rest of the path? For example, if a malicious ISP
has a valid path to a certain prefix that traverses five ASes, but
chooses to falsely advertise that it can reach that prefix in two AS
hops, it is likely to attract traffic destined for that
prefix. Whatever the motive for such a step may be (e.g., to increase
revenue or to censor certain traffic, or even simple misconfiguration)
it clearly undermines the correct operation of Internet routing. The
solution to such attacks is to validate not just the originator of a
prefix but the entire path. It turns out this is a considerably harder
problem to solve than ROV.

There are a few different proposals for how to securely validate
paths. We focus here on the BGPsec standard from the IETF which
illustrates the overall approach and the challenges with achieving
widespread deployment.

In contrast to ROV, BGPsec path validation relies on cryptographic
operations being adopted as part of BGP itself. Each BGP speaker
(router) taking part in path validation signs its BGP announcements
using a private key specific to the router. The public key
corresponding to such a private key is included in a certificate that
is published in the RPKI. Such certificates also include the AS number
or numbers corresponding to the AS in which the router is located.

As in the discussion above, we need a chain of trust from a
trusted root. For example, an RIR could
provide the root of trust, and sign certificates for ISPs, who
could then act as the certificate authorities for their own routers. The
use of the word "could" in this paragraph reflects the lack of
real-world deployment of BGPsec.

With a certificate hierarchy in place, anyone receiving such a BGP
announcement can verify that it came from a router within the AS that it claims to
represent, and that it has not been modified in transit. The RPKI
enables the recipient to obtain the public key corresponding to the
announcing AS and thus validate the message.

The harder part of the problem is validating that the *contents* of
the message are correct from the perspective of BGP. Since a BGP
announcement is an ordered list of ASes, each of which has added
itself into the path to the destination, we need to validate that
every AS in the path has correctly announced a route to the
destination when it added itself into the path.

The way this is achieved is to have every router that adds an AS to the path sign its
announcement. We saw above that the RPKI could be used to create
bindings between public keys and entities authorized to advertise a
particular prefix. For path validation, we use the RPKI to create
bindings between public keys and Autonomous Systems.
With the RPKI in place, every router participating in BGPsec can be assumed
to have a well-known public key and matching private key.

Now consider the process of constructing a path to a particular
prefix. The path consists of a set of ASes. For example, a router in AS1, the origin AS, signs
an announcement that says it is the origin for the prefix, using its
private key. Furthermore, it includes the number of the target AS,
AS2, to which it is sending the announcement, in the set of fields
covered by the signature. Thus, we end up with a message that says
"AS1 can reach prefix P and has sent this information to AS2" signed
by a router from AS1.

A router in AS2 receives this announcement, and, having validated the
signature, it can now add itself to the path. The router in AS2 can now issue a
signed announcement that says "the path <AS2,AS1> leads to prefix P"
and sign this using its private key. It includes the full signed
message from AS1 as well as the new path. Again, before signing, it
includes the number of the target AS to which it is sending this
announcement. This announcement is received by AS3 which can now add
itself to the path and sign the result, and so on.

Including the target AS in the material that is signed is essential to
the correct operation of BGPsec. Suppose that, for example, AS3 tries
to lie about the path it has to AS1, claiming that the path <AS3,AS1>
is valid (skipping over AS2). It can't construct a valid message to
make this claim with the information that it received from
AS2, because AS2 is the target given by AS1. An
attempt to create a signed path <AS3,AS1> could be detected as
invalid, because the signed statement from AS1 indicates that
its target was AS2, not AS3.

Thus, when a valid signed announcement is received, the receiver is
able to validate that every AS in the chain to the destination has
received an announcement of the rest of the path to the
destination. While this still does not prove that the path to the
destination will actually be able to carry data, it does prove that a
set of announcements made their way along the stated path. It remains
a possibility that some AS might have advertised a path that it will
not honor—AS2, for example, might refuse (or be unable) to forward
traffic from AS3 to AS1 in spite of having advertised the path. A
particular concern is route leaks, in which misconfiguration causes an
AS to advertise a route by mistake, with no intention of attracting
traffic to that prefix. When such traffic arrives it might overwhelm
the resources of the AS that accidentally advertised the route,
causing traffic to be dropped.

Compared to ROV, the deployment story for path validation using BGPsec
is disappointing. We've only described one of several proposals to
cryptographically validate the paths advertised in BGP, but the sad
fact is that there is little to no deployment of any of them. There
are at least two challenges with path validation that contribute to
this situation. One is that it is relatively costly to start
performing cryptographic operations as part of BGP (in contrast to
ROV, where cryptographic operations happen separately from the
validation of BGP messages). The second is a "collective action
problem": when a single ISP pays the cost of implementing BGPsec, it
does little if anything to improve the situation for that ISP. Only
when a critical mass of ISPs are using BGPsec does it start to provide
significant incremental benefits over ROV. Issuing an ROA, by
contrast, immediately helps the provider who issues it. The situation
is captured in the paper "BGP Security in Partial Deployment". An
approach that holds promise to address both these issues is described
in the following section.


.. _reading_bgpsec:
.. admonition::  Further Reading

   R. Lychev, S. Goldberg and M. Schapira. `BGP Security
   in Partial Deployment: Is the Juice Worth the Squeeze?
   <https://dl.acm.org/doi/10.1145/2534169.2486010>`__.
   ACM SIGCOMM '13 Symposium, August 2013.

|BGP|.3.5 Provider Authorization
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

At the time of writing, there is an effort underway at the IETF to
standardize an approach to path validation known as ASPA (AS Provider
Authorization). The idea is to use a new set of objects in the RPKI to
capture the relationships among ASes, and then use that information to
check the validity of BGP advertisements as they are received.

ASPA shares an attractive property with ROV: no cryptographic
operations are added to BGP itself. Just as ROV builds a database (in
the RPKI) of who is allowed to originate an advertisement, ASPA builds
a database showing which ASes provide transit to other ASes. This,
too, uses the RPKI, but with different types of certificates, known as
ASPA records.

An important ingredient in ASPA is the insight that the relationships
between ASes can be placed into a small set of categories, as we
observed in the previous section. First, if there is
no BGP connection between a pair of ASes, they have no relationship—and
hence we should never see this pair of ASes next to each other in an
advertised path. For any pair of ASes that do interconnect, the
relationship can normally be classified as customer-to-provider, or
peer-to-peer.  A customer depends on a provider to deliver traffic to
and from their AS, and that means that it is expected that the
provider's AS number will appear in routing advertisements to reach
the customer AS. Customer ASes, on the other hand, only deliver
traffic to their provider ASes if it originates in the customer AS itself or
comes from the customer's customers.

The relationship between customers and providers is normally captured
visually as "valley-free" routing. Routing advertisements flow "up" from customers
to providers, then (optionally) across between peers, then down from
providers to customers, as depicted in :numref:`Figure %s
<fig-valleyfree>`. In this figure, customer ASes are depicted below
their provider AS, while the two ASes at the top have a peer-to-peer
relationship. Valley-free routes have the property that they never
start to go down (towards customers) and then head up again towards
providers. The appearance of a valley is a strong indication of a
route leak. A database that establishes the customer-to-provider
relationships gives us the ability to detect such anomalies.

.. _fig-valleyfree:
.. figure:: policy/figures/valleyfree.png
   :width: 300px
   :align: center

   Valley-free topology of Autonomous Systems

Suppose that two ASes, X and Y, publish a list of their providers
using ASPA objects in the RPKI. Let's say that there is an ASPA object
from AS Y that asserts AS X is one of its providers, as well as an ASPA object
from AS X that does not include AS Y among its providers. If a router
receives an advertisement in which Y appears to be a provider for X,
this is clearly wrong and the router drops the advertisement. The
question of how we can tell that a particular AS is a provider,
customer, or peer of another AS is a bit subtle, but it depends on the
properties of valley-free routing. We can't have an arbitrary mix of
customer-provider and provider-customer links in a valid path; there
must be a set of paths going "up" towards providers followed by at
most one lateral path followed by a set of paths going "down" towards
customers. The more relationships that are placed in the RPKI, the more
power a BGP speaker gains to detect paths that are invalid.

ASPA adds further security to the routing system beyond that offered
by ROV, because it catches attacks where an AS announces routes where
the origin AS is correct (i.e., covered by a valid ROA) but the path is not
legitimate. Such attacks are known as forged-origin prefix hijacks.

Furthermore, ASPA catches some routing problems (such as accidental
leakage of routes) that are not caught by BGPsec. This is because
BGPsec shows that ASes are connected to each other but does not
capture the customer-provider relationships. ASPA also provides
benefits even if it is only partially deployed on a path, as the above
example illustrates. In other words, it is amenable to incremental
deployment.

.. _reading_aspa:
.. admonition::  Further Reading

   A, Azimov et al. `BGP AS_PATH Verification Based on
   Autonomous System Provider Authorization (ASPA) Objects
   <https://www.ietf.org/archive/id/draft-ietf-sidrops-aspa-verification-18.html>`__.
   Internet Draft, July 2024.


