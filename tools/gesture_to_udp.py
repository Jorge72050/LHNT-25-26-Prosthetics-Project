import socket
import time

ESP32_IP = "127.0.0.1"   # change to the real ESP32 IP later
ESP32_PORT = 5005        # change to match run_udp.py later
TIMEOUT_SECONDS = 1.0
LOG_FILE = "gesture_log.txt"

GESTURE_TO_COMMAND = {
    "fist": "GRIP_CLOSE",
    "open_hand": "GRIP_OPEN",
    "index_tap": "POINT",
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

    # replayed (mock) gestures
    replay = ["fist", "open_hand", "index_tap", "rest"]

    seq = 1
    for gesture in replay:
        send_gesture(sock, seq, gesture)
        seq = seq + 1
        time.sleep(0.5)

    sock.close()


main()
