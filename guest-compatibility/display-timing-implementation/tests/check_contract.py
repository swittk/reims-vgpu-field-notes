#!/usr/bin/env python3
"""Static contract: check exact source, driver match, version, and timing validation."""
from pathlib import Path
import hashlib, json, plistlib, re

base=Path(__file__).resolve().parent.parent
source=(base/"src/ReimsDisplayTiming.cpp").read_text()
header=(base/"src/exact_code.hpp").read_text()
provenance=json.loads((base/"manifest/source-provenance.json").read_text())
assert hashlib.sha256(source.encode()).hexdigest()==provenance["published_source_sha256"]
assert hashlib.sha256(header.encode()).hexdigest()==provenance["exact_code_header_sha256"]
assert "org.reims.fieldnotes.ReimsDisplayTiming" in source
assert "KernelVersion::Sequoia" in source
assert "AppleParavirtFramebuffer27getTimingInfoForDisplayMode" in source
assert "routeMultiple" in source
assert "sizeof(ExactCode)" in source
assert "memcmp(reinterpret_cast<const void *>(function), ExactCode, sizeof(ExactCode))" in source
assert "copyProperty(\"IOFBConfig\")" in source
assert "IOFBModes" in source and "getObject(\"TM\")" in source
assert "minPixelClock != t.pixelClock" in source and "maxPixelClock != t.pixelClock" in source
assert "verticalBlankingExtension" in source
assert "kIODetailedTimingValid" in source
assert "Original(fb, mode, info)" in source
assert "static_assert(sizeof(IODetailedTimingInformationV2) == 160" in source
assert "static_assert(sizeof(ExactCode) == 52" in source
parts=re.search(r"ExactCode\[\] = \{(.*?)\};",header,re.S)
assert parts
blob=bytes(int(x.strip(),0) for x in parts.group(1).split(",") if x.strip())
assert len(blob)==52
info=plistlib.loads((base/"src/Info.plist").read_bytes())
identifier="org.reims.fieldnotes.ReimsDisplayTiming"
assert info["CFBundleIdentifier"]==identifier
assert info["IOKitPersonalities"]["ReimsDisplayTiming"]["CFBundleIdentifier"]==identifier
assert info["CFBundleVersion"]=="0.1.0"
assert info["OSBundleLibraries"]["as.vit9696.Lilu"]=="1.7.1"
print("PASS: timing helper exact-method guard, non-hardcoded validated mode, version/symbol, source and bundle identities")
