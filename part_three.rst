.. SPDX-FileCopyrightText: 2026 Larry L. Peterson and Bruce S. Davie
.. SPDX-License-Identifier: CC-BY-4.0

.. role:: pop

:pop:`Part III:  Edge of the Network`
=============================================

The edge devices connected to a network like the Internet—whether they
are servers, laptops, mobile phones, or other consumer appliances—are
not managed as part of the network, per se, but they do play a
critical role in realizing a global network like today’s Internet. The
edge is where applications run, along with a networking software stack
that interacts with (connects the device to) the switches running
inside the network. Part III focuses on the edge software stack that
supports applications.  The chapters in this part address the key
challenges that arise when you try to “fill the gap” between what
applications need and what the underlying packet delivery service
described in Part II provides.

The chapters in Part III are organized around the main challeges
addressed by the edge network stack, but in exploring those
challenges, we incude descriptions of the following artifacts:

.. table::
   :align: center
   :widths: auto

   ===========   ===========
   Protocol                  Coverage
   ===========   ===========
   DCTCP                       :ref:`Section 13.4.1 <artifact-dctcp>`
   DNS                         :ref:`Section 11.2 <artifact-dns>`
   gRPC                        :ref:`Section 15.2 <artifact-grpc>`
   JPEG                        :ref:`Section 17.2 <artifact-jpeg>`
   MPEG                        :ref:`Section 17.2 <artifact-mpeg>`
   QUIC                        :ref:`Section 15.3 <artifact-quic>`
   RDMA                        :ref:`Section 15.4 <artifact-rdma>`
   RoCE                        :ref:`Section 15.5 <artifact-roce>`
   RTP                        :ref:`Section 17.3 <artifact-rtp>`
   SIP                        :ref:`Section 17.4 <artifact-sip>`
   TCP                         :ref:`Chapter 12 <artifact-tcp>`
   TLS                         :ref:`Chapter 14 <artifact-tls>`
   ===========   ===========
