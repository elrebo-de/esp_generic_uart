# gnss_example

This example uses the generic_uart component to connect to a LC76G GNSS system via UART to read the NMEA records `$GNVTG`, `$GNRMC` and `$GNGGA` every second.

The LC76G GNSS system used is a Waveshare LC76G GNSS Module. I have connected VCC, GND, TX and RX lines to the Grove Port of my ESP32 SoC:

| LC76G GNSS Module | Grove Port |
|:------------------|:-----------|
| VCC               | 5V         |
| GND               | G          |
| TX                | G33 (RX)   |
| RX                | G32 (TX)   |

AFter some seconds (up to 15 sec.) the PPS LED on the GNSS module is flashing once per minute and the data can be read with method `GenericUart::ReadString`.  

The String read contains all the different NMEA messages, the GNSS module sends out.

From these messages the `$GNVTG`, `$GNRMC` and `$GNGGA` messages are interpreted and written to the log every second.

``` log
I (10215) LC76G GNSS receiver: $GNVTG
 TrueTrackAngle: 346.76,
 MagneticTrackAngle: ,
 SpeedOverGround(Knots): 0.33,
 SpeedOverGround(km/h): 0.61
I (10215) LC76G GNSS receiver: $GNRMC
 Time: 102731.000,
 Validity: A,
 Latitude: 4928.207782,N,
 Longitude: 00845.354162,E,
 SpeedOverGround(Knots): 0.33,
 TrueTrackAngle: 346.76,
 Date: 130826,
 Variation: ,
I (10235) LC76G GNSS receiver: $GNGGA
 Time: 102731.000,
 Latitude: 4928.207782,N,
 Longitude: 00845.354162,E,
 Quality: 1,
 NrOfSatellites: 25,
 HDOP: 0.78,
 OrthometricHeight: 418.121,
 GeoidSeparation: 47.959
```
