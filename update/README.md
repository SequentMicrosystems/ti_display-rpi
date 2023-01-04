# update

This is the HEATWEB DISPLAY board firmware update tool.

## Usage

```bash
~$ git clone https://github.com/SequentMicrosystems/ti_display-rpi.git
~$ cd ti_display-rpi/update/
~/ti_display-rpi/update$ ./update
```

If you clone the repository already, skip the first step. 
The command will download the newest firmware version from our server and write it  to the board.
Please make sure no program or script tries to access the I2C port during update. 
