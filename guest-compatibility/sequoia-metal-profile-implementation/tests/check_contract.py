#!/usr/bin/env python3
"""Offline source/manifest checks. Does not verify an Apple driver or installed kernel."""
from pathlib import Path
import json
import hashlib
import re
import plistlib
base=Path(__file__).resolve().parent.parent
code=(base/"src/ReimsSequoiaMetal.cpp").read_text()
patch=(base/"src/UserPatch.cpp").read_text()
header=(base/"src/UserPatch.hpp").read_text()
meta=json.loads((base/"manifest/source-provenance.json").read_text())
contract=json.loads((base/"tests/profile-contract.json").read_text())

for name in ("ReimsSequoiaMetal.cpp","UserPatch.cpp","UserPatch.hpp"):
    source=(base/"src"/name).read_bytes()
    assert hashlib.sha256(source).hexdigest()==meta["published_source_sha256"][name]

def arr(name):
    m=re.search(r"const uint8_t "+name+r"\[\]=\{([^}]*)\};",code)
    assert m is not None,name
    return bytes(int(i.strip(),0) for i in m.group(1).split(",") if i.strip())
found,replace=arr("Find"),arr("Replace")
assert found.hex()==contract["find"] and replace.hex()==contract["replace"]
assert len(found)==len(replace)==11
assert [i for i,(x,y) in enumerate(zip(found,replace)) if x!=y]==[5]
assert int.from_bytes(found[5:9],"little")==10000
assert int.from_bytes(replace[5:9],"little")==10001
assert "KernelVersion::Sequoia" in code
assert "onPatcherLoadForce" in code and "onKextLoadForce" in code
assert "ProfilePagesPatched" in code
assert 'fileSize != 744032' in patch
assert 'fileOffset != 57685' in patch
assert "expectedUUID[16]" in patch
assert "memcmp(page, pp.pristine, pp.pageLength)" in patch
assert "memcmp(buffer + 18688,expectedUUID,sizeof(expectedUUID))" in patch
assert "if (after)" in patch and "findSectionBounds" in patch
assert "_cs_validate_page" in patch and "_cs_validate_page" in header
info=plistlib.loads((base/"src/Info.plist").read_bytes())
label="org.reims.fieldnotes.ReimsSequoiaMetal"
assert label in code and info["CFBundleIdentifier"]==label
assert info["IOKitPersonalities"]["ReimsSequoiaMetal"]["CFBundleIdentifier"]==label
assert info["CFBundleVersion"]=="0.1.0"
assert info["OSBundleLibraries"]["as.vit9696.Lilu"]=="1.7.1"
assert not meta["sanitized_package_guest_tested"]
print("PASS: exact eleven-byte profile change, bounded identity checks, retained source hash and version")
