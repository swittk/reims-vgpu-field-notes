# REIMS vGPU field investigations — macOS on Linux/Vulkan

This repository documents a hands-on, multi-week effort to make an accelerated x86-64 macOS guest useful on Linux/KVM: native Metal and browser WebGL, hardware video, real display streaming, frame pacing, and guest lifecycle. It includes **working source patches, exact replay checks, bounded live measurements and failed experiments**.

It is an independent research archive, not an official REIMS release or a collection of upstream-ready PRs.

## Start here

| What you're looking for | Where to read |
|---|---|
| **The full engineering backlog, seven additional source patches, test scope and measured outcomes** | [Source and performance case index](archive/README.md) |
| Platform behaviors demonstrated, and claims we cannot independently verify | [Capability and test matrix](CAPABILITIES.md) |
| Guest FIFO wakeups, Sequoia Metal/Safari compatibility, display clocks, SMC and streaming | [Guest compatibility / operations](guest-compatibility/README.md) and [Sequoia display-timing helper source](guest-compatibility/display-timing-implementation/README.md), including the [tested FIFO guest-patch source](guest-compatibility/fifo-publication-implementation/README.md) |
| How we measured, and why some plausible optimizations failed | [Research methodology and failed approaches](RESEARCH_METHOD.md) |
| REIMS video-loop issue and final narrow memory-layout fix | [REIMS #157](https://github.com/steelbrain/reims-vgpu/issues/157) · [accepted implementation](diary/03-accepted-implementation.md) |
| Metal2Vulkan shadowing, source patches, Spotlight corruption and negative GPU results | [Companion Metal2Vulkan archive](https://github.com/swittk/metal2vulkan-field-notes) |

## The recurring HEVC video-loop issue

The final investigation found that optional direct host registration of a video surface could **itself** create thousands of synthetic guest-memory mappings. Declining that optional registration when the guest pages were not naturally contiguous retained direct imports for eligible buffers and the canonical-copy path for scattered ones.

In a bounded final 260-second original screensaver run, QEMU reported 30 FPS with no >60 ms presentation gaps (worst 59.6 ms). A separate 150-second native looping control ran at 59.935 FPS with no >60 ms gaps (worst 55.357 ms). One independent capture-transport interval of 61 ms occurred away from loop transitions. These are not universal GPU or physical-client latency guarantees.

The original reference artifacts remain available:
- [Complete historical 17-file local patch](patches/complete-loop-fix.patch), [narrow last-stage decision](patches/native-import-only.patch) and [source identity manifest](manifest/complete-patch-bases.json).
- [Observed loop characteristics](diary/01-player-loop.md), [failed approaches](diary/02-approaches-and-failures.md) and [accepted validation](diary/03-accepted-implementation.md).
- [The upstream issue](https://github.com/steelbrain/reims-vgpu/issues/157) shares these observations without asking maintainers to merge the historical patch directly.

## Applicability and provenance

The seven additional archived source patches were all replayed by reversing them from their original recorded final files and applying them forward again, matching source bytes exactly. Their individual SHA-256 preimages and postimages are in each case's source-contract.json, with a [read-only offline checker](tools/README.md).

**Source replay does not mean a patch applies to current upstream master, compiles with current APIs, or has a portable performance gain.** These cases come from different incremental versions of our local integrated REIMS and Metal2Vulkan source. Some overlap and must not be stacked blindly. The larger final 17-file patch also uses a mixed baseline. One small QEMU VNC change is from a different upstream project and retains that project's licensing.

No proprietary Apple firmware/code bundles, VM or iOS device images, raw host logs, machine paths/hostnames, hardware UUIDs, credentials, guest media, or private shader corpus is published.

## License

REIMS and Metal2Vulkan derivative source changes retain their original choice of **GPL-2.0-or-later OR LGPL-3.0-or-later**. The QEMU VNC modification retains QEMU's source-file license. Original descriptive text and aggregate measurement tables are offered under **CC BY 4.0**. Existing upstream licenses and authorship remain credited in the corresponding repository sources.

Publicly discussed findings are limited to [REIMS #157](https://github.com/steelbrain/reims-vgpu/issues/157) and the companion [Metal2Vulkan #17](https://github.com/steelbrain/metal2vulkan/issues/17) at this time. Other historical source patches are offered as research references, not unsolicited upstream submissions.
