OUTFILE_A = ultraSonic
OUTDIR = $(HOME)/cmpt433/public/myApps
CROSS_COMPILE = arm-linux-gnueabihf-
CC_C = $(CROSS_COMPILE)gcc
CFLAGS = -pthread -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L  -Wshadow 
PUBDIR = $(HOME)/cmpt433/public/myApps

all: app
app:
	$(CC_C) $(CFLAGS) ultraSonic.c  -o $(OUTDIR)/$(OUTFILE_A)
clean:
	rm $(OUTDIR)/$(OUTFILE_A)
	rm $(OUTDIR)/$(OUTFILE_B)