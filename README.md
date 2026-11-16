# REIMS vGPU: video handoff and guest-memory field notes

A public, privacy-reviewed archive of implementation patches, experimental findings, and negative results from a Linux/Vulkan macOS-guest investigation.

**Reference material, not an official release or ready-to-merge upstream PR.** No private guest images, media, firmware, system dumps, raw host logs, hardware identifiers, credentials, IP addresses, hostnames, or absolute machine-specific paths are distributed.

## Main finding

A repeating 4K HEVC video workload exposed costly synthetic contiguous aliases over scattered guest IOSurface pages, in the optional host registration path. The tested correction **declines optional direct registration for scattered synthetic aliases**, while keeping registration for already contiguous memory. The canonical row-copy fallback remains available and hardware video decode/encode stays enabled.

On one anonymized Linux x86-64 NVIDIA host, the final 260-second original screensaver capture produced approximately 30 FPS, with no QEMU presentation interval above 60 ms (worst 59.6 ms). An independent 150-second native-player loop ran at 59.94 FPS with zero >60 ms intervals (worst 55.36 ms). One isolated 61 ms *capture transport* interval was observed away from loop transitions. These bounded, single-host results do not establish universal zero-jitter behavior.

## Repository index

- [Complete 17-file tested delta](patches/complete-loop-fix.patch) relative to a **mixed, recorded local source baseline**.
- [Narrow final host-import selection delta](patches/native-import-only.patch) relative to the **already modified source**, not independently applicable to upstream master.
- [Per-file baseline and output SHA-256 manifest](manifest/complete-patch-bases.json), plus [contract checker](tools/check_patch_contract.py).
- [Patch applicability details](patches/README.md).
- [Loop-causality diary](diary/01-player-loop.md).
- [Rejected approaches diary](diary/02-approaches-and-failures.md).
- [Accepted implementation and test limits](diary/03-accepted-implementation.md).
- [Final normalized results](results/loop-control.json) and [negative-control results](results/rejected-candidates.json).
- [Potential upstream discussion and already-open related work](UPSTREAM.md).

A separate Spotlight ghost-text render-pass issue is tracked at [Metal2Vulkan #17](https://github.com/steelbrain/metal2vulkan/issues/17); this is **not** the VideoToolbox loop defect.

The Metal2Vulkan-specific investigation is published separately in [metal2vulkan-field-notes](https://github.com/swittk/metal2vulkan-field-notes).

## Upstream issue

The bounded VideoToolbox/scattered IOSurface finding is filed at [REIMS #157 — Synthetic host registration and video-loop stalls](https://github.com/steelbrain/reims-vgpu/issues/157). This is a research issue, not an upstream-ready pull request.

## Important patch limitation

The final REIMS patch was built from source files coming from more than one recorded local baseline. **There is no single public upstream Git commit to which this 17-file patch can truthfully be said to apply.** A file-by-file SHA contract is provided so researchers can inspect or reconstruct the particular input and output. Upstream integration requires rebase, dependency reconciliation, and independent build/test on current master.

## License and provenance

Source modifications inherit **GPL-2.0-or-later OR LGPL-3.0-or-later** as described in LICENSE and LICENSES/. We offer original measurement tables and explanatory prose under CC BY 4.0. This is independent of [steelbrain/reims-vgpu](https://github.com/steelbrain/reims-vgpu). No upstream issue or PR is claimed to have been posted yet.
