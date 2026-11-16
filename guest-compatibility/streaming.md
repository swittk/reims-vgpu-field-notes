# D-Bus display readiness, hardware video and audio streaming

A working accelerated macOS guest does not by itself imply a usable host-side frame stream. REIMS publishes QEMU's guest-frame content, and a streaming encoder client consumes a display source after the QEMU process registers that source.

A historical startup race let Sunshine run before QEMU registered the display's D-Bus service name; Sunshine reported missing display/encoder and could remain running in a useless state. The solution used the existing D-Bus name-ownership check as a health/readiness dependency, then let the stream encoder join the actual VM process namespace. Startup ordering was integrated into one Docker Compose project, rather than requiring end users to start separate sidecars in a remembered order.

The complete local integration exercised QEMU's D-Bus frame-delivery path, hardware H.264/HEVC encoder discovery, persistent display/audio helper services, and normal whole-stack Compose shutdown/start. The guest's usable hardware render path, GPU source frames and decoded client displays are different stages; a successful encoder initialization alone is not a physical-client video or audio latency measurement.

**Operational lesson:** wait for the *actual producer service*, not merely an existing PID, before initializing the consumer. Preserve the already-running guest's storage independently of disposable containers. Avoid absolute machine paths, exported GPU UUIDs or embedded pairing credentials in public recipes.

This report contains no private Compose file, network address, streaming credential, machine identification or guest recording.
