#!/usr/bin/env python3
"""Verify hashes of exact file-level inputs/outputs; never modifies the source tree."""
from pathlib import Path
import sys
import hashlib
import json

def main():
    if len(sys.argv) != 3 or sys.argv[1] not in ('before', 'after'):
        print('Usage: check_patch_contract.py before|after BUNDLE_ROOT', file=sys.stderr)
        return 2
    phase, root = sys.argv[1], Path(sys.argv[2])
    manifest = Path(__file__).resolve().parents[1] / 'manifest/complete-patch-bases.json'
    files = json.loads(manifest.read_text())['files']
    bad = []
    for spec in files:
        path = root / spec['path']
        digest = hashlib.sha256(path.read_bytes()).hexdigest() if path.is_file() else None
        if digest != spec[phase+'_sha256']:
            bad.append((spec['path'], 'missing' if digest is None else 'hash mismatch'))
    for file, reason in bad:
        print(f'{file}: {reason}', file=sys.stderr)
    print(f'{phase} contract: {len(files)-len(bad)}/{len(files)} exact source hashes match')
    return int(bool(bad))

if __name__ == '__main__':
    sys.exit(main())
