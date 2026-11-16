# Research methods and limits

The goal was to identify actual correctness and frame-timing bottlenecks, not produce isolated microbenchmark wins.

1. **Pin the source and workload.** Record exact source-file hashes and which guest media/window was active. A static desktop or failed app launch is never valid moving-content evidence.
2. **Keep stages separate.** Browser RAF counts are not delivered frames; host capture transport timestamps are not QEMU presentation timestamps; GPU utilization is not a throughput guarantee.
3. **Use before/after/return controls when practical.** Match image configuration, screen dimensions and decoder settings. Trials from different boots, scene loads and instrumentation are reported as sequential observations, not randomized experiments.
4. **Test bytes and lifetimes, not only throughput.** External guest writes, row padding, resource generation, aliases, device returns and decoder state matter. The accepted source cases retain fallbacks for ineligible layouts.
5. **Inspect overhead from diagnostics.** Even apparently read-only probes may alter scheduling. Timestamp clock domains must be calibrated before cross-trace inference.
6. **Record failures honestly.** The selective-byte-store candidate passed narrow tests yet dropped ~30 FPS to ~6.47 FPS; direct external host-image rendering measured ~12.47 presentations/s and ~89.73% GPU usage. These were rejected rather than introduced as vendor-specific performance switches.
7. **Separate platform workarounds from source algorithms.** Guest-driver FIFO and display timing patches were exact-version guarded. They do not imply portable code suitable for every guest.
8. **Treat tests as finite evidence.** A 30-second accepted clip does not imply an entire day without stalls; an on/off/on can support causality without asserting cross-platform behavior.

Raw traces, screenshots, proprietary guest binaries and identifying machine details remain private. Reproducible source-level patches are provided with before/after digest contracts instead.
