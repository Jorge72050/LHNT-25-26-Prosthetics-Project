import json
import socket
import time

ESP32_IP = "127.0.0.1"   # change to the real ESP32 IP later
ESP32_PORT = 5005        # change to match run_udp.py later
TIMEOUT_SECONDS = 1.0
LOG_FILE = "gesture_log.txt"
RECORDING = "sessions/mock_gestures.jsonl"

# Keys must match the gesture names in the recording.
# These are the mock names. When real band recordings exist, replace
# them with the real gesture names and agree the commands with your team.
GESTURE_TO_COMMAND = {
    "close": "GRIP_CLOSE",
    "open": "GRIP_OPEN",
    "rest": "STOP",
}


def log(message):
    line = time.strftime("%H:%M:%S") + " " + message
    print(line)
    f = open(LOG_FILE, "a")
    f.write(line + "\n")
    f.close()


def build_packet(seq, gesture):
    command = GESTURE_TO_COMMAND[gesture]
    return str(seq) + "," + command


def send_gesture(sock, seq, gesture):
    packet = build_packet(seq, gesture)
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


def main():
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.settimeout(TIMEOUT_SECONDS)

    seq = 1
    f = open(RECORDING, "r")
    for line in f:
        event = json.loads(line)

        # only gesture events have a "label"; header and raw samples are skipped
        if "label" not in event:
            continue

        gesture = event["label"]
        if gesture not in GESTURE_TO_COMMAND:
            log("IGNORED unknown gesture " + gesture)
            continue

        send_gesture(sock, seq, gesture)
        seq = seq + 1
        time.sleep(0.5)
    f.close()

    sock.close()


main()