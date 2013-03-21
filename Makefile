CFLAGS = -O3
CC = gcc
BUNDLE = Makefile README.txt LICENSE.txt lpd8806led.h lpd8806led.c lpd8806_speedtest.c blink_test.c simple_example.c
VERSION = 1.0
ARCHIVE = elinux-lpd8806
LIBS = -lm

all: lpd8806_speedtest blink_test simple_example pattern_test_01 rainbow blink_hsv

archive: $(BUNDLE)
	mkdir $(ARCHIVE)-$(VERSION)
	cp $(BUNDLE) $(ARCHIVE)-$(VERSION)/
	tar cvfz $(ARCHIVE)-$(VERSION).tar.gz $(ARCHIVE)-$(VERSION)
	rm -rf $(ARCHIVE)-$(VERSION)

clean:
	$(RM) *.o
	$(RM) $(ARCHIVE)-$(VERSION).tar.gz

lpd8806_speedtest: lpd8806_speedtest.o lpd8806led.o
	$(CC) $(CFLAGS) $(LIBS) -o $@ $^

lpd8806_speedtest.o: lpd8806led.h lpd8806_speedtest.c

blink_hsv.o: lpd8806led.h blink_hsv.c

blink_hsv: blink_hsv.o lpd8806led.o
	$(CC) $(CFLAGS) $(LIBS) -o $@ $^

blink_test.o: lpd8806led.h blink_test.c

blink_test: blink_test.o lpd8806led.o
	$(CC) $(CFLAGS) $(LIBS) -o $@ $^

rainbow.o: lpd8806led.h rainbow.c

rainbow: rainbow.o lpd8806led.o
	$(CC) $(CFLAGS) $(LIBS) -o $@ $^

pattern_test_01.o: lpd8806led.h pattern_test_01.c

pattern_test_01: pattern_test_01.o lpd8806led.o
	$(CC) $(CFLAGS) $(LIBS) -o $@ $^

simple_example.o: lpd8806led.h simple_example.c

simple_example: simple_example.o lpd8806led.o
	$(CC) $(CFLAGS) $(LIBS) -o $@ $^

lpd8806led.o: lpd8806led.h lpd8806led.c
