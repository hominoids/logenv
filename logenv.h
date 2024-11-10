/*
    logenv Copyright 2019,2020,2024 Edward A. Kisiel
    hominoid @ cablemi . com

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

*/

void usage(void);
int itoa(int, char[]);
int set_interface_attribs(int, int);

FILE *cpu_online, *cpu_freq, *cpu_thermal, *thermal_type, *cpu_use, *log_file, *gnuplot_file;

time_t now;
struct tm *t;

int  pwr_in;
int  sensor_in;

char *smartpower = "/dev/ttyUSB0";
char *sensor = "/dev/i2c-0";
char *cpuonline = "/sys/devices/system/cpu/online";
char *cpuusage = "/proc/stat";

static double long use[10][256] = {0};

char cpufreq[255];
char *cpufreq1 = "/sys/devices/system/cpu/cpu";
char *cpufreq2 = "/cpufreq/scaling_cur_freq";

char thermalzone[255];
char *thermalzone1 = "/sys/devices/virtual/thermal/thermal_zone";
char *thermalzone2 = "/temp";

char thermaltype[255];
char *thermaltype1 = "/type";
char thermalname[255];

char spfile[255];
char spline[25];
char logfile[255];
char gplotfile[255];
char version[] = "0.98 pre release";
char one2one[] = "1,1";
char two2one[] = "2,1";
char three2one[] = "3,1";
char four2one[] = "4,1";

static int SP_ENABLE = 0;
static int SENSOR_ENABLE = 0;
static int FREQ_ENABLE = 0;
static int THERMAL_ENABLE = 0;
static int QUIET_ENABLE = 0;
static int VERBOSE_ENABLE = 0;
static int INTERACTIVE_ENABLE = 0;
static int LOG_ENABLE = 0;
static int RAW_ENABLE = 0;
static int GNUPLOT_ENABLE = 0;
static int COUNT_ENABLE = 1;
static int DT_ENABLE = 0;
static int USAGE_ENABLE = 0;

static int xmtics = 10;
static int temperature;
static int humidity;
static int pressure;
static int freq;
static float coretemp;
static float volt;
static float amp;
static float watt;

static double long sp_ms;
static unsigned int in_mv;
static unsigned int in_ma;
static unsigned int in_w;
static unsigned int in_on;
static unsigned int ch1_mv;
static unsigned int ch1_ma;
static unsigned int ch1_w;
static unsigned int ch1_on;
static unsigned int ch1_int;
static unsigned int ch2_mv;
static unsigned int ch2_ma;
static unsigned int ch2_w;
static unsigned int ch2_on;
static unsigned int ch2_int;
static unsigned int chk_comp;
static unsigned int chk_xor;
static char gpscript_freq1[30];
static char gpscript_freq2[30];
static char gpscript_thermal1[30];
static char gpscript_thermal2[30];
static char gpscript_power1[30];
static char gpscript_power2[30];
static char gpscript_usage1[30];
static char gpscript_usage2[30];
static char charttitle[255] = "Main Title";

static char gpscript_start[11][75] = {
    "#!/usr/bin/gnuplot --persist\n",
    "#\n",
    "#\n",
    "# GNUPLOT script generated from logenv that plots temperature, frequency,\n",
    "# ambient temperature, power and CPU core usage from a data file\n",
    "# formated as \"count,freq1-N,thermal1-N,sensor,volts,amps,watts,usage1-N\"\n",
    "#\n\n",

    "set term pngcairo size 1280,1024 enhanced font \'Verdana,10\'\n",
    "set output ARG1\n",
    "set datafile separator \",\"\n\n",

    "# title and line style definitions\n\n" };


static char gpscript_thermal_title[9][2][32] = {
    {"data_title1 = \"", "Thermal Zone 0"},
    {"data_title2 = \"", "Thermal Zone 1"},
    {"data_title3 = \"", "Thermal Zone 2"},
    {"data_title4 = \"", "Thermal Zone 3"},
    {"data_title5 = \"", "Thermal Zone 4"},
    {"data_title6 = \"", "Thermal Zone 5"},
    {"data_title7 = \"", "Thermal Zone 6"},
    {"data_title8 = \"", "Thermal Zone 7"},
    {"data_title9 = \"", "Ambient Temp"} };

static char gpscript_mid[18][45] = {
    "\n# line styles\n",
    "set style line 1 lc rgb \"dark-violet\" lw 1\n",
    "set style line 2 lc rgb \"orange\" lw 1\n",
    "set style line 3 lc rgb \"blue\" lw 1\n",
    "set style line 4 lc rgb \"forest-green\" lw 1\n",
    "set style line 5 lc rgb \"red\" lw 1\n",
    "set style line 6 lc rgb \"dark-cyan\" lw 1\n",
    "set style line 7 lc rgb \"dark-magenta\" lw 1\n",
    "set style line 8 lc rgb \"royalblue\" lw 1\n",
    "set style line 9 lc rgb \"black\" lt 1 lw 1\n",
    "set style line 10 lc rgb \"black\" lt 0 lw 1\n\n",

    "# graph style\n",
    "set border 3 front ls 9\n",
    "set tics nomirror\n",
    "set grid back ls 10\n",
    "set grid ytics ls 10\n\n",

    "# legend top right corner\n",
    "set key default noauto font \'Verdana,10\'\n\n"};

static char gpscript_layout[3][65] = {
    "set multiplot layout ",
    " title \"",
    "\" font \'Verdana,16\'\n\n"};

static char gpscript_xaxis [5][30] = {
    "# seconds x axis\n",
    "set xlabel \'Time (seconds)\'\n",
    "set xtics 0,",
    " border nomirror out rotate\n",
    "set mxtics\n\n"};

static char gpscript_thermal[9][55] = {
    "# temperature plot\n",
    "set size 1,.5\n",
    "set origin 0,.5\n",
    "set lmargin 11\n",
    "# temperature y axis\n",
    "set ylabel \'Temperature (c)\' font \'Verdana,12\'\n",
    "set yrange [0:100]\n",
    "set ytics 0,5 border nomirror out\n",
    "set mytics\n\n"};
    
static char gpscript_freq[9][55] = {
    "# frequency plot\n",
    "set size 1,.2\n",
    "set origin 0,.3\n",
    "set lmargin 11\n",
    "# frequency y axis\n",
    "set ylabel \'Frequency (GHz)\' font \'Verdana,12\'\n",
    "set yrange [0:5]\n",
    "set ytics 0,.5 border nomirror out\n",
    "#set format y \'%.1f\'\n\n"};

static char gpscript_power[11][55] = {
    "# power plot\n",
    "set size 1,.3\n",
    "set origin 0,0\n",
    "set lmargin 11\n",
    "# power y axis\n",
    "set ylabel \'Amps, Volts, Watts\' font \'Verdana,12\'\n",
    "set yrange [0:20]\n",
    "set ytics 0,1 border nomirror out\n",
    "set format y \'%.0f\'\n",
    "# power x axis\n",
    "set noxlabel\n\n"};

static char gpscript_usage[11][55] = {
    "# proc stat plot\n",
    "set size 1,.2\n",
    "set origin 0,0\n",
    "set lmargin 11\n",
    "# power y axis\n",
    "set ylabel \'% Core Use\' font \'Verdana,12\'\n",
    "set yrange [0:101]\n",
    "set ytics 0,10 border nomirror out\n",
    "set format y \'%.0f\'\n",
    "# usage x axis\n",
    "set noxlabel\n\n"};

static char gpscript_end[18] = {"unset multiplot\n" };
