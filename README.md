# logenv

This is a development branch and a work in progress that is not fully completed or documented.  The original Logenv is available on the classic branch or as release v1.0. 

## Introduction
logenv is a Linux command-line utility for the aggregating, logging, charting and displaying of timestamped CPU core frequencies, thermal zone temperatures, ambient temperature, CPU core usage, memory usage, sensors data and volts, amps and watts, from a HardKernel SmartPower2 or SmartPower3, as a single-shot or continuos interval based feed.  Many sensors are supported and logenv can also generate GNUplot scripts for any collected data set, as well as provide a UDP network stream. Local display of data on small oled and eInk displays is supported for the SSD1306, SH1107 and SSD1681 controllers. 

![Image](./example/ocl-m2_g610-a76_1.png)

License: GPLv3.


### Prerequisite

cjson and gpiod are required to compile logenv while gnuplot is only needed if generating charts. Any OS repository version of cjson should work but gpiod version 1.x is needed until 2.x is supported.  If gpiod version 1.x is not available in your repository then compile gpiod v1.64 from source.

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
````

If the installed library path is not in the ldconfig cache then add the library path(default /usr/local/lib) on a new line at the end of /etc/ld.so.conf or create an entry in the directory /etc/ld.so.conf.d. 
```
$ sudo vi /etc/ld.so.conf
  add /usr/local/lib or installed path
$ sudo ldconfig
```

or

```
$ echo "/usr/local/lib" | sudo tee /etc/ld.so.conf.d/custom.conf
$ sudo ldconfig
```

### Build
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
        -i,  --milliseconds <number> Polling Interval <number> in milliseconds
        -d,  --date                  Date and Time stamp
        -f,  --frequency             CPU core frequency
        -t,  --temperature           Thermal zone temperature
        -u,  --usage                 CPU core usage, aggregate and core 0 to core n-1
        -m,  --memory                Physical memory usage (total - available, see man free)
        -p,  --smartpower3-ch1 <tty> Volt, Amp, Watt (HK SmartPower3 USBC port), default /dev/ttyUSB0
             --smartpower3-ch2 <tty>
             --smartpower2 <tty>     Volt, Amp, Watt (HK SmartPower2 microUSB port), default /dev/ttyUSB0
        -a   <sensor>                Ambient Temperature sensor to use (e.g. --bme280) for Thermal chart
             --mcp9808 <device>      High Accuracy Temperature Sensor I2C 0x18 default /dev/i2c-0
             --sht4x <device>        Temperature and Humidity I2C 0x44 default /dev/i2c-0
             --shtc3 <device>        Temperature and Humidity I2C 0x70 default /dev/i2c-0
             --aht20 <device>        Temperature and Humidity I2C 0x70 default /dev/i2c-0
             --htu31d <device>       Temperature and Humidity I2C 0x40 default /dev/i2c-0
             --bme280 <device>@addr  Temperature, Humidity, Pressure Sensor I2C 0x76 or 0x77 default /dev/i2c-0
             --bme680 <device>       Temperature, Humidity, Pressure & VOC Sensor I2C 0x76 or 0x77 default /dev/i2c-0
             --bmp180 <device>       Barometric Pressure, Altitude & Temperature Sensor default /dev/i2c-0
             --bmp388 <device>       Barometric Pressure, Altitude & Temperature Sensor I2C 0x76 or 0x77 default /dev/i2c-0
             --bmp390 <device>       Barometric Pressure, Altitude & Temperature Sensor I2C 0x76 or 0x77 default /dev/i2c-0
             --sgp30 <device>        VOC and eCO2 Sensor I2C 0x58 default /dev/i2c-0
             --scd30 <device>        CO2 Temperature and Humidity Sensor I2C 0x61 default /dev/i2c-0
             --scd41 <device>        CO2 Temperature and Humidity Sensor I2C 0x62 default /dev/i2c-0
        -r,  --raw                   Raw output, no formatting of freq. or temp.  e.g. 35000 instead of 35
        -q,  --quiet                 No output to stdout
        -v,  --verbose               Readable dashboard output
        -n,  --udp <host>:<port>     UDP output to <host>:<port>
        -g,  --gnuplot <file>        Gnuplot script generation
             --title <string>        Chart title <string>
             --xmtics <number>       Chart x-axis major second tics <number>
        -o,                          Display output on eInk/Oled/LCD using configuration logenv.json
             --version               Version
        -h,  --help                  Help screen
