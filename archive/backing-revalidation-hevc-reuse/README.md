# Cache invalidation without needless remapping, and one retained HEVC decoder

**State:** source-level fixes and actual GPU regressions passed; the **combined live candidate still had large presentation stalls**. This is not the final video-loop correction.

Two independent improvements were packaged together:

**Backing discard revalidation.** A cache discard properly releases guest-memory mapping/transfer buffers but need not automatically destroy already-valid private GPU pixel data. The implementation retains only canonical binding identity and write-generation metadata, then re-walks the actual guest pages before reuse. Equality requires ordered physical ranges, repeated pages, offsets and no missed external writes; checks handle racing discards, changed/reordered pages and invalidation. In the unchanged-binding regression, unnecessary mapping recreation and upload calls dropped to zero without retaining the original mapping.

**Retain the real decoder.** A hardware HEVC stream opened a disposable four-surface capability probe, destroyed it, then allocated a second correctly sized decoder for its first picture. The revised path checks suitability with the correct decoder and retains that allocation, reusing it only when the complete stream specification matches and it is healthy. An actual-GPU count test changed **two allocations to one**, and different spec/stalled status correctly declined reuse.

The combined release build completed ten stages; focused source suites covered 223 CPU checks, two decoder lifecycle GPU cases and 31 selected renderer GPU checks.

**Important negative result:** the combined 180-second moving screensaver candidate still had presentation gaps, including five seconds with gaps above 100 ms and a worst one-second gap of roughly 1,013 ms. It cannot be cited as proof that the loop issue was solved. The later canonical-copy/layout-eligibility fix addressed a separate cost.

The historical six-file bundle is preserved for separate adaptation, not offered as a clean mergeable upstream PR. Some before-hashes in an earlier full-tree snapshot differ from this incremental patch's actual preimages; this archive records verified *patch-specific* preimages.
