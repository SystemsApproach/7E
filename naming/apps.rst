|Naming|.3 Application Naming Schemes
--------------------------------------------

DNS is a general-purpose naming system—it both scales and supports
typed values. In principle, it could be used to name any Internet
resource, including those that application programs define and
manage. But applications have adopted their own naming schemes, the
most notable of which is the Uniform Resource Identifiers (URIs)
introduced in Chapter |Apps|.  This section describes three such
naming systems, with a focus on what value they provide beyond DNS.

|Naming|.3.1 Federated Names
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In contrast to DNS, which provides a self-contained naming system,
URIs define a federated name space that encompasses one or more
existing name spaces. The specification for URIs, defined in RFC 3986,
includes a *syntax* for identifiers and an extensible process for
resolving those identifiers. The process is extensible in the sense
that it takes advantage of resolution mechanisms defined by other name
spaces, including DNS.

.. admonition:: Further Reading

   T. Berners-Lee, R. Fielding, and L. Masinter. `Uniform Resource
   Identifier (URI): Generic Syntax
   <https://www.rfc-editor.org/info/rfc3986>`__. RFC 3986,
   January 2005.

Each URI begins with a *scheme* that prescribes how the rest of the
URI is to be interpreted. Each scheme then specifies its own syntax
for the rest of the URI, with the requirement that all URIs must be of
the form:\ [#]_

      scheme ":" hierarchical-part [ "?" query ]

The square brackets "[ ]" indicate that the query part is optional. The
hierarchical-part is, by definition, scheme-specific, but the two
examples shown in :numref:`Figure %s <fig-uri-format>` illustrate the
most common formats. Note that the full grammar defined in RFC 3986
includes other definitions that all schemes adhere to (e.g., how to
specify a *port* or a *user*) so the hierarchical-part is not
arbitrary.

.. [#] One of the details we are glossing over is that URIs can be
       *absolute* (a fully-qualified name that is resolved in the same
       way no matter who presents it) or *relative* (a partial name
       that is interpreted within some implied context, such as "this
       server" or "this document"). We are focused on absolute URIs in
       this section.

.. _fig-uri-format:
.. figure:: naming/figures/uri-format.png
   :width: 500px
   :align: center

   URIs consist of a *scheme*, a *hierarchical-part*, and an
   optional *query*. The hierarchical-part is scheme-specific, with
   two examples shown.

The ``https`` scheme is most familiar, so we start with it. The
hierarchical part begins with a *naming authority* that is responsible
for resolving names in the remainder of the URI. The naming authority
is often identified by a domain name. HTTPS is used to connect to that
authority, which in turn triggers DNS to resolve the domain name.
The authority component is preceded by a double slash ("//") and is
terminated by the next slash ("/"), question mark ("?"), or number
sign ("#") character, or by the end of the URI.

The *path* component is also a hierarchical name, for example, a file
name. The naming authority (typically a web server) resolves this name
in the local file system, and returns the corresponding resource. The
*fragment* component, if present, identifies a secondary resource
within the primary resource, such as a subsection of a document. The
client web browser "resolves" this name as it processes the resource
(i.e., interprets the HTML).

In addition to the syntax of the hierarchical-part, a scheme also
specifies a corresponding action. This action is what gives the
hierarchical-part meaning (i.e., defines its semantics). Since URIs
are typically processed by a browser, we can think of this action
as being executed by the browser:

 * https://... — the browser opens an HTTPS connection
 * mailto:...  — the browser launches an email client
 * ftp://...  — the browser opens an FTP connection
 * file://... — the browser asks the OS to open a file
 * urn:...  — there is no action for the browser to execute

The lack of an action for the ``urn`` scheme may be surprising, but by
definition, if we knew a way to access the resource, the URI would be
a URL and not a URN. So what are URNs good for? We'll explore that
question more carefully in the next subsection, but for now let's
limit ourselves to the syntactic issues. The first thing to notice
about the example URN in :numref:`Figure %s <fig-uri-format>` is that
it uses ``urn`` to identify the scheme. This is an example of a
URN-compliant scheme (there are others, as we'll see in a moment), but
the ``urn`` scheme was established as an exemplar for other schemes to
follow.

As shown in :numref:`Figure %s <fig-uri-format>`, the
hierarchical-part consists of an NID (*Namespace Identifier*) and an
NSS (*Namespace String*). The NID, ``doi`` in the example, is
registered to the *Digital Object Identifier Foundation*, an
organization established to serve as a registration authority for
names/identifiers assigned to any-and-all digital, virtual, and
physical resources. In the example, the DOI is able to resolve the
name ``10.17487/RFC3986``, where prefix (``10.17487``) identifies a
sub-authority (in this case the RFC Editor), and the suffix
(``RFC3986``) uniquely identifies a specific RFC.

Stepping back a bit, you can think of a URN as specifying a hierarchy
of contexts: the ``urn`` context identifies the ``doi`` context, which
in turn identifies the ``10.17487`` context, which in turn identifies
the unique resource ``RFC3986``. This works in the same way that DNS
name ``cicada.cs.princeton.edu`` identifies a hierarchy of domains,
where "domain" and "context"—as well as terms like "registry" and
"directory"—are often used interchangeably to denote the same
abstraction: a set of name/value bindings.

One might ask then, how is the root context established? For URIs, the
top-level schemes ``https`` and ``urn`` (along with ``http``, ``ftp``,
and ``mailto``, ``doi``, among others) are approved by the IANA, the
*Internet Assigned Numbers Authority*. This is the same organization
that assigns well-known ports (e.g., ``53`` for DNS), IP protocol
numbers (e.g., ``6`` for TCP), top-level domains (e.g., ``.com`` and
``.edu``), and AS numbers (e.g., ``AS15169`` for Google).  The IANA is
also responsible for registering the official NIDs under the ``urn``
scheme, including ``doi``, ``ietf``, ``uuid``, ``isbn``, and
``issn``. The IANA publishes official assignment documents, in this
case, one for URI schemes and one for URN name spaces.

.. admonition:: Further Reading

   `Uniform Resource Identifier (URI) Schemes.
   <https://www.iana.org/assignments/uri-schemes/uri-schemes.xhtml>`__

   `Uniform Resource Names (URN) Namespaces.
   <https://www.iana.org/assignments/urn-namespaces/urn-namespaces.xhtml>`__

Finally, in the same way URNs name objects at the application level,
we can think of URLs as a kind of application address. Does this mean
they are used to route messages to the appropriate destination? In a
sense, yes. We're now talking about application messages, such as an
HTTP request, and not individual packets, but it is by interpreting
the naming authority part of a URL that a request message is forwarded
to the appropriate server. DNS plays a role selecting this part of the
route. Moreover, that server might "reroute" the request to another
server by returning a 302 code. This sort of application-level routing,
involving both DNS and HTTP, plays an important role in distributed
services, such as CDNs. We return to this topic in Chapter |Overlay|.

|Naming|.3.2 Persistent Names
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

We have seen URNs included as part of the URI specification, but there
is still the unanswered question about why they exist in the first
place; URLs seem to be more useful in practice. The answer is to meet
the requirement that an object's name be permanently bound to the
object. Where an object is located and how it is accessed—and hence,
its URL—may change, but there are circumstances in which we need to
officially record that the object remains the same.

The physical world is full of documents and other objects that for legal
and other reasons need to be uniquely identifiable over time. A great
example is the ISBN (International Standard Book Number) assigned to
books and other published material. ISBNs were established in 1970,
decades before URIs, as a way to globally record the uniqueness of
every published book. The number provides commercial value (you know
exactly what you're buying and selling), legal value (you know exactly
what content has been copyrighted), and consumer value (you can have a
conversation with another user and know you are talking about the
content.).

When it became clear that the Internet had the potential to support
arbitrary digital objects, it followed that the Internet also needed
to establish a digital identifier for those objects. Bob Kahn, who
along with Vint Cerf is considered the father of the Internet,
designed such a naming service, called the *Handle System*, which
eventually influenced the inclusion of URNs in the URI syntax. It also
led to the creation of organizations like the DOI to register and
manage these persistent names. The Handle System calls such
identifiers *handles* since they represent the "handle" by which a
given object can be accessed.

.. TODO -- Could add a sidebar about the Kahn/Berners-Lee

.. admonition:: Further Reading

   R. Kahn and R. Wilensky. `A Framework for Distributed Digital Object
   Services <https://dl.acm.org/doi/10.1007/s00799-005-0128-x>`__.
   *International Journal of Digital Libraries*, April 2006.

Since URIs define a federated name space, and the Handle System
already provided a framework for defining persistent object names, the
syntax for handles was incorporated into the URN definition. The
example NSS component ``10.17487/RFC3986`` is a handle for RFC 3986.
What's missing is the resolution mechanism, and a specification for
the kinds of values that mechanism returns. On that question the
``urn`` scheme is silent (i.e., there is no action), but the DOI does
provide the infrastructure needed to map a handle into the
corresponding object. It uses URLs to locate the object, or more
specifically, an object's handle is mapped into a set of values
(*metadata* associated with the object), where one of the values is a
URL that can be used to access the object. Another piece of metadata
is the document's publication date, which is likely fixed, but the
handle-to-URL binding has to be kept up-to-date should the object move.

All of this results in the following being legitimate URIs for the
example digital object we've been using as a running example in
this section: RFC 3986:

 * ``urn:doi:10.17487/RFC3986`` — a URN, with ``urn`` as scheme and ``doi`` namespace
 * ``doi:10.17487/RFC3986`` — a URN, with ``doi`` as an IANA-sanctioned scheme
 * ``urn:ietf:rfc:3986`` — a URN, with ``urn`` as scheme and ``ietf``
   as an IANA-sanctioned namespace
 * ``https://doi.org/10.17487/RFC3986`` — a URL, with ``https`` as scheme
 * ``https://www.rfc-editor.org/rfc/rfc3986`` — a URL, with ``https`` as scheme

If you try each of these in your browser, you will find that only the
last two return something meaningful. This is because only the
``https`` scheme triggers a protocol capable of actually locating and
retrieving content. Requesting the ``doi.org`` URL resolves the
object's URN, returning a 302 Redirect code pointing to the set of
metadata associated with RFC 3986 (as opposed to the RFC itself).
This metadata includes links to various representations of the RFC,
including an HTML version. The last URL above is for that web page,
and so accessing it displays the RFC.

Note that the DOI site, which is based on the Handle System, returns a
"representation" of the named object, specifically an information page
containing the relevant metadata. Technically, a handle-based URN
names an abstract resource, not necessarily an HTML web page.  Among
other things, this means that third-party sites can also use DOI's
URNs. For example, the following URL returns metadata for the
DOI-managed handle corresponding to Kahn and Wilensky's paper
describing the Handle System.

 * ``https://dl.acm.org/doi/10.1007/s00799-005-0128-x``

We conclude with two points about persistent names. First, it is
important to recognize the value an organization like DOI provides.
As a naming authority, their value follows from being trusted to
register only legitimate names, and to delegate authority to other
entities that can be trusted to do the same. Second, having a
persistent name does not imply that the object itself is immutable.
For example, an ISBN remains the same for a second and third printing
of a book that corrects minor typos and mistakes. On the other hand,
an object that adheres to strict immutability does imply a persistent
identity, although it could be implemented by a one-way hash. Doing so
moves the problem into the security domain, and the challenges of
ensuring data integrity.

|Naming|.3.3 Attribute-Based Names
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The idea that an object has associated metadata, perhaps including a
unique and persistent handle, is not unique to documents. It actually
suggests another approach to naming, informally known as
*attribute-based names*. The idea is that instead of maintaining a
name/value pair for each object, the naming system maintains a set of
*attributes* for each object. Each attribute is a *type/value* pair,
where any of the attributes in the set can be used as a lookup key to
discover the other attributes. When one of the attributes is
guaranteed to be unique, as with the case with the object's handle,
you can learn the rest of the attributes by referencing the object
with its handle. But you can also use other attributes—or a set of
them—to learn an object's handle. In such cases, the naming system
might return information about multiple objects since any given set of
attributes may or may not identify just one.

For example, suppose you need to find a printer in your workplace. You
may know that you need one that supports color and prints in
full-duplex mode. You might also prefer that it be in your building,
but you are willing to walk across campus if necessary. In many
enterprises, you would use LDAP (Lightweight Directory Access
Protocol), to learn your options. LDAP is a popular attribute-based
naming system used to learn information about both computing resources
(e.g., servers, printers), and people (e.g., their office, phone number,
email address, department).

LDAP is best understood by example, so we'll use our hypothetical
search for a printer as the basis for concrete query using
``ldapsearch``, a client command available on Linux. The following
gives an example invocation, where the first line gives three command
options: ``-h`` says which server to contact, ``-x`` says to use
simple authentication and ``-b`` gives the base *distinguished name*
for the search (an important concept we explain below). The second
line gives the actual query; the ``&`` indicates that we want printers
that match the intersection of the three attributes; note the ``*``
(wildcard) operator for the location value. The final six lines
specify the attributes we want returned for the matched devices.

 .. code::

    ldapsearch -h ldap.princeton.edu -x -b "ou=Printers,dc=princeton,dc=edu" \
       "(&(printer-location=CS-Building*)(printer-color=TRUE)(printer-duplex=TRUE))" \
       printer-name \
       printer-location \
       printer-uri \
       printer-color \
       printer-duplex \
       printer-pages-per-minute

The result ``ldapsearch`` returns might look like the one shown below,
which includes two printers located in the CS Building. The ``uri``
attribute gives the address one would pass along to the print command;
``ipp`` is an IANA-sanctioned scheme for printers.

 .. code::

    dn: cn=ColorPrinter-A3,ou=Printers,dc=princeton,dc=edu
    printer-name: ColorPrinter-A3
    printer-location: CS-Building, Floor-3, Room-312
    printer-uri: ipp://192.168.1.45/printers/ColorPrinter-A3
    printer-color: TRUE
    printer-duplex: TRUE
    printer-pages-per-minute: 40

    dn: cn=ColorPrinter-A1,ou=Printers,dc=princeton,dc=edu
    printer-name: ColorPrinter-A1
    printer-location: CS-Building, Floor-1, Room-102
    printer-uri: ipp://192.168.1.23/printers/ColorPrinter-A1
    printer-color: TRUE
    printer-duplex: TRUE
    printer-pages-per-minute: 25

The query and result are straightforward to understand, but the ``dn``
attribute in the output (which also corresponds to the ``-b`` option
in the input) is not obvious. They both correspond to the object's
distinguished name. The syntax is unusual, but it is effectively a
hierarchical path name. The attributes recorded in LDAP are arranged
in a hierarchy of directories, with ``dc=princeton,dc=edu`` giving the
starting point (from the root). Attribute type ``dc`` stands for
"domain component", so ``dc=princeton,dc=edu`` is LDAP's version of
the DNS name ``princeton.edu``. (Even though this implies global
context, LDAP is typically used locally, limited to the server
initially contacted.)

The ``ou=Printers`` attribute in the query further narrows the search,
where ``ou`` stands for "organizational unit". You would need to know
"Printers" is the local convention for that category of data, but you
could also do a search for the available organizational units. (In
general, LDAP has a well-defined schema that can be searched, in the
same way you would search for physical resources like printers.) Note
that you can view all objects that match ``ou=Printers`` being in the
same directory context, similar to a DNS domain, or as just another
attribute that filters out acceptable answers. Functionally, they serve
the same purpose.

You might be wondering about the origins of such an unusual scheme.
LDAP is based on an ISO naming system called X.500, which was designed
to make it easier to identify people. It allows you to name a person
by giving a set of attributes: name, title, phone number, postal
address, and so on. X.500 proved too cumbersome—and, in some sense,
was usurped by powerful search engines now available on the Web—but it
did eventually evolve into LDAP, and is now widely used in
enterprises.  And while our example focused on printers, LDAP is
typically used by system administrators to keep track of information
about their users, including their home directory, login shell, login
id, home page URL, and so on.

Finally, in describing LDAP by example, we have glossed over a lot of
detail, most notably the full range of standardized attribute types
(beyond ``dc``, ``dn`` and ``ou``) and the over-the-wire protocol
specification. These details far exceed the conceptual model, which is
quite simple. The details are defined in a suite of ten RFCs, of which we
recommend RFC 4510 as the starting point.

.. admonition:: Further Reading

   K. Zeilenga (Ed.). `Lightweight Directory Access Protocol (LDAP): Technical
   Specification Road Map <https://www.rfc-editor.org/info/rfc4510>`__. RFC 4510,
   June 2006.
