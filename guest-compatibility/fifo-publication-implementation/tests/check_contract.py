#!/usr/bin/env python3
"""Inspect the published source/Info.plist/patch shape without loading a kext."""
import hashlib
import json
import plistlib
import re
from pathlib import Path

base = Path(__file__).resolve().parent.parent
source = (base / "src/ReimsFifoOrder.cpp").read_text()
provenance = json.loads((base / "manifest/source-provenance.json").read_text())
assert hashlib.sha256(source.encode()).hexdigest() == provenance["published_source_sha256"]

def parsed_bytes(label):
    label_text = "const uint8_t " + label + "[] = {"
    assert source.count(label_text) == 1, label
    body = source.split(label_text, 1)[1].split("};", 1)[0]
    return bytes(int(item.strip(), 0) for item in body.split(",") if item.strip())

before, after = parsed_bytes("Find"), parsed_bytes("Replace")
assert len(before) == len(after) == 34
differences = [(i, a, b) for i, (a, b) in enumerate(zip(before, after)) if a != b]
assert differences == [(11, 0x89, 0x87)], differences
assert "constexpr size_t FunctionLength = 346;" in source
assert "offset!=0x10d" in source
assert "KernelVersion::Sequoia" in source
assert "matches!=1 || already!=0" in source
assert "memcmp(bytes+offset,Replace,sizeof(Replace))" in source
assert "onKextLoadForce" in source
assert "thread_call_enter_delayed" in source  # A one-shot reporting timer, not FIFO polling.
identifier = "org.reims.fieldnotes.ReimsFifoOrder"
assert source.count(identifier) == 1
info = plistlib.loads((base / "src/Info.plist").read_bytes())
assert info["CFBundleIdentifier"] == identifier
assert info["IOKitPersonalities"]["ReimsFifoOrder"]["CFBundleIdentifier"] == identifier
assert info["CFBundleVersion"] == "0.1.3"
assert info["OSBundleLibraries"]["as.vit9696.Lilu"] == "1.7.1"
print("PASS: 34-byte exact match, only MOV 89 08 -> XCHG 87 08, guarded source + bundle metadata")
