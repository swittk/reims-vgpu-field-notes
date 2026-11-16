#!/usr/bin/env python3
"""Build only C++ objects; requires separately obtained public Lilu and MacKernelSDK headers."""
from pathlib import Path
import os
import subprocess

root = Path(__file__).resolve().parent
lilu = Path(os.environ["LILU_RESOURCES"]).resolve()
sdk = Path(os.environ["MAC_KERNEL_SDK_HEADERS"]).resolve()
if not (lilu / "Library/plugin_start.cpp").is_file() or not (lilu / "Headers/plugin_start.hpp").is_file():
    raise SystemExit("LILU_RESOURCES must point to Lilu.kext/Contents/Resources")
if not (sdk / "libkern/libkern.h").is_file():
    raise SystemExit("MAC_KERNEL_SDK_HEADERS must point to MacKernelSDK/Headers")

out = root / "build"
out.mkdir(exist_ok=True)
flags = [
    "--target=x86_64-apple-macos10.12", "-mkernel", "-nostdlib", "-fno-builtin",
    "-fno-stack-protector", "-mno-red-zone", "-Os", "-Wall", "-Wextra", "-Werror",
    "-std=c++17", "-fapple-kext", "-fno-rtti", "-fno-exceptions",
    "-fno-use-cxa-atexit", "-fno-register-global-dtors-with-atexit",
    "-mllvm", "-disable-atexit-based-global-dtor-lowering",
    "-DKERNEL", "-DKERNEL_PRIVATE", "-DLILU_KEXTPATCH_SUPPORT",
    "-DPRODUCT_NAME=ReimsFifoOrder", "-DMODULE_VERSION=0.1.3",
    "-isystem", str(lilu), "-isystem", str(sdk),
]
environment = os.environ.copy()
environment["SOURCE_DATE_EPOCH"] = "1793059200"
for name, source in [
    ("ReimsFifoOrder", root / "src/ReimsFifoOrder.cpp"),
    ("plugin_start", lilu / "Library/plugin_start.cpp"),
]:
    command = [environment.get("CXX", "clang++"), *flags]
    if name == "plugin_start":
        command.append("-Wno-unused-parameter")
    command += ["-c", str(source), "-o", str(out / (name + ".o"))]
    subprocess.run(command, check=True, env=environment, timeout=90)
    print("COMPILED", name)
print("Objects only; use cctools ld64 with -kext support and validate MH_KEXT_BUNDLE before guest use.")
