.. index:: IXP: Internet Exchange Point
.. index:: BSD: Berkeley Software Distribution
.. index:: API: Application Programming Interface
.. index:: ToR: Top-of-Rack
.. index:: VM: Virtual Machine

2.1 Anatomy of an Application
---------------------------------------

The applications that run on the Internet have come a long way. The
original set included email (still an important application, described later
in this chapter), a remote login service called Telnet (which was not
secure and has been replaced by SSH), and the FTP file transfer
protocol (which still exists but has largely been supplanted by the
web). Dramatic improvements in available bandwidth have enabled whole
new classes of applications, such as streaming and video conferencing.
There has also been a significant upgrade in the software tools
available to developers, most notably those provided by the
cloud. Before getting into specific examples, we first look at the
basic structure that is common to nearly all applications, including a
brief overview of how they are implemented in the cloud.

2.1.1 Client-Server Computing
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Most of the Internet's early applications could be classified as
"client-server", by which we mean that a client device establishes a
connection to a server and then issues requests to which the server
responds. The server might be something like a mail server while the
client is an email application running on a laptop or desktop. In
addition to the application code running on the client and server
devices, there is generally a protocol that is specific to the
application, such as SMTP in the case of email. Sitting underneath the
application layer protocol are standard, general-purpose protocols
such as TCP and IP.  Because these protocols are common across many
applications, there is no reason (today) for application developers to
implement them. Instead, they are provided as part of the operating
system, and exposed to applications through a standard API
(application programming interface).

For several decades, *sockets* have provided the standard interface
for client-server applications. Originally implemented in the Berkeley
Software Distribution (BSD) of Unix, the Socket API is now supported
in virtually all operating systems. It is also the foundation of
language-specific interfaces, such as the Java or Python socket
library. We use Linux and C for all code examples in this book; Linux
because it is open source and C because it remains the language of
choice for network internals. (C also has the advantage of exposing
all the low-level details, which is helpful in understanding the
underlying ideas.)

It is hard to overstate the importance of the Socket API. It defines
the demarcation point between the applications running on top of the
Internet, and the details of how the Internet is implemented. As a
consequence of Sockets providing a well-defined and stable interface,
writing Internet applications exploded into a multi-billion dollar
industry. Starting from the humble beginnings of the client-server
paradigm and a handful of simple application programs such as email, file
transfer, and remote login, everyone now has access to a never-ending
supply of cloud applications from their smartphones.

This section lays the foundation by revisiting the simplicity of a
client program opening a socket so it can exchange messages with a
server program, but today a rich software ecosystem is layered on top
of the Socket API. This layer includes a plethora of cloud-based tools
that lower the barrier for implementing scalable applications as we
discuss below.

The main abstraction of the socket interface, not surprisingly, is the
*socket*. A good way to think of a socket is as the point where a local
application process attaches to the network. The interface defines
operations for creating a socket, attaching the socket to the network,
sending/receiving messages through the socket, and closing the socket.
To simplify the discussion, we will limit ourselves to showing how
sockets are used with TCP. (The API can also be used to send and
receive messages via UDP and IP.)

The first step is to create a socket, which is done with the following
operation:

.. code-block:: c

   int socket(int domain, int type, int protocol);

The reason that this operation takes three arguments is that the socket
interface was designed to be general enough to support any underlying
protocol suite. Specifically, the ``domain`` argument specifies the
protocol *family* that is going to be used: ``PF_INET`` denotes the
Internet family, ``PF_UNIX`` denotes the Unix pipe facility, and
``PF_PACKET`` denotes direct access to the network interface (i.e., it
bypasses the TCP/IP protocol stack). The ``type`` argument indicates the
semantics of the communication. ``SOCK_STREAM`` is used to denote a byte
stream. ``SOCK_DGRAM`` is an alternative that denotes a message-oriented
service, such as that provided by UDP. The ``protocol`` argument
identifies the specific protocol that is going to be used. In our case,
this argument is ``UNSPEC`` because the combination of ``PF_INET`` and
``SOCK_STREAM`` implies TCP. Finally, the return value from ``socket``
is a *handle* for the newly created socket—that is, an identifier by
which we can refer to the socket in the future. It is given as an
argument to subsequent operations on this socket.

The next step depends on whether you are a client or a server. On a
server machine, the application process performs a *passive* open—the
server says that it is prepared to accept connections, but it does not
actually establish a connection. The server does this by invoking the
following three operations:

