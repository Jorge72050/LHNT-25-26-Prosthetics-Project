# Band Data (Mudra Band)

What the Mudra Band (Wearable Devices) sends, what each value means, and what is still unknown. This is based on the official Mudra SDK (`mudra-sdk` 0.2.10, Python), its docs, and a community recorder that decoded the raw Bluetooth packets. **None of this has been checked on our band yet.** Items marked *unverified* need a test with the real band.

## Summary

| Stream | Raw or processed? | What you get | How often | License needed |
|---|---|---|---|---|
| **SNC** (nerve signals) | Raw | 3 channels, 18 samples each per packet | Continuous, rate not published | RawData (extra fee) |
| **IMU accelerometer** | Raw | 3 axes (x, y, z), 8 samples each per packet | Continuous, rate not published | RawData |
| **IMU gyroscope** | Raw | 3 axes, 8 samples each per packet | Continuous, rate not published | RawData |
| **Pressure** | Processed on the band | One number, 0.00 to 1.00 | Sent as it changes | Main |
| **Gestures** | Classified on the band | Event: tap, double tap, twist, double twist | Once per gesture | Main |
| **Button** (press / release) | Classified on the band | Event: `press` or `release` | When it changes | Main |
| **Navigation** | Processed on the band | dx, dy cursor movement (integers) | Continuous while moving | Main |
| **Navigation direction** | Classified on the band | Event: up/down/left/right/roll left/roll right (+ "reverse" versions) | Once per motion | Main |
| Battery, charging | Status | Battery 0-100 %, charging true/false | When it changes | - |

**In short:** SNC and IMU arrive **raw** (numbers straight from the sensors). Pressure, gestures and navigation are **already worked out by the band's firmware**, so we only get the result, not how it was computed.

## How it connects

- **Bluetooth Low Energy only.** There is no USB serial and no Wi-Fi. Our `tools/band_stream.py` can't read the band directly yet: it needs a Mudra input added (see "What this means for us").
- The official SDK runs on **Windows and macOS** (Python 3.10+), plus Android, iOS (Swift) and Dart. There is no official Linux support; the community recorder `mudrarecord` works on Linux.
- You must sign in through the SDK to load licenses before the band will send data.
- Known SDK issue: after closing your app, unpair the band in the computer's Bluetooth settings before it can be found again.

## Licenses (important)

- **Main license:** pressure, gestures, navigation, air-touch.
- **RawData license:** SNC, gyroscope, accelerometer. The vendor says it "may incur an additional fee." Get it by making an account in the Mudra Developer Kit app and contacting the vendor.

**Check which license our band/account has.** Without RawData we only get pressure and gestures, and can't train our own gesture model from nerve signals.

## Each stream in detail

### SNC (Surface Nerve Conductance)

The band's main sensor: 3 sensors on the wrist that pick up the electrical signal from nerves (similar to EMG).

- Each packet: **54 numbers = 3 channels x 18 samples**, plus one RMS value per channel (RMS = how strong the signal was over that packet), a timestamp, and the measured packet rate.
- SDK order: channel 1 = values 0-17, channel 2 = 18-35, channel 3 = 36-53.
  - The raw Bluetooth packet is mixed instead (ch1, ch2, ch3, ch1, ch2, ...). The SDK reorders it. If you read raw packets yourself, split by `index % 3`.
- Values are raw ADC counts (16-bit by default; the band can be switched to 24-bit). There are no units like microvolts.
- `-32768` and `32767` mean **clipped or bad contact**: drop those samples instead of treating them as real values.

```python
# SDK callback
def on_snc_ready(timestamp, data_list, frequency, frequency_std, rms_list):
    ch1, ch2, ch3 = data_list[0:18], data_list[18:36], data_list[36:54]
```

### IMU (accelerometer and gyroscope)

- Each packet: **24 numbers = 3 axes x 8 samples**, mixed order (x, y, z, x, y, z, ...).
- Units and scaling are not documented. The values are raw counts.
- The community decoder could decode the accelerometer reliably but **not the gyroscope**. Use the official SDK if you need gyro data.

