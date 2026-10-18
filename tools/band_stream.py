"""Record, replay and fake EMG band data, so gesture code can be built without the band.

Every stream (live band, recorded file, fake generator) gives the same events, so gesture
code only needs to change one string to switch between them:

    from band_stream import open_stream
    for event in open_stream("fake"):        # or "serial:/dev/cu.usbserial-0001", "file:session.jsonl"
        if "label" in event:
            print("gesture marker:", event["label"])   # "close", "open", "rest", ...
        else:
            values = event["data"]           # one raw line from the band, e.g. "12,-30,5,..."

Each event is a dict with "t" (seconds since the stream started) and either
"data" (one raw line from the band, stored as-is) or "label" (a gesture marker).

Command line (run from the tools/ folder):

    # Record the band to a file. While recording, type a gesture name + Enter
    # (e.g. "close") to mark when you start doing it, and "rest" when you stop.
    python band_stream.py record serial:/dev/cu.usbserial-0001 sessions/alex_close.jsonl

    # Replay a recording at real speed (or --speed 4, --loop), printing lines or sending UDP
    python band_stream.py replay sessions/alex_close.jsonl
    python band_stream.py replay sessions/alex_close.jsonl --udp 127.0.0.1:5005

    # Fake band: 8 channels at 200 Hz with random "open"/"close" bursts
    python band_stream.py fake --channels 8 --rate 200

Stream specs for open_stream() and "record":
    serial:<port>[@baud]   band plugged in over USB serial (default 115200 baud, needs pyserial)
    udp:<port>             band (or another script) sending lines over UDP to this computer
    file:<path>            a recording made with "record"
    fake[:channels]        generated data, no hardware needed
"""
import argparse    # builds the command-line interface (record / replay / fake)
import json        # events are saved and loaded as JSON, one per line
import math        # math.sin and math.pi shape the fake muscle-activity curve
import os          # used to create the output folder when recording
import random      # random gesture timing and noise for the fake band
import socket      # UDP sending and receiving
import sys         # stdin (typed labels) and stderr (status messages)
import threading   # lets the terminal-label reader run alongside the recording loop
import time        # clocks and sleeping to keep real-time pace
from datetime import datetime  # timestamps the recording header


# ---------------------------------------------------------------------------
# Sources: generators that yield events {"t": ..., "data": ...} or {"t": ..., "label": ...}
# ---------------------------------------------------------------------------

def serial_source(port, baud=115200):
    """Live band over USB serial. Each line the band prints becomes one event."""
    import serial  # only needed for live serial, so imported here

    start = time.monotonic()  # reference point so "t" means seconds since the stream began
    with serial.Serial(port, baud, timeout=1) as ser:  # open the port; closes automatically on exit
        while True:  # keep reading until the consumer stops iterating
            line = ser.readline().decode(errors="ignore").strip()  # bytes -> text, drop garbage bytes and newline
            if line:  # readline returns "" on timeout
                yield {"t": time.monotonic() - start, "data": line}  # hand one event to the caller


def udp_source(port):
    """Live band (or another program) sending text over UDP. Each line in a packet is one event."""
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)  # create a UDP socket (IPv4)
    sock.bind(("0.0.0.0", port))  # listen on this port on all network interfaces
    start = time.monotonic()  # time zero for the "t" field
    try:
        while True:
            packet, _ = sock.recvfrom(4096)  # block until a packet arrives (ignore sender address)
            t = time.monotonic() - start  # every line in one packet shares the arrival time
            for line in packet.decode(errors="ignore").splitlines():  # a packet may hold several lines
                if line.strip():  # skip blank lines
                    yield {"t": t, "data": line.strip()}  # one event per non-empty line
    finally:
        sock.close()  # always release the port, even if the loop is interrupted


def read_header(path):
    """Return the header dict a recording starts with (source, start time, note)."""
    with open(path) as f:
        return json.loads(f.readline()).get("header", {})  # first line is {"header": {...}}; {} if missing


def file_source(path, speed=1.0, loop=False, realtime=True):
    """Replay a recording. realtime=False returns events as fast as possible (good for tests)."""
    while True:  # outer loop lets --loop restart the recording
        events = []
        with open(path) as f:
            for line in f:
                event = json.loads(line)  # each line is one JSON object
                if "header" not in event:  # first line is the header, not an event
                    events.append(event)
        if not events:  # empty recording: nothing to play, avoid looping forever
            return

        start = time.monotonic()  # playback clock starts now
        for event in events:
            if realtime:
                # Wait until this event's original time (divided by speed) has passed
                delay = event["t"] / speed - (time.monotonic() - start)  # how long until it's due
                if delay > 0:
                    time.sleep(delay)  # pause so playback matches the original timing
            yield event  # hand the event to the caller

        if not loop:  # play once unless looping was requested
            return


