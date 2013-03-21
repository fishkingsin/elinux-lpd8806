/*
 * blink_test.c
 *
 * Copyright 2012 Christopher De Vries
 * This program is distributed under the Artistic License 2.0, a copy of which
 * is included in the file LICENSE.txt
 */
#include "lpd8806led.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/time.h>
#include <stdlib.h>
#include <signal.h>

static const char *device = "/dev/spidev0.0";
static const int leds = 500;
static const unsigned long min_interval = 700000L;
static const unsigned long max_interval = 1300000L;
static int continue_looping;

unsigned long microseconds_since(struct timeval *tv);
void stop_program(int sig);

int main(int argc, char *argv[]) {
  lpd8806_buffer buf;
  int fd;
  int return_value;
  lpd8806_color *p;
  int i;
  struct timeval start_time;
  unsigned long *change_times;
  unsigned long current_time;
  
  /* Open the device file using Low-Level IO */
  fd = open(device,O_WRONLY);
  if(fd<0) {
    fprintf(stderr,"Error %d: %s\n",errno,strerror(errno));
    exit(1);
  }

  /* Initialize the SPI bus for Total Control Lighting */
  return_value = spi_init(fd);
  if(return_value==-1) {
    fprintf(stderr,"SPI initialization error %d: %s\n",errno, strerror(errno));
    exit(1);
  }

  /* Initialize pixel buffer */
  if(lpd8806_init(&buf,leds)<0) {
    fprintf(stderr,"Pixel buffer initialization error: Not enough memory.\n");
    exit(1);
  }

  /* Set the gamma correction factors for each color */
  set_gamma(2.5,2.5,2.5);

  /* Get the current time */
  return_value = gettimeofday(&start_time,NULL);
  if(return_value==-1) {
    fprintf(stderr, "Error reading the current time: %s\n", strerror(errno));
    exit(1);
  }

  /* Set scattered intervals for the lights to change */
  /* Set all pixels to black */
  change_times = (unsigned long *)malloc(leds*sizeof(unsigned long));
  if(change_times==NULL) {
    fprintf(stderr, "Error allocating array of change times.\n");
    exit(1);
  }
  for(i=0;i<leds;i++) {
    change_times[i] = min_interval+random()%(max_interval-min_interval);
    write_gamma_color(&buf.pixels[i],0x00,0x00,0x00);
  }

  send_buffer(fd,&buf);

  /* Prepare to receive ctrl-c to stop looping */
  continue_looping = 1;
  signal(SIGINT,stop_program);

  /* Loop while continue_looping is true */
  while(continue_looping) {
    current_time = microseconds_since(&start_time);
    for(i=0;i<leds;i++) {
      if(current_time>change_times[i]) {
        p = &buf.pixels[i];
        if(p->red==128 && p->blue==128 && p->green==128) {
          write_gamma_color(p,(int)random()%256,(int)random()%256,(int)random%256);
        }
        else {
          write_gamma_color(p,0x00,0x00,0x00);
        }
        change_times[i]=current_time+min_interval+random()%(max_interval-min_interval);
      }
    }
    send_buffer(fd,&buf);
  }

  for(i=0;i<leds;i++) {
    p = &buf.pixels[i];
      write_gamma_color(p,0x00,0x00,0x00);
      
  }
    send_buffer(fd,&buf);


  lpd8806_free(&buf);
  free(change_times);
  close(fd);
  printf("Program Terminated.\n");

  return 0;
}

unsigned long microseconds_since(struct timeval *tv) {
  int ret;
  struct timeval now;
  struct timeval diff;
  unsigned long retval;

  ret = gettimeofday(&now,NULL);
  if(ret==-1) {
    return 0L;
  }

  diff.tv_sec=now.tv_sec-tv->tv_sec;
  diff.tv_usec=now.tv_usec-tv->tv_usec;

  retval = diff.tv_sec*1000000L;
  retval += diff.tv_usec;

  return retval;
}

void stop_program(int sig) {
  /* Ignore the signal */
  signal(sig,SIG_IGN);

  /* stop the looping */
  continue_looping = 0;

  /* Put the ctrl-c to default action in case something goes wrong */
  signal(sig,SIG_DFL);
}
