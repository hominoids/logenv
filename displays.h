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

//int display_set(struct display *dp);
//int display_get_content(struct display_content *dc);
//int display_open(int (*dp_open)(struct));
//int display_write(int (*dp_write)(struct), struct display_content *dc);
//int display_update(int (*dp_update)(struct));
//int display_close(int (*dp_close)(struct));

int displays(int (*op)(int, int), int, int);
int ssd1681(int, int);
int ssd1306(int, int);

int open_ssd1681(void);


struct display_content {
    char name[15];
    int xloc;
    int yloc;
    int color;
    char font[30];
};

struct display {
    char name[15];
    char device[30];
    int address;
    int xsize;
    int ysize;
    int rotation;
    int *dp_open;
    int *dp_write;
    int *dp_update;
    int *dp_close;
    struct display_content dc[256];
};

static int DISPLAY_ENABLE = 0;
static int DISPLAY_OPEN = 1;
static int DISPLAY_WRITE = 2;
static int DISPLAY_READ = 3;
static int DISPLAY_UPDATE = 4;
static int DISPLAY_CLOSE = 5;

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
static int DP_TEXT = 0;
static int DP_POINT = 0;
static int DP_LINE = 0;
static int DP_CIRCLE = 0;
static int DP_RECTANGLE = 0;
static int DP_IMAGE = 0;

