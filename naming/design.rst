|Naming|.1 Design Issues
---------------------------------

.. TODO -- As currently written, this section introduces a set of
   concepts, but only weakly links them to "requirements". It probably
   could be rewritten to be more requirement-centric.

A naming system maintains a collection of *bindings* of names to
values. The value can be anything we want the naming system to return
when presented with a name; it might be an address, or another
name. Each name is said to be drawn from a *name space* that
establishes the rules for what constitutes a legitimate identifier.
These rules—often included as part of a protocol spec—establish the
syntax for names, what types of value(s) each name may be bound to,
how new name/value pairs are added to the set of bindings, various
properties those bindings should preserve, and the process by which
names are resolved and the corresponding value returned.

The best way to understand the "rules" that define a name or address
space is by example. Fortunately, we have seen several examples
throughout this book, most of which are probably familiar. This
collection is diverse enough to illustrate the set of requirements
that may be established for a particular naming scheme.

  * Domain name: ``cicada.cs.princeton.edu``
  * IP address: ``128.112.155.72``
  * Ethernet address: ``40:4d:7f:b5:62:b0``
  * URL: ``http://www.cs.princeton.edu/index.html``
  * URN: ``urn:doi:10.17487/RFC0791``
  * Email address: ``alice@systemsapproach.org``

Starting with Shoch's definition, domain names, IP addresses, and
Ethernet addresses are appropriately named: the first is a
human-readable string used to identify what host you want to connect
to and the latter two are 32-bit and 48-bit variables, respectively,
that routers and switches use to locate (forward packets towards) that
host. That we also have a readable representation for the two
addresses (e.g, dot notation for IP) is a convenience; it does not
play a role in how switches process addresses.

Dealing with scale is an important consideration for any Internet
naming system, and as we have seen in other situations, a hierarchy
can be introduced to improve scalability. Consider the specific
problem of how names and addresses are assigned to objects. Domain
names come from a *hierarchical* name space, where an
Internet-sanctioned registrar is responsible for the top-level domains
(.e.g., ``.edu``, ``.com``), and local administrators assign names to
the hosts they manage. The IP address space is also hierarchical, with
an Internet-sanctioned authority assigning prefixes to organizations
(e.g., ``128.112.0.0/16``), which in turn assign addresses to their
hosts. As we saw in Chapters |Routing| and |BGP|, this hierarchy
encodes topology information into the address, and that information is
then used to determine the route.

.. TODO -- Identify the "Internet-sanctioned authority". Right
   terminology for Ethernet too.

In contrast, Ethernet addresses are drawn from a *flat* address space,
in the sense that there is no topologically meaning information
embedded in them. When you resolve a MAC address, you use the full 48
bits as a lookup key. But MAC addresses are allocated to network
adaptors using a two-layer process: a unique prefix is assigned to
each vendor (e.g., ``40:4d:7f`` is one of the prefixes assigned to
Apple), and the vendor then assigns a unique 48-bit address to each of
the devices it manufactures and ships using that prefix.

One might conclude that hierarchical name and address assignment
insures that all identifiers are *unique*, so that any entity that
tries to resolves a given identifier will get back the same value.
This is true for domain names and Ethernet addresses—for which global
uniqueness is a requirement—but not for IP addresses. This is because
virtual networks like the ones described in Chapter |Virt| create a
situation in which IP addresses are interpreted in the context of a
private network rather than the public Internet. By looking at private
networks through the lens of naming, IP addresses sometimes
recursively resolve into other IP addresses instead of into
lower-level Ethernet addresses. IP addresses are unique within some
context, but they are not universally unique. By convention, private
IP addresses typically use certain prefixes, such as
``192.168.0.0/16``, so as to imply a local context. The more general
takeaway is that names are always resolved in some context. The
context is usually implied (e.g., the context is "the Internet"). In
other cases, such as URIs, the context is explicitly identified in the
name.

.. sidebar:: Through the Looking-Glass

   *The Shoch paper cited in the introduction to this chapter motivates
   the naming problem with a story from Lewis Carroll's novel "Through
   the Looking Glass", in which the White Knight explains the
   important differences between what a song* **is**, *what it* **is
   called**, *what it* **is named**, *and what* **the name is called**.
   *Carroll was a pen name for Oxford Mathematitian Charles Dodgson,
   and so he surely understood the nuances of that statement. For our
   purposes, looking at protocols and layers of abstration through the
   "naming lens" is lesson in being precise about the language that
   we use use to talk about (and think about) networked systems.*

   *For example, this book has two chapters focused on routing, so it
   may seem strange to suddenly start talking about routing as part of
   the naming problem. But if you start from the perspective that all
   systems are just a combination of objects, references to objects,
   and bindings from one object reference to another, then you can see
   a route as a sequence of "link objects; those link objects also
   have to be named and routing tables are filled with bindings
   necessary to discover them. This perspective proves more important
   as we build layer upon layer of abstract (virtualized) objects. In
   other words, everything is a naming problem and context (scope) is
   important.*

Looking more closely at uniqueness, we have to be careful about how we
interpret "get back the same value". A name is considered unique if it
always resolves in the same way, independent of who asks, but the
returned value may be multi-faceted. One common example is that a name
resolves to an equivalent set of values; that is, the name space
supports one-to-many bindings. For example, resolving a domain name
might return a set of IP addresses, any one of which will work.
Another common scenario is that the values are typed; that is,
associated with different kinds of objects. Domain names support such
a feature, which is an idea borrowed from Unix, where "everything is a
file" means file names can be used to name all sorts of resources; for
example, files (``/user/llp/file.txt``), directories
(``/user/llp/tmp``), devices (``/dev/disk0``), or the system's host
name (``/proc/sys/kernel/hostname``). The next section describes how
domain names achieve a similar goal.

Another question to ask about a naming scheme is whether name-to-value
bindings are permanent, or if they are allowed to change over time.
It's not uncommon to buy a new server and reassign an old (previously
used) name or address to it, but that can be problematic for some
objects.  Imagine if the object that changes is a digital document,
for example, a legal contract. Such an object might be immutable,
making it a requirement that its name change if the object changes;
the original name is permanently bound to the original document.
There can always be multiple *aliases* for an object—known as a
many-to-one binding—but at least one name is designated as
permanent. The URN included in the list of names at the beginning of
this section is an example of such a name.

Finally, we have been referring to name resolution as an abstract
process, but it is implemented by a program. The resolver that maps IP
addresses into Ethernet addresses is implemented by ARP, as we saw in
Chapter |Fed|.2.5. The resolution process for domain names is
described in the next section, where as we'll see, the name resolver
is a network application, just like those described in Chapter |Apps|.
The domain name resolver is used by all other applications, and those
applications in turn, have their own naming systems. For example, URLs
and Email addresses listed at the beginning of this section are
application-level names.

:numref:`Figure %s <fig-names>` shows what happens when a user
presents such a name to an application. The app engages the domain
naming system, denoted DNS, to translate this name into a host
address. The application then opens a connection to this host by
presenting some transport protocol (e.g., TCP) with the host’s IP
address. IP, in turn, uses ARP to determine the corresponding Ethernet
address, keeping in mind that this is for only the first hop along the
end-to-end path. The next section details how DNS works. The following
section then takes a closer look at application-level names.

.. _fig-names:
.. figure:: naming/figures/layers.png
   :width: 400px
   :align: center

   Names translated into addresses at multiple layers of the protocol
   stack.  DNS and ARP are resolvers that may initiate their own
   message exchange (not shown) to determine the right value to
   return.
