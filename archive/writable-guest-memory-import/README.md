# Avoid redundant Linux permission checks on proven-owned guest mappings

**State:** historical locally validated optimization; source patch replay verified. **Scope:** REIMS Linux/Vulkan guest-memory import, not a renderer algorithm change.

On a Linux kernel without PROCMAP_QUERY, a growing QEMU address space forced repeated parsing of procfs mapping data for host imports. In one sampled FIFO-thread profile, approximately 76.8% of CPU-cycle-weighted samples were inside the runtime's writability refusal check and its mapping parser. The workload had about 20,000 virtual-memory areas. This is attribution of a sampled hotspot, not a claim that 76.8% of total application time was saved.

The tested change uses already-held ownership, complete-range writability and mapping-lifetime evidence for REIMS-owned windows. It avoids rechecking kernel permissions for such proven ranges, while unknown or caller-provided ranges still use the original kernel validation. The targeted live-GPU tests counted two kernel queries per import/rebind before the change and zero afterward, with the same expected bytes. Untracked imports still took the normal permission checks. The frozen candidate passed 28 targeted import/backing/lifetime checks.

**Caution:** do not replace the permission check with unconditional trust, a stale VMA cache, or a device-ID exception. The API that certifies writable ranges has changed in current upstream. This patch is an implementation reference against the old source layout, not an upstream-master cherry-pick.

The exact historical patch and a 6-file source-hash contract are alongside this note. The archive's reported performance benefit is specifically the removal of redundant kernel queries; we did not establish a clean application-frame-rate A/B caused solely by this patch.
