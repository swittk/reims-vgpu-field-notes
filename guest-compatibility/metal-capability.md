# Sequoia guest Metal profile and stock Safari WebGL

**Reviewable tested implementation:** [ReimsSequoiaMetal v0.1.0, original-source classification and post-validation hook, build recipe and selected test results](sequoia-metal-profile-implementation/README.md). Only the private bundle identifier was replaced in the public source; the renamed bundle was not reinstalled in a guest.

**Observation:** stock Safari initially refused to initialize its Metal-backed WebGL path even though native Metal drawing and another browser could function. Direct tests of the guest's actual bundled ANGLE library showed that the device's reported Mac2 family requirement failed *before* an unrelated application-level ANGLE flag would help.

In the historical Sequoia 15.x x86 guest, a version-specific guest Metal profile returned 10000 and reported Mac1, but not Mac2. A narrowly guarded local helper changed the recognized profile value to 10001 for only that inspected driver implementation. It did not modify the sealed on-disk Apple binary, bypass SIP, install a general ANGLE shim, lie about arbitrary Apple GPU families, or reduce rendering quality. The helper refused unrecognized driver code.

**After the correction**, original stock Safari (without an injected JS or shared-library workaround) initialized WebGL1/2, created shaders and produced expected GPU readback pixels. Native Managed/Private R8, RGBA8 and BGRA8 clear/blit controls passed all 24 cases, including unusual sizes and full-HD dimensions. Safari and Chromium also passed selected texture/mipmap and lifetime tests; selected graphics workloads ran for 90–120 seconds without context loss.

The reported 3D animation frame rates varied widely across Safari and Chromium and between viewports, so this record is a **compatibility success**, not a claim that WebGL became equally fast across browsers or all graphical apps.

**Caution:** Apple's reported Metal capability families and host-advertised features have correctness implications. A different guest driver or more advanced API should not be enabled by copying one profile number. The exact guest helper, proprietary driver text and binary artifacts are intentionally not republished.
