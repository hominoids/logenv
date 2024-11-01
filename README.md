# logenv


## Introduction
logenv is a command-line utility for the aggregating, logging and charting of timestamped CPU core frequency, thermal zone temperatures, ambient temperature, volts, amps and watts.  It can also generate GNUplot scripts for any collected data set.

License: GPLv3.

### Install
```
  git clone https://github.com/hominoids/logenv.git
  cd logenv
  make
```

  
## Usage
```
logenv [options]

logenv - Version 0.98 pre release Copyright (C) 2019,2020,2024 by Edward Kisiel
logs count or time stamp, cpu frequency, thermal zones, sensor temperature, volts, amps and watts

usage: logenv [options]

Options:
 -l,  --log <file>            Log to <file>
 -s,  --seconds <number>      Poll every <number> seconds
 -f,  --frequency             CPU core frequency
 -t,  --temperature           Thermal zone temperature
 -b,  --bme280 <device>       BME280 Temperature Sensor(HK Weatherboard 2), default /dev/i2c-1
      --bmp180 <device>       BMP180 Temperature Sensor(HK Weatherboard 1), default /dev/i2c-1
 -p,  --smartpower3-ch1 <tty> Volt,Amp,Watt (HK SmartPower3 USBC port), default /dev/ttyUSB0
      --smartpower3-ch2 <tty>
      --smartpower2 <tty>     Volt,Amp,Watt (HK SmartPower2 microUSB port), default /dev/ttyUSB0
 -d,  --date                  Date and Time stamp
 -r,  --raw                   Raw output, no formatting of freq. or temp.  e.g. 35000 instead of 35
 -v,  --verbose               Readable output
 -q,  --quiet                 No output to stdout
 -g,  --gnuplot <file>        Gnuplot script generation
      --title <string>        Chart title <string>
      --xmtics <number>       Chart x-axis major second tics <number>
      --version               Version
 -h,  --help                  Help screen
```

### Examples
```
Data capture every 2 seconds:
logenv -l logfile.csv -s 2 -f -t -b /dev/i2c-1 -p /dev/ttyUSB0

Gnuplot script generation for data capture:
logenv -g gplotscript.gpl --title "logenv GNUPlot Chart" --xmtics 60 -s 2 -f -t -b -p 

Gnuplot chart creation:
gnuplot -c gplotscript.gpl chart.png logfile.csv
```
