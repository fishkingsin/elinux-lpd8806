Total Control Lighting (p9813) Embedded Linux Library

Copyright 2012 Christopher De Vries

www.idolstarastronomer.com


LPD8806 Embedded Linux Library

Copyright 2013 James Kong

www.fishkingsin.com

INTRODUCTION
=====================
This library is modified from [elinux-tcl](https://bitbucket.org/xdseed/elinux-lpd8806/src/) and designed to help you get started using the LPD8806 LED Strip  on your Raspberry Pi.
This library should work on any linux platform in which the spidev kernel
module has been included.

BACKGROUND
----------------------

Thanks to [Adafruit](http://www.adafruit.com/products/306) Reversed Enginnering and their [LPD8806 Python Library](https://github.com/Sh4d/LPD8806)
They provide very good explaination of how the LPD8806 chip is doing and the protocol 


[The-LPD8806-protocol-for-Adafruit-RGB-LED-Strips](http://riderx.info/post/The-LPD8806-protocol-for-Adafruit-RGB-LED-Strips.aspx) provide and explaination of the protocol

*   When it receives a zero byte, it resets its byteCount to 0, and writes a zero byte on the output.
*   When it receives a byte with the high bit set (ie ORed with 0x80) and its byteCount < 6, it uses the lower 7 bits of the byte to set the output of one of the PWM outputs. It then increments byteCount to move to the next PWM output.
*   If byteCount == 6, it just sends the byte to the output.

When these chips are chained together, with the inputs of the second chip connected to the output of the first chip, they are quite easy to use. You just send a total of 3 bytes per LED, and bytes 1-3 go to the first one, 4-6 to the second, and so on.  The byte order is not GRB instead of RGB, which is presumably done so that the PCB layout is simpler.

The color value is 7 bits per color, for 128 * 128 * 128 = 2,097,152 colors.

finally set 4 zero byte and the end of the buffer which mean length_of_led x 3 + 4 is the length of your total buffer size
<!-- The p9813 chip operates with a voltage between 5V and 6.5V. In addition to V+
and Ground inputs, the chip also has a clock input, serial data input, clock
output, and serial data output. The p9813 chip has a maximum serial clock rate
of 15 MHz. 

The p9813 accepts data from the Serial Data in on the rising edge of the
serial input clock signal. It will read one bit per tick. A frame of 4 bytes
(32 bits) is required to set the pixel color. Each byte is sent on MSB order.
The first byte is a flag byte, the second byte is the blue data, the third is
green, and the fourth is red. Each color is represented as an 8 bit unsigned
integer running from 0 (completely off) to 255 (completely on). The flag byte
is made up of the following bits (from highest order, or most significant,
down): -->

<!-- Bit 7: Always 1
Bit 6: Always 1
Bit 5: Inverse of blue bit 7
Bit 4: Inverse of blue bit 6
Bit 3: Inverse of green bit 7
Bit 2: Inverse of green bit 6
Bit 1: Inverse of red bit 7
Bit 0: Inverse of red bit 6 -->

<!-- As an example, the following sample of C code calculates the flag bit (this is
included in the library):

    uint8_t make_flag(uint8_t red, uint8_t green, uint8_t blue) {
      uint8_t flag;

      flag =  (red&0xc0)>>6;
      flag |= (green&0xc0)>>4;
      flag |= (blue&0xc0)>>2;

      return ~flag;
    } -->

<!-- Once a pixel's color is set, it will pass along subsequent 32 bit frames via
its clock out and serial data out ports. 

To initialize a p9813 chip to receive a new color you must send a frame
composed entirely of 0s (32 bits of 0s). In order to display the colors you
have sent to a chip, you must also send a frame of 32 bits of 0s. It appears
as though you actually must send more than one frame of 0s if the chain of
pixels is long. This library will send 1 frame of 0s to initialize the chip
prior to sending out pixel data and 2 frames of 0s to display the pixels
after.

The BeagleBone and Raspberri PI have processors with built-in support for SPI,
making it very fast.  In order to use this capability from userspace linux
programs, you must use a linux kernel with the spidev module built in. The
Angstrom linux BeagleBone demo image (available at
http://downloads.angstrom-distribution.org/demo/beaglebone/) starting with the
June 18, 2012 image have spidev compiled in. I have done my testing with the
June 18, 2012 image available at
http://downloads.angstrom-distribution.org/demo/beaglebone/archive/Angstrom-Cloud9-IDE-GNOME-eglibc-ipk-v2012.05-beaglebone-2012.06.18.img.xz
You can find more information about installing the SPI drivers on your
Raspberry Pi by reading this entry in the Brian's Life blog:
http://www.brianhensley.net/2012/07/getting-spi-working-on-raspberry-pi.html . -->

HARDWARE
----------------------
The LPD8806 strands, sold by http://www.adafruit.com/products/306, have 4 wires
running from pixel to pixel. These wires are color coded in the following way:

GND: Ground
DI: Data
CI: Clock
+5V: Vcc (+5 to +6.5 V)

Each pixel has an input set of wires and an output set. Usually there is an
arrow indicating the direction on the casing of the LED/chip unit.
<!--  It is more
convenient to use the connectors which you can purchase from coolneon.com. You
will need the female connector to send data to the pixels.  -->
The wire coloring on the connector is slightly different:

GND: Ground
DO: Data
CO: Clock
+5V: Vcc (+5 to +6.5 V)

It is useful, though not required, to purchase the "T-connectors" sold at
coolneon.com. The T-connectors pass through the Data and Clock lines, but
splice the Vcc and Ground lines to a 2.5mm center positive barrel connector.
We typically find that we provide power by using a T-connector every 100
pixels or so (depending on the application). 

<!-- BEAGLEBONE CONNECTIONS

On the BeagleBone the following pins are configured for SPI:

P9 Header Pin 30: Data out (SPI1_D1)
P9 Header Pin 31: Clock out (SPI1_SCLK)

You should also hook up a ground wire from the pixels and the power supply for
the pixels to P9 Header Pin 1 or 2. A typical wiring diagram is shown below:

<pre>
+-----------------------------+
|                         5 V +---------+
| Pixel Power Source          |         |
|                         GND +----+    |
+-----------------------------+    |    |
                                   |    |     +-----------------------------+
+-----------------------------+    |    +-----+ V+                          |
+                   P9 Pin 1  +----+----------+ GND        lpd8806 Pixel        |
+ BeagleBone        P9 Pin 30 +---------------+ Data                        |
+                   P9 Pin 31 +---------------+ Clock                       |
+-----------------------------+               +-----------------------------+
</pre> -->
RASPBERRY PI CONNECTIONS

On the Raspberry PI the following pins are configured for SPI:

Pin 19: Data out (SP10 MOSI)
Pin 23: Clock out (SP10 SCLK)

You should also hook up a ground wire from the pixels and the power supply for
the pixels and connect it to Pin 6 so the Raspberry Pi and the pixels share a
common ground. A typical wiring diagram is shown below:

<pre>
+-----------------------------+
|                         5 V +---------+
| Pixel Power Source          |         |
|                         GND +----+    |
+-----------------------------+    |    |
+-----------------------------+    |    |     +-----------------------------+
+                      Pin 4  +----|----+-----+ 5V+                          |
+                      Pin 6  +----+----------+ GND        lpd8806 Pixel     |
+ Raspberry Pi         Pin 19 +---------------+ Clock                        |
+                      Pin 23 +---------------+ Data                         |
+-----------------------------+               +-----------------------------+
</pre>

LIBRARY

This library is written in ANSI C, and I will demonstrate how to use it by
creating a simple program below.  To use the C library, you first need to
include several header files, including "lpd8806led.h" for this library, and
"fcntl.h" and "unistd.h" for low level I/O. To run the examples below you will
also need to include "stdio.h". Include all the header files like so:

    #include "lpd8806led.h"
    #include <fcntl.h>
    #include <unistd.h>
    #include <stdio.h>

Within the program, you will want to open the spidev device using low-level
I/O, and initiate the SPI bus. In this example I use "/dev/spidev2.0" as the
device, which is typical on the BeagleBone. For the Raspberry Pi this is
typically "/dev/spidev0.0". Initialization can be accomplished using the code
below.

    int fd;  /* SPI device file descriptor */

    /* Open SPI device */
    fd = open("/dev/spidev2.0",O_WRONLY);
    if(fd<0) {
        /* Open failed */
        fprintf(stderr, "Error: SPI device open failed.\n");
        exit(1);
    }

    /* Initialize SPI bus for lpd8806 pixels */
    if(spi_init(fd)<0) {
        /* Initialization failed */
        fprintf(stderr, "Unable to initialize SPI bus.\n");
        exit(1);
    }

Next create a buffer for your pixel color and flag values. I will assume there
are 50 LEDs in our Total Control Lighting strand for the example below.

    const int leds = 50; /* 50 LEDs in the strand */
    lpd8806_buffer buf;      /* Memory buffer for pixel values */

    /* Allocate memory for the pixel buffer and initialize it */
    if(lpd8806_init(&buf,leds)<0) {
        /* Memory allocation failed */
        fprintf(stderr, "Insufficient memory for pixel buffer.\n");
        exit(1);
    }

<!-- After creating the pixel buffer, you can then start writing to it. There are
two options. You can send raw pixel colors to the lpd8806 strands using the
"write_color" function. This function accepts red, green, and blue colors as
uint8_t integers in the range of 0 to 255. Unfortunately a color of 127 for
any of the LED colors does not appear to be half as bright as 255. It is in
fact much brighter to our eyes. Therefore, you can also use the
"write_gamma_color" function which will apply a gamma correction for each
color that you establish using the "set_gamma" function. Both the
"write_color" and "write_gamma_color" functions accept four arguments. The
first is a pointer to a lpd8806_color structure. The second is a uint8_t integer
that represents red, the third is a uint8_t integer representing green, and
the fourth is a uint8_t integer representing blue. To set the 24th pixel in a
strand to solid bright blue, I could use the following code:
 -->
for LPD8806 you only use  "write_gamma_color" to write to gamma corrected buffer to set you color to 128-255 value
"write_color" dosent work for LPD8806 ,because it only read 7bits from 128-255 to represent each color
<!-- write_color(&buf.pixels[24],0,0,255); -->

<!-- If I wanted to apply gamma correction to that pixel, and set it to about 1/2
brightness green. I would use the following code:
 -->

    set_gamma(2.5,2.5,2.5);
    write_gamma_color(&buf.pixels[24],0,127,0);

<!-- Note that before using the "write_gamma_color" function you must set the gamma
correction factors. They can be set differently for each LED color. Values
betwen 2.2 and 3.0 appear to work best. The "set_gamma" function takes three
double precision floating point values as arguments. The red gamma correction
facor is first, followed by the green and then the blue factor. To set all
colors to a gamma correction of 2.2 I would use the following code:
 -->
    

<!-- The gamma correction function sets up a lookup table so that each
"write_gamma_color" will be fast. -->

Finally, you must send the data over the SPI bus to the pixels. To do this you
use the function "send_buffer", which takes two arguments. The file descriptor
and a pointer to the lpd8806_buffer structure. The send_buffer function will
always either send all the data in the buffer (and return the number of bytes
sent) or return a negative integer when an error occurs. Sending the data can
in the lpd8806_buffer buf can be accomplished with the code below.

    /* Send the data to the lpd8806 lighting strand */
    if(send_buffer(fd,&buf)<0) {
        fprintf(stderr, "Error sending data.\n");
        exit(1);
    }

When you are finished using the pixels you should free up the allocated memory
in the lpd8806_buffer structure and close the file description. This can be
accomplished with the code below.

    /* Free the pixel buffer */
    lpd8806_free(&buf);

    /* Close the SPI device */
    close(fd);

COMPILING

The code compiles with the gcc compiler available on each platform. Note that
you must include the math library ( -lm ) when compiling because the gamma
correction subroutine makes use of it. Feel free to look at the included
Makefile for information about compiling the code. You must compile the
following files along with your source code:

lpd8806led.h - The library header file.
lpd8806led.c - The library source code.

ADDITIONAL RESOURCES

Additional documentation for the individual functions can be found in the
lpd8806led.h header file. There are also a few sample programs:

1. simple_example.c - This runs through a simple example setting the lights to
be red, green, and blue and cycling through those colors with one color change
a second. 

2. blink_test.c - This is a code which blinks random gamma corrected colors at
random intervals all along the strand.

3. lpd8806_speedtest.c - This sample program sends 10,000 images (of a single blue
moving along a strand) to a strand of 1,250 pixels as fast as it can. It
measures the time elapsed and calculates the number of frames per second.


Version history
------------

### Version 0.1 (Date):20130321


[![Bitdeli Badge](https://d2weczhvl823v0.cloudfront.net/fishkingsin/elinux-lpd8806/trend.png)](https://bitdeli.com/free "Bitdeli Badge")

