/*
 * lpd8806led.h
 *
 * Copyright 2012 Christopher De Vries
 * This program is distributed under the Artistic License 2.0, a copy of which
 * is included in the file LICENSE.txt
 */
#ifndef _lpd8806LED_H
#define _lpd8806LED_H
#include <stdint.h>
#include <linux/types.h>
#include <stdlib.h>

/* This header file contains a bit of information about each function and
 * data structure in the library. More information can be found in the
 * README.txt file which accompanies this library.
 */

/* The lpd8806_color structure consists of 4 bytes (32 bits) in the order in which
 * the p9813 chip expects them (flag byte, followed by blue, green, and red). 
 * You can modify the colors directly, however the functions write_color and
 * write_gamma_color are designed to write to the appropriate data to memory
 * with a properly calculated flag byte.
 */
typedef struct _lpd8806_color {
  // uint8_t flag;
  
  uint8_t green;
  uint8_t red;
  uint8_t blue;
} lpd8806_color;

/* The lpd8806_buffer structure is a buffer in which pixel data is stored in
 * in memory prior to being written out to the SPI device for transmission
 * to the pixels. A buffer can be allocated with the lpd8806_init function and 
 * deallocated with the lpd8806_free function. The buffer contains a pointer to
 * an array of lpd8806_color structures representing the array of colors you will
 * send to the leds using the send_buffer function. The "pixels" element of
 * this structure points to the array of pixels.
 */
typedef struct _lpd8806_buffer {
  int leds; /* number of LEDS */
  size_t size; /* size of buffer */
  lpd8806_color *buffer; /* pointer to buffer memory */
  lpd8806_color *pixels; /* pointer to start of pixels */
} lpd8806_buffer;

/* The lpd8806_init function allocates memory for the pixels in an order that
 * allows for efficient transfer by the send_buffer command. The function
 * takes two arguments:
 *
 * lpd8806_buffer *buf - A pointer to a lpd8806_buffer structure.
 * int leds - The number of LEDs for which memory is to be allocated.
 *
 * The function returns 0 if successful or a negative number if it cannot
 * allocate sufficient memory.
 */
int lpd8806_init(lpd8806_buffer *buf, int leds);

/* The spi_init function initializes the SPI device for use by the lpd8806
 * LED strands. It takes one argument:
 *
 * int filedes - An open file descriptor to an spidev device.
 *
 * The function returns a negative number if the initialization fails.
 */
int spi_init(int filedes);

/* The write_color function writes the lpd8806_color structure corresponding to
 * a color to a location in memory. Typically this location is part of the
 * lpd8806_buffer's allocated memory. The function takes four arguments:
 *
 * lpd8806_color *p - A pointer to a lpd8806_color structure to write to.
 * uint8_t red - The red value between 0 (off) and 255 (full on).
 * uint8_t green - The green value between 0 (off) and 255 (full on).
 * uint8_t blue - The blue value between 0 (off) and 255 (full on).
 */
void write_color(lpd8806_color *p, uint8_t red, uint8_t green, uint8_t blue);

/* The send_buffer function writes the contents of the lpd8806_buffer structure to
 * the spi device. It takes two arguments:
 *
 * int filedes - The open and initialized file descriptor for the spi device.
 * lpd8806_buffer *buf - A pointer to a lpd8806_buffer which will be transferred to spi.
 *
 * This function returns the number of bytes written if successful or a
 * negative number if it fails. This function will always block while writing
 * and ensure all data is transferred out to the SPI bus.
 */
int send_buffer(int filedes, lpd8806_buffer *buf);

/* The lpd8806_free function will free memory allocated by the lpd8806_init function.
 * It wakes one argument:
 *
 * lpd8806_buffer *buf - A pointer to a lpd8806_buffer that has been allocated by
 *      lpd8806_init.
 */
void lpd8806_free(lpd8806_buffer *buf);

/* The set_gamma function creates lookup tables which are used to apply the
 * gamma correction in the write_gamma_color function. Separate gamma
 * correction factors are chosen for each color. This function must be called
 * prior to using the write_gamma_color function. It takes three arguments:
 *
 * double gamma_red - The gamma correction for the red LEDs.
 * double gamma_green - The gamma correction for the green LEDs.
 * double gamma_blue - The gamma correction for the blue LEDs.
 *
 * Values between 2.2 and 3.0 appear to work well.
 */
void set_gamma(double gamma_red, double gamma_green, double gamma_blue);

/* The write_gamma_color function writes a gamma corrected pixel color to a
 * lpd8806_pixel structure in memory. It behaves the same way as the write_color
 * function except that a gamma correction is applied to each color. It takes
 * four arguments:
 *
 * lpd8806_color *p - A pointer to the location in which to write the pixel data.
 * uint8_t red - The red value between 0 (off) and 255 (full on).
 * uint8_t green - The green value between 0 (off) and 255 (full on).
 * uint8_t blue - The blue value between 0 (off) and 255 (full on).
 */
void write_gamma_color(lpd8806_color *p, uint8_t red, uint8_t green, uint8_t blue);

#endif /*!_lpd8806LED_H*/
