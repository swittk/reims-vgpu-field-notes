# Diary 03 — accepted memory-layout policy (November 8, 2026)

## Implementation

- Already naturally contiguous guest/host-owned memory: retain direct host registration.
- Scattered guest pages requiring a freshly joined alias: decline optional host registration and use the canonical row-copy fallback.
- Keep hardware NVDEC/NVENC and global/per-frame direct-copy enabled, with no exact GPU/driver performance gate.
- Internal forward copies use immutable command-owned upload buffers, validated ordinary-RAM pixel copying, and a guest-write generation check started before the copy. GPU-to-guest writeback and explicit CPU/dma-buf view consumers are preserved.

The complete, locally integrated source delta is in patches/complete-loop-fix.patch; the last eligibility decision alone is in patches/native-import-only.patch and **depends on earlier changes**.

## Bounded observed outcomes

- Original screensaver: **7,801 frames / 259.994 s**, approximately **30.00075 FPS**; two loop-transition windows, worst **39.0 ms**; zero dropped/capture-error frames and no backward frame-order anomalies.
- QEMU presentation counters during that capture: **30 FPS**, worst **59.6 ms**, **zero** intervals >60 ms. The independent D-Bus capture-delivery path showed **one isolated 61 ms interval** away from loop transitions.
- Native 150-second looping-player control: **8,690 presentations / 59.935 FPS**, worst **55.357 ms**, zero gaps >60 ms. The native control is not the 30-FPS screensaver.
- Tests: 20 content-guarded release stages; 108 Linux surface/VM checks, 197 memory checks, 18 original structured guest-pointer checks, 43 backing tests, 391 video-core tests, 19 selected real-GPU checks and 31 unchanged renderer fixtures.
- Native codec acceptance: hardware H.264/HEVC independent and session-pool encoders, each 90 valid changing frames; 8-bit and 10-bit decodes each 64 frames with exact reference frame index, timestamp, format and pixel hashes.
- Separate CPU sample: about **1.76 host cores** with final policy, vs ~**2.46** for a blanket-disable diagnostic and ~**1.57** for a preceding direct-write candidate. Sequential finite samples, **not** a universal CPU-cost conclusion.

A temporary diagnostic debugger setting was not shipped. On the final fresh process, read-only instrumentation observed 121 hardware-decoded frame readbacks and **zero optional synthetic host registrations** for the scattered video buffers while the ordinary direct-copy settings remained enabled.

We deliberately excluded guest disks, licensed video assets, host logs, local identifiers and compiled images. No claim is made that these patches have already been ported or built on public upstream HEAD.
