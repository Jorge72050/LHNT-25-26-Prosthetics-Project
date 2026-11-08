# Recording format (JSON Lines)

Line 1: {"header": {"source": ..., "started": ..., "note": ...}}
Other lines, one event each:
  {"t": 2.4, "data": "12,-30,5,..."}    raw sample, t = seconds since start
  {"t": 2.4, "label": "close"}          gesture marker

Recorded and replayed with tools/band_stream.py.
Mock recordings are marked "fake" in the header source and in the note.
Live Mudra capture (tools/mudra_record.py) writes the same format;
TAP/DOUBLE_TAP/TWIST/DOUBLE_TWIST become label events.