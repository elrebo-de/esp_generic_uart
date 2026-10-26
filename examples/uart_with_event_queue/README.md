# UART with Event Queue

This example uses the generic_uart component to connect to a LC76G GNSS system via UART to read the NMEA records.

The SoC used is an M5STACK CORE2 V1.1.

The LC76G GNSS system used is a Waveshare LC76G GNSS Module. I have connected VCC, GND, TX and RX lines to the Grove Port of my ESP32 SoC:

| LC76G GNSS Module | Grove Port |
|:------------------|:-----------|
| VCC               | 5V         |
| GND               | G          |
| TX                | G33 (RX)   |
| RX                | G32 (TX)   |

in this example, a function is supplied, that listens to the event queue of the UART. This function is run in a separate task.

The task is created inside the constructor of `GenericUart`.

