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
extern uint8_t display_count;
extern uint8_t page;
extern char display_time[];
extern char display_date[];
extern ssd1681_handle_t gs_handle;

uint8_t fontoi(char *);
uint8_t open_ssd1681(void);

struct display_content {
    char name[15];
    char device[30];
    uint8_t address;
    char type[15];
    uint16_t xloc;
    uint16_t yloc;
    uint16_t color;
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
    uint8_t address;
    uint16_t xsize;
    uint16_t ysize;
    uint16_t rotation;
    uint16_t color;
    uint8_t page;
    uint16_t seconds;
    uint8_t init;
    uint8_t dc_count;
    struct display_content dc[32];
};

static uint8_t DISPLAY_ENABLE = 0;
static uint8_t DISPLAY_OPEN = 1;
static uint8_t DISPLAY_WRITE = 2;
static uint8_t DISPLAY_READ = 3;
static uint8_t DISPLAY_UPDATE = 4;
static uint8_t DISPLAY_CLOSE = 5;
static uint8_t DISPLAY_TIME = 6;
static uint8_t DISPLAY_DATE = 7;
static uint8_t DISPLAY_THERMAL = 8;
static uint8_t DISPLAY_SENSOR = 9;

static uint8_t DP_TIME = 0;
static uint8_t DP_DATE = 0;
static uint8_t DP_FREQ = 0;
static uint8_t DP_THERMAL = 0;
static uint8_t DP_MEMORY = 0;
static uint8_t DP_USAGE = 0;
static uint8_t DP_SP2 = 0;
static uint8_t DP_SP3CH1 = 0;
static uint8_t DP_SP3CH2 = 0;
static uint8_t DP_BMP180 = 0;
static uint8_t DP_BME280 = 0;
static uint8_t DP_MCP9808 = 0;
static uint8_t DP_SCD41 = 0;
static uint8_t DP_SGP30 = 0;
static uint8_t DP_TEXT = 0;
static uint8_t DP_POINT = 0;
static uint8_t DP_LINE = 0;
static uint8_t DP_CIRCLE = 0;
static uint8_t DP_RECTANGLE = 0;
static uint8_t DP_IMAGE = 0;

