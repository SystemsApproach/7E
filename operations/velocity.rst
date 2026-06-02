|Ops|.4 Feature Velocity
--------------------------------------

We have seen many examples in this chapter of how cloud operators have
influenced best practices in network operations. But there is still
one big difference between managing a network's packet delivery
service and managing the wide array of cloud services that run on top
of that packet delivery service. Most notably, by running at the
network edge, applications evolve at a much faster pace than any of
the protocols we've see so far in Part II. We are getting a little bit
ahead of ourselves—we discuss the software that runs on edge hosts in
Part III—but seeing the two operational challenges side-by-side helps
to appreciate that there is spectrum of options every operator has to
take into account. That spectrum is how much they are willing to trade
*network stability* for *feature velocity.* Every feature you add to a
network (supposedly) provides value, but it comes at the risk of
stability.

One reason we have split the topics covered in this book into "inside
the network" and "at the network edge" is that is has proven to be a
natural dividing line between favoring stability (delivering packets)
and feature velocity (building applications). But it's a continuum,
and there is no reason, in principle, why network providers couldn't
adopt the same practices as cloud providers.\ [#]_ So we conclude this
chapter by briefly outlining what those practices involve.

.. [#] This is not just an academic exercise. Traditional network
       operators are constantly trying to find ways to increase their
       value-add by including a richer set of functionality to what
       they offer their customers.

Show expanded CI/CD pipeline and talk about GitOps/SREs.... Probably
need to mention how tracing gets harder and logging becomes more
important. As an example, show how Google's best practices played a
role in Jupiter, which spans multiple generations of technology (i.e., these
practices are good ones even if your service ambitions are modest).


.. admonition:: Further Reading

   L. Peterson, A. Bavier, S. Baker, Z. Williams, and B. Davie.
   `Edge Cloud Operations: A Systems Approach
   <https:ops.systemsapproach.org>`__.  June 2025.
