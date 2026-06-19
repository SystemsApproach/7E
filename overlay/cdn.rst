|Overlay|.2 Content Distribution Networks
-----------------------------------------------

When the World Wide Web started to emerge as a dominant application of
the Internet in the late 1990s, there were plenty of bottlenecks
around the Internet ranging from slow access links (including dialup
modems), congested backbone links, and routers that operated at a
fraction of the bandwidth of today's devices. On top of that, a web
site might suddenly become popular enough that the server hosting the
site would be overwhelmed with requests for content.  The first
content distribution networks (CDNs) started to appear around this
time to ensure that web content could be delivered efficiently in
spite of these bottlenecks. Even though everything from server
hardware to internet access links is dramatically faster today, CDNs
have become an essential feature of the Internet.

When it comes to downloading Web pages there
are four potential bottlenecks in the system:

-  *The first mile.* The Internet may have high-capacity links in it,
   but that doesn’t help you download a Web page any faster when you’re
   connected over a low-speed (or poorly performing) access link.

-  *The last mile.* The link that connects the server to the Internet
   can be overloaded by too many requests, even if the aggregate
   bandwidth of that link is quite high.

-  *The server itself.* A server has a finite amount of resources (CPU,
   memory, disk bandwidth, etc.) and can be overloaded by too many
   concurrent requests.

-  *Peering points.* The handful of ISPs that collectively implement the
   backbone of the Internet may internally have high-bandwidth pipes,
   but they do not always provide high-capacity connectivity
   to their peers. If you are connected to ISP A and the server is
   connected to ISP B, then the page you request may get dropped at the
   point where A and B peer with each other.

There’s not a lot anyone except you or your local service provider can
do about the first-mile problem, but it is possible to use content
replication to address the remaining problems.  Content distribution
networks are the systems that manage the process of replicating
content and delivering it to clients, which has a few more moving
parts than first meet the eye. Akamai was one of the first operators
of a CDN and today there are a number of large CDN operators with
global footprints.

The idea of a CDN is to geographically distribute a collection of
*server surrogates* that cache pages normally maintained in some set
of *backend servers*. Thus, rather than having millions of users wait
forever to contact when a big news story breaks—such a situation is
known as a *flash crowd*—it is possible to spread this load across
many servers. Moreover, rather than having to traverse multiple ISPs
to reach ``www.cnn.com``, if these surrogate servers happen to be
spread across all the backbone ISPs, then it should be possible to
reach one without having to cross a peering point. Clearly,
maintaining thousands of surrogate servers all over the Internet is
too expensive for any one site that wants to provide better access to
its web pages. Commercial CDNs provide this service for many sites,
thereby amortizing the cost across many customers.

Although we call them surrogate servers, in fact, they can just as
correctly be viewed as caches. If they don’t have a page that has been
requested by a client, they ask the backend server for it. In practice,
however, the backend servers proactively replicate their data across the
surrogates rather than wait for surrogates to request it on demand. It’s
also the case that only static pages, as opposed to dynamic content, are
distributed across the surrogates. Clients have to go to the backend
server for any content that either changes frequently (e.g., sports
scores and stock quotes) or is produced as the result of some
computation (e.g., a database query).

.. TODO -- need to check - can't code run in modern CDN nodes?

.. _fig-cdn:
.. figure:: overlay/figures/f09-30-9780123850591.png
   :width: 600px
   :align: center

   Components in a Content Distribution Network (CDN).

Having a large set of geographically distributed servers does not fully
solve the problem. To complete the picture, CDNs also need to provide a
set of *redirectors* that forward client requests to the most
appropriate server, as shown in :numref:`Figure %s <fig-cdn>`. The primary
objective of the redirectors is to select the server for each request
that results in the best *response time* for the client. A secondary
objective is for the system as a whole to process as many requests per
second as the underlying hardware (network links and web servers) is
able to support. The average number of requests that can be satisfied in
a given time period—known as the *system throughput*—is primarily an
issue when the system is under heavy load, such as when a flash crowd is
accessing a small set of pages or a Distributed Denial of Service (DDoS)
attacker is targeting a particular site.

CDNs use several factors to decide how to distribute client requests.
For example, to minimize response time, a redirector might select a
server based on its *network proximity*. In contrast, to improve the
overall system throughput, it is desirable to evenly *balance* the load
across a set of servers. Both throughput and response time are improved
if the distribution mechanism takes *locality* into consideration; that
is, it selects a server that is likely to already have the page being
requested in its cache. The exact combination of factors that should be
employed by a CDN is open to debate. This section considers some of the
possibilities.

