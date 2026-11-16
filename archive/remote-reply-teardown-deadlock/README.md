# Last-reference reply destruction must not block its own completion queue

**State:** deterministic source-level correctness fix, verified locally; no evidence that this was the cause of any particular historical user-visible VM freeze.

In the earlier REIMS remote-task architecture, a reply could release its final RemoteTask reference while running on the link's delivery queue. Task destruction synchronously drained an outstanding GPU completion, but the completion message was already queued **behind the currently executing reply on that same queue**. That creates an avoidable self-deadlock.

The historical correction introduces a reply-held reference wrapper. It releases non-last references normally; the actual last reference is released through the existing off-thread release helper, while retaining platform/thread-service lifetime and the required token-before-task and handshake task-before-endpoint release ordering. It does not synthesize completion, skip draining, or add a product timeout.

A fixed-order regression demonstrated a 10.01-second expected hang before the change versus completion in 0.01 seconds afterward. The focused remote-task group passed **104 tests** after the correction. The published one-file patch is derived from an exact recorded Git baseline, independently replayed byte-for-byte.

**Upstream status:** current REIMS source has substantially reorganized task components, and the old file path may no longer exist. Review the invariant in the new architecture before considering any source port; do not suggest this is a demonstrated current-upstream deadlock or a patch that applies to current master.
