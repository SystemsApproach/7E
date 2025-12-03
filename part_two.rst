.. role:: pop

:pop:`Part Two:  Edge of the Network`
=========================================

The edge devices connected to a network like the Internet—whether they
are servers, laptops, mobile phones, or other consumer appliances—are
not managed as part of the network, per se, but they do play a
critical role in realizing a global network like today’s Internet. The
edge is where applications run, along with a networking software stack
that interacts with (connects the device to) the switches running
inside the network. Part Two focuses on the applications and this edge
software stack.

Because software is pliable, and different applications have different
requirements, this edge software stack is much less rigidly
defined. There are different ways to modularize it, and over the
history of the Internet, it has seen substantial change. The edge is
where most innovation occurs, which is another key to the Internet’s
success. On the other hand, there is a benefit for edge devices and
their applications to share some common components, and so we focus
our attention on the edge modules that provide the most value to the
most use cases. As in Part One, the following chapters both explore
the design space and drill down on the artifacts that are in
widespread use today.

We begin Part Two by introducing a representative set of applications,
and identifying both how they differ and what they have in
common. Based on what we know about the underlying network—that it
provides the universal, best-effort packet delivery service described
in Part One—the subsequent chapters then discuss the key challenges
that arise when you try to “fill the gap” between what applications
