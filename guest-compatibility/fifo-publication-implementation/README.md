# ReimsFifoOrder v0.1.3 — Sequoia FIFO lost-wakeup case study

**Source available for review; not an upstream-approved fix or universal macOS driver patch.** This is the small Lilu plugin used in our historical x86-64 macOS Sequoia test. The published C++ is the tested source logic with **only the original private bundle identifier replaced** by `org.reims.fieldnotes.ReimsFifoOrder`. The matching Info.plist has the same identifier substitution. The posted source was independently recompiled, but **the sanitized package is not byte-identical to the deployed 0.1.3 kext**. Its historical original-source and executable SHA-256 digests are in [source provenance](manifest/source-provenance.json).

## Contents

- [Actual Lilu plugin implementation](src/ReimsFifoOrder.cpp) — inspected symbol and 346-byte function boundary, exact 34-byte instruction preimage, one-byte `MOV` → `XCHG` change, verification state, and fail-closed conditions.
- [Matching kext Info.plist](src/Info.plist) — version 0.1.3, Lilu 1.7.1 dependency and a non-private example bundle identifier.
- [Portable object-compilation recipe](build_objects.py) and [build prerequisites](BUILD.md) — adapted to work outside the original local project layout.
- [Native x86-64 conditional-doorbell litmus test](tests/fifo_publication_litmus.cpp), [runner](tests/run_litmus.py), and [recorded six-run results](tests/native-litmus-observations.json).
- [Static patch and metadata contract check](tests/check_contract.py) and [independent bounded store-buffer protocol model](tests/storebuffer_model.py).

## Captured failure and why this code exists

The live guest had **416 unread FIFO bytes, 16 valid pending commands, and an outstanding stamp**, while the host's six FIFO worker threads were sleeping. The original guest driver could publish its producer counter with an ordinary store and then decide against sending a doorbell based on a stale consumer-counter observation. The host's double-check also could miss a producer update still in the guest's CPU store buffer, leaving work pending without either side planning a wake.

The inspected version of `AppleParavirtVirtualChannel::submitBuffer` has one 34-byte sequence inside the observed 346-byte function. The **only intended instruction change** is `89 08` (`mov [rax], ecx`) → `87 08` (`xchg [rax], ecx`, implicitly locked). That supplies StoreLoad ordering before the consumer-count test. An independent store-buffer model excluded the lost-wakeup interleaving with ordered publication; the native host litmus below produced **93,970 / 137,553** lost wakeups in two unfenced five-million-iteration runs versus **zero** in two exchange runs and two full-fence controls.

In the live test, Lilu 1.7.1 loaded the original v0.1.3 helper on Sequoia **15.8.1 (24H32)**. It reported `PublicationOrdered=1`, `PublicationInstruction=2183` (`0x0887`), and `PublicationVerificationState=7` (patch applied and read back). We subsequently ran bounded interactive hardware-Metal, 3D game and idle/wake tests without an observed recurrence of that captured FIFO freeze.

The helper's delayed callback is a **one-shot diagnostic report** of the recorded patch state; it does not inspect the FIFO, periodically wake it, complete commands or advance GPU stamps.

## Scope and limitations

- Scoped to the observed **Darwin 24 / x86 Sequoia** guest-driver code shape. There is a kernel-version guard, exact function symbol, bounded search, unique expected 34-byte preimage and required offset. It **does not** establish that every Darwin 24 build uses this exact instruction pattern, or that later macOS versions require the same change.
- The tested runtime source contained its original private bundle identifier; this public source substitutes a generic identifier solely for publication. The **logic and instruction bytes are unchanged**, and we successfully compiled the sanitized C++ into Mach-O objects, but did **not** load this renamed package in a guest or relink a new kext for publication.
- The original binary, Apple driver/kernel collections, system dumps, NVRAM, boot configs, device identifiers, host paths and guest logs are **not** in this repository.
- Our implementation is a **guest-side corrective experiment**, not a host-side `reims-vgpu` patch; maintainers may prefer other protocol approaches or find the bug already absent in a newer guest-driver version.
- Live acceptance is finite. It does not prove that all GPU freezes, stamp waits, or host resource deadlocks are the same issue.

See the [narrative report](../fifo-publication.md) for the original problem, and [REIMS PR #110](https://github.com/steelbrain/reims-vgpu/pull/110) for *different* host-side async-harvest/doorbell gating work. We are not claiming #110 is a duplicate.

## Quick independent checks

Requires Python 3 and a C++17 compiler; the native litmus is for x86-64 Linux with at least three schedulable logical CPUs.

```sh
python3 tests/check_contract.py
python3 tests/storebuffer_model.py
python3 tests/run_litmus.py --iterations 100000
```

For the historical five-million-iteration exposure, substitute `--iterations 5000000`. Results vary with scheduling and CPU architecture; the native test is **not** a substitute for a guest freeze reproduction. See [BUILD.md](BUILD.md) for the optional kernel-plugin compilation prerequisites and verification scope.

Original research source and build artifacts remain on the operator's private test machine; only reviewable source and aggregate evidence are published here. Source code follows the repository's GPL-2.0-or-later OR LGPL-3.0-or-later licensing; external Lilu, MacKernelSDK and cctools sources retain their own licenses and are not vendored.
