# REIMS integration and performance archive (October–November 2026)

This is an **evidence-led archive of independent source changes and field tests**, not a monolithic fork. The original work brought an x86-64 macOS guest on Linux/KVM and Vulkan through web graphics, hardware media, live frame streaming, and reliable lifecycle behavior. Each case below separates tested source from deployment workarounds and rejected experiments.

## Accepted source records

| Case | Evidence strength | Source |
|---|---|---|
| [Proven-owned writable guest imports](writable-guest-memory-import/README.md) | Counted GPU-import kernel queries (2 to 0), focused actual-GPU tests | [6-file patch](writable-guest-memory-import/source.patch) |
| [Remote reply-teardown deadlock](remote-reply-teardown-deadlock/README.md) | Deterministic before-hang / after-completion regression; 104 focused tests | [1-file patch](remote-reply-teardown-deadlock/source.patch) |
| [External-write freshness + GPU-private legacy backing](external-write-private-backing/README.md) | Guest pixel correctness, 31 actual-GPU cases, 3 valid motion windows around 19.8% GPU / 30 FPS | [14-file mixed REIMS/Metal2Vulkan patch](external-write-private-backing/source.patch) |
| [Backing revalidation + HEVC decoder reuse](backing-revalidation-hevc-reuse/README.md) | Exact unchanged-binding tests, actual-GPU decoder allocation 2 to 1; live candidate NOT smooth | [6-file combined patch](backing-revalidation-hevc-reuse/source.patch) |
| [Video encoder source alignment](native-video-encoder-alignment/README.md) | Hardware H.264/HEVC independently allocated/pool frames, measured reference decode | [1-file patch](native-video-encoder-alignment/source.patch) |
| [Direct packed decoded-plane writes](direct-packed-video-rows/README.md) | Synchronous guarded copy behavior; intermediate to final import policy | [12-file patch](direct-packed-video-rows/source.patch) |
| [QEMU VNC unchanged-row scan](qemu-vnc-unchanged-rows/README.md) | 1,500 randomized differential cases, sanitizer checks, modest CPU scan win | [1-file QEMU patch](qemu-vnc-unchanged-rows/source.patch) |

The final structural host-registration fix that resolved the repeated media-loop stalls is preserved separately in [the existing final-loop patch and diaries](../diary/03-accepted-implementation.md), with [REIMS issue #157](https://github.com/steelbrain/reims-vgpu/issues/157) already open. The published [Spotlight Clear/self-read finding](https://github.com/steelbrain/metal2vulkan/issues/17) is in the Metal2Vulkan archive.

Each source-case directory contains a brief technical note, the original source-only patch, and a **before/after source SHA-256 manifest**. All seven new source archives were reconstructed by reversing the patch from recorded final bytes and reapplying it, matching the original final files exactly. This verifies the historical patch, **not** compatibility with the current upstream codebase.

## Other important findings

- [Guest protocol, browser compatibility, frame timing, SMC and shutdown](../guest-compatibility/README.md): separate, strongly scoped investigations, often involving exact versions of proprietary guest drivers rather than a portable host patch.
- [Integrated capability/validation matrix](../CAPABILITIES.md): which workloads were actually demonstrated and what remains a user report.
- [Research methods and rejected paths](../RESEARCH_METHOD.md): distinguish browser callbacks, QEMU presentation and real streamed frames; avoid broad GPU-specific tuning and misleading cross-run speedups.
- [Metal2Vulkan shadow and import experiments](https://github.com/swittk/metal2vulkan-field-notes): further GPU-side lessons, including failures.

## How to interpret this archive

A patch with a valid before/after SHA contract is **not** necessarily a clean change against current upstream master. Some source families were developed cumulatively from mixed local baselines, and newer source crates have been reorganized. Several cases overlap by design. Do not concatenate all patches and treat the result as a reproducible build. The current upstream architecture, tests, and ownership semantics deserve re-evaluation before reusing a particular idea.

The archive contains only source diffs, summarized metrics, public test descriptions and provenance hashes. No guest images, Apple firmware, user screenshots, host process dumps, machine-specific settings, credentials, or licensed media are included.