.. code-block:: c

   int bind(int socket, const struct sockaddr *address, socklen_t addr_len);
   int listen(int socket, int backlog);
   int accept(int socket, struct sockaddr *address, socklen_t *addr_len);

The ``bind`` operation, as its name suggests, binds the newly created
``socket`` to the specified ``address``. This is the network address of
the *local* participant—the server. Note that, when used with the
Internet protocols, ``address`` is a data structure that includes both
the IP address of the server and a TCP port number. Ports are used to
indirectly identify processes. They are a form of *demux keys*. The port
number is usually some well-known number specific to the service being
offered; for example, web servers commonly accept connections on port
80.

The ``listen`` operation then defines how many connections can be
pending on the specified ``socket``. Finally, the ``accept`` operation
carries out the passive open. It is a blocking operation that does not
return until a remote participant has established a connection, and when
it does complete it returns a *new* socket that corresponds to this
just-established connection, and the ``address`` argument contains the
*remote* participant’s address. Note that when ``accept`` returns, the
original socket that was given as an argument still exists and still
corresponds to the passive open; it is used in future invocations of
``accept``.

On the client machine, the application process performs an *active*
open; that is, it says who it wants to communicate with by invoking the
following single operation:

.. code-block:: c

   int connect(int socket, const struct sockaddr *address, socklen_t addr_len);

This operation does not return until TCP has successfully established a
connection, at which time the application is free to begin sending data.
In this case, ``address`` contains the remote participant’s address. In
practice, the client usually specifies only the remote participant’s
address and lets the system fill in the local information. Whereas a
server usually listens for messages on a well-known port, a client
typically does not care which port it uses for itself; the OS simply
selects an unused one.

Once a connection is established, the application processes invoke the
following two operations to send and receive data:

.. code-block:: c

   ssize_t send(int socket, const void *message, size_t msg_len, int flags);
   ssize_t recv(int socket, void *buffer, size_t buf_len, int flags);

The first operation sends the given ``message`` over the specified
``socket``, while the second operation receives a message from the
specified ``socket`` into the given ``buffer``. Both operations take a
set of ``flags`` that control certain details of the operation.

In the supplemental resources site for this book, you can find the code
for a simple simple client-server program that
uses the socket interface to send messages over a TCP connection. The
program also uses other Linux networking utilities, which we introduce as
we go. The application allows a user on one machine to type in and send
text to a user on another machine. It is a simplified version of the
Linux ``talk`` program, which is functionally similar to the core of
instant messaging applications.


.. admonition:: Further Reading

   `Supplemental Resources for Computer Networks: A Systems Approach
   <https://github.com/SystemsApproach/7E/tree/main/resources/>`__.



2.1.2 Scalable Services
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The cloud did not exist when the first iteration of network
applications were created. But it does exist today—thanks to the
Internet providing the necessary communication substrate—and it has
reshaped how we build the server side of those applications. Today’s
commercial datacenters operate tens of thousands of servers, making it
possible for applications to “rent” compute capacity, typically in the
form of a Virtual Machine (VM). The more users your service attracts,
the more VMs you spin up, thereby lowering the barrier to launching
new applications. In short, you need not purchase hardware and you
need not invest in an operations team to manage it. You just invoke an
API to create a new VM on demand.

From the client side, a cloud service looks exactly the same as
before: you send a request message to an IP address and wait for the
reply. The difference is on the server side, where an extra level of
indirection is introduced. Specifically, that IP address is for a
logical server—which is called a *service*\ —where a *load balancer*
intercepts the request message and forwards it to one of a scalable
number of backend processes.

It’s an implementation detail, but rather than a process, per se, each
instance is often a container, which is essentially a process
encapsulated inside a self-contained environment that includes all the
software packages the process needs to run. Docker is today’s
canonical example of a container platform. Clouds also offer other
programming models, such as serverless computing, but conceptually,
they all encapsulate a computation in some environment that can be
quickly instantiated, executed, and deleted. For our purposes, the
important takeaway is that clients invoke operations on a service, and
the underlying network is able to forward packets to an IP address
that represents the “entry point” for that service. The service is
then implemented by code packaged inside multiple containers, running
inside one or more VMs, which in turn have been instantiated on one or
more physical servers.

.. _fig-service:
.. figure:: applications/figures/service.png
    :width: 400px
    :align: center

    Client process sending a request to a scalable cloud service.

:numref:`Figure %s <fig-service>` shows the basic structure, where
the load balancer can be implemented in different ways, including a
hardware device, but it is typically implemented by a proxy process
that runs in a VM (also hosted in the cloud) rather than as a physical
appliance. The individual servers in the diagram can be physical
machines, but in commercial clouds, they are more likely to be VMs.

