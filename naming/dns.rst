|Naming|.2 Domain Naming System
-------------------------------------------

Scalability is the foremost design issue addressed by the Internet's
Domain Naming System (DNS). Everything else about DNS can be traced to
other naming systems, most notably, Unix file names. Interestingly,
the Internet did not always use DNS. Early in its history, when there
were only a few hundred hosts, a central authority called the *Network
Information Center* (NIC) maintained a flat table of name-to-address
bindings; this table was called ``HOSTS.TXT``.\ [#]_ Whenever a site
wanted to add a new host to the Internet, the site administrator sent
email to the NIC giving the new host’s name/address pair. This
information was manually entered into the table, the modified table
was mailed out to the various sites every few days, and the system
administrator at each site installed the table on every host at the
site. Name resolution was then simply implemented by a procedure that
looked up a host’s name in the local copy of the table and returned
the corresponding address.

.. [#] Believe it or not, there was also a paper book (like a phone
       book) published periodically that listed all the machines
       connected to the Internet and all the people that had an
       Internet email account.

It should come as no surprise that the approach to naming did not work
well as the number of hosts in the Internet started to
grow. Therefore, in the mid-1980s, the Domain Naming System was put
into place. DNS employs a hierarchical name space rather than a flat
name space, and the “table” of bindings that implements this name
space is partitioned into disjoint pieces and distributed throughout
the Internet. These subtables are made available in *name servers*,
which are application processes that can be queried over the network.
The rest of this section outlines the details.

|Naming|.2.1 Domain Names
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

DNS implements a hierarchical name space for Internet objects. Unlike
Unix file names, which are processed from left to right with the naming
components separated with slashes, DNS names are processed from right to
left and use periods as the separator.\ [#]_  An
example domain name for a host is ``cicada.cs.princeton.edu``. Notice
that we said domain names are used to name Internet “objects.” What we
mean by this is that DNS is not strictly used to map host names into
host addresses. It is more accurate to say that DNS maps domain names
into values. For the time being, we assume that these values are IP
addresses; we will come back to this issue later in this section.

.. [#] While machines process domain names from right to left, humans
       read them as text strings, which means the direction of
       processing for humans depends on the language and character set. While
       originally the DNS only allowed for Latin characters, and would
       always be read left to right by humans, today it supports a
       wide range of character sets and languages that are read right
       to left.

.. _fig-domains:
.. figure:: naming/figures/f09-15-9780123850591.png
   :width: 700px
   :align: center

   Example of a domain hierarchy.

Like the Unix file hierarchy, the DNS hierarchy can be visualized as a
tree, where each node in the tree corresponds to a domain, and the
leaves in the tree correspond to the hosts being named.
:numref:`Figure %s <fig-domains>` gives an example of a domain
hierarchy. Note that we should not assign any semantics to the term
*domain* other than that it is simply a context in which additional
names can be defined. Confusingly, the word *domain* is also used in
Internet routing, where it means something different than it does in
DNS, being roughly equivalent to the term *autonomous system*.

There was actually a substantial amount of discussion that took place
when the domain name hierarchy was first being developed as to what
conventions would govern the names that were to be handed out near the
top of the hierarchy. Without going into that discussion in any detail,
the hierarchy in the early days was not very wide at the first level. There are
domains for each country, plus the “big six” domains: ``.edu``,
``.com``, ``.gov``, ``.mil``, ``.org``, and ``.net``. These six domains
were all originally based in the United States (where the Internet and
DNS were invented); for example, only U.S.-accredited educational
institutions can register an ``.edu`` domain name. In recent years, the
number of top-level domains has been expanded, partly to deal with the
high demand for ``.com`` domains names. The newer top-level domains
include ``.biz``, ``.coop``, and ``.systems``. There are over 1400
top-level domains at the time of writing.

|Naming|.2.2 Name Servers
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The complete domain name hierarchy exists only in the abstract. We now
turn our attention to the question of how this hierarchy is
implemented. The first step is to partition the hierarchy into
subtrees called *zones*. :numref:`Figure %s <fig-zones>` shows how the
hierarchy given in :numref:`Figure %s <fig-domains>` might be divided
into zones. Each zone can be thought of as corresponding to some
administrative authority that is responsible for that portion of the
hierarchy. For example, the top level of the hierarchy forms a zone
that is managed by the Internet Corporation for Assigned Names and
Numbers (ICANN). Below this is a zone that corresponds to Princeton
University. Within this zone, some departments do not want the
responsibility of managing the hierarchy (and so they remain in the
university-level zone), while others, like the Department of Computer
Science, manage their own department-level zone.

.. _fig-zones:
.. figure:: naming/figures/f09-16-9780123850591.png
   :width: 700px
   :align: center

   Domain hierarchy partitioned into zones.

The relevance of a zone is that it corresponds to the fundamental unit
of implementation in DNS—the name server. Specifically, the information
contained in each zone is implemented in two or more name servers. Each
name server, in turn, is a program that can be accessed over the
Internet. Clients send queries to name servers, and name servers respond
with the requested information. Sometimes the response contains the
final answer that the client wants, and sometimes the response contains
a pointer to another server that the client should query next. Thus,
from an implementation perspective, it is more accurate to think of DNS
as being represented by a hierarchy of name servers rather than by a
hierarchy of domains, as illustrated in :numref:`Figure %s <fig-servers>`.

.. _fig-servers:
.. figure:: naming/figures/f09-17-9780123850591.png
   :width: 500px
   :align: center

   Hierarchy of name servers.

Note that each zone is implemented in two or more name servers for the
sake of redundancy; that is, the information is still available even
if one name server fails. On the flip side, a given name server is
free to implement more than one zone. Commercial companies like
GoDaddy, Cloudflare, and Porkbun (among many others) take advantage of
this feature to run name servers on behalf of people and organizations
that do not want to host their own servers.

Each name server implements the zone information as a collection of
*resource records*. In essence, a resource record is a name-to-value
binding or, more specifically, a 5-tuple that contains the following
fields:

::

   (Name, Value, Type, Class, TTL)

The ``Name`` and ``Value`` fields are exactly what you would expect,
while the ``Type`` field specifies how the ``Value`` should be
interpreted. For example, ``Type=A`` indicates that the ``Value`` is
an IP address.  Thus, ``A`` records implement the name-to-address
mapping we have been assuming. Other record types include:

-  ``NS``—The ``Value`` field gives the domain name for a host that is
   running a name server that knows how to resolve names within the
   specified domain.

-  ``CNAME``—The ``Value`` field gives the canonical name for a
   particular host; it is used to define aliases.

-  ``MX``—The ``Value`` field gives the domain name for a host that is
   running a mail server that accepts messages for the specified domain.

The ``Class`` field was included to allow entities other than the NIC to
define useful record types. To date, the only widely used ``Class`` is
the one used by the Internet; it is denoted ``IN``. Finally, the
time-to-live (``TTL``) field shows how long this resource record is
valid. It is used by servers that cache resource records from other
servers; when the ``TTL`` expires, the server must evict the record from
its cache.

To better understand how resource records represent the information in
the domain hierarchy, consider the following examples drawn from the
domain hierarchy given in :numref:`Figure %s <fig-domains>`. To
simplify the example, we ignore the ``TTL`` field and we give the
relevant information for only one of the name servers that implement
each zone.

First, a root name server contains an ``NS`` record for each top-level
domain (TLD) name server. This identifies a server that can resolve
queries for this part of the DNS hierarchy (``.edu`` and ``.com``\ in
this example). It also has ``A`` records that translates these names
into the corresponding IP addresses. Taken together, these two records
effectively implement a pointer from the root name server to one of the
TLD servers.

::

   (edu, a3.nstld.com, NS, IN)
   (a3.nstld.com, 192.5.6.32, A, IN)
   (com, a.gtld-servers.net, NS, IN)
   (a.gtld-servers.net, 192.5.6.30, A, IN)
   ...

Moving our way down the hierarchy by one level, the server has records
for domains like this:

::

   (princeton.edu, dns.princeton.edu, NS, IN)
   (dns.princeton.edu, 128.112.129.15, A, IN)
   ...

In this case, we get an ``NS`` record and an ``A`` record for the name
server that is responsible for the ``princeton.edu`` part of the
hierarchy. That server might be able to directly resolve some queries
(e.g., for\ ``email.princeton.edu``) while it would redirect others to a
server at yet another layer in the hierarchy (e.g., for a query about
``penguins.cs.princeton.edu``).

::

   (email.princeton.edu, 128.112.198.35, A, IN)
   (penguins.cs.princeton.edu, dns1.cs.princeton.edu, NS, IN)
   (dns1.cs.princeton.edu, 128.112.136.10, A, IN)
   ...

Finally, a third-level name server, such as the one managed by domain
``cs.princeton.edu``, contains ``A`` records for all of its hosts. It
might also define a set of aliases (``CNAME`` records) for each of those
hosts. Aliases are sometimes just convenient (e.g., shorter) names for
machines, but they can also be used to provide a level of indirection.
For example,\ ``www.cs.princeton.edu`` is an alias for the host named
``coreweb.cs.princeton.edu``.This allows the site’s web server to move
to another machine without affecting remote users; they simply continue
to use the alias without regard for what machine currently runs the
domain’s web server. The mail exchange (``MX``) records serve the same
purpose for the email application—they allow an administrator to change
which host receives mail on behalf of the domain without having to
change everyone’s email address.

::

   (penguins.cs.princeton.edu, 128.112.155.166, A, IN)
   (www.cs.princeton.edu, coreweb.cs.princeton.edu, CNAME, IN)
   (coreweb.cs.princeton.edu, 128.112.136.35, A, IN)
   (cs.princeton.edu, mail.cs.princeton.edu, MX, IN)
   (mail.cs.princeton.edu, 128.112.136.72, A, IN)
   ...

Note that, although resource records can be defined for virtually any
type of object, DNS is typically used to name hosts (including servers)
and sites. It is not used to name individual people or other objects
like files or directories; other naming systems are typically used to
identify such objects. We'll see some examples in the next section.


|Naming|.2.3 Name Resolution
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Given a hierarchy of name servers, we now consider the issue of how a
client engages these servers to resolve a domain name. To illustrate the
basic idea, suppose the client wants to resolve the name
``penguins.cs.princeton.edu`` relative to the set of servers given in
the previous subsection. The client could first send a query containing
this name to one of the root servers (as we’ll see below, this rarely
happens in practice but will suffice to illustrate the basic operation
for now). The root server, unable to match the entire name, returns the
best match it has—the ``NS`` record for ``edu`` which points to the TLD
server ``a3.nstld.com``. The server also returns all records that are
related to this record, in this case, the ``A`` record for
``a3.nstld.com``. The client, having not received the answer it was
after, next sends the same query to the name server at IP host
``192.5.6.32``. This server also cannot match the whole name and so
returns the ``NS`` and corresponding ``A`` records for the
``princeton.edu`` domain. Once again, the client sends the same query as
before to the server at IP host ``128.112.129.15``, and this time gets
back the ``NS`` record and corresponding ``A`` record for the
``cs.princeton.edu`` domain. This time, the server that can fully
resolve the query has been reached. A final query to the server at
``128.112.136.10`` yields the ``A`` record for
``penguins.cs.princeton.edu``, and the client learns that the
corresponding IP address is ``128.112.155.166``.

This example still leaves a couple of questions about the resolution
process unanswered. The first question is how did the client locate the
root server in the first place, or, put another way, how do you resolve
the name of the server that knows how to resolve names? This is a
fundamental problem in any naming system, and the answer is that the
system has to be bootstrapped in some way. In this case, the
name-to-address mapping for one or more root servers is well known; that
is, it is published through some means outside the naming system itself.

In practice, however, not all clients know about the root servers.
Instead, the client program running on each Internet host is initialized
with the address of a *local* name server. For example, all the hosts in
the Department of Computer Science at Princeton know about the server on
``dns1.cs.princeton.edu``. This local name server, in turn, has resource
records for one or more of the root servers, for example:

::

   ('root', a.root-servers.net, NS, IN)
   (a.root-servers.net, 198.41.0.4, A, IN)

Thus, resolving a name actually involves a client querying the local
server, which in turn acts as a client that queries the remote servers
on the original client’s behalf. This results in the client/server
interactions illustrated in :numref:`Figure %s <fig-resolution>`. One
advantage of this model is that all the hosts in the Internet do not
have to be kept up-to-date on where the current root servers are
located; only the servers have to know about the root. A second
advantage is that the local server gets to see the answers that come
back from queries that are posted by all the local clients. The local
server *caches* these responses and is sometimes able to resolve
future queries without having to go out over the network. The ``TTL``
field in the resource records returned by remote servers indicates how
long each record can be safely cached. This caching mechanism can be
used further up the hierarchy as well, reducing the load on the root
and TLD servers.

The second question is how the system works when a user submits a
partial name (e.g., ``penguins``) rather than a complete domain name
(e.g., ``penguins.cs.princeton.edu``). The answer is that the client
program is configured with the local domain in which the host resides
(e.g., ``cs.princeton.edu``), and it appends this string to any simple
names before sending out a query.

.. _fig-resolution:
.. figure:: naming/figures/f09-18-9780123850591.png
   :width: 600px
   :align: center

   Name resolution in practice, where the numbers 1 to 10 show the sequence
   of steps in the process.


.. TODO -- Somewhere in this section we might mention where the root
   servers are. We might also want to cover DNS over HTTP, plus other
   security-related topics.