```


## Example
This example is a typical data collection and charting workflow:

Every 2 seconds collect the CPU frequency, thermal zone temperatures, CPU usage, SmartPower3 Volt, Amp and Watts, and mcp9808 as ambient temperature. 
```
./logenv -l ocl-m2_g610-a76_1.csv -i 2000 -f -t --mcp9808 -u -p
```

Generate the GNUplot script
```
./logenv -g ocl-m2_g610-a76_1.gpl --title "OpenCL-Benchmark Odroid-M2 GPU-CPU" --xmtics 60 -i -f -t -a --mcp9808 -u -p
```

Generate the chart
```
gnuplot -c ocl-m2_g610-a76_1.gpl ocl-m2_g610-a76_1.png ocl-m2_g610-a76_1.csv
```


## SmartPower Settings
Settings for the Hard Kernel SmartPower3 and SmartPower2 are baud rate 115200,8N1 with no HW or SW control.

## Builtin Sensor Support
Many types of sensors are directly supported using built in drivers and require no other setup.  At this time only I2C access is available for those that also have a SPI interface.  The I2C device and or the address can follow any sensor argument while using the command line interface, otherwise the default values are used.  The I2C address follows standard C language notation for decimal, hex or octal e.g. 119, 0x77, 0167 
```
logenv --bme680
       --bme680 /dev/i2c-0
       --bme680 /dev/i2c-0@119.
       --bme680 /dev/i2c-0@0x77.
       --bme680 /dev/i2c-0@0167.
```

```
aht20 - Temperature & Humidity I2C ADD 0x38
htu31 - Temperature & Humidity I2C ADD 0x40
sht40,sht41,sht43,sht45 - Temperature & Humidity I2C ADD 0x44
shtC3 - Temperature & Humidity I2C ADD 0x70
mcp9808 - High Accuracy Temperature Sensor I2C ADD 0x18

bme280 - Pressure, Altitude, Temperature & Relative Humidity, ADD 0x76 or 0x77
bme680 - Pressure, Altitude, Temperature, Relative Humidity & VOC ADD 0x76 or 0x77

bmp180 - Barometric Pressure, Altitude and Temperature
bmp388 - Barometric Pressure, Altimeter and Temperature I2C ADD 0x76 or 0x77
bmp390 - Barometric Pressure, Altimeter and Temperature I2C ADD 0x76 or 0x77

scd30 - NDIR True CO2 (400ppm – 10,000ppm), Temperature and Humidity Sensor ADD 0x61
scd40,scd41,scd43 - True CO2, Temperature and Humidity Sensor ADD 0x62

sgp30 - VOC and eCO2 I2C ADD 0x58
```

## Kernel Industrial IO Sensor Support
The kernel industrial IO(iio) sensors can be read by using the iio command instead of the sensor command.  The iio sensor name, as presented in the device tree and in */sys/bus/iio/devices/iio:device0/name*, is used to identify the correct sensor entry since the iio device number is not persistent across system boots. The JSON device entry is used to identify the file system file name to read for the sensor datum e.g. */sys/bus/iio/devices/iio:device0/in_illuminance_input*
```
    {
    "cmd": "iio",
    "name": "tsl2591",
    "device": "in_illuminance_input",
    "address": 0,
    "dtype": "",
    "xloc": 10,
    "yloc": 40,
    "color": 0,
    "font": "MONOSPACE_16",
    "label": "illum: ",
    "unit": ""
    },