There is a set of best practices for implementing the server code that
eventually responds to that request, and some additional cloud
machinery to create/destroy containers and load balance requests
across those containers. Kubernetes is today’s most widely used
container management system, and the *microservices architecture* is
what we call the best practices in building services in this cloud
native manner. Both are interesting topics, but beyond the scope of
this book. We recommend an excellent tutorial if you want to learn how
to use Kubernetes.

.. _reading_k8s:
.. admonition:: Further Reading

   `Kubernetes Tutorial
   <https://kubernetes.io/docs/tutorials/kubernetes-basics/>`__.

Keep in mind that the thousands of servers hosted in every cloud
datacenter are interconnected using Internet technology. So the cloud
is simultaneously a platform for implementing network applications and
a use case for the underlying network.  :numref:`Figure %s
<fig-leaf-spine>` shows a simplified schematic of how a modern
datacenter is structured, with a switching fabric built from the same
commodity switches described in Chapter |Tech| arranged according to a
*leaf-spine* topology. For the small 4-rack example in the figure, each
rack has a Top-of-Rack (ToR) switch that interconnects the servers in
that rack; these are referred to as the leaf switches of the
fabric. (There are typically two such ToR switches per rack for
resilience, but the figure shows only one for simplicity.) Each leaf
switch then connects to a subset of available spine switches, with two
requirements: (1) that there be multiple paths between any pair of
racks, and (2) that each rack-to-rack path is two-hops (i.e., via a
single intermediate spine switch). Note that this means in leaf-spine
designs like the one shown in the figure, every server-to-server path
is either two hops (server-leaf-server in the intra-rack case) or four
hops (server-leaf-spine-leaf-server in the inter-rack case).

.. _fig-leaf-spine:
.. figure:: applications/figures/leaf-spine.png
    :width: 500px
    :align: center

    Example of a leaf-spine switching fabric common to cloud
    datacenters and other clusters, such as on-premises clouds.

.. sidebar:: Datacenter Networks

   The cloud has had a transformative impact on how we implement
   network applications, but building the networks that underlie the
   cloud—for example, a datacenter fabric like the one depicted in
   :numref:`Figure %s <fig-leaf-spine>`\ —raises an interesting set of
   networking challenges. We address those challenges throughout the
   book, but note that they are not unique; datacenter networks face
   the same set of problems as any network faces. What's unique is the
   specific circumstances (e.g., traffic patterns, round-trip times,
   topology, scale) of datacenter networks, which influences design
   choices we end up making.

   We address each of those datacenter-specific challenges in the
   context of the general networking problems. Section 4.5 describes
   how to support routing in datacenter fabrics; Section 8.4 looks at
   queue management in fabric switches; Section 9.4 explains how
   virtual networks are deployed in datacenters; and Sections 15.4 and
   15.5 show how datacenter networks are being optimized for AI
   workloads. In addition, Chapter 10 describes how best practices in
   operating datacenter networks is now influencing how networks
   everywhere are being managed.


2.1.3 Distributed Services
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

While the previous subsection describes how applications have evolved
from a process running on a single server to a scalable service
running on a cluster of servers, there is a second way to scale a
service: distribute a set of clusters across multiple
points-of-presence throughout the Internet. Doing so provides more
aggregate resources (thereby improving the number of client requests
that can be served every second), and at the same time, improves the
opportunity for each client to be served by a nearby cluster (thereby
improving the average latency of all requests). Just as importantly,
replicating an application across multiple sites improves its
availability should a site fail.

We’ll see concrete examples of how this is done in Chapter |Overlay|, but for
now it is easy to understand the general idea. The key is to recognize
that the clouds that host applications are themselves widely
distributed. It is estimated that cloud datacenters are positioned at
over 5000 sites throughout the world, including both the hyperscale
datacenters that companies like Google, Amazon, and Microsoft operate,
and more modest-sized facilities, often co-located with Internet
Exchange Points (IXPs) that interconnect operator backbones. Plus,
increasingly, there are “on premises” clusters embedded within
enterprises connected to the edge of the network.

In other words, the cloud is structured as an interleaving of
datacenters and networks, similar to that shown in :numref:`Figure %s
<fig-structure>`. In such an environment, invoking a service involves
additional levels of indirection. For example, the first step might be
to resolve the service name into the IP address for a service endpoint
running in a nearby cloud site, and then a load balancer distributes
those requests across local servers within that site. In general, how
an application is partitioned into independent functions, how those
functions are placed in the right cloud location, and how client
requests are forwarded to the best location are application-specific
questions.

