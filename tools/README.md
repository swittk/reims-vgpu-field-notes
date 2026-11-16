# Verify an archived patch without changing files

Each patch archive contains a **source-contract.json** with SHA-256 of its patch, exact preimage files and exact postimage files. Null preimage means the source file did not exist before that particular patch.

Given a source tree laid out with the same *relative path names appearing in that patch*, check it read-only:

```shell
python3 tools/check_archived_source.py archive/external-write-private-backing before /path/to/recorded-input
python3 tools/check_archived_source.py archive/external-write-private-backing after /path/to/recorded-output
```

The source path must match the patch layout. For example, the external-write bundle uses both `reims/crates/...` and `metal2vulkan-runtime/...` relative paths; it is **not** an upstream Git repository root. The manifest is designed to make this mismatch obvious rather than silently suggest a fresh upstream apply.

The original source patches were each checked by reversing them from the exact historical final source and applying them back to the exact final bytes. This checker simply verifies a researcher-provided tree. It does not build, install, change, or validate current upstream.