def fake_source(channels=8, rate=200, seed=None, duration=None, gestures=("close", "open")):
    """Fake EMG band: noisy channels at `rate` Hz with random gesture bursts.

    Rest 1.5-3 s, then a gesture for 0.8-1.5 s, repeating. "close" makes the first half of the
    channels (like forearm flexors) fire, "open" the second half (extensors). A "label" event
    marks each gesture start and the return to "rest", so the fake data comes with the right
    answers for testing a gesture detector. Values are signed ints, one CSV line per sample.
    """
    rng = random.Random(seed)  # fixed seed = same fake session every run
    half = max(1, channels // 2)  # split point between the "close" and "open" channel groups
    active_channels = {
        "close": range(0, half),  # first half of channels fire for "close"
        "open": range(half, channels) if channels > 1 else range(0, 1),  # second half for "open" (1-channel fallback)
    }

    start = time.monotonic()  # wall-clock reference for pacing
    sample = 0  # how many samples have been produced so far
    gesture = "rest"  # begin in the resting state
    phase_end = rng.uniform(1.5, 3.0)  # time when the current rest/gesture ends
    phase_start = 0.0  # time the current phase began

    while duration is None or sample / rate < duration:  # run forever, or until the duration is reached
        t = sample / rate  # simulated time of this sample (exact, not wall-clock)

        # Switch between rest and a gesture when the current phase is over
        if t >= phase_end:
            if gesture == "rest":
                gesture = rng.choice(gestures)  # pick a random gesture to perform
                phase_end = t + rng.uniform(0.8, 1.5)  # gestures are short
            else:
                gesture = "rest"  # gesture finished, go back to rest
                phase_end = t + rng.uniform(1.5, 3.0)  # rests are longer
            phase_start = t  # remember when this new phase started
            yield {"t": t, "label": gesture}  # emit the ground-truth marker

        # Muscle activity rises and falls smoothly over the gesture (half a sine wave)
        if gesture == "rest":
            envelope = 0.0  # no muscle activity at rest
        else:
            # 0 -> 1 -> 0 across the gesture's duration
            envelope = math.sin(math.pi * (t - phase_start) / (phase_end - phase_start))

        values = []
        for ch in range(channels):
            gain = 1.0  # baseline: just noise
            if gesture != "rest" and ch in active_channels[gesture]:
                gain += 8.0 * envelope  # active muscles: much bigger signal
            values.append(int(rng.gauss(0, 10) * gain))  # Gaussian noise (std 10) scaled by gain, as an int

        yield {"t": t, "data": ",".join(str(v) for v in values)}  # one CSV line, like the real band prints
        sample += 1  # advance to the next sample

        # Keep real-time pace so it behaves like the real band
        delay = start + sample / rate - time.monotonic()  # time until the next sample is due
        if delay > 0:
            time.sleep(delay)  # wait so output runs at `rate` Hz


def open_stream(spec, speed=1.0, loop=False):
    """Turn a spec like "serial:/dev/cu.x@115200", "udp:5005", "file:a.jsonl" or "fake:8" into a source."""
    kind, _, arg = spec.partition(":")  # split "serial:/dev/x" into "serial" and "/dev/x"
    if kind == "serial":
        port, _, baud = arg.partition("@")  # optional "@baud" suffix
        return serial_source(port, int(baud) if baud else 115200)  # default baud if none given
    if kind == "udp":
        return udp_source(int(arg))  # arg is the port number
    if kind == "file":
        return file_source(arg, speed=speed, loop=loop)  # arg is the file path
    if kind == "fake":
        return fake_source(channels=int(arg) if arg else 8)  # optional channel count, default 8
    raise ValueError(f"Unknown stream '{spec}'. Use serial:, udp:, file: or fake")  # anything else is a typo


# ---------------------------------------------------------------------------
# Commands
# ---------------------------------------------------------------------------

def record(spec, out_path, duration=None, note=""):
    """Save every event from `spec` to a JSON Lines file. Typed lines on stdin become labels."""
    lock = threading.Lock()  # the stdin thread and the main loop both write to the file
    counts = {"data": 0, "label": 0}  # tallies for the summary at the end
    os.makedirs(os.path.dirname(out_path) or ".", exist_ok=True)  # e.g. create sessions/ the first time

    with open(out_path, "w") as out:
        # First line: where and when this was recorded
        header = {"source": spec, "started": datetime.now().isoformat(timespec="seconds"), "note": note}
        out.write(json.dumps({"header": header}) + "\n")  # header goes on line 1

        def write(event):
            with lock:  # only one thread writes at a time so lines don't get interleaved
                out.write(json.dumps(event) + "\n")  # one event per line
                out.flush()  # flush every line so a crash or unplugged band keeps the data
                counts["label" if "label" in event else "data"] += 1  # update the right counter

        start = time.monotonic()  # time zero for typed labels (matches the source's own clock closely)

        def read_labels():
            # Each line typed in the terminal (e.g. "close") is saved as a gesture marker
            for line in sys.stdin:  # blocks waiting for you to type + Enter
                if line.strip():  # ignore empty lines
                    write({"t": time.monotonic() - start, "label": line.strip()})  # timestamp the marker
                    print(f"  marked '{line.strip()}'", file=sys.stderr)  # confirm it was saved

        threading.Thread(target=read_labels, daemon=True).start()  # daemon: won't keep the program alive at exit
        print(f"Recording {spec} -> {out_path}. Type a gesture name + Enter to mark it, "
              f"Ctrl+C to stop.", file=sys.stderr)  # instructions go to stderr

        try:
            for event in open_stream(spec):  # pull events from the chosen source
                write(event)  # save each one
                if duration is not None and event["t"] >= duration:  # stop after --duration seconds
                    break
        except KeyboardInterrupt:
            pass  # Ctrl+C is the normal way to stop; fall through to the summary

    print(f"Saved {counts['data']} samples and {counts['label']} labels to {out_path}", file=sys.stderr)


def play(events, udp_target=None):
    """Send events to the terminal (data on stdout, labels on stderr) or as UDP packets."""
    sock = None
    if udp_target:
        host, _, port = udp_target.rpartition(":")  # split "127.0.0.1:5005" at the last colon
        target = (host or "127.0.0.1", int(port))  # default to localhost if only a port was given
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)  # UDP socket for sending

    try:
        for event in events:
            if "label" in event:
                # Labels go to stderr so piping stdout into another program gives only band data
                print(f"# {event['t']:.2f}s label: {event['label']}", file=sys.stderr)
            elif sock:
                sock.sendto(event["data"].encode(), target)  # send one line as one UDP packet
            else:
                print(event["data"], flush=True)  # print the raw line; flush so pipes see it immediately
    except KeyboardInterrupt:
        pass  # Ctrl+C ends playback quietly
    finally:
        if sock:
            sock.close()  # clean up the socket


