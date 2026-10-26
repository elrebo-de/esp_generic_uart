# UART without Event Queue

This example uses the generic_uart component to connect to a LC76G GNSS system via UART to read the NMEA records.

The LC76G GNSS system used is a Waveshare LC76G GNSS Module. I have connected VCC, GND, TX and RX lines to the Grove Port of my ESP32 SoC:

| LC76G GNSS Module | Grove Port |
|:------------------|:-----------|
| VCC               | 5V         |
| GND               | G          |
| TX                | G33 (RX)   |
| RX                | G32 (TX)   |

First an instance of GenericUart is created without a receive task to listen for the event queue.

Then method `writeString` is used to send two commands to the LC76G device, 
* a cold start command and
* a command to set the update rate to 1 Hz.

After this initialization, method `readString` is used to receive the data from the LC76G module with a timeout of 500 ms. This is repeated 600 times.

The String read contains all the different NMEA messages, the GNSS module sends out.

The data is printed on the log like this:

``` log

I (270198) Basic LC76G GNSS receiver: Received Data (269): 
$GNGGA,141847.000,4928.197672,N,00845.357155,E,1,20,0.61,410.560,M,47.959,M,,*7A
$GNGLL,4928.197672,N,00845.357155,E,141847.000,A,A*4A
$GNGSA,A,3,13,24,19,15,17,10,22,32,20,12,,,0.93,0.61,0.70,1*04
$GNGSA,A,3,66,67,68,,,,,,,,,,0.93,0.61,0.70,2*07
$GNGSA,A,3,25,03,05,32,,,,,,,,,0.93,0.61,0.70,3*09
$GNGSA,A,3,41,33,31,,,,,,,,,,0.93,0.61,0.70,4*09
$GPGSV,4,1,14,43,,,,13,56,242,18,24,85,107,19,19,44,083,16,1*5E
$GPGSV,4,2,14,15,31,186,21,17,26,048,17,25,20,241,,10,20,289,18,1*6E
$GPGSV,4,3,14,22,17,058,17,23,14,255,,32,14,320,16,20,11,163,18,1*60
$GPGSV,4,4,14,01,01,008,,12,58,233,20,1*67
$GLGSV,2,1,05,66,30,192,27,77,65,292,17,67,58,272,24,68,24,330,17,1*7D
$GLGSV,2,2,05,86,20,094,23,1*4D
$GAGSV,2,1,08,25,66,213,16,03,65,278,16,16,60,063,09,05,48,164,20,7*77
$GAGSV,2,2,08,32,41,303,16,08,18,315,18,33,05,042,,26,03,356,,7*73
$GBGSV,2,1,06,25,43,268,16,41,42,282,21,33,34,216,20,31,26,110,23,1*77
$GBGSV,2,2,06,26,24,079,15,34,06,156,,1*7B
$GNRMC,141847.000,A,4928.197672,N,00845.357155,E,0.46,289.73,251026,,,A,V*00
$GNVTG,289.73,T,,M,0.46,N,0.86,K,A*28

```
