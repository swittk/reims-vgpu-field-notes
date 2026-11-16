#!/usr/bin/env python3
"""Validate published summarized time-clock data without processing private logs."""
from pathlib import Path
import json
base=Path(__file__).resolve().parent.parent
data=json.loads((base/"tests/clock-analyses.json").read_text())
for key in ("clock60-unpatched","clock60-candidate","clock60-candidate-game",
            "clock60-return","clock60-return-game","clock60-final","clock60-final-game"):
    assert key in data,key
    result=data[key]
    assert result["samples"]>=470
    assert result["validHostTimes"]==result["samples"]
    assert "outputLeadMs" in result and "periodNs" in result
for key in ("clock60-candidate","clock60-candidate-game","clock60-final","clock60-final-game"):
    result=data[key]
    assert result["outside100ms"]==0
    assert result["periodNs"]["min"]==16666658.0==result["periodNs"]["max"]
assert data["clock60-unpatched"]["outside100ms"]==9
assert data["clock60-return"]["outside100ms"]==10
assert data["clock60-return-game"]["outside100ms"]==373
print("PASS: 60 Hz timing intervention, return, and post-reapplication clock summaries")
