# logenv

This is a development branch and a work in progress.  The original Logenv is available on the classic branch or as release v1.x. 

## Introduction
logenv is a Linux command-line utility for the aggregating, logging, charting and displaying of timestamped CPU core frequencies, thermal zone temperatures, ambient temperature, CPU core usage, memory usage, sensors and volts, amps and watts, from a HardKernel SmartPower2 or SmartPower3, as a single-shot or continuos interval based feed.  Many sensors are supported and logenv can also generate GNUplot scripts for any collected data set as well as a UDP network stream. Local display of data on small oled and eInk displays is supported for the SSD1306, SH1107 and SSD1681 controllers. 

![Image](./example/ocl-m2_g610-a76_1.png)

License: GPLv3.


### prerequisite
Any OS repository version of cjson should work but gpiod version 2.x is not supported at this time.  If gpiod version 1.x is not available in your repository then compile gpiod v1.64 from source.

gnuplot

cjson
https://github.com/DaveGamble/cJSON

gpiod v1.x
https://libgpiod.readthedocs.io/en/stable/

````
wget https://mirrors.edge.kernel.org/pub/software/libs/libgpiod/libgpiod-1.6.4.tar.xz
tar -xvf ./libgpiod-1.6.4.tar.xz
cd ./libgpiod-1.6.4/
./configure --enable-tools
make
sudo make install
sudo ldconfig /usr/local/lib
````

### Install
```
  git clone https://github.com/hominoids/logenv.git
  cd logenv
  make
```

  
## Usage
```
usage: logenv [options]

Options:
 -l,  --log <file>            Log to <file>
 -i,  --milliseconds <number> Poll Interval <number> in milliseconds
 -f,  --frequency             CPU core frequency
 -t,  --temperature           Thermal zone temperature
 -a,  --bme280 <device>       Temperature, Humidity, Pressure Sensor I2C 0x76 or 0x77 default /dev/i2c-0
      --bmp180 <device>       Barometric Pressure, Altitude & Temperature Sensor default /dev/i2c-0
      --bmp388 <device>       Barometric Pressure, Altitude & Temperature Sensor I2C 0x76 or 0x77
      --bmp390 <device>       Barometric Pressure, Altitude & Temperature Sensor I2C 0x76 or 0x77
      --mcp9808 <device>      High Accuracy Temperature Sensor I2C 0x18 default /dev/i2c-0
      --sht4x <device>        Temperature and Humidity I2C 0x44 default /dev/i2c-0
      --shtc3 <device>        Temperature and Humidity I2C 0x70 default /dev/i2c-0
      --aht20 <device>        Temperature and Humidity I2C 0x70 default /dev/i2c-0
      --htu31d <device>       Temperature and Humidity I2C 0x40 default /dev/i2c-0
 -p,  --smartpower3-ch1 <tty> Volt, Amp, Watt (HK SmartPower3 USBC port), default /dev/ttyUSB0
      --smartpower3-ch2 <tty>
      --smartpower2 <tty>     Volt, Amp, Watt (HK SmartPower2 microUSB port), default /dev/ttyUSB0
 -u,  --usage                 CPU core usage, aggregate and core 0 to core n-1
 -m,  --memory                Physical memory usage (total - available, see man free)
 -d,  --date                  Date and Time stamp
 -r,  --raw                   Raw output, no formatting of freq. or temp.  e.g. 35000 instead of 35
 -v,  --verbose               Readable dashboard output
 -q,  --quiet                 No output to stdout
 -o,                          Output to eInk/Oled/LCD display using logenv.json
 -n,  --udp <host>:<port>     UDP output to <host>:<port>
 -s,  --sgp30 <device>        VOC and eCO2 Sensor I2C 0x58 default /dev/i2c-0
      --scd30 <device>        CO2 Temperature and Humidity Sensor I2C 0x61 default /dev/i2c-0
      --scd41 <device>        CO2 Temperature and Humidity Sensor I2C 0x62 default /dev/i2c-0
 -g,  --gnuplot <file>        Gnuplot script generation
      --title <string>        Chart title <string>
      --xmtics <number>       Chart x-axis major second tics <number>
      --version               Version
 -h,  --help                  Help screen
```


## Example
This example is a typical data collection and charting workflow:

