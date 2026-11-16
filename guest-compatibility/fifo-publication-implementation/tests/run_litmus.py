#!/usr/bin/env python3
"""Re-run the finite host-side FIFO publication experiment, never touching any guest."""
import argparse
import json
import platform
import subprocess
import tempfile
from pathlib import Path

base = Path(__file__).resolve().parent
parser = argparse.ArgumentParser()
parser.add_argument("--iterations", type=int, default=500_000)
args = parser.parse_args()
if not (1 <= args.iterations <= 10_000_000):
    parser.error("iterations must be 1 through 10,000,000")
if platform.system() != "Linux" or platform.machine() not in ("x86_64", "AMD64"):
    parser.error("this native TSO litmus is intended for x86-64 Linux")
results = []
with tempfile.TemporaryDirectory(prefix="fifo-publication-") as temporary:
    binary = Path(temporary) / "fifo_publication_litmus"
    subprocess.run(["c++", "-O2", "-std=c++17", "-pthread",
                    str(base / "fifo_publication_litmus.cpp"), "-o", str(binary)],
                   check=True)
    for mode in ("unfenced", "guest-exchange", "guest-fence") * 2:
        completed = subprocess.run([str(binary), str(args.iterations), mode, "double-check"],
                                   check=True, text=True, capture_output=True, timeout=60)
        item = json.loads(completed.stdout)
        if mode != "unfenced" and item["lostWakeups"] != 0:
            raise AssertionError(f"ordered mode {mode} lost {item['lostWakeups']} wakeups")
        results.append({"mode": mode, "iterations": item["loops"],
                        "lostWakeups": item["lostWakeups"],
                        "seconds": item["seconds"]})
        print(json.dumps(results[-1]), flush=True)
print("PASS: all ordered runs returned zero lost wakeups in this finite sample")
print("Note: baseline count varies with scheduling; this is not a live macOS or QEMU test")
