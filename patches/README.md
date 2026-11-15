# Source patch contracts

**complete-loop-fix.patch:** complete 17-file local change from a **mixed recorded baseline**, using bundle-relative paths (reims/crates/... plus zero_copy.rs). In the REIMS workspace zero_copy.rs corresponds to crates/reims-vgpu-video-nvidia/src/zero_copy.rs. Not a standalone Git cherry-pick onto current upstream master.

**native-import-only.patch:** final layout-eligibility change relative to the **already integrated** local source, including earlier memory/staging groundwork. It cannot independently be applied to upstream master. Avoid guessing.

The SHA-256 before/after hashes in manifest/complete-patch-bases.json are the source identity contract. Given a reconstructed bundle root, check with:

```sh
python3 tools/check_patch_contract.py before /path/to/bundle-root
git -C /path/to/bundle-root apply /path/to/complete-loop-fix.patch
python3 tools/check_patch_contract.py after /path/to/bundle-root
```

The manifest establishes exact source bytes; a passing hash check does not imply full source-tree buildability, correctness on other GPUs, or current-upstream integration. All experiments should preserve old behavior when a new import or device capability is declined.
