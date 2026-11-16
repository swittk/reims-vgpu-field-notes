# Sequoia 60 Hz detailed-timing validity and CVDisplayLink behavior

**Original helper source and reproducible evidence:** [ReimsDisplayTiming v0.1.0 implementation, build guidance, clock results and provenance](display-timing-implementation/README.md). The published source uses a sanitized bundle identifier; the tested live guest package is recorded by SHA-256.

An x86 Sequoia guest using a paravirtual display could report visually disruptive timing despite a nominal 60 Hz mode. The inspected guest driver did not mark fixed detailed timing valid when returning it to the framebuffer timing API. The fallback estimated output time from interrupt intervals, which were sensitive to software scheduling jitter.

A narrow, exact-version guest helper preserved the selected mode's real pixel-clock/pixel-count timing and indicated that the detailed timing was valid. It did **not** change the fixed refresh rate, invent a clock, or alter the host renderer.

In bounded eight-second on/off/on tests at 60 Hz:

| Condition | Valid output timestamps outside ±100 ms | Observed nominal frame period |
|---|---:|---|
| Original behavior, 483 samples | 9 | 15.648–16.808 ms |
| Corrected idle, 479 samples | **0** | Fixed 16.666658 ms |
| Corrected game, 479 samples | **0** | Fixed 16.666658 ms |
| Correction removed, 483 samples | 10 | 15.578–16.894 ms |

A subsequent on/off/on control found hundreds of seconds-scale timestamp deviations only with the uncorrected path, and none in the corrected bounded windows.

In one deterministically marked GPU game scene, **1,695 distinct delivered frame IDs / 59.646 frames per second** were recorded after the final correction, compared with **50.591 frames per second** in the off control. An earlier corrected measurement was 55.543, so do not report only the best run or infer universal timing improvements. These are source-frame measurements, not physical Moonlight display latency.

This is **not necessarily the same issue** as [REIMS #141](https://github.com/steelbrain/reims-vgpu/issues/141), which concerns idle VBL behavior on a different platform and guest generation. Version-specific helper source and Apple binaries are not distributed here.
