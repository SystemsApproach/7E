.. index:: CI/CD: Continuous Integration / Continuous Deployment

|Ops|.5 Feature Velocity
--------------------------------------

We have seen many examples in this chapter of how cloud operations
have influenced best practices in network operations. But there is
still one big difference between managing a network's packet delivery
service and managing the cloud services that run on top of that packet
delivery service. Most notably, applications, implemented on hosts at
the network edge, evolve at a much faster pace than any of the
protocols we've see so far in Part II. We are getting a little bit
ahead of ourselves—we discuss the software that runs on edge hosts in
Part III—but seeing the two operational challenges side-by-side helps
us to appreciate that there is spectrum of options every operator has
to take into account. That spectrum is how much they are willing to
trade *network stability* for *feature velocity.* Every feature you
add to a network (supposedly) provides value, but it comes at the risk
of negatively impacting stability.

One reason we have split the topics covered in this book into "inside
the network" and "at the network edge" is that is has proven to be a
natural dividing line between favoring stability (delivering packets)
and feature velocity (building applications). But it's a continuum,
and there is no reason, in principle, why network providers couldn't
adopt the same practices as cloud providers.\ [#]_ So we conclude this
chapter by briefly outlining what those practices involve.

.. [#] This is not just an academic exercise. Traditional network
       operators are continually trying to find ways to increase their
       value-add by including a richer set of functionality in what
       they offer their customers. For example, one of the biggest
       arguments for 5G was that MNOs would be able develop and sell
       additional services beyond broadband connectivity. But that's
       easier said than done, and the challenge of operating the
       resulting set of services is a big reason why.

The biggest factor limiting the deployment of software into the cloud
(or network) is its inclusion in the management process. Hence there
has been a blurring of the distinction between operations and
development. Combining them results in a pipeline similar to the one
depicted in :numref:`Figure %s <fig-cicd-pipeline>`. The diagram shows
operators responsible for inventory and developers responsible for all
code (including configuration), but in practice, a team that works on
a particular subsystem or service typically shares responsibility for
operation and development tasks, a practice known as *DevOps*.

.. _fig-cicd-pipeline:
.. figure:: operations/figures/cicd-pipeline.png
   :width: 650px
   :align: center

   Continuous Integration / Continuous Deployment (CI/CD) pipeline.

As also depicted in :numref:`Figure %s <fig-cicd-pipeline>`, the
pipeline includes two major processes, labelled *Continuous
Integration (CI)* and *Continuous Deployment (CD*). Every time a
change is checked into the code repo the CI system runs, integrating
the change into a full a system build, testing the new system under
various scenarios, and if successful, updating the image repo with a
new executable. Changes in the inventory or configuration repos then
trigger the CD part of the pipeline, which deploys the changes
throughout the cloud or network. Both sides of the pipeline are said
to be "continuous" because they process every commit to the repos,
potentially resulting in continuous improvement to the operational
system. This end-to-end automation is key to feature velocity.

This brief summary hides many important details, including the
sophistication of the versioning and testing components, as well as
the fact that each system is deployed on a combination of
physical and virtual resources; these resources require their own set of
configuration specs. Those specifications are similar in spirit to
OpenConfig (e.g., they use YAML), but they have evolved around a
different set of abstractions (models). For an in-depth description of
all the management infrastructure needed to operate even a
modest-sized cloud running within an enterprise, we recommend a
companion book.

.. admonition:: Further Reading

   L. Peterson, A. Bavier, S. Baker, Z. Williams, and B. Davie.
   `Edge Cloud Operations: A Systems Approach
   <https:ops.systemsapproach.org>`__.  June 2025.

As a final note, while we have motivated CI/CD and DevOps as enabling
feature velocity for cloud services and applications, the same
practices have also been successfully applied to the underlying
network. This works as long as that network is also software-based
(i.e., adopts SDN practices), so that its functionality can be
included in the CI/CD pipeline. A well publicized example of this
approach is Jupiter, Google's datacenter network. Jupiter has
accommodated constant evolution and improvement while maintaining high
(99.999%) availability as a production system. Jupiter fabrics have
run production traffic for over a decade while every component has
been replaced, including multiple generations of switches, patch
panels swapped for optical circuit switches, multiple generations of
SDN controllers, spine blocks removed entirely as the topology was
restructured, and so on.

.. admonition:: Further Reading

    L. Poutievski et al. `Jupiter Evolving: Transforming Google's Datacenter
    Network via Optical Networks and Software-Defined Networking
    <https://dl.acm.org/doi/abs/10.1145/3544216.3544265>`__.
    ACM SIGCOMM '22 Symposium, August 2022.
