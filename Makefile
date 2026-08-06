CC=gcc
CFLAG=-Wall -g -std=c11
LDFLAG=-lm -lpthread

OBJGROUP=drivers/mcp9808/mcp9808.o \
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
	 drivers/pmsa003i/driver_pmsa003i.o drivers/pmsa003i/driver_pmsa003i_basic.o \
	 drivers/pmsa003i/driver_pmsa003i_interface.o \
	 drivers/interface/iic.o \
	 logenv.o

all: logenv

logenv: $(OBJGROUP)
	$(CC) $(CFLAG) -o logenv $(OBJGROUP) $(LDFLAG)

clean:
	rm *.o logenv drivers/interface/*.o \
	drivers/bmp180/*.o drivers/bmp388/*.o drivers/bmp390/*.o \
	drivers/bme280/*.o drivers/bme680/*.o \
	drivers/mcp9808/*.o drivers/sht4x/*.o drivers/shtc3/*.o drivers/aht20/*.o drivers/htu31d/*.o \
	drivers/scd30/*.o drivers/scd4x/*.o drivers/sgp30/*.o drivers/pmsa003i/*.o


