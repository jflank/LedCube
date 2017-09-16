# LedCube
Models an 8x8x8 LED cube using OpenGL on linux. Future versions will program the cube via the serial port as well.

There are some commands on the GLcube that are slowly growing.
* Change rotation the "uijkm," keys.
* speed-up/slow down "-+"

To see the 5x5x5 Z cube being solved, just make & run:
make
./LedCube -g -s

Options:
	-h	Show this help message
	-g	See The OpenGL threads only
	-a	Set the solver speed (-a 1 is around 1 piece per second)
	-s	See The solver in action
	-p	Send data over the serial port


Search for "Great Z Cube Wooden Puzzle Brain Teaser" for info on what is being solved.


... To program the cube with the programmable firmware...
joshua:~/Programs/py/stcgal$ sudo ./stcgal.py ../../ledcube8x8x8/firmware/v2/ledcube8.hex 
Waiting for MCU, please cycle power: done
Protocol detected: stc12
Target model:
  Name: STC12C5A60S2
  Magic: D17E
  Code flash: 60.0 KB
  EEPROM flash: 2.0 KB
Target frequency: 12.269 MHz
Target BSL version: 7.1I
Target options:
  reset_pin_enabled=True
  low_voltage_reset=False
  oscillator_stable_delay=32768
  por_reset_delay=long
  clock_gain=high
  clock_source=external
  watchdog_por_enabled=False
  watchdog_stop_idle=True
  watchdog_prescale=256
  eeprom_erase_enabled=False
  bsl_pindetect_enabled=False
Loading flash: 3806 bytes (Intel HEX)
Switching to 19200 baud: testing setting done
Erasing 16 blocks: done
Writing 4096 bytes: ................................ done
Finishing write: done
Setting options: done
Target UID: D17E013F41E83A
Disconnected!
joshua:~/Programs/py/stcgal$ 5~^C

