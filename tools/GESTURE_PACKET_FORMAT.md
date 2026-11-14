# Gesture to Command Packet Format (proposal)

Transport: UDP text packets to the ESP32 (192.168.4.1:4210 on its Wi-Fi AP).

Packet:  seq,COMMAND        example: 12,GRIP_CLOSE
Ack:     ACK,seq            example: ACK,12

seq starts at 1 and increases by 1 per packet, so each ack matches its packet.
Sender timeout: 1 second without a matching ack is logged as TIMEOUT.

Commands (gesture names are placeholders until real band gestures are agreed):
  GRIP_CLOSE, GRIP_OPEN, STOP

Status: the ESP32 firmware is not written yet. tools/mock_esp32.py is the
reference for expected board behavior. Actual motor movement is out of scope.