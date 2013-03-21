/*
 * lpd8806_speedtest.c
 *
 * Copyright 2012 Christopher De Vries
 * This program is distributed under the Artistic License 2.0, a copy of which
 * is included in the file LICENSE.txt
 */
#include <unistd.h>
#include <stdio.h>
#include "lpd8806led.h"
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>

static const char *device = "/dev/spidev0.0";
static const int leds = 500;
static const int frames = 10000;

int main(int argc, char *argv[]) {
  lpd8806_buffer buf;
  int fd;
  int ret;
  int i, j;
  lpd8806_color *p;
  struct timeval tv_start, tv_end, tv_diff;
  struct timezone tz;
  double fps;
set_gamma(2.5,2.5,2.5);
  tz.tz_minuteswest=0;
  tz.tz_dsttime=0;

  ret = gettimeofday(&tv_start,&tz);
  if(ret==-1) {
    fprintf(stderr,"Error %d: %s\n",errno, strerror(errno));
    exit(1);
  }

  fd = open(device,O_WRONLY);
  if(fd<0) {
    fprintf(stderr, "Can't open device.\n");
    exit(1);
  }

  ret=spi_init(fd);
  if(ret==-1) {
    fprintf(stderr,"error=%d, %s\n", errno, strerror(errno));
    exit(1);
  }

  lpd8806_init(&buf,leds);

  for(i=0;i<frames;i++) {
    p = buf.pixels;
    for(j=0;j<leds;j++) {
      if(j==i%leds) {
        write_gamma_color(p,0xff,0xff,0xff);
      }
      else {
        write_gamma_color(p,0x00,0x00,0x00);
      }
      p++;
    }
    send_buffer(fd,&buf);
    /* usleep(1000000); */
  }

  ret = gettimeofday(&tv_end,&tz);
  if(ret==-1) {
    fprintf(stderr,"Error %d: %s\n",errno, strerror(errno));
    exit(1);
  }

  tv_diff.tv_sec=tv_end.tv_sec-tv_start.tv_sec;
  tv_diff.tv_usec=tv_end.tv_usec-tv_start.tv_usec;

  while(tv_diff.tv_usec<0) {
    tv_diff.tv_usec+=1000000;
    tv_diff.tv_sec-=1;
  }

  printf("Time elapsed: %d sec and %d microseconds.\n",tv_diff.tv_sec,tv_diff.tv_usec);

  fps = (double)frames/((double)tv_diff.tv_sec+(double)tv_diff.tv_usec/1000000.0);

  printf("%.2f frames per second.\n",fps);

  lpd8806_free(&buf);
  close(fd);
  return 0;
}
