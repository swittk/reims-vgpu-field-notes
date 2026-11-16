# Linux/Vulkan: synthetic host registration of scattered IOSurface pages may contribute to video-loop stalls

## Summary

On a Linux x86-64/KVM host using the Vulkan backend, an x86-64 macOS Sequoia guest experienced recurring presentation stalls around hardware-decoded HEVC video-loop transitions.

We investigated guest playback timing, decoder submissions, IOSurface creation, guest-memory mappings, and presentation callbacks.

One significant finding was that optional host registration of video surfaces could involve constructing a synthetic contiguous mapping over thousands of scattered guest pages.

In our local build, avoiding that registration for scattered memory while retaining direct import for naturally contiguous memory resolved the recurring stalls in our bounded tests.

## Reproduction and measurements

Environment: Linux x86-64/NVIDIA Vulkan host, x86-64 macOS Sequoia guest, original 4K HEVC Main 10 media asset. One host tested.

A matched display-only native looping-player control using the same encoded asset produced:

| Metric | Before | After |
|---|---:|---:|
| Duration | 150 s | 150 s |
| QEMU presentations/sec | 59.626 | 59.935 |
| Presentation gaps >60 ms | 6 | **0** |
| Worst presentation gap | 110.855 ms | **55.357 ms** |

The original macOS screensaver was also tested after the correction:

- 7,801 captured frames over approximately 260 seconds, at 30 FPS.
- Zero QEMU presentation gaps over 60 ms; worst 59.6 ms.
- Both observed video-loop transition windows had maximum gaps of 39.0 ms.
- No dropped frames, backward frame-order anomalies, or capture errors.

One isolated 61 ms interval remained in the separate capture-delivery path, away from either loop boundary. These observations are limited to the tested environment and workloads.

## What the traces revealed

New 4K/10-bit guest video surfaces could trigger approximately **6,079 individual mappings** while constructing a contiguous host alias. Some new pixel-buffer requests took approximately 54–89 ms.

Cached HEVC decoder sessions were opened ahead of the observed stalls. The larger gaps did not directly overlap decoder-session creation.

This led us to investigate whether preparing memory for optional direct host registration was more expensive than simply copying the decoded frames.

## What worked in our local implementation

Our final approach was to check the backing-memory layout before attempting optional registration.

Naturally contiguous buffers continued using direct registration. Scattered guest buffers that would otherwise require a synthetic contiguous alias instead used the existing canonical copy fallback.

Hardware NVDEC/NVENC and other eligible direct-import paths remained enabled.

The approach worked well in our tested environment, although other platforms and memory layouts would need their own validation.

### Source references

- [Final narrow import-eligibility correction](https://github.com/swittk/reims-vgpu-field-notes/blob/479e3a5a80210a7e684036634c99866825ce4974/patches/native-import-only.patch)
- [Complete integrated 17-file source delta](https://github.com/swittk/reims-vgpu-field-notes/blob/479e3a5a80210a7e684036634c99866825ce4974/patches/complete-loop-fix.patch)
- [Source-file hashes and applicability notes](https://github.com/swittk/reims-vgpu-field-notes/blob/479e3a5a80210a7e684036634c99866825ce4974/manifest/complete-patch-bases.json)

## Other approaches we tried

We also experimented with reducing the cost of synthetic aliases rather than avoiding them:

| Approach | Worst presentation gap | Observation |
|---|---:|---|
| Deferred mapping into a reserved address | 574.7 ms | Regressed significantly |
| Fresh deferred mapping | 161.8 ms | Still produced substantial stalls |
| Immutable internal upload with scalar guest-byte reads | 92.2 ms | Improved, but stalls remained |
| Validated bulk guest-RAM pixel upload | 81.7 ms | Faster copying, but stalls remained |

These were separate finite runs with differing instrumentation, rather than statistically matched comparisons.

Disabling optional direct writes entirely also eliminated the measured long gaps, but increased observed host CPU usage to approximately 2.46 cores. Our final layout-based approach measured approximately 1.76 cores in a separate steady-playback sample.

The experiments suggested that, at least for this workload, avoiding unnecessary synthetic alias construction was more effective than optimizing or deferring it.

[Full investigation diary and rejected experiments](https://github.com/swittk/reims-vgpu-field-notes/blob/479e3a5a80210a7e684036634c99866825ce4974/diary/02-approaches-and-failures.md)

## Correctness validation

The final integrated implementation passed the relevant memory, surface, video, and actual-GPU tests.

Native H.264/HEVC hardware encoding remained functional. Both 8-bit and 10-bit HEVC decoding matched 64 reference frame indices, timestamps, formats, and pixel hashes.

The final runtime used the normal direct-copy configuration, rather than a temporary debugger override. Instrumentation confirmed that scattered video buffers avoided synthetic registration while hardware decoding continued.

[Accepted implementation and validation details](https://github.com/swittk/reims-vgpu-field-notes/blob/479e3a5a80210a7e684036634c99866825ce4974/diary/03-accepted-implementation.md)

## Notes

Our patches come from a locally integrated source snapshot with some earlier memory and staging changes, so they aren't directly applicable to current upstream master.

We're sharing the investigation and implementation as a reference in case they're useful. We'd be interested to hear whether the underlying issue or approach makes sense in the context of the current codebase.
