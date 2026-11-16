# Copy packed decoded planes directly to canonical guest RAM

**State:** tested intermediate improvement; a prerequisite to the later structural import decision, but *not by itself* a successful final loop-pacing fix.

NVIDIA decoded readback already supplied cropped, tightly packed CPU-accessible rows. Creating a second contiguous guest-side alias solely to copy those bytes could fault thousands of unrelated page-table entries. The optional local implementation exposes validated packed plane descriptors and performs synchronous row/region copies directly to the guest surface's canonical pages, while preserving padding and bounds, exact ordering and the existing slow-path behavior for other decoder backends.

The twelve-file recorded patch spans the generic video backend, decoded-frame surfaces, Vulkan/Linux mapping access and NVDEC packed staging. The normal copy fallback remains for unsupported layouts. Focused tests checked exact crop rows, destination geometry, untouched padding and alias-page-table avoidance.

Faster canonical copies alone did not remove all periodic 4K transition stalls. Later measurements still showed the additional optional-registration cost, addressed by the final memory-layout eligibility rule reported in [REIMS issue #157](https://github.com/steelbrain/reims-vgpu/issues/157).

This source is an incremental historical patch against a locally integrated mixed base. It is not directly applicable to current upstream master, nor a claim of universal faster decoding.