```python
def on_imu_acc_ready(timestamp, data_list, frequency, frequency_std, rms_list):
    x, y, z = data_list[0::3], data_list[1::3], data_list[2::3]
```

### Pressure

How hard you press your fingertips together (or on a surface), estimated from the nerve signals. **There is no physical force sensor**: it is the firmware's estimate.

- The band sends **one whole number from 0 to 100**. The SDK divides it by 100, so you get **0.00 to 1.00 in steps of 0.01** (100 levels). This is in the SDK code: `float(data[1]) / 100`.
- Two modes:
  - `direct`: pressure is sent all the time, even at rest.
  - `pinch`: pressure is only sent during a tap / press-and-hold, and is 0 the rest of the time.
- **How pressure scales with push strength is not documented.** The vendor does not say:
  - whether it is linear (twice the force = twice the number);
  - what force gives 1.00 (it's not in newtons or grams);
  - whether it is adjusted per person or per session;
  - how much it is smoothed.

  We have to measure this ourselves (see the test below).

```python
await device.set_on_pressure_ready(on_pressure_ready, PressureType.direct)
def on_pressure_ready(pressure):   # 0.0 .. 1.0
    ...
```

### Gestures, button and navigation

All of these come out of the band already decided. We get events, not signals.

- **Gestures:** `TAP`, `DOUBLE_TAP`, `TWIST`, `DOUBLE_TWIST`. The SDK lists more (pinch directions, reverse tap, etc.) that are probably for other firmware/devices.
- **Button:** `press` when a tap starts, `release` when both fingers let go. Use this for "hold to keep the hand closed."
- **Navigation:** whole-number dx, dy, like a mouse. Up/down flips depending on which hand the band is on.
- The band can also act as a Bluetooth mouse/keyboard. Turn that off while recording so gestures don't move the computer's cursor.

## Sample rates

- **Not published for the Mudra Band.** The SDK reports the measured rate (`frequency`) with every SNC/IMU packet, so we can find it by printing that value.
- The vendor's dev-kit page lists **2080 Hz** for SNC and **1125 Hz** for the IMU, with **up to 20 ms latency**. Those specs appear to be for their full-wrap dev band, not necessarily ours. *Unverified for our band.*
- Turning off streams you don't need leaves more Bluetooth bandwidth for the rest.

## Test to run with the real band

1. **Rates:** print `frequency` from the SNC and IMU callbacks for 30 s.
2. **Pressure vs. force:**
   - Press a fingertip on a kitchen scale at roughly 0, 100, 200, ... 1000 g, and hold each level for 3 s.
   - Log the pressure value at each step, in both `direct` and `pinch` mode. Repeat on two people and two days.
   - Plot grams vs. pressure. That tells us if it's linear, where it maxes out, and whether it needs calibrating per person.
3. **Gesture timing:** record SNC and gestures at the same time, and check how long after the movement the gesture event arrives.
4. **License check:** confirm SNC and IMU callbacks actually fire (that needs the RawData license).

## What this means for us

- **For opening/closing the hand,** `pressure` (0-1) is the easiest signal to use. It could map straight to how closed the hand is, and `press`/`release` can latch the grip. This needs only the Main license.
- **For our own gesture model,** we need SNC, which means the RawData license.
- `tools/band_stream.py` currently reads serial/UDP text lines. To record the real band it needs a `mudra` input that uses the SDK and saves SNC, IMU, pressure and gesture events. The fake stream should also be changed to match this format (3 SNC channels, pressure 0-1, gesture events) instead of the current 8 generic channels.

## Sources

- Official SDK docs: https://wearable-devices.github.io/
- Python SDK: https://pypi.org/project/mudra-sdk/ (pressure scaling and packet handling read from its source code, `service/ble_service.py` and `models/enums.py`)
- Community raw recorder (packet layouts): https://github.com/flaminerva/mudrarecord
- Vendor dev-kit specs: https://www.wearabledevices.co.il/mudra-dev-kit-program
- Vendor plugin/signal list: https://github.com/wearable-devices/mudra-skills
