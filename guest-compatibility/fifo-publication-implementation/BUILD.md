# Build prerequisites and source provenance

This is a **source review and C++ object-compilation record**, not a ready-to-install kext release.

The local, live-verified v0.1.3 was built with Clang 19, the public Lilu 1.7.1 plugin headers and `plugin_start.cpp`, MacKernelSDK headers, and a cctools-port `ld64` supporting `-kext` / `MH_KEXT_BUNDLE`. LLVM `lld` in this setup did not produce an appropriate kext binary when given the older kernel-extension linker mode.

Recorded external source revisions:
- [MacKernelSDK](https://github.com/acidanthera/MacKernelSDK) commit `3f750085caa17ec3a7880f11c11bf4f48cd6a164`
- [cctools-port](https://github.com/tpoechtrager/cctools-port) commit `df82634ce7c3eddba9722e881433fbe357396e16`
- [Lilu](https://github.com/acidanthera/Lilu) 1.7.1, with matching SDK and `Library/plugin_start.cpp`.

### Compile the source only

Obtain the compatible dependencies separately and provide their header locations. **No Apple driver or kernel collection is required for this object compilation.**

```sh
export LILU_RESOURCES="/path/to/Lilu.kext/Contents/Resources"
export MAC_KERNEL_SDK_HEADERS="/path/to/MacKernelSDK/Headers"
python3 build_objects.py
```

The script compiles `src/ReimsFifoOrder.cpp` and Lilu's external `Library/plugin_start.cpp`, generating only temporary objects in the ignored `build/` directory. It uses the same cross-compilation flags as the historical object build (including `-fapple-kext` and the Clang 19 global-destructor lowering control). The published source was successfully object-compiled with those flags in a clean, network-disabled Linux container using the original pinned headers.

**A complete installable kext is not included.** Linking requires an appropriate `ld64 -kext` toolchain, a genuine Darwin kernel-extension output type, the correct unresolved kernel/Lilu symbols, and matching Info.plist. The original local package validated `MH_KEXT_BUNDLE`, imports against its own guest snapshot, and Lilu-loaded verification state 7. Those original snapshot-specific binaries are deliberately not included.

The published C++ differs from the tested source by the private bundle identifier only. [The provenance manifest](manifest/source-provenance.json) distinguishes the original source hash, published source hash, tested kext binary hash and actual guest validation. Do not call the public sanitized source byte-identical to the original compiled artifact. No current REIMS master integration, downstream distro support or independent installability is being claimed.

### Contract verification without compilation

```sh
python3 tests/check_contract.py
```

This checks the two 34-byte sequences differ in exactly one instruction byte, that the expected function/offset/version guards and verification checks exist, that the published C++ and Info.plist bundle identities agree, and that source SHA-256 matches the provenance file. It does not prove the patched instruction is appropriate for other versions of Apple's driver.
