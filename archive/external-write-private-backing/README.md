# External-write freshness and GPU-private legacy texture backing

**State:** an accepted local graphics correctness/performance milestone with real guest-motion and selected GPU validation; **not** a universal 4x speedup claim.

A legacy GPU resource could keep a host-valid cached copy even after an independent device had finished writing to the associated canonical guest-RAM range. Subsequent draws reused stale pixels, occasionally replaying old animated-wallpaper content. The correction observes **completed external writes**, tracks overlapping canonical RAM ranges and generation changes, and invalidates stale guest-authoritative copies without discarding GPU-authoritative data.

The same integrated work adds GPU-private backing for *internal* legacy textures, avoiding repeated host-import/shadow traffic for resources that cannot have an external CPU writer. Imported/caller guest memory retains its existing coherent path. The **14-file historical patch includes 13 REIMS paths and one Metal2Vulkan runtime path**, and should be treated as one recorded experimental integration bundle. The distinct small Metal2Vulkan classification change is linked from its own field-note repository.

**Actual accepted moving 1080p screensaver:** three valid independent 30-second samples with **30.00 presentations/s** and mean GPU utilization **19.87%, 19.83%, and 19.80%**. Pre-stream QEMU checks captured 32/32 distinct successive frames in each of three sequences, with no observed old-frame returns. Focused acceptance included 29 backing tests, four external-generation checks and 31 real-GPU rendering/alias/padding checks.

Earlier correctness-only and other experiments had much higher GPU costs, but were not a randomized matched-code comparison; don't turn their CPU/GPU percentages into a portable speedup figure. A 1.2-second stall in one separate run was excluded from the valid 30-second acceptance and is not hidden here.

Later discard/revalidation and decoder-import work continued from this milestone. The source patch uses a mixed recorded local REIMS/Metal2Vulkan baseline, not a public single-commit upstream base.
