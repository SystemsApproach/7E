|Ops|.3 Monitoring and Telemetry
--------------------------------------

Expand on the ``counter`` variable in Section |Ops|.2.2; e.g, the
following shows telemetry data that might be read (or streamed) for an
Ethernet interface. At this point we are building on the
OpenConfig/gNMI foundation.

.. literalinclude:: operations/code/counters.yang

Note that the ``last-clear`` variable indicates when the device last
set the counters to zero, which can then be used to calculate average
transmission rates over various time periods.

Next look at event examples; e.g., BGP state changes. Emphasize the
importance of watching for link flaps or dropping a BGP session.

Then look at flow telemetry; e.g., sFlow

Then look at active monitoring; e.g., iperf

Then look at MeasurementLab (and similar databases); off-line analysis.

Finally, talk about inband network telemetry
