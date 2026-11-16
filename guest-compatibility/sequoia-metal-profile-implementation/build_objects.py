#!/usr/bin/env python3
"""Compile the archived source to reviewable Mach-O objects, not a loadable kext."""
from pathlib import Path
import os
import subprocess

root = Path(__file__).resolve().parent
lilu = Path(os.environ["LILU_RESOURCES"]).resolve()
sdk = Path(os.environ["MAC_KERNEL_SDK_HEADERS"]).resolve()
assert (lilu / "Library/plugin_start.cpp").is_file()
assert (lilu / "Headers/plugin_start.hpp").is_file()
assert (sdk / "libkern/libkern.h").is_file()
out = root / "build"
out.mkdir(exist_ok=True)
flags = [
    "--target=x86_64-apple-macos10.12", "-mkernel", "-nostdlib", "-fno-builtin",
    "-fno-stack-protector", "-mno-red-zone", "-Os", "-Wall", "-Wextra", "-Werror",
    "-std=c++17", "-fapple-kext", "-fno-rtti", "-fno-exceptions",
    "-fno-use-cxa-atexit", "-fno-register-global-dtors-with-atexit",
    "-mllvm", "-disable-atexit-based-global-dtor-lowering",
    "-DKERNEL", "-DKERNEL_PRIVATE", "-DLILU_KEXTPATCH_SUPPORT",
    "-DPRODUCT_NAME=ReimsSequoiaMetal", "-DMODULE_VERSION=0.1.0",
    "-isystem", str(lilu), "-isystem", str(sdk)
]
env = os.environ.copy()
env["SOURCE_DATE_EPOCH"] = "1793059200"
for name, path in [
    ("ReimsSequoiaMetal", root / "src/ReimsSequoiaMetal.cpp"),
    ("UserPatch", root / "src/UserPatch.cpp"),
    ("plugin_start", lilu / "Library/plugin_start.cpp"),
]:
    cmd = [env.get("CXX", "clang++"), *flags]
    if name == "plugin_start":
        cmd.append("-Wno-unused-parameter")
    cmd += ["-I", str(root / "src"), "-c", str(path), "-o", str(out / (name + ".o"))]
    subprocess.run(cmd,env=env,check=True,timeout=90)
    print("COMPILED", name)
print("Objects only; full kext linking, installation, and guest validation are not claimed")
