CC=gcc
CFLAG=-g
OBJGROUP=drivers/ssd1681/driver_ssd1681.o drivers/ssd1681/driver_ssd1681_interface.o \
drivers/ssd1681/driver_ssd1681_basic.o \
drivers/ssd1306/driver_ssd1306.o drivers/ssd1306/driver_ssd1306_interface.o \
drivers/ssd1306/driver_ssd1306_advance.o \
drivers/scd4x/driver_scd4x.o drivers/scd4x/driver_scd4x_basic.o \
drivers/scd4x/driver_scd4x_interface.o drivers/scd4x/driver_scd4x_shot.o \
drivers/sgp30/driver_sgp30_advance.o drivers/sgp30/driver_sgp30.o \
drivers/sgp30/driver_sgp30_interface.o \
drivers/bmp180/driver_bmp180.o drivers/bmp180/driver_bmp180_basic.o \
drivers/bmp180/driver_bmp180_interface.o \
drivers/bme280/driver_bme280.o drivers/bme280/driver_bme280_basic.o \
drivers/bme280/driver_bme280_shot.o drivers/bme280/driver_bme280_interface.o \
drivers/sht4x/driver_sht4x.o drivers/sht4x/driver_sht4x_basic.o \
drivers/sht4x/driver_sht4x_interface.o \
drivers/interface/iic.o drivers/interface/spi.o drivers/interface/wire.o \
drivers/mcp9808/mcp9808.o \
drivers/displays.o logenv.o 

all: logenv

logenv: $(OBJGROUP)
	$(CC) -Wall -o logenv $(OBJGROUP) -lm -lpthread -lgpiod -lcjson

clean:
	rm *.o logenv drivers/ssd1681/*.o drivers/ssd1306/*.o \
drivers/bmp180/*.o drivers/bme280/*.o drivers/mcp9808/*.o \
drivers/scd4x/*.o drivers/sgp30/*.o

