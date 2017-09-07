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

