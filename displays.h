/*
    logenv Copyright 2019,2020,2024,2025 Edward A. Kisiel
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

    displays.h

*/
extern int display_count;
extern char display_time[];
extern char display_date[];
extern ssd1681_handle_t gs_handle;

int fontoi(char *);
int open_ssd1681(void);

struct display_content {
    char name[15];
    char device[30];
    int address;
    char type[15];
    int xloc;
    int yloc;
    int color;
    char font[30];
    char label[20];
    char unit[5];
    char data1[20];  // Temperature c
    char data2[20];  // Humidity %
    char data3[20];  // Pressure hPa
    char data4[20];  // CO2 as ppm
    char data5[20];  // TVOC as ppb
};

struct display {
    char name[15];
    char device[30];
    int address;
    int xsize;
    int ysize;
    int rotation;
    int color;
    int dc_count;
    struct display_content dc[64];
};

static int DISPLAY_ENABLE = 0;
static int DISPLAY_OPEN = 1;
static int DISPLAY_WRITE = 2;
static int DISPLAY_READ = 3;
static int DISPLAY_UPDATE = 4;
static int DISPLAY_CLOSE = 5;
static int DISPLAY_TIME = 10;
static int DISPLAY_DATE = 11;
static int DISPLAY_THERMAL = 12;
static int DISPLAY_SENSOR = 13;

static int DP_TIME = 0;
static int DP_DATE = 0;
static int DP_FREQ = 0;
static int DP_THERMAL = 0;
static int DP_MEMORY = 0;
static int DP_USAGE = 0;
static int DP_SP2 = 0;
static int DP_SP3CH1 = 0;
static int DP_SP3CH2 = 0;
static int DP_BMP180 = 0;
static int DP_BME280 = 0;
static int DP_MCP9808 = 0;
static int DP_SCD41 = 0;
static int DP_SGP30 = 0;
static int DP_TEXT = 0;
static int DP_POINT = 0;
static int DP_LINE = 0;
static int DP_CIRCLE = 0;
static int DP_RECTANGLE = 0;
static int DP_IMAGE = 0;

