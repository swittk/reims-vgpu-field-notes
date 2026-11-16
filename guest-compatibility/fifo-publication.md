# x86-64 Sequoia guest FIFO lost notification

**System:** x86-64 macOS Sequoia Apple paravirtual GPU task FIFO, a Linux REIMS host consumer. **Evidence:** exact guest memory state, finite independent concurrency tests and a carefully version-scoped guest workaround.

An affected guest could stop delivering commands even though its FIFO still contained valid unread commands. Inspection found 416 unread bytes containing 16 correctly formed commands while the host FIFO worker slept on the empty-queue condition variable; no later wake notification appeared. This was not evidence of a stalled shader or absent GPU capability.

The guest publishes a new producer write-count and then reads the host consumer count to decide whether to ring an empty-to-nonempty doorbell. Its ordinary producer-count store did not enforce StoreLoad ordering; the host could simultaneously publish its consumed count, decide the ring remained empty, and sleep. The guest's write could become visible only *after* both sides decided no notification was needed.

A narrowly guarded memory-ordering correction changed publication to an atomic exchange before the guest checked the host count. Native two-thread litmus controls saw **93,970 and 137,553 lost wakeups** in two separate original five-million-iteration tests, versus **zero** in two corresponding ordered-publication tests. An exhaustive finite store-buffer model independently confirmed the allowed/unallowed interleavings.

A failed early boot-image patch made no actual runtime change, despite a valid-looking configuration; it was rejected. A later exact-driver Lilu load-time helper verified source bytes, applied the change, read it back and reported its fully verified state. Live guest tests exercised 3D, input and idle/wake behavior without introducing host FIFO polling, fake completions, GPU drops or refresh caps.

**Scope boundary:** this is a *specific historical Apple guest-driver instruction pattern and ABI*. Different guest OS versions require their own proof; the referenced Apple driver and guest disk are not redistributed. New REIMS guest-driver code or upstream protocol changes may make this workaround unnecessary. The report is a technical protocol case study, not a recommendation to blindly binary-patch other installations.
