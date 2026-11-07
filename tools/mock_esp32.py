import socket

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind(("127.0.0.1", 5005))
print("Mock ESP32 listening on port 5005...")

while True:
    data, addr = sock.recvfrom(1024)
    message = data.decode()
    print("Got:", message)

    seq = message.split(",")[0]
    reply = "ACK," + seq
    sock.sendto(reply.encode(), addr)
