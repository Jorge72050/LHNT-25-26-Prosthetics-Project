"""Read motor telemetry from the ESP32 (src/motor.cpp) over USB serial.

Use from the 3D visualization:

    from motor_telemetry import read_frames
    for frame in read_frames("/dev/cu.usbserial-0001"):
        finger_angle = frame["motors"][0]["deg"]

Or run it directly to print frames:  python motor_telemetry.py /dev/cu.usbserial-0001
Needs pyserial:  pip install pyserial
"""
import sys

FIELDS_PER_MOTOR = 6


def parse_line(line):
    """Turn one "D,..." telemetry line into a dict. Returns None for any other line."""
    parts = line.strip().split(",")
    if parts[0] != "D" or (len(parts) - 2) % FIELDS_PER_MOTOR != 0:
        return None

    motors = []
    for i in range(2, len(parts), FIELDS_PER_MOTOR):
        cmd, pwm, count, deg, rpm, direction = parts[i:i + FIELDS_PER_MOTOR]
        motors.append({
            "id": len(motors) + 1,
            "cmd": cmd,               # last command: "F", "B" or "S"
            "pwm": int(pwm),          # commanded speed 0-255
            "count": int(count),      # raw encoder count since zeroing
            "deg": float(deg),        # output shaft angle (cumulative, can pass 360)
            "rpm": float(rpm),        # output shaft speed, + = forward
            "dir": int(direction),    # measured direction: 1, -1, or 0 (stopped)
        })
    return {"ms": int(parts[1]), "motors": motors}


def read_frames(port, baud=115200):
    """Yield telemetry frames forever from the given serial port."""
    import serial

    with serial.Serial(port, baud, timeout=1) as ser:
        while True:
            line = ser.readline().decode(errors="ignore")
            try:
                frame = parse_line(line)
            except ValueError:
                frame = None          # half-received line right after connecting
            if frame:
                yield frame


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python motor_telemetry.py <serial port>")
        sys.exit(1)
    for frame in read_frames(sys.argv[1]):
        print(frame["ms"], " | ".join(
            f"m{m['id']} {m['cmd']}{m['pwm']:>3} {m['deg']:8.1f}deg {m['rpm']:6.1f}rpm"
            for m in frame["motors"]))
