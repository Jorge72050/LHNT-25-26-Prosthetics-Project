import socket
import time

def run_udp(val):

    ESP32_IP = "192.168.4.1"   # Default AP IP
    PORT = 4210

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.settimeout(1.0)

    print("Starting UDP test...")
    print(f"Target: {ESP32_IP}:{PORT}\n")

    print(f"Sending UDP packet: {val}")
    sock.sendto(val.encode(), (ESP32_IP, PORT))

    try:
        data, addr = sock.recvfrom(255)
        reply = data.decode(errors='ignore')
        print(f"Reply from ESP32: {reply}")
    except socket.timeout:
        print("No reply received")


    time.sleep(1)

    print("\nUDP test complete.")
    sock.close()

