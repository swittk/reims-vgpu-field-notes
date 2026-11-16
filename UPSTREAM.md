# Existing upstream discussions and possible future contributions

The FIFO guest-driver lost-wakeup issue has now been filed as [REIMS #158](https://github.com/steelbrain/reims-vgpu/issues/158). A separate display-timing issue and Safari/Metal-profile compatibility issue remain only proposals; neither has been filed.

## Published issues

- **[REIMS #158](https://github.com/steelbrain/reims-vgpu/issues/158)** — guest FIFO producer publication memory-ordering race, with captured pending commands and a [reviewable Lilu helper](guest-compatibility/fifo-publication-implementation/README.md).

- **[REIMS #157](https://github.com/steelbrain/reims-vgpu/issues/157)** — synthetic scattered guest-page host registration and HEVC loop pacing. The full local source evidence remains on its recorded mixed baseline.
- **[Metal2Vulkan #17](https://github.com/steelbrain/metal2vulkan/issues/17)** — Clear-loaded color attachment self-read and stale glyph/caret pixels, with a bounded diagnostic implementation.

## Families worth evaluating before reporting them separately

- [Remote-task reply teardown deadlock](archive/remote-reply-teardown-deadlock/README.md): deterministic local regression and small source patch; compare the present REIMS task architecture, which has moved beyond the recorded source layout.
- [External-write coherency and GPU-private backing](archive/external-write-private-backing/README.md): significant correctness/performance work with a mixed-source patch; compare current resource/backing semantics and [Metal2Vulkan #11](https://github.com/steelbrain/metal2vulkan/issues/11) before assuming related cause.
- [Guest-memory import permission checks](archive/writable-guest-memory-import/README.md): measured redundant kernel permission queries; inspect current upstream already using assume_writable before proposing a fix.
- [NVIDIA decoder reuse and encoder alignment](archive/backing-revalidation-hevc-reuse/README.md): separate testable video subproblems, but source modules have since changed.
- [Guest-driver FIFO and display timing](guest-compatibility/README.md) (including [timing helper source](guest-compatibility/display-timing-implementation/README.md)) (with the [reviewable Lilu FIFO helper source and tests](guest-compatibility/fifo-publication-implementation/README.md)): very narrow version-specific root-cause reports, not generic host GPU tuning. The separate [REIMS #141](https://github.com/steelbrain/reims-vgpu/issues/141) concerns a different idle-VBL symptom.
- [QEMU unchanged-row VNC scan](archive/qemu-vnc-unchanged-rows/README.md): small differential-tested QEMU patch, with no proven physical-client benefit.

## Related discussions — avoid duplicate or irrelevant comments

[REIMS #85](https://github.com/steelbrain/reims-vgpu/pull/85) and [#121](https://github.com/steelbrain/reims-vgpu/pull/121) examine guest-memory import fallbacks under other driver conditions; [#106](https://github.com/steelbrain/reims-vgpu/pull/106) concerns narrowing direct writes. Our video-loop root-cause issue #157 is distinct.

[Metal2Vulkan #11](https://github.com/steelbrain/metal2vulkan/issues/11) examines shadowing on RADV. Our NVIDIA [selective-byte-store rejection](https://github.com/swittk/metal2vulkan-field-notes/blob/main/diary/selective-byte-store-regression.md) is independent negative evidence, not an explanation of that report.

The archive prioritizes measured causes and source provenance. Current-upstream correctness/performance validation would be needed before opening new PRs. No maintainer acceptance is presumed.

- [Sequoia Safari WebGL / Metal-profile correction](guest-compatibility/sequoia-metal-profile-implementation/README.md): exact-version guest compatibility helper is archived with source, measurements and licensing. Not a ready-to-merge general feature-profile change. Check any newer guest driver before filing.

## Architecture compatibility caution (current master)

Current [REIMS graphics-kext design](https://github.com/steelbrain/reims-vgpu/blob/818868564e31f9023c074f73840de97542592c91/docs/graphics-kext/design.md) describes a new REIMS-owned guest graphics device (reims-graphics-pci) with its own kext, replacing the historical AppleParavirtGPU graphics lane. Our Sequoia FIFO, detailed-timing, and feature-profile helpers were measured against the *older Apple driver path*. They remain useful for installations on that older path, but should **not** be reported as currently reproducible defects in the new guest driver without matching evidence. The separate [native Apple FIFO comparison](guest-compatibility/native-apple-fifo-comparison.md) has its own scope limitations.
