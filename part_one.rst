.. role:: pop

:pop:`Part One: Inside the Network`
==============================================

It is estimated that over 22 billion devices are connected to today’s
Internet. Those devices are widely distributed across the planet (and
in low orbit around the planet), where any one of them can send and
receive messages with one or more of the other 22 billion
devices. Clearly, the network that connects those devices must itself
also be highly distributed, and the Internet is just such a
distributed system. It is built from special devices known as
switches. Each switch has a modest set of communication ports
connected to some link technology, and its job is to receive data on
one port and send it out on another port. This means a large and
distributed collection of switches can be interconnected to form a
network, as shown in Figure X. That diagram illustrates the idea, but
keep in mind there are hundreds of millions of such switching devices
in today’s Internet, making visualizations like the one shown in
Figure Y more like an abstract piece of art than an engineering
diagram.

Throughout the history of computer networks, there have been many
different kinds of switches. We begin Part One by looking at the
possible design space for switches, but settle on a particular design
that plays an important role in today’s Internet: Ethernet
switches. The rest of Part One then builds on top of this foundation,
introducing all the challenges that have to be addressed in order to
send and receive messages over a global network of interconnected
switches inside the network. We assume the 22 billion devices at the
edge of the network are trying to communicate with each other, but
save the challenges of building applications on top of the
interconnected network of switches for Part Two. Our goal in Part One
is to provide logical connectivity between every pair of connected
edge devices.

We say “logical connectivity” because it’s obviously the case that
there is no direct physical connection—say, a fiber optic or copper
cable—between all of those edge 22 billion devices; nearly all
communication is indirect through a sequence of one or more
switches. Moreover, in the same way a physical connection might fail
from time-to-time—a network’s natural enemy is a backhoe that cuts
through a cable, but there are many other more subtle failure
modes—the end-to-end logical connection does not guarantee every sent
message is actually received. Our goal for Part One is to support what
is commonly known as best-effort packet delivery, where dealing with
this imperfection is one of the factors that makes Part Two
interesting. In short, you can read Parts One and Two in either order,
knowing that the “interface” between the two halves is a universal,
best-effort packet delivery service.
