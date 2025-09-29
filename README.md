# FlightDeck
ESP32-P4 based Flight Simulator 24 touchscreen controller

The goal of this project was to design an interface to help control Flight Simulator 2024.
A clean/modern touchscreen-UI, running on a powerful microcontroller, and configured with some rotary encoders (for quick changing settings like radios, autopilot alt/hdg and so on). Able to be used either for quick visual information, or for more in-depth control.

This project is still very much in early development, so hopefully will rapidly improve. As of the initial upload I am still working through the UI configuration, however the radios page and partially the autopilot pages are working.

Hardware used:
I've used an ESP32-P4 based touch display (JC1060P470C_I_W), along with a couple of rotary encoders. I will likely re-design this with a single dual rotary encoder and at the same time will re-design the 3D enclosure for the whole setup. There's a 14P ribbon cable attached to one of the expansion ports on the ESP32-P4, and this has a small breakout board on the other end for ease of soldering the encoders to 3v3, GND, and the various GPIOs required (6 in total, 2 encoders each with a button).

Software used:
Code is written in C++ (Arduino) using a number of tools: Arduino IDE, EEZ Studio for the LVGL UI design, and lots of ChatGPT help debugging. It connects as a serial device to the excellent SPAD.NEXT, which is used for sending/receiving all FS24 data. It does not use any particular libraries for the serial communication with spad, as I found things like CmdMessenger quite unreliable, so it's making direct serial calls. Speaking of which the project is configured to use 2 of the 3 USB-C ports: USB-UART (bottom of the three) is for flashing and debug output, USB-FAST (top of the three) is used for serial comms to spad.

No detailed instructions provided yet as this is a very early development phase, however once I feel it's ready I'll do a full write-up including a hardware build and configuration guide.

I've included a few design guides that I'm loosely following, and there are STLs for the 3D printed case (in three parts; front cover, rear cover, and optional stand).