Every 2 seconds collect the CPU frequency, thermal zone temperatures, ambient temperature, CPU usage and SmartPower3 Volt, Amp and Watts
```
./logenv -l ocl-m2_g610-a76_1.csv -i 2000 -f -t --mcp9808 -u -p
```

Generate the GNUplot script
```
./logenv -g ocl-m2_g610-a76_1.gpl --title "OpenCL-Benchmark Odroid-M2 GPU-CPU" --xmtics 60 -i -f -t --mcp9808 -u -p
```

Generate the chart
```
gnuplot -c ocl-m2_g610-a76_1.gpl ocl-m2_g610-a76_1.png ocl-m2_g610-a76_1.csv
```


## SmartPower Settings
Settings for the Hard Kernel SmartPower3 and SmartPower2 are baud rate 115200,8N1 with no HW or SW control.

## Sensor Support
Many types of sensors are directly supported using built in drivers and require no other setup.  At this time only I2C access is available for those that also have a SPI interface.

AHT20 - Temperature & Humidity I2C ADD 0x38 - libdriver
HTU31 - Temperature & Humidity I2C ADD 0x40 - libdriver
SHT40,SHT41,SHT43,SHT45 - Temperature & Humidity I2C ADD 0x44 - libdriver
SHTC3 - Temperature & Humidity I2C ADD 0x70 - libdriver
MCP9808 - High Accuracy Temperature Sensor I2C ADD 0x18 - libdriver

BME280 - Barometric Pressure, Altitude, Temperature & Relative Humidity, ADD 0x76 or 0x77 - libdriver
BME680 - Barometric Pressure, Altitude, Temperature, Relative Humidity & VOC ADD 0x76 or 0x77 - libdriver

BMP180 - Barometric Pressure, Altitude and Temperature ADD 0xEF (read) and 0xEE (write) - libdriver
BMP388 - Barometric Pressure, Altimeter and Temperature I2C ADD 0x76 or 0x77 - libdriver
BMP390 - Barometric Pressure, Altimeter and Temperature I2C ADD 0x76 or 0x77 - libdriver
>BMP580 - Barometric Pressure, Altimeter and Temperature I2C ADD 0x47 - LINUX

SCD30 - NDIR True CO2, 400 ppm – 10,000 ppm, Temperature and Humidity Sensor I2C ADD 0x61 - libdriver
SCD40,SCD41,SCD43 - True CO2 Temperature and Humidity Sensor ADD 0x62 - libdriver
SGP30 - VOC and eCO2 I2C ADD 0x58 - libdriver




## GNUPlot Charts
Single or stacked charts are created based on the type and number of datum that are contained in the data set.  Core Frequency, Thermal Zone Temperatures, CPU core usage and SmartPower data can all be charted with the addition of Ambient Temperature when Thermal Zone Temperatures are also charted.  When a GNUPlot Script file is generated, part of it's contents is based on the number of CPU cores and the number and name of thermal Zones.  For this reason the GNUPlot script needs to be generated on the machine the data was collected from if any of those datum are included.  The GNUPlot Scripts or .gpl files can be reused and don't need to be regenerated if the type of data being collected and the machine are the same.

## UDP Client
The UDP client -n option is followed by the server host and port <host:port>. The host entry can be either an IP address or host name.  Make sure the port is open if using a firewall.  From the command-line on the server host system, netcat can be used to read the UDP feed.

```
netcat -l -u -p <port>
```

## Display Configuration
The display configuration file logenv.json is a JSON formatted file that describes the display, pages and items for each page.  The current working directory ./logenv.json is searched for first and then /etc/logenv/logenv.json.
Multple diplays is supported with each capable of multiple pages containing time, date, system information or sensor data. 

###Display Content Codes
date
time
frequency
thermal
memory
usage
sensors

## Compatibility
logenv has been tested with both a HardKernel SmartPower2 and SmartPower3, including MCP9808 and BME280 sensors.  Several architectures have been test including Armv7, Armv8, Armv9 and different generations of INTEL and AMD processor up to 32 cores.  There is no limitation on the number of cores or thermal zones with the exception of the usage option which supports up to 256 cores.  It has also been used on several Linux distributions including Ubuntu, Debian, Manjaro and Arch.  It should run on just about anything that runs Linux.
