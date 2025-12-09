1.1  Requirements
---------------------------

This chapter establishes the ambitious goal of building a network that
scales to a global footprint and supports a wide range of
applications.  Scalability and generality are actually just two—of
many—requirements our design must address. This section expands on
these, and other requirements.  To place the requirements in their
proper context, it is important to recognize that there are multiple
stakeholders interested in the outcome.  They include:

* **Application Developers:** People that program application care
  about the communication services their applications need. This
  includes, for example, a guarantee that each message the application
  sends will be delivered without error within a certain amount of
  time or the ability to switch gracefully among different connections
  to the network as the user moves around.

* **Network Operators:** People that operate networks care that it is
  easy to administer and manage. This includes, for example, that
  faults can be easily isolated, new devices can be added to the
  network and configured correctly, and it is easy to account for
  usage.

* **Network Engineers:** People that assemble networks for a
  particular deployment scenario, and likely have to consider
  budgetary limits. This includes, for example, that network resources
  are efficiently utilized and fairly allocated to different users.

It's possible to come up with other more specific stakeholders, but
this list is inclusive enough to stress our design. And more
importantly, instead of adopting just one of these as our primary
perspective, we approach the problem of building a network from the
perspective of the architect that is responsible for crafting a design
that takes all stakeholder requirements into account.

* **Scalability:** When we say a network must scale, we certainly mean
  it must be able to connect billions of devices (or perhaps even
  more). Less obviously, we also need to be able to scale the number
  of stakeholders responsible for different aspects of the
  network. For example, it would not be practical for a
  planetary-scale network to limit the number of gatekeepers that
  decide what applications may be deployed; anyone should be able to
  stand up a new application. As another example, it would not be
  practical for a limited number of operations teams to be responsible
  for fixing every outage reported anywhere on the globe; anyone
  should be able to own, and then take responsibility for operating, a
  piece of the network.

* **Generality:** Requiring a network to support a wide-range of
  applications is another way of saying it must be general-purpose. In
  practice, this means that the lowest levels of the network that is
  shared by all applications should be relatively minimal; it should
  provide a service that is of value to all applications and leave most
  decisions about functionality to higher layers (with the application
  at the top of the software stack).  For example, it make no sense for
  the base communication service provide absolute guarantees about
  message delivery if not all applications need it. On the other hand,
  many applications share common requirements about such things as
  reliability and security, so it is helpful to provide optional
  capabilities they can employ if they want to. Getting this right is
  a balancing act, which we revisit at many points throughout the
  book.

* **Cost-Effectiveness:** Networks are built using finite
  resources. They do not have infinite capacity and while there are
  exceptions, it is not generally practical to provide dedicated
  connectivity between any two communicating entities. This means
  networks are fundamentally a shared resource, making the resource
  allocation mechanism one of the central technical challenges we
  face. This mechanism should be fair (for some definition of
  fairness), but also efficient. The latter requirement means that we
  want to maximize the amount data it delivers on behalf of its users.
  We expand on this challenge in Section 1.5, with multiple chapters
  playing a role in the solution.

* **High Performance:** Everyone wants the network to be fast, but
  performance is multi-faceted. Some applications case about response
  time; how long it takes to send and receive a message. Others care
  about throughput; how long it takes to download a large file. Still
  others care that packet delivery is predictable, so that a video
  stream plays without jarring pauses. All of these metrics, in turn,
  depend on physical limitations, such as the speed of light, but
  there is also significant work that a network has to do in order to
  deliver on the potential. We discuss this challenge in Section 1.6,
  with multiple chapters playing a role in the solution.

We highlight the first four requirements because they dominate the
design challenges discussed throughout the book. There are other
requirements as well, but they largely mirror the requirements one
would place on any system. Two of the most important are:

* **Managability:** The network should provide mechanisms to observe
  the behavior of its constituent components, diagnose problems when
  they are detected, and take corrective action by changing the
  configuration.  This is the subject of Chapter 8.

* **Reliability:** The network should recover from intermittent
  failures, and remain available in the face of serious outages and
  attacks.  This is topic that comes up at multiple places throughout
  the book, but especially in Chapter 11.

* **Security:** The network should protect the confidentiality and
  integrity of messages exchanged between participants. This is the
  subject of Chapter 12.




