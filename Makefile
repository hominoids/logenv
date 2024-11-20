CC=gcc
CFLAG=-g
OBJGROUP=bme280/bme280-i2c.o bme280/bme280.o bme280/bmp180.o logenv.o

all: logenv

logenv: $(OBJGROUP)
	$(CC) -Wall -o logenv $(OBJGROUP) -lm

clean:
	rm *o logenv bme280/*o