```

## GNUPlot Charts
Single or stacked charts are created based on the type and number of datum that are contained in the data set.  Core Frequency, Thermal Zone Temperatures, Ambient Temperature, CPU core usage, memory usage, SmartPower and sensor data can all be charted.  When a GNUPlot Script file is generated, part of it's contents is based on the number of CPU cores and the number and name of thermal Zones.  For this reason the GNUPlot script needs to be generated on the machine the data was collected from if any of those datum are included.  The GNUPlot Scripts or .gpl files can be reused and don't need to be regenerated if the type of data being collected and the machine are the same.

## UDP Client
The UDP client -n option is followed by the server host and port <host:port>. The host entry can be either an IP address or host name.  Make sure the port is open if using a firewall.  From the command-line on the server host system, netcat can be used to read the UDP feed.

```
netcat -l -u -p <port>
```

## Timing accuracy
The timing accuracy for both the logging and display functions are dependant on the number and type of data being collected.  Some sensors can inject significant latency due to the time it takes for readings.  This is especially true for those sensors that require heaters, e.g. MOX sensors, or other pre-read initialization process.  If the logging, UDP or display functions are simultaneously being used, they share the same data read for each interval which reduces the number of sensor and data reads, and thereby reducing latency.
 
## Display Configuration
The display configuration file logenv.json is a JSON formatted file that describes a display, its pages and content for each page.  The current working directory is searched first (./logenv.json) and then /etc/logenv/logenv.json.
Multiple displays are supported with each capable of multiple pages containing time, date, system information or sensor data.  Each display must contain at least one *page* - page 0. Subsequent page entries are identified by increasing the page number and setting the *seconds* for that page to be displayed.

### JSON Format

```
{
"displays": [
    {
        "name": "ssd1306",
        "device": "/dev/i2c-1",
        "address": 61,
        "xsize": 128,
        "ysize": 64,
        "rotation": 0,
        "page": 0,
        "seconds": 60,
        "contrast": 125,
        "segment_column_address": 1,
        "scan_direction_start": 1,
        "left_right_remap": 0,
        "pin_config_alt": 1,
        "content": [
            {
            "cmd": "time",
            "name": "",
            "device": "",
            "address": 0,
            "dtype": "",
            "xloc": 15,
            "yloc": 5,
            "color": 0,
            "font": "DEFAULT_24",
            "label": "",
            "unit": ""
            },
            {
            "cmd": "date",
            "name": "",
            "device": "",
            "address": 0,
            "dtype": "",
            "xloc": 35,
            "yloc": 30,
            "color": 0,
            "font": "DEFAULT_12",
            "label": "",
            "unit": ""
            },
            {
            "cmd": "iio",
            "name": "tsl2591",
            "device": "in_illuminance_input",
            "address": 0,
            "dtype": "",
            "xloc": 5,
            "yloc": 50,
            "color": 0,
            "font": "DEFAULT_12",
            "label": "illum:",
            "unit": ""
            },
            {
            "cmd": "sensor",
            "name": "mcp9808",
            "device": "/dev/i2c-0",
            "address": 24,
            "dtype": "F",
            "xloc": 70,
            "yloc": 50,
            "color": 0,
            "font": "MONOSPACE_12",
            "label": "",
            "unit": "c"
            }
         ]
      }
   ]
}


```

Most entries in the json file are self explanatory with the possible exception of the initialization variables *segment_column_address*, *scan_direction_start*, *left_right_remap* and *pin_config_alt*.  They are set to a 0 or 1 value to enable or disable the configuration command.  The first two control the direction and starting column address.  If the oled is displaying upside down, change the *segment_column_address* and *scan_direction_start* values to the opposite.  If they are 0 change them to 1 and retest.
 
### Page Content Commands
- date
- disk
- frequency
- governor
- hostname
- iio
- ip
- kernel
- memory
- sensor
- swap
- sysload
- text
- thermal
- time
- uptime
- usage

***date***
```
    COMMAND: date
DESCRIPTION: displays date at xloc, yloc using font.

      DTYPE: dd
             mm
             yy
             mm/dd/yyyy
             dd/mm/yyyy
             yyyy/mm/dd
             short      Fri 01-May-26
             long       Friday 01 May 2026
             day        Friday
             month      May
             year       2026

    EXAMPLE:
             {
             "cmd": "date",
             "name": "",
             "device": "",
             "address": 0,
             "dtype": "long",
             "xloc": 10,
             "yloc": 40,
             "color": 0,
             "font": "DEFAULT_16",
             "label": "",
             "unit": ""
             },
```

***disk***
```
    COMMAND: disk
DESCRIPTION: displays mounted disk info at xloc, yloc using font.

     DEVICE: path
      DTYPE: free
             used
             percent

    EXAMPLE:
            {
            "cmd": "disk",
            "name": "",
            "device": "/",
            "address": 0,
            "dtype": "free",
            "xloc": 10,
            "yloc": 40,
            "color": 0,
            "font": "MONOSPACE_16",
            "label": "/ ",
            "unit": "G free"
            },
```

***frequency***
```
    COMMAND: frequency
DESCRIPTION: displays core name and frequencies at xloc, yloc using font.

     DEVICE: empty for all cores or core number
      DTYPE: D = datum
             N = name
            DN = datum name
            ND = name datum

    EXAMPLE:
            {
            "cmd": "frequency",
            "name": "",
            "device": "",
            "address": 0,
            "dtype": "D",
            "xloc": 10,
            "yloc": 40,
            "color": 0,
            "font": "MONOSPACE_16",
            "label": "",
            "unit": ""
            },
