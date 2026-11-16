# Two SMC providers, recursive guest shutdown, and the correct stop request

A historical guest shutdown reliably entered macOS's final shutdown phase and then panicked. This was a real guest kernel teardown failure, not a Docker process timeout.

Forensic inspection found both QEMU's legacy SMC device and the guest's OpenCore VirtualSMC provider active. The AppleSMC handler's saved predecessor pointed back to the active handler; the captured stack contained **174 matching return addresses** with 173 intervening identical **80-byte frame strides**, establishing recursion rather than guessing from a hang.

The local configuration correction was to **use exactly one SMC provider** and refuse duplicate-provider startup. The accepted configuration kept VirtualSMC. In five controlled full restart/inventory cycles, shutdown completed normally and each next boot showed one AppleSMC instance. This was a configuration/root-cause fix, not a Metal2Vulkan shader patch.

Another independently observed issue was that a virtual ACPI power-button request could be recorded by QEMU while the macOS guest continued running. The guest's native Control–Option–Command–Power shortcut instead led to QMP guest-SHUTDOWN and a normal container exit in the recorded same-boot comparison. This does not prove every guest can always shut down, especially with unsaved-document dialogs.

These observations are useful for deployment/documentation and for distinguishing **stuck before shutdown** from **stuck during actual guest shutdown**. They should not be attached to unrelated Vulkan performance issues.

Only general configuration logic and aggregate results are published. Original kernel snapshots, code bytes and machine-specific boot files stay private.