|Overlay|.2.1 Mechanisms
~~~~~~~~~~~~~~~~~~~~~~~~

As described so far, a redirector is just an abstract function, although
it sounds like what something a router might be asked to do since it
logically forwards a request message much like a router forwards
packets. In fact, there are several mechanisms that can be used to
implement redirection. Note that for the purpose of this discussion we
assume that each redirector knows the address of every available server.
(From here on, we drop the “surrogate” qualifier and talk simply in
terms of a set of servers.) In practice, some form of out-of-band
communication takes place to keep this information up-to-date as servers
come and go.

First, redirection could be implemented by augmenting DNS to return
different server addresses to clients. For example, when a client asks
to resolve the name ``www.cnn.com``, the DNS server could return the
IP address of a server hosting CNN’s Web pages that is known to have
the lightest load.  Alternatively, for a given set of servers, it
might just return addresses in a round-robin fashion. Note that the
granularity of DNS-based redirection is usually at the level of a site
(e.g., ``cnn.com``) rather than a specific URL within that site. However,
when returning an embedded link, the server can rewrite the URL,
thereby effectively pointing the client at the most appropriate server
for that specific object.

Commercial CDNs essentially use a combination of URL rewriting and
DNS-based redirection. For scalability reasons, the high-level DNS
server first points to a regional-level DNS server, which replies with
the actual server address. In order to respond to changes quickly, the
DNS servers tweak the TTL of the resource records they return to a very
short period, such as 20 seconds. This is necessary so clients don’t
cache results; instead they go back to the DNS server for the most
recent URL-to-server mapping.

Another possibility is to use the HTTP redirect feature: The client
sends a request message to a server, which responds with a new (better)
server that the client should contact for the page. Unfortunately,
server-based redirection incurs an additional round-trip time across the
Internet, and, even worse, servers can be vulnerable to being overloaded
by the redirection task itself. Instead, if there is a node close to the
client (e.g., a local Web proxy) that is aware of the available servers,
then it can intercept the request message and instruct the client to
instead request the page from an appropriate server. In this case,
either the redirector would need to be on a choke point so that all
requests leaving the site pass through it, or the client would have to
cooperate by explicitly addressing the proxy (as with a classical,
rather than transparent, proxy).

.. sidebar:: Are CDNs Overlays?

   Many of the early overlays built on top of the Internet use some
   sort of tunneling to create virtual point-to-point links, and
   created a virtual topology between the overlay nodes to offer some
   function not yet implemented in the Internet, such as multicast of
   IPv6 support. CDNs don't quite conform to this model, since they
   don't generally build tunnels between the CDN nodes. We would
   argue, however, that they have enough in common with other types of
   overlay to qualify. They offer functionality not natively provided
   by the Internet—caching—while using the Internet to interconnect
   the nodes in the CDN. A redirector makes an application-level
   routing decision, much like other types of overlay nodes.  Rather
   than forward a packet based on an address and its knowledge of the
   network topology, it forwards HTTP requests based on a URL and its
   knowledge of the location and load of a set of servers.  The
   complete collection of redirectors and surrogate servers that make
   up a CDN are effectively an application-specific network that
   leverages the underlying connectivity of the Internet bring
   additional functionality to the Internet: efficient delivery of
   content to clients.


|Overlay|.2.2 Policies
~~~~~~~~~~~~~~~~~~~~~~~

We now consider some example policies that redirectors might use to
forward requests. Actually, we have already suggested one simple
policy: round-robin. A similar scheme would be to simply select one of
the available servers at random. Both of these approaches do a good job
of spreading the load evenly across the CDN, but they do not do a
particularly good job of lowering the client-perceived response time.

It’s obvious that neither of these two schemes takes network proximity
into consideration, but, just as importantly, they also ignore locality.
That is, requests for the same URL are forwarded to different servers,
making it less likely that the page will be served from the selected
server’s in-memory cache. This forces the server to retrieve the page
from its disk, or possibly even from the backend server. How can a
distributed set of redirectors cause requests for the same page to go to
the same server (or small set of servers) without global coordination?
The answer is surprisingly simple: all redirectors use some form of
hashing to deterministically map URLs into a small range of values. The
primary benefit of this approach is that no inter-redirector
communication is required to achieve coordinated operation; no matter
which redirector receives a URL, the hashing process produces the same
output.

