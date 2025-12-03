Chapter 1:  Introduction
=============================

   I must create a System, or be enslav’d by another Man’s; I will not
   Reason and Compare: my business is to Create.

   *—William Blake*

Suppose you want to build a computer network, one that has the
potential to grow to global scale and support applications as diverse
as video conferencing, live and on-demand streaming, e-commerce,
social media, automated manufacturing, and more. What available
technologies would serve as the underlying building blocks, and what
kind of software architecture would you design to integrate these
building blocks into an effective communication service? Answering
this question is the overriding goal of this book—to describe the
available building materials and then to show how they can be used to
construct a network from the ground up.

Of course just such a network already exists, and so we use the
Internet as our model. But our goal is to understand the fundamental
challenges that had to be addressed for the Internet to become the
ubiquitous system it is today. This includes exploring the design
space of possible solutions and building intuition about the design
decisions that eventually won the day. The path from conception to
today’s reality is not a straight line, nor is today’s Internet the
end of the process. There have been many false starts and stop-gap
solutions, followed by years of iterative improvement. This evolution
culminates in today’s artifacts (a combination of hardware and
software components), which we describe in detail, keeping in mind
that those too will likely change over time.

The book is organized in two parts:

* *Part One: Inside the Network*
* *Part Two: Edge of the Network*

Topics in both parts are central to networking, but making the
division explicit serves to highlight an important system design
principle: the separation of concerns. Fully appreciating the
implications of this principle comes with reading the whole book, but
the gist is straightforward: when faced with the design of a complex
system, carving out independent challenges and addressing them in
isolation, without being overly concerned about how you’re going to
address the other issues, is a proven strategy. Conflating too many
issues and trying to address them all at once is often tempting (it’s
easy to convince yourself that doing so yields a more optimized
solution), but most of the time it’s a recipe for failure.

Of course knowing how to break a complex problem into smaller
subproblems takes experience, but this first cut—inside-vs-edge—has
proved to be an important factor in the Internet’s success. We’ll
explain how to draw the line between the two halves in the
introduction to each Part, with the subsequent chapters tackling the
set of the issues that arises when building that part of the
whole. The rest of this introductory chapter introduces some
network-specific terminology that is used throughout the book. The
terminology has familiar counterparts in other systems, such as
Operating Systems, but networking has its own peculiar way of talking
about certain topics.  Like any deconstruction of a complex system
into separate parts, this decomposition is not perfect. This
particular boundary is important to the Internet’s success, and also
helps us organize the material in this book. But there are important
challenges that require attention, and yet fall outside this
particular framing of the problem space. We call attention to such
“exceptions to the rule” when they arise, and use them to illustrate
that every system design requires judgement and makes tradeoffs.

.. include:: introduction/architecture.rst
