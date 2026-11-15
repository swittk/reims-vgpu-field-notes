# Diary 02 — costly approaches we rejected

Do NOT interpret a new optimization passing unit tests as proof of faster presentation.

| Approach | Measured result | Lesson |
|---|---|---|
| Build joined mapping from scattered 4K/10-bit pages before optional host import | Roughly **6,079** individual mappings per new surface; some buffer requests **54–89 ms** | An optional zero-copy registration may cost much more than copying |
| Reserve, then defer mapping (v1) | **15** gaps >60 ms / 280 s; worst **574.7 ms** | Delaying work until renderer use made things worse |
| Fresh deferred mapping (v2) | **16** gaps >60 ms / 280 s; worst **161.8 ms** | No proof that relocation alone avoids critical-path stalls |
| Command-owned immutable upload, scalar guest-byte reader (v3) | **5** gaps >60 ms / 280 s; worst **92.2 ms** | Snapshot bytes are correct but volatile scalar reads are still costly |
| Validated bulk ordinary-RAM snapshot (v4) | Upload p99 improved about **18.39 → 9.73 ms**; **12** gaps >60 ms; worst **81.7 ms** | Faster copies alone did not fix the loop |
| Blanket disable of direct write registration (control only) | 260 s/7,801 frames, **0** gaps >60 ms; worst **48 ms**; around **2.46 host CPU cores** | Useful causal control but too broad, with observed CPU cost |
| **Final eligibility rule** | Structural decline of synthetic scattered alias registration, preserving naturally contiguous imports | Tested bounded outcome and correct fallback without GPU/driver-specific gating |

The tests are sequential and have differing instrumentation/load conditions. This table reports what happened, **not statistically matched treatment effects**.

For a finite CPU-only comparison of exactly identical 4 MiB data over 64 passes, the structured scalar read took **52.28/38.75 ms**, versus **20.40/20.88 ms** for a verified bounded bulk pixel copy. The approximately **2.2×** mean CPU-only speedup did NOT by itself eliminate loop jitter.

Guideline: check whether a contiguous buffer truly exists *before* doing expensive synthetic alias construction solely to obtain optional registration. Keep the canonical copy fallback. Don't convert a fast registration check into expensive mapping work on the render thread.
