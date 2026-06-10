2.2 World Wide Web
-----------------------------

We begin our discussion of applications by focusing on the one that is
so ubiquitous, it is often mixed up with the Internet itself: the
World Wide Web. The web is so widely used today that it is hard to
call it an application; it might be better thought of as a framework
for building and delivering applications. But it did in fact start out
as a single application, with a single application layer
protocol—HTTP—underpinning it. In this section we dig into the details
of that protocol, and in doing so, tease out the reasons the web has
become such a ubiquitous framework for building new applications.

.. _artifact-http:

2.2.1 Application Protocols
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Before we go any further, it is important to distinguish between
application *programs* and application *protocols*. For example, the
HyperText Transport Protocol (HTTP) is an application protocol that is
used to retrieve web pages from remote servers. Many different
application programs—that is, web clients like Internet Explorer,
Chrome, Firefox, and Safari—provide users with a different look and
feel, but all of them use the same HTTP protocol to communicate with
web servers over the Internet. Indeed, it is the fact that the
protocol is published and standardized that enables application
programs developed by many different companies and individuals to
interoperate. That is how so many browsers are able to interoperate
with all the web servers (of which there are also many varieties).

Second, we observe that many application layer protocols, including
HTTP, have a companion protocol that specifies the format of the data
that can be exchanged. This is one reason why these protocols are
relatively simple: much of the complexity is managed in this companion
standard.  HTTP is a protocol for fetching web pages, but HyperText
Markup Language (HTML) is a companion specification that defines the
basic form of those pages. This separation between the data transfer
protocol and the data format shows up again repeatedly, as we will see
in the next section.

