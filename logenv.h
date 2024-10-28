/*
    logenv Copyright 2019,2020 Edward A. Kisiel
    hominoid @ www.forum.odroid.com

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
    Code released under GPLv3: http://www.gnu.org/licenses/gpl.html

    logenv.h
       
    20200202 Version 1.0.0    
*/

void usage(void);
int itoa(int, char[]);

FILE *pwr_in, *cpu_online, *cpu_freq, *cpu_thermal, *log_file, *gnuplot_file;

time_t now;
struct tm *t;

char spfile[255];
char *smartpower = "/dev/ttyUSB0";
char *weatherboard = "/dev/i2c-1";
char *cpuonline = "/sys/devices/system/cpu/online";

char cpufreq[255];
char *cpufreq1 = "/sys/devices/system/cpu/cpu";
char *cpufreq2 = "/cpufreq/scaling_cur_freq";

char thermalzone[255];
char *thermalzone1 = "/sys/devices/virtual/thermal/thermal_zone";
char *thermalzone2 = "/temp";

char logfile[255];
char gplotfile[255];
char spline[25];
char version[] = "0.98 pre release";

static int temperature;
static int humidity;
static int pressure;
static int freq;
static int coretemp;
static float volt;
static float amp;
static float watt;

static float sp_ms;
static float in_mv;
static float in_ma;
static float in_w;
static int  in_on;
static float ch1_mv;
static float ch1_ma;
static float ch1_w;
static int  ch1_on;
static int  ch1_int;
static float ch2_mv;
static float ch2_ma;
static float ch2_w;
static int ch2_on;
static int ch2_int;
static int chk_comp;
static int chk_xor;

static int SP_ENABLE = 0;
static int WB_ENABLE = 0;
static int CPU_ENABLE = 0;
static int THERMAL_ENABLE = 0;
static int QUIET_ENABLE = 0;
static int VERBOSE_ENABLE = 0;
static int INTERACTIVE_ENABLE = 0;
static int LOG_ENABLE = 0;
static int RAW_ENABLE = 0;
static int GNUPLOT_ENABLE = 0;
static int COUNT_ENABLE = 1;
static int DT_ENABLE = 0;

static char gpscript_start [53][80] = { 
    "#!/usr/bin/gnuplot --persist\n",
    "#\n",   
    "#\n",
    "# GNUPLOT script generated from logenv that plots temperature, frequency,\n",
    "# ambient temperature and power for multiple cpu cores from a data file\n",
    "# formated as \"count,freq1,freq2,freq3,freq4,freq5,freq6,freq7,freq8,\n",
    "#               core1,core2,core3,core4,gpu,airtemp,volts,amps,watts\"\n",
    "#\n",
    "# Author: hominoid @ www.forum.odroid.com\n",
    "# 05/03/2017 version 1.0 Public domain with no warranty of any kind\n",
    "# 02/02/2020 version 1.1 Added ambient temp, volts, amps, watts\n",
    "#\n",
    "# Version 5.x\n",
    "# Use: gnuplot -c logenv.gpl chart.png datafile.csv\n",
    "#\n",
    "# Version 4.x\n",
    "# Use: gnuplot logenv.gpl\n",
    "#\n",
    "# version 4.x only, uncomment file names instead of on the commandline\n\n",

    "#ARG1 = \"chart.png\"\n",
    "#ARG2 = \"datafile.csv\"\n\n",

    "set term pngcairo size 1024,1280 enhanced font \'Verdana,10\'\n",
    "set output ARG1\n",
    "set datafile separator \",\"\n\n",

    "# title and line style definitions\n\n",

    "data_title1 = \"Thermal Zone 0\"\n",
    "set style line 1 lc rgb \"dark-violet\" lw 1\n\n",

    "data_title2 = \"Thermal Zone 1\"\n",
    "set style line 2 lc rgb \"orange\" lw 1\n\n",

    "data_title3 = \"Thermal Zone 2\"\n",
    "set style line 3 lc rgb \"blue\" lw 1\n\n",

    "data_title4 = \"Thermal Zone 3\"\n",
    "set style line 4 lc rgb \"forest-green\" lw 1\n\n",

    "data_title5 = \"Thermal Zone 4\"\n",
    "set style line 5 lc rgb \"red\" lw 1\n\n",

    "data_title6 = \"Thermal Zone 5\"\n",
    "set style line 6 lc rgb \"cyan\" lw 1\n\n",

    "data_title7 = \"Thermal Zone 6\"\n",
    "set style line 7 lc rgb \"khaki\" lw 1\n\n",

    "data_title8 = \"Thermal Zone 7\"\n",
    "set style line 8 lc rgb \"light-blue\" lw 1\n\n",

    "data_title9 = \"Ambient Temp\"\n",
    "set style line 8 lc rgb \"black\" lw 1\n\n",
    
    "# border and grid line styles\n",
    "set style line 9 lc rgb \"black\" lt 1 lw 1\n",
    "set style line 10 lc rgb \"black\" lt 0 lw 1\n\n",

    "# graph style\n",
    "set border 3 front ls 9\n",
    "set tics nomirror\n",
    "set grid back ls 10\n",
    "set grid ytics ls 10\n\n",

    "# legend top right corner\n",
    "set key default noauto font \'Verdana,10\'\n\n"};

static char gpscript_layout [65] = {
    "set multiplot layout 3,1 title \"Main Title\" font \'Verdana,16\'\n\n"};

static char gpscript_thermal [13][60] = {
    "# temperature plot\n",
    "set size ratio 0 1,.5\n",
    "set origin 0,.5\n",
    "set lmargin 11\n",
    "# temperature y axis\n",
    "set ylabel \'Temperature (c)\' font \'Verdana,12\'\n",
    "set yrange [0:100]\n",
    "set ytics 0,2 border nomirror out\n",
    "set mytics\n",
    "# temperature x axis\n",
    "set xlabel \'Time (seconds)\'\n",
    "set xtics 0,10 border nomirror out rotate\n",
    "set mxtics\n\n"};
    
static char gpscript_freq [11][55] = {
    "# frequency plot\n",
    "set size ratio 0 1,.2\n",
    "set origin 0,.3\n",
    "set lmargin 11\n",
    "# frequency y axis\n",
    "set ylabel \'Frequency (GHz)\' font \'Verdana,12\'\n",
    "set yrange [0:4]\n",
    "set ytics 0,.5 border nomirror out\n",
    "#set format y \'%.1f\'\n",
    "# frequency x axis\n",
    "set xlabel \'Time (seconds)\'\n\n"};

static char gpscript_power [11][55] = {
    "# power plot\n",
    "set size ratio 0 1,.3\n",
    "set origin 0,0\n",
    "set lmargin 11\n",
    "# power y axis\n",
    "set ylabel \'Amps, Volts, Watts\' font \'Verdana,12\'\n",
    "set yrange [0:20]\n",
    "set ytics 0,1 border nomirror out\n",
    "set format y \'%.0f\'\n",
    "# power x axis\n",
    "set noxlabel\n\n"};

static char gpscript_end [18] = {"unset multiplot\n" };