```

***governor***
```
    COMMAND: governor
DESCRIPTION: displays governor from device at xloc, yloc using font.

     DEVICE: path to sysfs entry

    EXAMPLE:
            {
            "cmd": "governor",
            "name": "",
            "device": "/sys/devices/system/cpu/cpufreq/policy0/scaling_governor",
            "address": 0,
            "dtype": "",
            "xloc": 10,
            "yloc": 40,
            "color": 0,
            "font": "MONOSPACE_16",
            "label": "CPU: ",
            "unit": ""
            },
```

***hostname***
```
    COMMAND: hostname
DESCRIPTION: displays hostname at xloc, yloc using font.

    EXAMPLE:
            {
            "cmd": "hostname",
            "name": "",
            "device": "",
            "address": 0,
            "dtype": "",
            "xloc": 10,
            "yloc": 40,
            "color": 0,
            "font": "MONOSPACE_16",
            "label": "",
            "unit": ""
            },
```

***iio***
```
    COMMAND: iio
DESCRIPTION: displays kernel iio sensor at xloc, yloc using font.

       NAME: name of sensor as defined in /sys/bus/iio/devices/iio:device0/name
     DEVICE: sensor datum entry to display in /sys/bus/iio/devices/iio:device0/
      DTYPE: divide / or multiply * sensor datum by a single scalar as a post process.

    EXAMPLE:
            {
            "cmd": "iio",
            "name": "tsl2591",
            "device": "in_illuminance_input",
            "address": 0,
            "dtype": "/10",
            "xloc": 10,
            "yloc": 40,
            "color": 0,
            "font": "MONOSPACE_16",
            "label": "illum: ",
            "unit": ""
            },
```

***ip***
```
    COMMAND: ip
DESCRIPTION: displays named network interface ip at xloc, yloc using font.

       NAME: interface name
      DTYPE: D = datum
             N = name
            DN = datum name
            ND = name datum

    EXAMPLE:
            {
            "cmd": "ip",
            "name": "eth0",
            "device": "0",
            "address": 0,
            "dtype": "ND",
            "xloc": 10,
            "yloc": 40,
            "color": 0,
            "font": "MONOSPACE_16",
            "label": "",
            "unit": ""
            },
```

***kernel***
```
    COMMAND: kernel
DESCRIPTION: displays kernel version at xloc, yloc using font.

    EXAMPLE:
            {
            "cmd": "kernel",
            "name": "",
            "device": "",
            "address": 0,
            "dtype": "",
            "xloc": 10,
            "yloc": 40,
            "color": 0,
            "font": "DEFAULT_12",
            "label": "kernel ",
            "unit": ""
            },
```

***memory***
```
    COMMAND: memory
DESCRIPTION: displays memory usage at xloc, yloc using font.

    EXAMPLE:
            {
            "cmd": "memory",
            "name": "",
            "device": "",
            "address": 0,
            "dtype": "",
            "xloc": 10,
            "yloc": 40,
            "color": 0,
            "font": "MONOSPACE_16",
            "label": "",
            "unit": ""
            },
```

***sensor***
```
    COMMAND: sensor
DESCRIPTION: displays sensor datum at xloc, yloc using font.

       NAME: aht20,htu31,sht40,sht41,sht43,sht45,shtc3,mcp9808
             bme280,bme680,bmp180,bmp388,bmp390,
             scd30,sdc40,scd41,scd43,sgp30
      DTYPE: C = Celsius
             F = Fahrenheit
             H = Humidity
             P = Pressure
             G = Gas
             V = VOC

    EXAMPLE:
            {
            "cmd": "sensor",
            "name": "bme680",
            "device": "/dev/i2c-1",
            "address": 119,
            "dtype": "C",
            "xloc": 5,
            "yloc": 50,
            "color": 0,
            "font": "DEFAULT_12",
            "label": "",
            "unit": "c"
            },
```
The sensor address is 7-bits I2C address in decimal e.g. 119 = 0x77

***swap***
```
    COMMAND: swap
DESCRIPTION: displays swap info at xloc, yloc using font.

     DEVICE: path
      DTYPE: total
             free
             used
             percent

    EXAMPLE:
            {
            "cmd": "swap",
            "name": "",
            "device": "",
            "address": 0,
            "dtype": "percent",
            "xloc": 10,
            "yloc": 40,
            "color": 0,
            "font": "MONOSPACE_16",
            "label": "swap ",
            "unit": "%"
            },
