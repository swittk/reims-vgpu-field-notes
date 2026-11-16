# Do not impose backend output alignment on already repacked encoder input

**State:** small tested protocol/geometry correction, one-file source patch.

The historical encoder admission path advertised a 64-byte source-alignment requirement even though REIMS first gathers byte-addressed IOSurface planes and repacks the rows before the backend reads them. This unnecessarily rejected or redirected valid guest buffers, including ordinary 32-byte plane offsets.

The local change sets the *source* alignment requirement to one byte while preserving all destination plane-size, stride and buffer bounds checks. This does not promise that NVIDIA's hardware output formats have no alignment requirements; it separates the guest's source addressing from the encoder's backend storage contract.

The native test accepted both independently allocated and session-pool frames, producing 90 changing, hardware-accelerated frames each for H.264 and HEVC in its accepted integration run. Decoding with duplicate-frame insertion disabled matched the expected luma ramp; the reported maximum mean-frame error was 0 for H.264 and around 0.000104 for HEVC.

The archived patch is verified against its recorded one-file before/after source. Current upstream has reorganized video crates; applicability and any similar existing correction need independent checking.
