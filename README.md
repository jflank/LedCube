# LedCube

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/8df80e1f37a340bd932bf18a0832d2a8)](https://www.codacy.com/app/jhflank/LedCube?utm_source=github.com&utm_medium=referral&utm_content=jflank/LedCube&utm_campaign=badger)

Models an 8x8x8 LED cube using OpenGL on linux - can be used to send cube info to the serial port as well.

To just try clicking on some Led's, and see them light up on cube via a serial
port connection, just make and run (need root access for serial connection) 

make

./sudo LedCube -g -p

Options:

        -h      Show this help message
        
        -g      Show the cube in OpenGL <uijkm for rotation. -/= for size>.
        
        -a #    Set the solver speed (-a 1 is around 1 piece per second)
        
        -s [5|8|d2|d3]
        
                 5   - Show 5x5 solver
                 
                 8   - Show 8 queens solver
                 
                 d2  - Dijskstra's in 2d - edges unidirectional
                 
                 d3  - Dijskstra's in 3d - edges unidirectional
                 
                 d30 - Dijskstra's in 3d - edges going in both directions
                 
        -p      Send data over the serial port
        
        -z [0-10]       Run different animations
        
                 0-2 - planes moving up/down left/right.
                 
                 3-4 - sine wave up/down left/right.
                 
                 5   - pulse.
                 
                 6   - rain.
                 
                 7   - spiral.
                 
                 8   - random Expand/contract.
                 
                 9   - display characters - ex: '-z 9HELLOWORLD'.
                 
                 10  - Current time, moving around the cube.
                 
        -S #    Set the size of the Cube (supported for g|"s d" right now.)
        


More Examples:

Solve the "Great Z Cube Wooden Puzzle Brain Teaser" and view it on a 8x8x8 matrix, while sending it via the serial port to the 8x8x8 LED cube!

./sudo LedCube -g -s 5 -p

<then press the keys: u,j,m to set the opengl cube spinning>


See Dijsktra's algorithm in action!

Try ./LedCube -g -S 20 -s d2 

<then press equals a few times "====" to expand out the LED's.


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


If you want to build your own HW 8x8x8 LED cube, check out: https://www.youtube.com/watch?v=iChNCz5YP-E&t=1990s