.. _fig-structure:
.. figure:: applications/figures/structure.png
    :width: 550px
    :align: center

    Cloud datacenters are distributed throughout the globe, with a
    site often in close proximity to users.


.. takeaway::

   An important takeaway from this discussion is that arbitrary
   computation can take place throughout the network, and not just at
   two endpoints. From one perspective, application software still
   runs on computers connected to the edge of the network, no
   different than in our simple client/server scenario. The network
   itself just delivers packets. But from another perspective, the
   application seemingly runs “inside” the network, with computations
   happening at multiple points along the end-to-end path from the
   ultimate source to the ultimate destination. Notably, such computations are not happening in the
   switches that implement the packet delivery service. After many
   years of trying to inject additional functionality into switches,
   the networking community reached the conclusion that it is best to
   give switches just one task—forwarding packets. The existence of
   millions of servers at thousands of locations around the world
   (i.e., the cloud) has made this a viable approach.

Today, the Internet and the cloud have a symbiotic relationship. The
Internet provides the communication substrate that the cloud runs on,
while the cloud provides the computing substrate that enables ever more
powerful applications to be distributed across the Internet.

2.1.4  Distributed Computation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

So far we have looked at how to scale network applications,
while remaining consistent with the client-server paradigm and using
the Socket API. There have been extensions to the API, for example to
avoid copying messages into and out of user space, but the programming
model has remained remarkably stable: one side does a passive open, the
other does an active open, both sides send and receive messages, and
the receive operation blocks waiting for a message to arrive.

This programming model matches the needs of so-called "network
applications" (which we now sometimes call "cloud apps" or simply
"apps"), but there are other types of computations that need to
communicate over a network. Such highly distributed computations are
parallel programs that might once have run on purpose-built
supercomputers, sometimes referred to as HPC (high performance
computing) workloads. Today, such distributed computations mostly run
in datacenters.  Among these parallel programs, training AI models is
probably the dominant use case.  These programs do not use the Socket
API and they are not best described as client-server.

The full complexity of parallel workloads is beyond the scope of this book,
but to appreciate the networking implications it's enough to
understand two key points about the high-level communication pattern.
First, a large parallel computation usually proceeds in a sequence of iterations. For each
iteration, data is first "scattered" across multiple nodes, the nodes
then compute on the subset of data sent to each of them, and finally
the results are "gathered" back in a central node. To support this
(and similar patterns), the API supports *Scatter* and *Gather*
operations among a collective of nodes. The first implies a
one-to-many communication, and the second implies a many-to-one
communication.

An example framework for these types of applications is MapReduce,
which was developed inside Google for managing such large distributed
computations as indexing the contents of millions of web pages. An
open source implementation of MapReduce was developed as part of the
Apache Hadoop project. The "Map" part of MapReduce entails farming out
(scattering) parallelizable tasks to many different servers. When
these tasks complete, they send outputs to a set of servers using a
*shuffle* operation, which is many-to-many communication. Those
servers then perform the "reduce" operation. Finally, the output of
the reduce step is gathered to produce the final result (e.g. a file
representing the index).

Importantly, there is often a synchronization barrier between each
iteration, such that one iteration has to complete before the next
iteration can begin. This means that the last transfer to complete
during each transfer limits how fast the overall computation runs. In
other words, it's not how fast the fastest communication can be
implemented; it's how fast the slowest communication completes. Being
able to achieve low latency in the face of traffic bursts—a natural
consequence of one-to-many and many-to-one exchanges—is the central
performance challenge for this communication pattern.

The second point is that parallel programs are typically structured to
maximize parallelism (the number of threads running concurrently) and
minimize blocking (waiting for a message to arrive or another thread
to complete a task). To this end, communication is typically organized
around *work queues* and *completion queues*. The idea is that threads
asynchronously insert tasks (messages) into a work queue and are later
asynchronously informed that the work is complete. Threads that must
know that work has completed before they can safely proceed typically
*poll* the completion queue rather than invoke a blocking operation.

All of this is to say that networks enable a wide range of
applications, not just those that immediately come to mind when you
think of email, web surfing, or video. The API is the demarcation
point between these applications and the network, and while
understanding the Socket API takes us a long way, there are other
models. We return to the API that AI training and other highly
parallel programs use in Chapter |Message|.