```

***sysload***
```
    COMMAND: sysload
DESCRIPTION: displays sysload info at xloc, yloc using font.

      DTYPE: short [%d] [%d] [%d]
             long  1min(%d) 5min(%d) 15min(%d)

    EXAMPLE:
            {
            "cmd": "sysload",
            "name": "",
            "device": "",
            "address": 0,
            "dtype": "short",
            "xloc": 10,
            "yloc": 40,
            "color": 0,
            "font": "MONOSPACE_16",
            "label": "",
            "unit": ""
            },
```

***text***
```
    COMMAND: text
DESCRIPTION: displays string at xloc, yloc using font.

     DEVICE: string of up to 255 characters

    EXAMPLE:
            {
            "cmd": "text",
            "name": "",
            "device": "This is a Test.",
            "address": 0,
            "dtype": "",
            "xloc": 10,
            "yloc": 40,
            "color": 0,
            "font": "MONOSPACE_16",
            "label": "",
            "unit": ""
            },
```

***thermal***
```
    COMMAND: thermal
DESCRIPTION: displays thermal temps at xloc, yloc using font.

     DEVICE: empty for all zones or zone number
      DTYPE: D = datum
             N = name
            DN = datum name
            ND = name datum

    EXAMPLE:
            {
            "cmd": "thermal",
            "name": "",
            "device": "0",
            "address": 0,
            "dtype": "ND",
            "xloc": 10,
            "yloc": 40,
            "color": 0,
            "font": "MONOSPACE_16",
            "label": "",
            "unit": "c"
            },
```

***time***
```
    COMMAND: time
DESCRIPTION: displays time at xloc, yloc using font.

      DTYPE: empty 4:00 PM
             12   04:00
             24   16:00

    EXAMPLE:
            {
            "cmd": "time",
            "name": "",
            "device": "",
            "address": 0,
            "dtype": "24",
            "xloc": 10,
            "yloc": 40,
            "color": 0,
            "font": "MONOSPACE_36",
            "label": "",
            "unit": ""
            },
```


***uptime***
```
    COMMAND: uptime
DESCRIPTION: displays uptime at xloc, yloc using font.

      DTYPE: short %d days HH:MM
             long  %ddays, %dhours, %dminutes

    EXAMPLE:
            {
            "cmd": "uptime",
            "name": "",
            "device": "",
            "address": 0,
            "dtype": "short",
            "xloc": 10,
            "yloc": 40,
            "color": 0,
            "font": "MONOSPACE_16",
            "label": "",
            "unit": ""
            },
```

***usage***
```
    COMMAND: usage
DESCRIPTION: displays CPU and core usage at xloc, yloc using font.

     DEVICE: empty for all cores or core number, 0 is total usage
      DTYPE: D = datum
             N = name
            DN = datum name
            ND = name datum
    EXAMPLE:
            {
            "cmd": "usage",
            "name": "",
            "device": "",
            "address": 0,
            "dtype": "ND",
            "xloc": 10,
            "yloc": 40,
            "color": 0,
            "font": "MONOSPACE_16",
            "label": "",
            "unit": ""
            },
```


### Display Font Names
```
  DEFAULT_12, DEFAULT_16, DEFAULT_24
  
  MONOSPACE_12, MONOSPACE_16, MONOSPACE_20, MONOSPACE_24, MONOSPACE_28,
  MONOSPACE_36, MONOSPACE_48

```


### Tested Displays
Basic display capability has not been fully implemented so only limited display tests have been performed.  More testing will occur as development proceeds.

 ssd1681 AdaFruit 4196 1.54" Monochrome 200x200 eInk / ePaper Display
 
 ssd1306 Odroid-HC4 Monochrome 128x64 OLED graphic display
 
 ssd1306 AdaFruit 938 Monochrome 1.3" 128x64 OLED graphic display
 
 sh1107 AdaFruit 5297 Monochrome 1.12" 128x128 OLED graphic display
 
 st7789 AdaFruit 3787 Color 1.54" 240x240 Wide Angle TFT LCD display
 
 
## Compatibility
logenv has been tested with both a HardKernel SmartPower2 and SmartPower3, including MCP9808 and BME280 sensors.  Several architectures have been test including Armv7, Armv8, Armv9 and different generations of INTEL and AMD processor up to 32 cores.  The current limitation is 256 cores.  It has also been used on several Linux distributions including Ubuntu, Debian, Manjaro and Arch.  It should run on just about anything that runs Linux.