So what makes for a good hashing scheme? The classic *modulo* hashing
scheme—which hashes each URL modulo the number of servers—is not
suitable for this environment. This is because should the number of
servers change, the modulo calculation will result in a diminishing
fraction of the pages keeping their same server assignments. While we do
not expect frequent changes in the set of servers, the fact that the
addition of new servers into the set will cause massive reassignment is
undesirable. An alternative is to use the a *consistent hashing*
algorithm.


.. _fig-unitcircle:
.. figure:: overlay/figures/f09-25-9780123850591.png
   :width: 300px
   :align: center

   Both nodes and objects map (hash) onto the ID space, where objects are
   maintained at the nearest node in this space.

Consistent hashing hashes a set of objects
*x* uniformly across a large ID space. :numref:`Figure %s <fig-unitcircle>`
visualizes a 128-bit ID space as a circle, where we use the algorithm to
place both objects

.. centered:: *hash(ObjectName) → ObjectID*

and nodes

.. centered:: *hash(IPAddr) → NodeID*

onto this circle. Since a 128-bit ID space is enormous, it is unlikely
that an object will hash to exactly the same ID as a server’s IP
address hashes to. To account for this unlikelihood, each object is
maintained on the node whose ID is *closest*, in this 128-bit space, to
the object ID. In other words, the idea is to use a high-quality hash
function to map both nodes and objects into the same large, sparse ID
space; you then map objects to nodes by numerical proximity of their
respective identifiers. Like ordinary hashing, this distributes objects
fairly evenly across nodes, but, unlike ordinary hashing, only a small
number of objects have to move when a node (hash bucket) joins or
leaves.


So in the CDN, each redirector first hashes every server into the unit
circle. Then, for each URL that arrives, the redirector also hashes
the URL to a value on the unit circle, and the URL is assigned to the
server that lies closest on the circle to its hash value. If a node
fails in this scheme, its load shifts to its neighbors (on the unit
circle), so the addition or removal of a server only causes local
changes in request assignments. Each redirector knows how the set of
servers map onto the unit circle, so they can each, independently,
select the “nearest” one.

This strategy can easily be extended to take server load into account.
Assume the redirector knows the current load of each of the available
servers. This information may not be perfectly up-to-date, but we can
imagine the redirector simply counting how many times it has forwarded a
request to each server in the last few seconds and using this count as
an estimate of that server’s current load. Upon receiving a URL, the
redirector hashes the URL plus each of the available servers and sorts
the resulting values. This sorted list effectively defines the order in
which the redirector will consider the available servers. The redirector
then walks down this list until it finds a server whose load is below
some threshold. The benefit of this approach compared to plain
consistent hashing is that server order is different for each URL, so if
one server fails, its load is distributed evenly among the other
machines. This approach is the basis for the Cache Array Routing
Protocol (CARP) and is shown in pseudocode below.

::

   SelectServer(URL, S)
       for each server s in server set S
           score[s] = hash(URL, address[s])
       sort score
       for each server s in decreasing order of score
           if Load(s) < threshold then
               return s
       return server with highest score

As the load increases, this scheme changes from using only the first
server on the sorted list to spreading requests across several servers.
Some pages normally handled by busy servers will also start being
handled by less busy servers. Since this process is based on aggregate
server load rather than the popularity of individual pages, servers
hosting some popular pages may find more servers sharing their load than
servers hosting collectively unpopular pages. In the process, some
unpopular pages will be replicated in the system simply because they
happen to be primarily hosted on busy servers. At the same time, if some
pages become extremely popular, it is conceivable that all of the
servers in the system could be responsible for serving them.

Finally, it is possible to introduce network proximity into the
equation in at least two different ways. The first is to blur the
distinction between server load and network proximity by monitoring
how long a server takes to respond to requests and using this
measurement as the “server load” parameter in the preceding
algorithm. This strategy tends to prefer nearby/lightly loaded servers
over distant/heavily loaded servers. A second approach is to factor
proximity into the decision at an earlier stage by limiting the
candidate set of servers considered by the above algorithms (*S*) to
only those that are nearby. The harder problem is deciding which of
the potentially many servers are suitably close. One approach would be
to select only those servers that are available on the same ISP as the
client. A slightly more sophisticated approach would be to look at the
map of autonomous systems produced by BGP and select only those
servers within some number of hops from the client as candidate
servers.  Finding the right balance between network proximity and
server load has been the subject of considerable research and we
assume that the CDN operators continue to fine-tune their algorithms.
