4.4 Distance Vector Routing
---------------------------------

.. _key-routing-alg:
.. admonition:: Key Takeaway

   Distance-vector and link-state are both distributed routing
   algorithms, but they adopt different strategies. In
   distance-vector, each node talks only to its directly connected
   neighbors, but it tells them everything it has learned (i.e.,
   distance to all nodes). In link-state, each node talks to all other
   nodes, but it tells them only what it knows for sure (i.e., only
   the state of its directly connected links). In contrast to both of
   these algorithms, we will consider a more centralized approach to
   routing in :ref:`Section 3.5 <3.5 Implementation>` when we
   introduce Software Defined Networking (SDN). :ref:`[Next] <key-kiss>`
