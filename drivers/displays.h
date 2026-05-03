/*
    logenv Copyright 2025,2026 Edward A. Kisiel
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

#ifndef DISPLAYS_H
#define DISPLAYS_H

#include "./ssd1681/driver_ssd1681_basic.h"
#include "./ssd1306/driver_ssd1306_advance.h"
#include "./ssh1107/driver_ssh1107_advance.h"
#include "./st7789/driver_st7789_basic.h"
#include "./bmp180/driver_bmp180_basic.h"
#include "./bmp388/driver_bmp388_basic.h"
#include "./bmp390/driver_bmp390_basic.h"
#include "./bme280/driver_bme280_basic.h"
#include "./bme680/driver_bme680_gas.h"
#include "./mcp9808/mcp9808.h"
#include "./scd30/driver_scd30_basic.h"
#include "./scd4x/driver_scd4x_basic.h"
#include "./scd4x/driver_scd4x_shot.h"
#include "./sgp30/driver_sgp30_advance.h"
#include "./sht4x/driver_sht4x_basic.h"
#include "./shtc3/driver_shtc3_basic.h"
#include "./aht20/driver_aht20_basic.h"
#include "./htu31d/driver_htu31d_basic.h"

#ifdef __cplusplus
extern "C"{
    #endif

#define DISPLAY_OPEN 1
#define DISPLAY_CLOSE 2
#define DISPLAY_READ 3
#define DISPLAY_WRITE 4
#define DISPLAY_UPDATE 5
#define DISPLAY_TIME 6
#define DISPLAY_DATE 7
#define DISPLAY_THERMAL 8
#define DISPLAY_SENSOR 9

extern uint8_t display_count;
extern uint8_t page;
extern char display_time[];
extern char display_date[];
extern ssd1681_handle_t ssd1681_handle;
extern ssd1306_handle_t ssd1306_handle;
extern ssh1107_handle_t ssh1107_handle;
extern st7789_handle_t st7789_handle;

uint8_t fontoi(char *);

struct display_content {
    char name[15];
    char device[255];
    uint8_t address;
    char type[15];
    uint16_t xloc;
    uint16_t yloc;
    uint32_t color;
    char font[30];
    char label[20];
    char unit[20];
    char data1[128];  // Temperature c
    char data2[30];  // Humidity %
    char data3[30];  // Pressure hPa
    char data4[30];  // CO2 as ppm
    char data5[30];  // TVOC as ppb
    uint8_t read;
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
    uint8_t (*dptr)(struct display *, uint8_t, uint8_t);
    uint8_t dc_count;
    struct display_content dc[32];
};

#ifdef __cplusplus
}
#endif

#endif
