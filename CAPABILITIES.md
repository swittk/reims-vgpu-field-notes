# Tested capabilities and honest boundaries

This documents what a specific independent x86-64 macOS Sequoia guest on a Linux/KVM Vulkan host demonstrated during October–November 2026. It is not a compatibility certification for arbitrary machines or macOS versions.

| Workload | Evidence and scope |
|---|---|
| macOS desktop / Finder / WindowServer | Working accelerated desktop, real GPU captures and sustained moving-wallpaper validation |
| Native Metal and web 3D | Hardware Metal WebGL and native draw/readback tests; a 180-second 3D game exercised full-width geometry and animation with no observed context loss |
| Stock Safari WebGL1/2 | After the exact guest-Metal capability correction, original (unmodified) Safari generated and read back expected pixels; both WebGL1 and 2 were tested |
| Chromium WebGL | Hardware shaders, texture/mipmap lifetimes, interactive input and measured game FPS; not just JS animation callback rate |
| VideoToolbox hardware video decode/encode | NVIDIA NVDEC/NVENC, H.264/HEVC and 8-/10-bit frame reference/pixel checks; the loop handoff fix was validated separately |
| Streaming to a client | QEMU D-Bus frame source, Sunshine hardware encoder discovery, independent audio service and client-side operation; no universal end-to-end latency guarantee |
| Spotlight typing / rasterization | The Clear/self-read correction eliminated ghost glyph/caret groups in bounded same-binary on/off tests |
| Guest lifecycle | Version-specific virtual-SMC conflict resolved; several normal macOS shutdown/start cycles, with the VM's existing disk preserved |
| iOS Simulator | **Reported working by the system operator**, but no standalone, publishable iOS Simulator test receipt was found among the curated records. This archive does not label it independently verified or universally perfect. |

Selected quantified observations are in the [source archive](archive/README.md), [loop investigation](diary/03-accepted-implementation.md), and [guest integration notes](guest-compatibility/README.md).

**Metric boundaries:** browser requestAnimationFrame rate, guest GPU command execution, QEMU presentation callbacks, QEMU capture transport, Sunshine encoder readiness and distinct Moonlight-displayed frames are *not the same measurement*. Some observed 60 FPS browser tests delivered fewer distinct host frames. Record the stage before reporting FPS or latency.

This work was performed on one host at a time with controlled, often sequential measurements. Hardware differences, source revisions, and workloads limit generality.
