# Optional source-only compilation

The original Sequoia guest compatibility helper is a version-scoped Lilu plugin. This publication provides **reviewable source**, not a signed or installable kernel extension. It is not intended as a general-purpose post-validation binary patcher.

The local version used Clang 19, [Lilu 1.7.1](https://github.com/acidanthera/Lilu) plugin headers and public Library/plugin_start.cpp, [MacKernelSDK](https://github.com/acidanthera/MacKernelSDK) commit `3f750085caa17ec3a7880f11c11bf4f48cd6a164`, and for linking an original full kext, [cctools-port](https://github.com/tpoechtrager/cctools-port) commit `df82634ce7c3eddba9722e881433fbe357396e16`.

For **object compilation only**, obtain those public SDK headers separately and set:

```sh
export LILU_RESOURCES="/path/to/Lilu.kext/Contents/Resources"
export MAC_KERNEL_SDK_HEADERS="/path/to/MacKernelSDK/Headers"
python3 build_objects.py
```

The script builds `ReimsSequoiaMetal.cpp`, `UserPatch.cpp`, and Lilu's public `plugin_start.cpp` into ignored `build/` object files. We verified the published source compiles with Clang 19 in an offline, isolated builder. This is **not** evidence that the renamed bundle installs or executes.

A full original kext build requires a Darwin ld64 kernel-extension link, MH_KEXT_BUNDLE validation, appropriate symbol/ABI resolution against the intended guest, correct user-code validation lifecycle, and version-specific runtime testing. The historic binary's SHA-256 appears in [source provenance](manifest/source-provenance.json). No complete tested kext is redistributed.

For offline, read-only source checks:

```sh
python3 tests/check_contract.py
```

The original local nine-case profile-contract test exercised the inspected Apple executable. That file is not distributed; the published checker verifies source contracts and intentionally does **not** claim the same binary-specific coverage. The helper deliberately touches executable code after ordinary page validation; this has security implications, so the matching and fail-closed behavior must be reviewed before attempting any adaptation.