def main():
    parser = argparse.ArgumentParser(description="Record, replay and fake EMG band data.")
    commands = parser.add_subparsers(dest="command", required=True)  # one sub-command is mandatory

    rec = commands.add_parser("record", help="save a stream to a file")
    rec.add_argument("source", help="serial:<port>[@baud], udp:<port> or fake[:channels]")  # what to record
    rec.add_argument("out", help="output file, e.g. sessions/alex_close.jsonl")  # where to save it
    rec.add_argument("--duration", type=float, help="stop after this many seconds")  # optional time limit
    rec.add_argument("--note", default="", help="free text saved in the header (who, which arm, ...)")

    rep = commands.add_parser("replay", help="play back a recording")
    rep.add_argument("file")  # the .jsonl recording to play
    rep.add_argument("--speed", type=float, default=1.0, help="2 = twice as fast")
    rep.add_argument("--loop", action="store_true", help="start over at the end")  # flag, no value needed
    rep.add_argument("--udp", help="send each line to host:port instead of printing it")

    fake = commands.add_parser("fake", help="generate fake band data")
    fake.add_argument("--channels", type=int, default=8)  # number of simulated sensor channels
    fake.add_argument("--rate", type=int, default=200, help="samples per second")
    fake.add_argument("--seed", type=int, help="same seed = same fake session")
    fake.add_argument("--duration", type=float, help="stop after this many seconds")
    fake.add_argument("--udp", help="send each line to host:port instead of printing it")

    args = parser.parse_args()  # parse what the user typed
    if args.command == "record":
        record(args.source, args.out, args.duration, args.note)
    elif args.command == "replay":
        play(file_source(args.file, speed=args.speed, loop=args.loop), args.udp)  # build the source, then play it
    elif args.command == "fake":
        play(fake_source(args.channels, args.rate, args.seed, args.duration), args.udp)


if __name__ == "__main__":  # only run main() when executed directly, not when imported
    main()