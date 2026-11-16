# Upstream issue #157 (filed; no PR)

Proposed title: **Linux/Vulkan VideoToolbox: synthetic scattered-page host registration can stall looping playback**

Share the actual recurring stalls, the structural fix, negative mapping experiments, observed CPU tradeoff, and a precise statement that our tested patch has NOT been rebased against current upstream master.

Files worth reviewing in the published patch:
- reims-vgpu-vulkan/src/os/linux/device_lend.rs — natural-layout eligibility and optional host registration
- reims-vgpu-vulkan/src/os/linux/surface.rs — whether a surface truly has contiguous host backing
- reims-vgpu/src/platform/vulkan/lent_memory.rs — refusal and canonical copy fallback
- reims-vgpu-video-nvidia/src/zero_copy.rs — prerequisite decoder integration; not the narrow final patch
- reims-vgpu-core/src/resources/texture/backing.rs — immutable staging and guest-write observation prerequisites

Do not open duplicate upstream issues for adjacent problems:
- [REIMS #121: fallback when backend refuses guest RAM import](https://github.com/steelbrain/reims-vgpu/pull/121).
- [REIMS #106: narrower imported GPU writes](https://github.com/steelbrain/reims-vgpu/pull/106).
- [REIMS #111: pinned resident growth](https://github.com/steelbrain/reims-vgpu/issues/111), a different lifetime symptom.
- [metal2vulkan #11: RADV shadowing regression](https://github.com/steelbrain/metal2vulkan/issues/11), which was measured on a different platform.

The issue has been filed, but there is no upstream acceptance or merge. Do not publish proprietary source media or private machine data.
