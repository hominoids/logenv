CC=gcc
CFLAG=-g
OBJGROUP=drivers/bme280/bme280-i2c.o drivers/bme280/bme280.o drivers/bme280/bmp180.o \
drivers/mcp9808/mcp9808.o \
drivers/ssd1681/driver_ssd1681.o drivers/ssd1681/driver_ssd1681_interface.o \
drivers/ssd1681/driver_ssd1681_basic.o drivers/ssd1681/spi.o drivers/ssd1681/wire.o \
drivers/scd4x/driver_scd4x.o drivers/scd4x/driver_scd4x_basic.o \
drivers/scd4x/driver_scd4x_interface.o drivers/scd4x/driver_scd4x_shot.o \
drivers/sgp30/driver_sgp30_advance.o  drivers/sgp30/driver_sgp30.o  \
drivers/sgp30/driver_sgp30_interface.o \
drivers/interface/iic.o displays.o  logenv.o 

all: logenv

logenv: $(OBJGROUP)
	$(CC) -Wall -o logenv $(OBJGROUP) -lm -lpthread -lgpiod -lcjson

clean:
	rm *.o logenv drivers/bme280/*.o drivers/ssd1681/*.o drivers/mcp9808/*.o drivers/scd4x/*.o

