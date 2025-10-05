CC=gcc
CFLAG=-g
OBJGROUP=displays.o drivers/bme280/bme280-i2c.o drivers/bme280/bme280.o drivers/bme280/bmp180.o \
drivers/ssd1681/driver_ssd1681.o drivers/ssd1681/driver_ssd1681_interface.o \
drivers/ssd1681/driver_ssd1681_basic.o drivers/ssd1681/spi.o drivers/ssd1681/wire.o logenv.o

all: logenv

logenv: $(OBJGROUP)
	$(CC) -Wall -o logenv $(OBJGROUP) -lm -lpthread -lgpiod -lcjson

clean:
	rm *.o logenv drivers/bme280/*.o drivers/ssd1681/*.o

