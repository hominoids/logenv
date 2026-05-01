/*
    logenv Copyright 2019,2020,2024,2025,2026 Edward A. Kisiel
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

#ifndef LOGENV_H
#define LOGENV_H

#ifdef __cplusplus
extern "C"{
    #endif

void usage(void);
int16_t itoa(int32_t, char[]);
int16_t set_tty_attributes(int16_t, int32_t, bool);
void sleep_ms(int32_t);
static void sig_handler(int);

extern uint8_t ssd1681(struct display *, uint8_t, uint8_t);
extern uint8_t ssd1306(struct display *, uint8_t, uint8_t);
extern uint8_t ssh1107(struct display *, uint8_t, uint8_t);
extern uint8_t st7789(struct display *, uint8_t, uint8_t);

static volatile sig_atomic_t go = 1;

FILE *cpu_online, *cpu_freq, *cpu_thermal, *thermal_type, *cpu_use, *mem_load, \
     *governor_file, *log_file, *gnuplot_file, *json_file;

bool VERBOSE_DEBUG = 0;

uint8_t (*dptr)(struct display *, uint8_t, uint8_t) = NULL;
uint8_t display_count = 0;
uint8_t page = 0;
uint8_t pg_count = 0;

time_t now;
struct tm *t;
char display_time[10];
char display_date[36];

int16_t udp_socket;
int16_t sin_size;
struct sockaddr_in udp_server_addr;
struct hostent *udp_host;
static int16_t udp_port = 5000;
static char udp_name[256] = "127.0.0.1";
static char udp_tx_data[1024] = {0};

char ssd1681_spi_dev[18] = "/dev/spidev0.0";
char st7789_spi_dev[14] = "/dev/spidev0.0";

uint16_t ssd1306_iic_addr = 0x3d << 1;
char ssd1306_iic_dev[14] = "/dev/i2c-0";
char ssd1306_spi_dev[14] = "/dev/spidev0.0";
uint8_t ssd1306_iic_init = 0;

uint16_t ssh1107_iic_addr = 0x3d << 1;
char ssh1107_iic_dev[14] = "/dev/i2c-0";
char ssh1107_spi_dev[14] = "/dev/spidev0.0";
uint8_t ssh1107_iic_init = 0;

int16_t pwr_in;
uint16_t mcp9808_in;

uint16_t bme280_iic_addr = 0x77 << 1;
char bme280_iic_dev[14] = "/dev/i2c-0";
uint8_t bme280_iic_init = 0;
char bme280_spi_dev[18] = "/dev/spidev0.0";

uint16_t bme680_iic_addr = 0x77 << 1;
char bme680_iic_dev[14] = "/dev/i2c-1";
char bme680_spi_dev[18] = "/dev/spidev0.0";
uint8_t bme680_iic_init = 0;

uint16_t bmp180_iic_addr = 0xEE;
char bmp180_iic_dev[14] = "/dev/i2c-0";
uint8_t bmp180_iic_init = 0;

uint16_t bmp388_iic_addr = 0x77 << 1;
char bmp388_iic_dev[14] = "/dev/i2c-0";
char bmp388_spi_dev[18] = "/dev/spidev0.0";
uint8_t bmp388_iic_init = 0;

uint16_t bmp390_iic_addr = 0x77 << 1;
char bmp390_iic_dev[14] = "/dev/i2c-0";
char bmp390_spi_dev[18] = "/dev/spidev0.0";
uint8_t bmp390_iic_init = 0;

uint16_t mcp9808_iic_addr = 0x18;
char mcp9808_iic_dev[14] = "/dev/i2c-0";
uint8_t mcp9808_iic_init = 0;

uint16_t scd30_iic_addr = 0x61 << 1;
char scd30_iic_dev[14] = "/dev/i2c-0";
uint8_t scd30_iic_init = 0;

uint16_t scd4x_iic_addr = 0x62 << 1;
char scd4x_iic_dev[14] = "/dev/i2c-0";
uint8_t scd4x_iic_init = 0;

uint16_t sgp30_iic_addr = 0x58 << 1;
char sgp30_iic_dev[14] = "/dev/i2c-0";
uint8_t sgp30_iic_init = 0;

uint16_t sht4x_iic_addr = 0x44 << 1;
char sht4x_iic_dev[14] = "/dev/i2c-0";
uint8_t sht4x_iic_init = 0;

uint16_t shtc3_iic_addr = 0x70 << 1;
char shtc3_iic_dev[14] = "/dev/i2c-0";
uint8_t shtc3_iic_init = 0;

uint16_t aht20_iic_addr = 0x70 << 1;
char aht20_iic_dev[14] = "/dev/i2c-0";
uint8_t aht20_iic_init = 0;

uint16_t htu31d_iic_addr = 0x40 << 1;
char htu31d_iic_dev[14] = "/dev/i2c-0";
uint8_t htu31d_iic_init = 0;

char *smartpower = "/dev/ttyUSB0";
char *sensor = "/dev/i2c-0";
char *interface = "/dev/i2c-0";
char *cpuonline = "/sys/devices/system/cpu/online";
char *cpuusage = "/proc/stat";

static double long use[10][256] = {0};

char cpufreq[255];
char *cpufreq1 = "/sys/devices/system/cpu/cpu";
char *cpufreq2 = "/cpufreq/scaling_cur_freq";

char *memload = "/proc/meminfo";
char *mem_total = "MemTotal:";
char *mem_free = "MemFree:";
char *mem_avail = "MemAvailable:";
char *mem_buffers = "Buffers:";
char *mem_cached = "Cached:";
char *mem_srec = "SReclaimable:";

char thermalzone[255];
char *thermalzone1 = "/sys/devices/virtual/thermal/thermal_zone";
char *thermalzone2 = "/temp";

char thermaltype[255];
char *thermaltype1 = "/type";
char thermalname[255];

char governor[25];
char *governorloc = "/sys/devices/system/cpu/cpufreq/policy0/scaling_governor";

char spfile[255];
char spline[5];
char spline1[5];
char spline2[5];
char logfile[255];
char gplotfile[255];
char jsonfile[255];
char version[] = "1.0";
char one2one[] = "1,1";
char two2one[] = "2,1";
char three2one[] = "3,1";
char four2one[] = "4,1";

static int8_t SP_ENABLE = 0;
static int8_t SENSOR_ENABLE = 0;
static int8_t FREQ_ENABLE = 0;
static int8_t MEM_ENABLE = 0;
static int8_t THERMAL_ENABLE = 0;
static int8_t QUIET_ENABLE = 0;
static int8_t VERBOSE_ENABLE = 0;
static uint32_t INTERACTIVE_ENABLE = 0;
static int8_t LOG_ENABLE = 0;
static int8_t RAW_ENABLE = 0;
static int8_t GNUPLOT_ENABLE = 0;
static int8_t COUNT_ENABLE = 0;
static int8_t DT_ENABLE = 0;
static int8_t USAGE_ENABLE = 0;
static int8_t UDP_ENABLE = 0;
static int8_t OPTIONS_COUNT = 0;
static int8_t SGP30_ENABLE = 0;
static int8_t SCD30_ENABLE = 0;
static int8_t SCD4X_ENABLE = 0;
static int8_t BMP388_ENABLE = 0;
static int8_t BMP390_ENABLE = 0;
static int8_t BME680_ENABLE = 0;

static int8_t DISPLAY_ENABLE = 0;
static int8_t SSD1681_ENABLE = 0;
static int8_t SSD1306_ENABLE = 0;
static int8_t SSH1107_ENABLE = 0;
static int8_t ST7789_ENABLE = 0;
static int8_t DP_TIME = 0;
static int8_t DP_DATE = 0;
static int8_t DP_FREQ = 0;
static int8_t DP_THERMAL = 0;
static int8_t DP_GOVERNOR = 0;
static int8_t DP_MEMORY = 0;
static int8_t DP_USAGE = 0;
static int8_t DP_SP2 = 0;
static int8_t DP_SP3CH1 = 0;
static int8_t DP_SP3CH2 = 0;
static int8_t DP_BME280 = 0;
static int8_t DP_BME680 = 0;
static int8_t DP_BMP180 = 0;
static int8_t DP_BMP388 = 0;
static int8_t DP_BMP390 = 0;
static int8_t DP_MCP9808 = 0;
static int8_t DP_SHT4X = 0;
static int8_t DP_SHTC3 = 0;
static int8_t DP_AHT20 = 0;
static int8_t DP_HTU31D = 0;
static int8_t DP_SCD30 = 0;
static int8_t DP_SCD4X = 0;
static int8_t DP_SGP30 = 0;
static int8_t DP_TEXT = 0;
static int8_t DP_POINT = 0;
static int8_t DP_LINE = 0;
static int8_t DP_CIRCLE = 0;
static int8_t DP_RECTANGLE = 0;
static int8_t DP_IMAGE = 0;

static int16_t xmtics = 10;
static int32_t temperature;
static int32_t humidity;
static int32_t pressure;
static int32_t freq;
static float coretemp;
static float volt;
static float amp;
static float watt;

static double long sp_ms;
static uint16_t in_mv;
static uint16_t in_ma;
static uint16_t in_w;
static uint16_t in_on;
static uint16_t ch1_mv;
static uint16_t ch1_ma;
static uint16_t ch1_w;
static uint16_t ch1_on;
static uint16_t ch1_int;
static uint16_t ch2_mv;
static uint16_t ch2_ma;
static uint16_t ch2_w;
static uint16_t ch2_on;
static uint16_t ch2_int;
static uint16_t chk_comp;
static uint16_t chk_xor;

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
    "set ylabel \'% CPU / Mem Use\' font \'Verdana,12\'\n",
    "set yrange [0:101]\n",
    "set ytics 0,20 border nomirror out\n",
    "set format y \'%.0f\'\n",
    "# usage x axis\n",
    "set noxlabel\n\n"};

static char gpscript_end[18] = {"unset multiplot\n" };

#ifdef __cplusplus
}
#endif

#endif
