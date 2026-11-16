# Native Apple FIFO wait-path comparison (Sequoia x86-64)

**Supplementary finding from the original Sequoia 15.8.1 (24H32) investigation**, not proof that REIMS has an independent host bug and not a complete native-Apple-host VM benchmark.

We inspected the installed Apple ParavirtualizedGraphics framework through a bounded, unprivileged Objective-C runtime and Mach-O import diagnostic. Only the *conclusions* are published here; the Apple framework binaries and raw disassembly remain local.

- The inspected `PGFIFO::poke` acquires a mutex, calls ordinary `condition_variable::notify_one`, then unlocks.
- The inspected `PGChildFIFO::getFifoBytes:into:` checks available FIFO bytes, takes its queue mutex when empty, rechecks, and invokes ordinary untimed `condition_variable::wait(unique_lock<mutex>&)`.
- The imported wait routine was not a `wait_for` or `wait_until` overload. This specific empty-queue path does **not** simply wake every 100 ms to check again.
- The inspected `PGChildFIFO::advance` publishes the consumed/read counter with a normal store and subsequently uses a separate queue-lock ordering point. No general claim about other native-host wake paths follows.

These observations rebut a tempting but unsupported explanation that Apple's native FIFO worker reliably avoids stranded work through periodic 100 ms polling. The distinction matters because our independently captured [REIMS #158](https://github.com/steelbrain/reims-vgpu/issues/158) showed **16 valid commands queued in guest memory while host FIFO workers slept**. The guest producer publication-ordering analysis and [version-specific tested Lilu correction](fifo-publication-implementation/README.md) address that protocol race without adding a periodic wakeup.

## Limits

We did **not** instantiate an x86 macOS guest on a native Apple host and compare its FIFO event stream against REIMS. Differences in cache attributes, wake sources, ordering in other code, and scheduling remain unmeasured. The findings therefore do not establish that native macOS hosts never experience lost notifications or that the x86 guest issue must be in host REIMS code.

This is a reverse-engineering observation, not redistributed Apple code. There are no proprietary Apple binaries, method dumps, original VM assets, machine identifiers, credentials or host logs in this archive.
