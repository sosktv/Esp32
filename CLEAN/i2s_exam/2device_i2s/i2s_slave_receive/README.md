| Supported Targets | ESP32 |
| ----------------- | ----- |

# I2S Slave Receive Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

In this example, we initialization a slave device of i2s.

## How to Use Example

### Hardware Required

* A ESP32 Board
* A USB cable for power supply and programming

The following is the hardware connection:

|hardware|GPIO|
|:---:|:---:|
|CLK|GPIO27|
|WS|GPIO26|
|DATA|GPIO25|

### Configure the Project

```
idf.py menuconfig
```

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

Reset your development board. The application it will first record the sound through the microphone. Then it will play the recorded sound, and finally a piece of audio stored in the flash. The following is the output log:

```
partiton addr: 0x00110000; size: 2097152; label: storage
Erasing flash
partiton addr: 0x00110000; size: 2097152; label: storage
Erase size: 323584 Bytes
I2S: PLL_D2: Req RATE: 16000, real rate: 1004.000, BITS: 16, CLKM: 83, BCK: 60, MCLK: 83.333, SCLK: 32128.000000, diva: 64, divb: 21
Sound recording 5%
Sound recording 10%
...
Sound recording 97%
Sound recording 102% 
playing: 0 %
playing: 1 %
playing: 2 %
...
playing: 96 %
playing: 97 %
playing: 98 %
Playing file example:
I2S: PLL_D2: Req RATE: 16000, real rate: 1004.000, BITS: 16, CLKM: 83, BCK: 60, MCLK: 83.333, SCLK: 32128.000000, diva: 64, divb: 21
```

## Troubleshooting

* Program upload failure

    * Hardware connection is not correct: run `idf.py -p PORT monitor`, and reboot your board to see if there are any output logs.
    * The baud rate for downloading is too high: lower your baud rate in the `menuconfig` menu, and try again.
