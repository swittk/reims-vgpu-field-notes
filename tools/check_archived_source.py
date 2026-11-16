#!/usr/bin/env python3
'''Check that a caller-supplied tree has a recorded patch's exact source bytes.

Read-only; no Git revision, branch or build compatibility is inferred.
'''
from pathlib import Path
import hashlib
import json
import sys

def sha(data):
    return hashlib.sha256(data).hexdigest()

def main(args):
    if len(args) != 3 or args[1] not in ('before', 'after'):
        print('Usage: check_archived_source.py CASE_DIR before|after PATCH_LAYOUT_ROOT', file=sys.stderr)
        return 2
    case, phase, root = Path(args[0]), args[1], Path(args[2])
    record = case / 'source-contract.json'
    if not record.is_file():
        print('Case has no source-contract.json', file=sys.stderr)
        return 2
    manifest = json.loads(record.read_text())
    patch = case / 'source.patch'
    if not patch.is_file() or sha(patch.read_bytes()) != manifest['patch_sha256']:
        print('Patch bytes do not match the recorded digest', file=sys.stderr)
        return 1
    failures = []
    files = manifest.get('files')
    if files is None and 'file' in manifest:
        files = [dict(path=manifest['file'], before_sha256=manifest.get('before_sha256'),
                      after_sha256=manifest.get('after_sha256'))]
    for entry in files or []:
        path = root / entry['path']
        wanted = entry[phase + '_sha256']
        actual = sha(path.read_bytes()) if path.is_file() else None
        if actual != wanted:
            failures.append(entry['path'])
    for name in failures:
        print('Mismatch: ' + name, file=sys.stderr)
    print(f'{phase} source digest: {len(files or []) - len(failures)}/{len(files or [])} matched')
    return int(bool(failures))

if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
