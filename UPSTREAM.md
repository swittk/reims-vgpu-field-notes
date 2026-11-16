# Existing upstream discussions and possible future contributions

No additional upstream issue, PR, or comment has been submitted while publishing the larger archive.

## Published issues

- **[REIMS #157](https://github.com/steelbrain/reims-vgpu/issues/157)** — synthetic scattered guest-page host registration and HEVC loop pacing. The full local source evidence remains on its recorded mixed baseline.
- **[Metal2Vulkan #17](https://github.com/steelbrain/metal2vulkan/issues/17)** — Clear-loaded color attachment self-read and stale glyph/caret pixels, with a bounded diagnostic implementation.

## Families worth evaluating before reporting them separately

- [Remote-task reply teardown deadlock](archive/remote-reply-teardown-deadlock/README.md): deterministic local regression and small source patch; compare the present REIMS task architecture, which has moved beyond the recorded source layout.
- [External-write coherency and GPU-private backing](archive/external-write-private-backing/README.md): significant correctness/performance work with a mixed-source patch; compare current resource/backing semantics and [Metal2Vulkan #11](https://github.com/steelbrain/metal2vulkan/issues/11) before assuming related cause.
- [Guest-memory import permission checks](archive/writable-guest-memory-import/README.md): measured redundant kernel permission queries; inspect current upstream already using assume_writable before proposing a fix.
- [NVIDIA decoder reuse and encoder alignment](archive/backing-revalidation-hevc-reuse/README.md): separate testable video subproblems, but source modules have since changed.
- [Guest-driver FIFO and display timing](guest-compatibility/README.md): very narrow version-specific root-cause reports, not generic host GPU tuning. The separate [REIMS #141](https://github.com/steelbrain/reims-vgpu/issues/141) concerns a different idle-VBL symptom.
- [QEMU unchanged-row VNC scan](archive/qemu-vnc-unchanged-rows/README.md): small differential-tested QEMU patch, with no proven physical-client benefit.

## Related discussions — avoid duplicate or irrelevant comments

[REIMS #85](https://github.com/steelbrain/reims-vgpu/pull/85) and [#121](https://github.com/steelbrain/reims-vgpu/pull/121) examine guest-memory import fallbacks under other driver conditions; [#106](https://github.com/steelbrain/reims-vgpu/pull/106) concerns narrowing direct writes. Our video-loop root-cause issue #157 is distinct.

[Metal2Vulkan #11](https://github.com/steelbrain/metal2vulkan/issues/11) examines shadowing on RADV. Our NVIDIA [selective-byte-store rejection](https://github.com/swittk/metal2vulkan-field-notes/blob/main/diary/selective-byte-store-regression.md) is independent negative evidence, not an explanation of that report.

The archive prioritizes measured causes and source provenance. Current-upstream correctness/performance validation would be needed before opening new PRs. No maintainer acceptance is presumed.
