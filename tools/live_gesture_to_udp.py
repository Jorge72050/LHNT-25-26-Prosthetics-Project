import asyncio
import json
import os
import socket
import time
from datetime import datetime
from prodilink import MudraLink

ESP32_IP = "127.0.0.1"   # mock listener; real board is 192.168.4.1
ESP32_PORT = 5005        # real board is 4210
TIMEOUT_SECONDS = 1.0
LOG_FILE = "gesture_log.txt"
OUT_PATH = "sessions/mudra_live.jsonl"
RECORD_SECONDS = 30

# Keys must be the exact gesture names Prodilink reports.
# Leave this empty for the first run: every gesture will show up as
# "IGNORED unmapped gesture NAME", which tells you the real names.
# Then fill it in, for example:  "SOME_NAME": "GRIP_CLOSE",
GESTURE_TO_COMMAND = {
}

state = {"seq": 1}


def log(message):
    line = time.strftime("%H:%M:%S") + " " + message
    print(line)
    f = open(LOG_FILE, "a")
    f.write(line + "\n")
    f.close()


def send_gesture(sock, seq, gesture):
    packet = str(seq) + "," + GESTURE_TO_COMMAND[gesture]
    sock.sendto(packet.encode(), (ESP32_IP, ESP32_PORT))
    log("SENT " + packet)

    try:
        data, addr = sock.recvfrom(1024)
        reply = data.decode()
        if reply == "ACK," + str(seq):
            log("ACK  " + reply)
        else:
            log("UNEXPECTED REPLY " + reply)
    except socket.timeout:
        log("TIMEOUT no ack for seq " + str(seq))


async def main():
    os.makedirs("sessions", exist_ok=True)
    out_file = open(OUT_PATH, "w")
    header = {
        "source": "prodilink",
        "started": datetime.now().isoformat(timespec="seconds"),
        "note": "Mudra Link live capture (gestures only)",
    }
    out_file.write(json.dumps({"header": header}) + "\n")

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.settimeout(TIMEOUT_SECONDS)
    start = time.monotonic()

    try:
        async with await MudraLink.discover() as device:
            await device.enable_gestures()

            @device.on_gesture
            def handle(gesture):
                name = gesture.name
                t = time.monotonic() - start
                out_file.write(json.dumps({"t": t, "label": name}) + "\n")
                out_file.flush()
                print("Gesture:", name)

                if name in GESTURE_TO_COMMAND:
                    send_gesture(sock, state["seq"], name)
                    state["seq"] = state["seq"] + 1
                else:
                    log("IGNORED unmapped gesture " + name)

            print("Running for", RECORD_SECONDS, "seconds. Make gestures now!")
            try:
                await asyncio.wait_for(device.stream(), timeout=RECORD_SECONDS)
            except asyncio.TimeoutError:
                pass
    finally:
        out_file.close()
        sock.close()

    print("Saved", OUT_PATH)


asyncio.run(main())