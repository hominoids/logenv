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

#define DISPLAY_OPEN 1
#define DISPLAY_WRITE 2
#define DISPLAY_READ 3
#define DISPLAY_UPDATE 4
#define DISPLAY_CLOSE 5
#define DISPLAY_TIME 6
#define DISPLAY_DATE 7
#define DISPLAY_THERMAL 8
#define DISPLAY_SENSOR 9

uint8_t fontoi(char *);

struct display_content {
    char name[15];
    char device[30];
    uint8_t address;
    char type[15];
    uint16_t xloc;
    uint16_t yloc;
    uint32_t color;
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

