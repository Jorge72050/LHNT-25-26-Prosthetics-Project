# Project Overview

The prosthetic team at LHNT aims to develop a functional prosthetic hand, similar to real-life medical prosthetics through the actuation, 3D modeling, and hardware-software integration teams. The prosthetic hand currently has one degree of freedom, an open and closed hand, and is being modeled after the [Tact](https://www.instructables.com/Tact-Low-cost-Advanced-Prosthetic-Hand/) prosthetic hand.

- The full parts list and bill of materials can be found in [PARTS.md](./PARTS.md)

## Documentation
Our team uses Notion, a code-friendly notetaking interface. Our documentation, detailed project setup, and notes can be found on this [Notion website](https://daffy-avatar-866.notion.site/LHNT-HSI-27d688da0d5e80a9a6f6f60b7bff9992)

### Drivers

We selected the ESP32 Dev Module as our microcontroller. ESP32s often need external driver installation. Windows and Mac drivers can be found [here](https://www.silabs.com/software-and-tools/usb-to-uart-bridge-vcp-drivers?tab=downloads).

The file structure was created in VS Code using the [PlatformIO](https://marketplace.visualstudio.com/items?itemName=platformio.platformio-ide) extension. All files are set up such that when the repository is cloned, all dependencies are installed and the code can be ran and built after installation.
In addition, an Arduino folder is present if the PlatformIO extension does not work for users. All code is identical but translated between setups.

## Current Goals

3D Modeling - The 3D modeling team is currently working on developing a wearable mount and DC Motor holders for full wearability. In addition, they are making modifications to the Tact prosthetic hand base design for greater grip control.

Electronics - The Actuation team is working towards implementing DC motors and the servos into the final hand to ensuring proper opening and closing of the hand.

Hardware-Software Interfacing (HSI) - The HSI team currently working on integrating DC motor code and processing EMG input for simultaneous EMG-Prosthetic Hand control.

## Team Structure

### Project Lead: Hailey Hyde

#### *Electronics*:
- Design the joints and tensile forces behind them to allow for the opening and closing of the hand. Responsible for electronics and wiring of the prosthetic hand.
- Team Lead: Jorge Nunez
- Members: Perla Mendoza, Alicia Richard, Aprameya Sudharsan, Zara Siddqi, Alper Yusuf

#### *3D Modeling Team*:
- Design the prosthetic hand and mounts necessary for motor placement and control.
- Team Lead: Kate Long
- Members: Shah Aarya, Andrew Jung, Arjun Kulkarni, Esha Shenoy, Arya Soppannavar

#### *Hardware Software Integration*:
- Integrate binary signals from machine learning into servo control for opening and closing the hand. Design and develop software for microcontrollers and all electronics
- Team Lead: Jorge Nunez
- Members: Maggie Flinn, Soorya Narayaran, Sojung Kim, Nikitha Vandanapu


[2024-2025 Github](https://github.com/ZS221/HSI-Servo-Code)