The World Wide Web has been so successful and has been the primary way
many people interact with the Internet for so long that sometimes, in
the popular consciousness, the Web *is* the Internet. (Of course, many
people now interact with the Internet mainly using their phones,
unaware that the Web underpins most mobile apps as well.)  In fact,
the design of the system that became the World Wide Web started around 1989, long
after the Internet had become a widely deployed system and numerous
other methods to retrieve information over the Internet already
existed. The original goal of the Web was to offer a new way to organize
and retrieve information, drawing on ideas about hypertext—interlinked
documents—that had been around since at least the 1960s.\ [#]_ The
core idea of hypertext is that one document can link to another
document, and the protocol (HTTP) and document language (HTML) were
designed to meet that goal.

.. [#] A short history of the Web provided by the World Wide Web
       consortium traces its roots to a 1945 article describing links
       between microfiche documents.

One way to think of the Web is as a set of cooperating clients
and servers, all of whom speak the same language: HTTP. Most people are
exposed to the Web through a graphical client program or web browser
such as Safari, Chrome, Firefox, or Internet Explorer. :numref:`Figure %s
<fig-netscape>` shows the Safari browser in use, displaying a page of
information from Princeton University.

.. _fig-netscape:
.. figure:: applications/figures/browser-screenshot.png
   :width: 600px
   :align: center

   The Safari web browser.

If you want to organize information into a system of linked
documents or objects, there needs to be a way to identify documents so
you can link to them. Uniform Resource Locators (URLs)
are so familiar to most of us by now that it’s easy to forget that they
haven’t been around forever. They provide information that allows
objects on the web to be located, and they look like the following:

.. code-block:: html

   http://www.cs.princeton.edu/index.html

If you opened that particular URL, your web browser would open a TCP
connection to the web server at a machine called
``www.cs.princeton.edu`` and immediately retrieve and display the file
called ``index.html``. Most files on the Web contain images and text,
and many have other objects such as audio and video clips, pieces of
code, etc. They also frequently include URLs that point to other files
that may be located on other machines, which is the core of the
“hypertext” part of HTTP and HTML. A web browser has some way in which
you can recognize URLs (often by highlighting or underlining some
text) and then you can ask the browser to open them. These embedded
URLs are an example of *hypertext links*. When you ask your web browser to
open one of these embedded URLs (e.g., by pointing and clicking on it
with a mouse), it will retrieve and display
the named file. It thus becomes very easy to hop from one machine to
another around the network, following links to all sorts of
information. Once you have a means to embed a link in a document and
allow a user to follow that link to get another document, you have the
basis of a hypertext system.

When you ask your browser to view a page, your browser (the client)
fetches the page from the server using HTTP, which traditionally runs
over TCP. (If you looked at the source for a web browser, you would
find the client-side socket calls shown in Section 2.1.1.)  At its
core, HTTP is a text-based request/response protocol, where every
message has the general form

::

   START_LINE <CRLF>
   MESSAGE_HEADER <CRLF>
   <CRLF>
   MESSAGE_BODY <CRLF>

where ``<CRLF>`` stands for carriage-return+line-feed. The
first line (``START_LINE``) indicates whether this is a request message
or a response message. In effect, it identifies the “remote procedure”
to be executed (in the case of a request message), or the *status* of
the request (in the case of a response message). The next set of lines
specifies a collection of options and parameters that qualify the
request or response. There are zero or more of these ``MESSAGE_HEADER``
lines—the set is terminated by a blank line—each of which looks like a
header line in an email message. HTTP defines many possible header
types, some of which pertain to request messages, some to response
messages, and some to the data carried in the message body. Instead of
giving the full set of possible header types, though, we just give a
handful of representative examples. Finally, after the blank line comes
the contents of the message (``MESSAGE_BODY``); this part of
the message is where a server would place the requested page when
responding to a request, and it is typically empty for request messages.

Why was HTTP designed to run over TCP? The designers didn’t have to do
it that way, but TCP provides multiple features that HTTP needs,
most notably, reliable data delivery (no-one wants a web page with
missing data). Using TCP (as opposed to either IP or
UDP) meant that the designers of HTTP didn't need to worry about
reliability. However, as we’ll see below, a few issues arose from
building a request/response protocol on top of TCP. These issues
become more apparent as you consider the details of the interactions
between the application and transport layer protocols, and were
exacerbated by the addition of security to the transport layer. This
has led to new versions of HTTP, which has in turn inspired new
transport protocols. The interplay between HTTP and the transport layer is a great
example of how modularity boundaries shift over time as systems
mature and evolve.


Request Messages
++++++++++++++++++++

The first line of an HTTP request message specifies three things: the
operation to be performed, the web page the operation should be
performed on, and the version of HTTP being used. Although HTTP
defines a wide assortment of possible request operations—including
*write* operations that allow a web page to be posted on a server—the
two most common operations are ``GET`` (fetch the specified web page)
and ``HEAD`` (fetch status information about the specified web
page). The former is obviously used when your browser wants to
retrieve and display a web page. The latter is used to test the
validity of a hypertext link or to see if a particular page has been
modified since the browser last fetched it. The full set of operations
is summarized in :numref:`Table %s <tab-ops>`. As innocent as it
sounds, the ``POST`` command enables much mischief (including spam) on
the Internet.

.. _tab-ops:
.. table::  HTTP Request Operations.
   :align: center
   :widths: auto

   +-----------+-----------------------------------------------------------+
   | Operation | Description                                               |
   +===========+===========================================================+
   | OPTIONS   | Request information about available options               |
   +-----------+-----------------------------------------------------------+
   | GET       | Retrieve document identified in URL                       |
   +-----------+-----------------------------------------------------------+
   | HEAD      | Retrieve metainformation about document identified in URL |
   +-----------+-----------------------------------------------------------+
   | POST      | Give information (e.g., annotation) to server             |
   +-----------+-----------------------------------------------------------+
   | PUT       | Store document under specified URL                        |
   +-----------+-----------------------------------------------------------+
   | DELETE    | Delete specified URL                                      |
   +-----------+-----------------------------------------------------------+
   | TRACE     | Loopback request message                                  |
   +-----------+-----------------------------------------------------------+
   | CONNECT   | For use by proxies                                        |
   +-----------+-----------------------------------------------------------+

For example, the ``START_LINE``

::

   GET http://www.cs.princeton.edu/index.html HTTP/1.1

says that the client wants the server on host ``www.cs.princeton.edu``
to return the page named
``index.html``.  This particular example uses an absolute URL. It is
also possible to request a path in the ``START_LINE`` and specify the host name
in one of the ``MESSAGE_HEADER`` lines; for example,

.. code-block:: http

   GET /index.html HTTP/1.1
   Host: www.cs.princeton.edu

Here, ``Host`` is one of the possible ``MESSAGE_HEADER`` fields. One
of the more interesting of these is ``If-Modified-Since``, which gives
the client a way to conditionally request a web page—the server
returns the page only if it has been modified since the time specified
in that header line.

Response Messages
++++++++++++++++++++

Like request messages, response messages begin with a single
``START_LINE``. In this case, the line specifies the version of HTTP
being used, a three-digit code indicating whether or not the request was
successful, and a text string giving the reason for the response. For
example, the ``START_LINE``

.. code-block:: http

   HTTP/1.1 202 Accepted

indicates that the server was able to satisfy the request, while

.. code-block:: http

   HTTP/1.1 404 Not Found

indicates that it was not able to satisfy the request because the page
was not found. There are five general types of response codes, with the
first digit of the code indicating its type. :numref:`Table %s <tab-codes>`
summarizes the five types of codes.

.. _tab-codes:
.. table::  Five Types of HTTP Result Codes.
   :align: center
   :widths: auto

   +------+---------------+--------------------------------------------------------+
   | Code | Type          | Example Reasons                                        |
   +======+===============+========================================================+
   | 1xx  | Informational | request received, continuing process                   |
   +------+---------------+--------------------------------------------------------+
   | 2xx  | Success       | action successfully received, understood, and accepted |
   +------+---------------+--------------------------------------------------------+
   | 3xx  | Redirection   | further action must be taken to complete the request   |
   +------+---------------+--------------------------------------------------------+
   | 4xx  | Client Error  | request contains bad syntax or cannot be fulfilled     |
   +------+---------------+--------------------------------------------------------+
   | 5xx  | Server Error  | server failed to fulfill an apparently valid request   |
   +------+---------------+--------------------------------------------------------+

As with the unexpected consequences of the ``POST`` request message, it
is sometimes surprising how various response messages are used in
practice. For example, request redirection (specifically code 302) turns
out to be a powerful mechanism that plays a big role in Content
Distribution Networks (CDNs) by redirecting requests to a nearby cache.

Also similar to request messages, response messages can contain one or
more ``MESSAGE_HEADER`` lines. These lines relay additional
information back to the client. For example, the ``Location`` header
line specifies that the requested URL is available at another
location. Thus, if the Princeton CS Department web page had moved from
``http://www.cs.princeton.edu/index.html`` to
``http://www.princeton.edu/cs/index.html``, for example, then the
server at the original address might respond with

.. code-block:: http

   HTTP/1.1 301 Moved Permanently
   Location: http://www.princeton.edu/cs/index.html

In the common case, the response message will also carry the requested
page. This page is an HTML document, but since it may carry nontextual
data (e.g., a GIF image), it is encoded using MIME (see the section on
email). Certain of the ``MESSAGE_HEADER`` lines give attributes of the
page contents, including ``Content-Length`` (number of bytes in the contents),
``Expires`` (time at which the contents are considered stale), and
``Last-Modified`` (time at which the contents were last modified at the server).

Connection Overhead
++++++++++++++++++++++

The original version of HTTP (1.0) established a separate TCP
connection for each data item retrieved from the server. It’s not too
hard to see how this was a very inefficient mechanism: a TCP
connection requires setup and teardown messages to be exchanged
between the client and server even if all the client wanted to do was
verify that it had the most recent copy of a page. Thus, retrieving a
page that included some text and a dozen icons or other small graphics
would result in 13 separate TCP connections being established and
closed.

To make matters worse, securely accessing an HTTPS page results in
even more round trips for the two sides to exchange and verify security
credentials. In this case, HTTPS runs over TLS, which in turn runs
over TCP. :numref:`Figure %s <fig-oldhttp>` shows the sequence of
events for fetching a page that has just a single embedded object.
Blue lines indicate TCP and TLS messages, while black lines indicate
the HTTP requests and responses. The figure is purposely vague about
the exact TCP/TLS messages being exchanged (we'll fill in the details
in Chapters |TCP| and |TLS|), but the key takeaway is clear: multiple round
trips are required to download even the simplest web page.  This is a
striking example of how passing off functionality to be implemented in
another layer without looking at the details can have negative
consequences.

.. _fig-oldhttp:
.. figure:: applications/figures/rtt-overhead.png
   :width: 500px
   :align: center

   HTTP 1.0 results in multiple round trips for a single
   Request/Response pair.

To overcome this situation, HTTP version 1.1 introduced *persistent
connections*—the client and server can exchange multiple
request/response messages over the same TCP connection. Persistent
connections have many advantages. First, they eliminate the
connection setup overhead, thereby reducing the load on the server,
the load on the network caused by the additional TCP packets, and the
delay perceived by the user. Second, because a client can send
multiple request messages down a single TCP connection, TCP
is able to operate more efficiently. (TCP includes a congestion
control mechanism that paces traffic so as to not overload the
network; that mechanism is described in Chapter |CC|.)
:numref:`Figure %s <fig-persist>` shows the transaction
from :numref:`Figure %s <fig-oldhttp>` using a persistent connection
in the case where the connection is already open (presumably due to
some prior access of the same server).

.. _fig-persist:
.. figure:: applications/figures/f09-05-9780123850591.png
   :width: 500px
   :align: center

   HTTP 1.1 behavior with persistent connections.

Persistent connections carry a price, however. The problem
is that neither the client nor server necessarily knows how long to keep
a particular TCP connection open. This is especially critical on the
server, which might be asked to keep connections opened on behalf of
thousands of clients. The solution is that the server must time out and
close a connection if it has received no requests on the connection for
a period of time. Also, both the client and server must watch to see if
the other side has elected to close the connection, and they must use
that information as a signal that they should close their side of the
connection as well. (Both sides must close a TCP connection
before it is fully terminated.) Concerns about this added complexity may
be one reason why persistent connections were not used from the outset,
but today it is widely accepted that the benefits of persistent
connections more than offset the drawbacks.

While 1.1 is still widely supported, version 2.0 was formally
approved by the IETF in 2015. Known as HTTP/2, the new version is
backwards compatible with 1.1 (i.e., it adopts the same syntax for
header fields, status codes, and URIs), but it adds several new features.

The first is to make it easier for web servers to *minify* the
information they send back to web browsers. If you look closely at the
makeup of the HTML in a typical web page, you will find a plethora of
references to other bits-and-pieces (e.g., images, scripts, style files)
that the browser needs to render the page. Rather than force the client
to request these bits-and-pieces (technically known as *resources*) in
subsequent requests, HTTP/2 provides a means for the server to bundle
the required resources and proactively *push* them to the client without
incurring the round-trip delay of forcing the client to request them.
This feature is coupled with a compression mechanism that reduces the
number of bytes that need to be pushed. The whole goal is to minimize
the latency an end-user experiences from the moment they click on a
hyperlink until the selected page is fully rendered.

Additionally, HTTP/2 adopts a form of header compression. As web pages
have become more complex and the number of requests required to fully
render a page has grown, reducing the overhead of each request is
important for the latency experienced by the client. Header
compression goes some way to improving the latency of fetching a page.

The third advance of HTTP/2 is to multiplex several request/response
pairs on a single TCP connection. This goes beyond what version 1.1
supports—allowing a *sequence* of requests to reuse a TCP
connection—by permitting these requests to overlap with each
other. The way HTTP/2 does this is to allow independent *streams*,
each of which is labeled in the HTTP header with a unique *stream
id*. A given stream is limited to one request/response exchange at a
time, but multiple streams can be active on a given TCP connection.

You can see from this discussion how the layering decisions made in
the early days of HTTP had long-lasting effects on its performance as
the Web evolved. Ultimately there was a realization that an
alternative approach to layering would be worth the effort. That
effort has resulted in a new version of HTTP (known as HTTP/3) and a
new transport protocol to be used in place of TCP (that protocol is
called QUIC). Since understanding QUIC requires a deeper look at
transport layer issues—including how we secure end-to-end
communication—we postpone it until Chapter |Message|. For the purposes of
this discussion, the big takeaway is that it is sometimes necessary to
look across protocol layer boundaries, in this case, across the
transport/application boundary.

Stated another way, layering decisions can have a profound impact
on system behavior and performance. Running HTTP version 1 on top of
TCP was a completely understandable decision that enabled the Web to
get off the ground, but we have now gone through 3 major revisions of
this layered approach, culminating in a totally new design for the
transport layer underpinning HTTP. This is partly a testament to the
ability of the Internet to support incremental evolution but also a
reminder that we need to think carefully about the entire system, not
just the behavior of a single layer, when designing protocols.

2.2.2 Uniform Resource Identifiers
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The URLs that HTTP uses as addresses are one type of *Uniform Resource
Identifier* (URI). A URI is a character string that identifies a
resource, where a resource can be anything that has identity, such as a
document, an image, or a service.

The format of URIs allows various more specialized kinds of resource
identifiers to be incorporated into the URI space of identifiers. The
first part of a URI is a *scheme* that names a particular way of
identifying a certain kind of resource, such as ``mailto`` for email
addresses or ``file`` for file names. The second part of a URI,
separated from the first part by a colon, is the *scheme-specific part*.
It is a resource identifier consistent with the scheme in the first
part, as in the URIs ``mailto:santa@northpole.org`` and
``file:///C:/foo.html``.

For URIs that most people are familiar with—where the scheme is
``http``, ``https``, ``mailto`` or ``file``\ —the scheme-specific part
includes information that can be used to *locate* the resource. The
domain name for a web server or an email server are common examples.
This locator information is what qualifies a URI as a URL. Another
type of URI, known as a *Uniform Resource Name (URN)* , is used to
specify a unique name for a resource, but without giving any hint as
to its location. For URNs, we can think of the scheme as specifying a
*name space* and the scheme-specific part as specifying a unique name
within that name space.

For example, the *DOI Foundation* (where DOI stands for *Digital
Object Identifier*) manages an effort to assign a unique and
persistent (never changing) identifier to every object in the
Internet. A URN corresponding to the DOI name space would look
something like this: ``doi:10.17487/RFC0791``. This example happens to
be a URN for the RFC that specifies the IP protocol. If you want to
learn more about the DOI effort, and how to interpret names like
``doi:10.17487/RFC0791``, you can visit the DOI web site using the
following URL: ``https://doi.org``. If you want to read RFC 791, you
can type the following URL into your browser:
``https://doi.org/10.17487/RFC0791``. As this example illustrates,
discussions about naming can easily become abstruse. We'll venture
back into that discussion in Chapter |Naming| when we take a closer look
at naming systems.

2.2.3 Caching
~~~~~~~~~~~~~~

An important implementation strategy that makes the web more usable is
to cache web pages. Caching has many benefits. From the client’s
perspective, a page that can be retrieved from a nearby cache can be
displayed much more quickly than if it has to be fetched from across the
world. From the server’s perspective, having a cache intercept and
satisfy a request reduces the load on the server.

Caching can be implemented in many different places. For example, a
user’s browser can cache recently accessed pages and simply display the
cached copy if the user visits the same page again. As another example,
a site can support a single site-wide cache. This allows users to take
advantage of pages previously downloaded by other users. Closer to the
middle of the Internet, Internet Service Providers (ISPs) can cache
pages.\ [#]_ Note that, in the second case, the users within the site most
likely know what machine is caching pages on behalf of the site, and
they configure their browsers to connect directly to the caching host.
This node is sometimes called a *proxy*. In contrast, the sites that
connect to the ISP are probably not aware that the ISP is caching pages.
It simply happens to be the case that HTTP requests coming out of the
various sites pass through a common ISP router. This router can peek
inside the request message and look at the URL for the requested page.
If it has the page in its cache, it returns it. If not, it forwards the
request to the server and watches for the response to fly by in the
other direction. When it does, the router saves a copy in the hope that
it can use it to satisfy a future request.

.. [#] There are quite a few issues with this sort of caching, ranging
       from the technical to the regulatory. One example of a
       technical challenge is the effect of *asymmetric paths*, when
       the request to the server and the response to the client do not
       follow the same sequence of router hops.

For the last couple of decades, caching has also been provided by
specialized networks of servers known as Content Distribution Networks
(CDNs). These are generally implemented as an overlay on top of the
Internet to serve popular content from caches that are widely
distributed and closer to the users than the origin servers. They not
only improve performance but also make it harder for a popular site to
be brought down by a *denial-of-service* attack.

No matter where pages are cached, the ability to cache web pages is
important enough that HTTP has been designed to make the job easier. The
trick is that the cache needs to make sure it is not responding with an
out-of-date version of the page. For example, the server assigns an
expiration date (the ``Expires`` header field) to each page it sends
back to the client (or to a cache between the server and client). The
cache remembers this date and knows that it need not reverify the page
each time it is requested until after that expiration date has passed.
After that time (or if that header field is not set) the cache can use
the ``HEAD`` or conditional ``GET`` operation (``GET`` with header line)
to verify that it has the most recent copy of the page. More generally,
there are a set of *cache directives* that must be obeyed by all caching
mechanisms along the request/response chain. These directives specify
whether or not a document can be cached, how long it can be cached, how
fresh a document must be, and so on. We’ll return to the subject of
CDNs in Chapter |Overlay|.

2.2.4 RESTful API
~~~~~~~~~~~~~~~~~~~~~

So far we have focused on interactions between a human and a web
server.  For example, a human uses a web browser to interact with a
server, and the interaction proceeds in response to input from the
user (e.g., by clicking on links). However, there is also significant
demand for direct computer-to-computer interaction, which is what
happens every time you initiate a commercial transaction in a browser
or by using a cell phone app: a retail service talks to a payment
service, a shipping service, an inventory service, an upstream
supplier, and so on. We conclude this section by looking at how HTTP
and the web have played a central role in supporting such
transactions, which are collectively known as *Web Services*. In
short, HTTP helps by defining an API for web services, and that API is
often characterized as *RESTful*.

REST is an acronym—\ *REpresentational State Transfer*\ —used to
explain the software architecture that underlies the web. It was
articulated in a PhD thesis by Roy Fielding in 2000, a few years after
the web started to become mainstream. A RESTful API, in turn, is an
API that is consistent with the REST architecture. If you understand
the basic ideas of the web we've just covered, you understand the
essence of REST: everything is a *resource*, resources are identified
by a URI, and the operations you can perform on resources include
``GET``, ``POST``, ``PUT``, and ``DELETE`` (see :numref:`Table %s
<tab-ops>`).

While this sounds like a circular definition, it really boils down to
an observation that the web, whether by design or accident, reinvented
an object-oriented software architecture—resource is just another name
for an object, and the set of HTTP operations are just a slight
variation of a well-known set of object-oriented operations referred
to as *CRUD*: *Create*, *Read*, *Update*, *Delete*.

.. TODO: show an example resource and operations on it. Point to
   tooling that helps you build a REST API. This could be a source
   of programming exercises (in addition to Socket programming).

In the context of web services, the REST architecture is based on the
assumption that the best way to integrate applications across networks
is by re-applying the model underlying the World Wide Web
architecture.  Since the only operations REST supports are the HTTP
methods, such as ``GET`` and ``POST`` to provide an interface to a Web
Service. In the REST model, any complexity in this interface is
shifted from the protocol to the payload. The payload is a
representation of the abstract state of a resource. For example, a
``GET`` could return a representation of the current state of the
resource, and a ``POST`` could send a representation of a desired
state of the resource.

The representation of a resource state is abstract; it need not resemble
how the resource is actually implemented by a particular backend
service. It is not necessary to transmit a complete resource state in
each message. The size of messages can be reduced by transmitting just
the parts of a state that are of interest (e.g., just the parts that are
being modified). And, because Web Services share a single protocol and
address space with other web resources, parts of states can be passed by
reference—by URI—even when they are other Web Services.

This approach is best summarized as a data-oriented or document-passing
style, as opposed to a procedural style. Defining an application
protocol in this architecture consists of defining the document
structure (i.e., the state representation). XML and the lighter-weight
JavaScript Object Notation (JSON) are the most frequently used
presentation languages for this state. Interoperability depends on
agreement, between a Web Service and its clients, on the state
representation.

One of the selling features of REST is that it leverages the
infrastructure that has been deployed to support the web. For example,
web proxies can enforce security or cache information. Existing content
distribution networks (CDNs) can be used to support RESTful
applications.

.. sidebar:: Alternative Design

     The Internet did not settle on REST overnight. An alternative
     approach to Web Services, called *SOAP (Simple Object Access
     Protocol)* also gained traction.  The SOAP architecture’s
     approach to the problem was to make it feasible, at least in
     theory, to generate protocols that are customized to each network
     application. The key elements of the approach are a framework for
     protocol specification, software toolkits for automatically
     generating protocol implementations from the specifications, and
     modular partial specifications that can be reused across
     protocols. In other words, SOAP supports a procedural style,
     while REST supports a data-centric style.

     The online retailer Amazon, not surprisingly, was an early
     adopter (2002) of Web Services. Interestingly, Amazon made its
     systems publicly accessible via *both* SOAP and REST approaches,
     and according to some reports a substantial majority of
     developers use the REST interface. Of course, this is just one
     data point and may well reflect factors specific to Amazon.  As
     for this book, we view REST as the more elegant approach to web
     services, and so elect to focus on it.

As a final note, if Web Services is what we call it when the web
server that implements my application sends a request to the web
server that implements your application, then what do we call it when
we both put our applications in the cloud so that they can support
scalable workloads? We can call both of them *Cloud Services* if we
want to, but is that a distinction without a difference? It depends.

Moving a server process from a physical machine running in my machine
room into a virtual machine running in a cloud provider’s datacenter
shifts responsibility for keeping the machine running from my system
admin to the cloud provider’s operations team, but the application is
still designed according to the Web Services architecture. On the
other hand, if the application is designed from scratch to run on a
scalable cloud platform, for example by adhering to the
*micro-services architecture*, then we say the application is *cloud
native*. So the important distinction is cloud native versus legacy
Web Services deployed in the cloud. Either approach can export
a RESTful API.
