# Skip redundant tile comparisons for entirely unchanged VNC framebuffer rows

**State:** modest local QEMU CPU optimization, validated with differential tests; no proven user-visible latency/FPS gain.

The original QEMU VNC dirty-region scanner compared 16-pixel tiles even when an entire row matched the previous buffer. The tiny patch compares rows once where both buffers have the same format. Fully unchanged rows clear only the relevant dirty marks; changed rows and foreign-format paths keep the preexisting scanner behavior.

The actual old and new C scan functions passed **1,500 randomized differential cases**, plus AddressSanitizer/UndefinedBehaviorSanitizer runs. CPU-only sparse 1080p scans improved from about **0.88–1.07 ms** to **0.63–0.65 ms** per scan. Full-motion trials were effectively neutral. In a finite decoded-client test, before/after delivered about 43.47/43.70 FPS and measured about 108.45/105.45 ms input latency; those tiny deltas were within variation and are not established causal user gains.

The one-file patch targets QEMU **ui/vnc.c**; it is not a REIMS Rust change, not an encoder/streaming acceleration, and has not been tested against current upstream QEMU. Source modifications follow the upstream QEMU source license. Given the current stack's hardware streaming path, this is low priority for upstream discussion.
