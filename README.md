# logenv


## Introduction
logenv is a commandline utility for the logging of time, cpu frequency, thermal zones, ambient temperature, volts, amps and watts


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
        -l,  --log <file>            Log to <file>
        -s,  --seconds <number>      Poll every <number> seconds
        -f,  --frequency             CPU core frequency
        -t,  --temperature           Thermal zone temperature
        -b,  --bme280 <device>       BME280 Temperature (HK Weatherboard 2), default /dev/i2c-1
             --bmp180 <device>       BMP180 Temperature (HK Weatherboard 1), default /dev/i2c-1
        -p,  --power <tty>           Volt,Amp,Watt (HK SmartPower2 microUSB port), default /dev/ttyUSB0
        -d,  --date                  Date and Time stamp
        -r,  --raw                   Raw output, no formatting of freq. or temp.  e.g. 35000 instead of 35
        -v,  --verbose               Readable output
        -q,  --quiet                 No output to stdout
        -g,  --gnuplot <file>        Gnuplot script generation
             --version               Version
        -h,  --help                  Help screen
```

### Examples
```
Data capture every 2 seconds:
logenv -l logfile.csv -s 2 -f -t -b /dev/i2c-1 -p /dev/ttyUSB0

Gnuplot script generation for data capture:
logenv -g gplotscript.gpl -s 2 -f -t -b -p 

Gnuplot chart creation:
gnuplot -c gplotscript.gpl chart.png logfile.csv
```
