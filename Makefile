CC=gcc
CFLAG=-Wall -g -std=c11
LDFLAG=-lm -lpthread -lgpiod -lcjson

OBJGROUP=drivers/ssd1681/driver_ssd1681.o drivers/ssd1681/driver_ssd1681_interface.o \
	 drivers/ssd1681/driver_ssd1681_basic.o \
	 drivers/ssd1306/driver_ssd1306.o drivers/ssd1306/driver_ssd1306_interface.o \
	 drivers/ssd1306/driver_ssd1306_advance.o \
	 drivers/st7789/driver_st7789.o drivers/st7789/driver_st7789_interface.o \
	 drivers/st7789/driver_st7789_basic.o \
	 drivers/scd30/driver_scd30.o drivers/scd30/driver_scd30_basic.o \
	 drivers/scd30/driver_scd30_interface.o \
	 drivers/scd4x/driver_scd4x.o drivers/scd4x/driver_scd4x_basic.o \
	 drivers/scd4x/driver_scd4x_interface.o drivers/scd4x/driver_scd4x_shot.o \
	 drivers/sgp30/driver_sgp30_advance.o drivers/sgp30/driver_sgp30.o \
	 drivers/sgp30/driver_sgp30_interface.o \
	 drivers/bmp180/driver_bmp180.o drivers/bmp180/driver_bmp180_basic.o \
	 drivers/bmp180/driver_bmp180_interface.o \
	 drivers/bmp388/driver_bmp388.o drivers/bmp388/driver_bmp388_basic.o \
	 drivers/bmp388/driver_bmp388_interface.o \
	 drivers/bmp390/driver_bmp390.o drivers/bmp390/driver_bmp390_basic.o \
	 drivers/bmp390/driver_bmp390_interface.o \
	 drivers/bme280/driver_bme280.o drivers/bme280/driver_bme280_basic.o \
	 drivers/bme280/driver_bme280_shot.o drivers/bme280/driver_bme280_interface.o \
	 drivers/bme680/driver_bme680.o drivers/bme680/driver_bme680_basic.o \
	 drivers/bme680/driver_bme680_gas.o drivers/bme680/driver_bme680_interface.o \
	 drivers/sht4x/driver_sht4x.o drivers/sht4x/driver_sht4x_basic.o \
	 drivers/sht4x/driver_sht4x_interface.o \
	 drivers/shtc3/driver_shtc3.o drivers/shtc3/driver_shtc3_basic.o \
	 drivers/shtc3/driver_shtc3_interface.o \
	 drivers/aht20/driver_aht20.o drivers/aht20/driver_aht20_basic.o \
	 drivers/aht20/driver_aht20_interface.o \
	 drivers/htu31d/driver_htu31d.o drivers/htu31d/driver_htu31d_basic.o \
	 drivers/htu31d/driver_htu31d_interface.o \
	 drivers/interface/iic.o drivers/interface/spi.o drivers/interface/wire.o \
	 drivers/interface/uart.o drivers/mcp9808/mcp9808.o \
	 drivers/displays.o logenv.o

all: logenv

logenv: $(OBJGROUP)
	$(CC) $(CFLAG) -o logenv $(LDFLAG) $(OBJGROUP)

clean:
	rm *.o logenv drivers/*.o drivers/interface/*.o \
	drivers/ssd1681/*.o drivers/ssd1306/*.o drivers/st7789/*.o \
	drivers/bmp180/*.o drivers/bme280/*.o drivers/mcp9808/*.o \
	drivers/scd4x/*.o drivers/sgp30/*.o drivers/sht4x/*.o drivers/shtc3/*.o \
	drivers/aht20/*.o drivers/htu31d/*.o drivers/bmp388/*.o drivers/scd30/*.o \
	drivers/bme680/*.o drivers/bmp390/*.o

