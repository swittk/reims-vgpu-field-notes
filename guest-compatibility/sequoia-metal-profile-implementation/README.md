# ReimsSequoiaMetal v0.1.0 — stock Safari WebGL on x86 Sequoia

**Research archive of one tested guest-compatibility change; not a general macOS GPU-family implementation or a ready-to-install kernel extension.**

An x86-64 macOS Sequoia 15.8.1 (24H32) guest running through REIMS/Metal2Vulkan on Linux/Vulkan could already render through native Metal and Chromium but stock Safari 18.6 initially failed its Metal-backed WebGL initialization/shader creation. Our inspected AppleParavirtGPUMetal plugin reported a constant feature profile of **10000**; in this particular implementation, the matching profile **10001** selected the Mac2-capable path required by the guest's ANGLE logic.

We tested a version-scoped guest-side helper that changes the exact eleven-byte function sequence from profile 10000 to 10001 while preserving all other inspected bytes and normal Metal resource checks. After that local change, **unmodified Safari** passed WebGL1/2 shader creation and actual GPU readback pixel checks. No injected browser library was required.

## Inspectable source

- [Tested Lilu helper logic (privacy-only bundle-ID substitution)](src/ReimsSequoiaMetal.cpp)
- [Scoped validated-page implementation](src/UserPatch.cpp) and [API contract](src/UserPatch.hpp)
- [Sample bundle metadata](src/Info.plist), [object-only builder](build_objects.py), and [build requirements](BUILD.md)
- [Exact source pattern and metadata](tests/profile-contract.json), [static checker](tests/check_contract.py)
- [Original-vs-published SHA-256 provenance](manifest/source-provenance.json) and [upstream license notes](LICENSE.upstream)
- [Selected live acceptance measurements](tests/compatibility-observations.json)

The original helper reused/adapted UserPatch mechanisms from the public reims-util source at commit **381f913**. Copyright and license conditions stay those of the original project (GPL-2.0-or-later OR LGPL-3.0-or-later) and the separately obtained Lilu SDK/plugin sources. No vendor Apple binaries, kernel collections, VM/boot disks, device identifiers or private logs are included.

## What changed and what was checked

The input is scoped to the inspected AppleParavirtGPUMetal bundled executable on **Darwin 24 x86-64**, with recorded file size, x86 Mach-O UUID, exact eleven-byte function pattern, unique match in the text section, exact file offset, and a **whole validated 4096-byte code page** matching the inspected original file. The helper routes the kernel's page-validation callback, first allows the ordinary validation to complete, then changes the matching code page in RAM. The sealed on-disk file is not modified.

The patch changes the local instruction constant from **10000 to 10001**. It does **not** modify REIMS's GPU wire ABI, FIFO publication, shader compiler, texture bounds/storage modes, or the host renderer. It runs no continuous GPU repair or polling task; its delayed diagnostic is one-shot.

**This mechanism modifies signed executable pages after validation** and is therefore a tightly bounded research workaround with security/maintenance implications, *not* a pattern for generic code-signing bypass or a recommended way to enable arbitrary advertised GPU families. A modern upstream fix should prefer proper feature/capability reporting if possible.

## Live findings

The historical original bundle, built and installed locally with Lilu 1.7.1, showed:
- Before: stock Safari WebGL1/2 did not pass the tested shader-creation smoke. Native Metal and another browser's Metal backend still worked.
- After: stock Safari WebGL1/2 successfully created shaders and rendered the expected readback pixel **RGBA (64,128,191,255)**.
- 24 native Metal R8/RGBA8/BGRA8 Managed/Private clear/blit checks passed; resource types and bounds remained as requested.
- Safari additionally passed selected texture sizes, mipmaps, cube/array sampling, resource lifetime cycles and a 90-second instanced drawing control. One measured Safari 1000-fish scene yielded **15.03 FPS** (real varying pixels); a separately tested Chromium process yielded **34.75 FPS** under its own conditions. These are not a matched causal browser performance comparison.
- Selected Safari/Chromium game checks ran without observed context loss in their bounded windows; this was **not** full Mac2 GPU conformance certification.

The public source is identical to the source of the tested helper except its original private bundle identifier is replaced with `org.reims.fieldnotes.ReimsSequoiaMetal`, and the sample Info.plist is updated accordingly. All three C++ objects were recompiled successfully using the retained dependency versions. The **renamed package was not relinked or reinstalled in the guest**; the historical tested original binary is recorded by SHA-256.

## Review/validation

```sh
python3 tests/check_contract.py
```

This validates the scoped source structure and the one-byte profile-function difference, not the existence/correctness of the proprietary file on another macOS version. See [BUILD.md](BUILD.md) for object compilation prerequisites.

The results are limited to a single historical Linux/NVIDIA host and Sequoia build. In particular, this report does not imply that advertising Mac2 on other guests is correct, that all native Mac2 applications work, or that stock Safari reaches 60 FPS.
