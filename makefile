CC = gcc
CFLAGS = -Wall -Wextra -std=c99
AR = ar
ARFLAGS = rcs

LIB = libFilmMaster2000.a

all: $(LIB) runme

$(LIB): video.o
	$(AR) $(ARFLAGS) $(LIB) video.o

runme: main.o $(LIB)
	$(CC) $(CFLAGS) -o runme main.o -L. -lFilmMaster2000

main.o: main.c video.h
	$(CC) $(CFLAGS) -c main.c

video.o: video.c video.h
	$(CC) $(CFLAGS) -c video.c

clean:
	rm -f *.o runme $(LIB) output*.bin

test: runme
	./runme test.bin output_reverse.bin -S reverse
	./runme test.bin output_swap.bin -M swap_channel 0,2
	./runme test.bin output_clip.bin -M clip_channel 1 [10,200]
	./runme test.bin output_scale.bin -S scale_channel 1 1.5

	python3 verify_output.py