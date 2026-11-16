# ReimsDisplayTiming v0.1.0 — Sequoia detailed-timing validity

**Version-scoped source and test evidence, not an upstream-ready macOS patch.**

In an x86-64 macOS Sequoia 15.8.1 (24H32) guest, the inspected AppleParavirtFramebuffer::getTimingInfoForDisplayMode cleared the detailed-timing-valid flag even though selected mode timing data were available under IOFBConfig / IOFBModes / TM. The IOFramebuffer fallback instead estimated its period from software-scheduled VBL interrupts, allowing wildly invalid CVDisplayLink output timestamp predictions.

Our tested Lilu helper calls the original method, then (only after a successful return with validity missing) recovers the selected mode's existing timing data. It validates geometry, pixel clock, matching min/max clocks, plausible refresh range and zero blanking extension before copying that data and setting kIODetailedTimingValid. **It does not hardcode 60 Hz or change the host renderer.** An exact 52-byte method signature prevents routing unknown code.

## Source and evidence

- [Reviewable original helper logic](src/ReimsDisplayTiming.cpp) and [52-byte exact-method guard](src/exact_code.hpp)
- [Matching sample bundle metadata](src/Info.plist), [object-only build script](build_objects.py), [build prerequisites](BUILD.md)
- [Normalized native clock observations](tests/clock-analyses.json) and [data checker](tests/check_clock_evidence.py)
- [Source/guard contract checks](tests/check_contract.py) and [original executable/source provenance hashes](manifest/source-provenance.json)

The published C++ differs from the live-tested original only by replacement of the private bundle identifier with org.reims.fieldnotes.ReimsDisplayTiming, with a corresponding Info.plist change. We compiled this sanitized source to Mach-O objects with Clang 19 and the pinned Lilu 1.7.1/MacKernelSDK dependencies. **The renamed package has not been relinked as an installable kext or retested in a guest.**

## Observed guest measurements

Eight-second native CVDisplayLink captures at nominal 60 Hz:

| Condition | Samples | Output time >100 ms away from host time | Nominal frame period |
|---|---:|---:|---|
| Original | 483 | 9 | 15.648–16.808 ms |
| Corrected, idle | 479 | **0** | Fixed 16.666658 ms |
| Corrected, game active | 479 | **0** | Fixed 16.666658 ms |
| Correction removed | 483 | 10 | 15.578–16.894 ms |

The separate uncorrected **active-game** return had 373/477 timestamps outside ±100 ms, ranging approximately −35.0 to +35.5 seconds. After reapplying the helper there were 0/480 idle and 0/479 game outliers.

A deterministic hardware-Metal game at 1920x992 produced distinct QEMU Display1 frames at **55.543 FPS** with the initial correction, **50.591 FPS** after removing it, and **59.646 FPS** after reapplication/reboot (1,695 unique valid frame IDs). The final corrected p95 gap was 22.2 ms versus 35.1 ms uncorrected.

This was a sequence of bounded, nonrandomized tests, not a universal 60 FPS guarantee or a physical Moonlight display measurement. The full presentation budget and other guest scheduling delays were not eliminated by this test alone.

## Not the idle-VBL issue

[REIMS #141](https://github.com/steelbrain/reims-vgpu/issues/141) describes a Tahoe guest missing VBL events during idle and starting animations 100–150 ms late. Our Sequoia defect causes incorrect predicted output timestamps **even while continuously rendering**. Related architecture, distinct observed failure.

## Run independent source/data checks

```sh
python3 tests/check_contract.py
python3 tests/check_clock_evidence.py
```

The helper refuses an unexpected Darwin/kernel implementation, incorrect exact method signature or inconsistent timing data. Do not extrapolate its method signature to other driver releases. No compiled Apple driver, kernel collection, firmware, private logs or VM images are included.
