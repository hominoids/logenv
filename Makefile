CC=gcc
CFLAG=-Wall -g -std=c11 -lm -lpthread
OBJGROUP=bme280/bme280-i2c.o bme280/bme280.o bme280/bmp180.o logenv.o

all: logenv

logenv: $(OBJGROUP)
	$(CC) $(CFLAG) -o logenv $(OBJGROUP)

clean:
	rm *.o logenv bme280/*.o

