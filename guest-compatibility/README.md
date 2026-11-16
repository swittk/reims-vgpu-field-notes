# Guest-specific compatibility and operational findings

Some of the most valuable work was **not a general REIMS Rust optimization**. These are evidence summaries of targeted guest-driver, boot, presentation and streaming problems. Because the fixes depend on proprietary driver versions or locally configured services, this archive publishes the mechanisms and aggregate test results but **not Apple binaries or machine-specific deployment state**.

| Investigation | Validated finding |
|---|---|
| [Lost FIFO notification](fifo-publication.md) ([Lilu helper source and reproducer](fifo-publication-implementation/README.md)) | Guest producer publication order can let the host miss an empty-to-nonempty wake, solved in an exact guest-driver experiment without polling |
| [Sequoia Metal capability and Safari WebGL](metal-capability.md) ([source and tests](sequoia-metal-profile-implementation/README.md)) | A version-scoped capability-profile correction let stock Safari use WebGL1/2 with actual pixel checks |
| [Display-link timing](display-timing.md) ([source and evidence](display-timing-implementation/README.md)) | Correcting detailed-timing validity removed ±seconds timestamp anomalies in on/off/on tests at 60 Hz |
| [SMC and shutdown](smc-and-shutdown.md) | Two competing AppleSMC providers led to reproducible recursive shutdown panic; one provider + correct shutdown request solved measured cycles |
| [D-Bus frame source, Sunshine and audio](streaming.md) | Publishing a frame and having an encoder client bind to it are distinct readiness stages; integration is possible without custom VM-specific repair commands |

**None** of these documents should be interpreted as a generic host-performance PR or a universal Apple driver patch. The focus is on reproducible symptoms, causal observations and the relevant protocol contract.
