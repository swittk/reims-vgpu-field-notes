# Build information — source review, not a guest installer

The recorded v0.1.0 helper was a version-scoped Lilu guest experiment. The public source is not an independently tested installable kext.

Dependencies:
- [Lilu 1.7.1](https://github.com/acidanthera/Lilu), including public plugin headers and Library/plugin_start.cpp.
- [MacKernelSDK](https://github.com/acidanthera/MacKernelSDK), commit 3f750085caa17ec3a7880f11c11bf4f48cd6a164.
- [cctools-port](https://github.com/tpoechtrager/cctools-port), commit df82634ce7c3eddba9722e881433fbe357396e16, if full Darwin kernel-extension linking is attempted.

Clang 19 is used with -fapple-kext, no C++ runtime, and the global-destructor lowering guard.

```sh
export LILU_RESOURCES="/path/to/Lilu.kext/Contents/Resources"
export MAC_KERNEL_SDK_HEADERS="/path/to/MacKernelSDK/Headers"
python3 build_objects.py
```

This compiles source and public Lilu plugin_start.cpp into ignored build/ objects, without any proprietary Apple kernel collection. We verified that object compilation works in a network-disabled Linux container with the original pinned headers.

A fully installable guest helper requires a suitable ld64 -kext (not generic lld), valid MH_KEXT_BUNDLE linking, symbol/ABI validation against the target guest, and verified kernel placement. None of that is claimed for this sanitized publication; the original tested compiled executable's SHA-256 is recorded in [source provenance](manifest/source-provenance.json).

For source/measurement checks without building:

```sh
python3 tests/check_contract.py
python3 tests/check_clock_evidence.py
```

The 52-byte exact-method fingerprint exists for evaluation and fail-closed routing; it is not a universal signature for every Sequoia or Tahoe build.
