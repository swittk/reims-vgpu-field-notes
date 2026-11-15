# Diary 01 — isolate loop-transition stalls (November 7, 2026)

Scope: single anonymized Linux x86-64 GPU host, x86-64 macOS guest, 4K 10-bit HEVC media loop. Source media is not redistributed.

An initial 280-second trace recorded 8,397 presentations, five gaps >60 ms (max 86.74 ms). Two cached HEVC sessions opened about 120 seconds apart, but their opens completed before the large gaps. Neither a new NVDEC allocation nor format negotiation directly overlapped them. A reduced moving-playback trace recorded 8,397 presentations and eight >60 ms gaps (max 81.60 ms), recurring at approximately +0.42 s and +1.8–2.1 s after session open.

A *matched native-player display-only* diagnostic used unchanged encoded media:
- Looping player, 150 s: 59.626 presentations/sec; **6** >60 ms intervals; worst **110.855 ms**.
- Single continuous player item, 150 s: 59.949 presentations/sec; **0** >60 ms intervals; worst **52.678 ms**.

These runs present around 60 FPS, whereas the original screensaver is near 30 FPS. The difference points to an item-transition-specific disturbance but does not by itself identify which framework component causes the delay.

**Measurement correction:** a default Linux perf timestamp was found about 0.920 seconds behind CLOCK_MONOTONIC. Cross-clock correlations were repaired using measured clocks, and a trace collected during *static desktop* was rejected as invalid 30-FPS evidence. The recurrence and within-trace intervals survived these corrections. Subsequent traces explicitly used perf --clockid mono.

Later instrumentation identified costly synthetic contiguous page aliases and optional registration. These observations motivated the memory-layout eligibility fix. They do not prove Apple playback scheduling was solely responsible.

Limits: sequential observations on one anonymized host; no claim of universal effect size.
