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

    int display (int (*op)(void), init)

*/
#include <stdint.h>
#include <stdio.h>
#include "drivers/ssd1681/driver_ssd1681_basic.h"
#include "drivers/ssd1681/driver_ssd1681_interface.h"
#include "displays.h"

int displays(int (*op)(struct display *, int, int), struct display *ptr, int dcidx, int cmd) {
    if(op(ptr, dcidx, cmd)) {
        return(1);
    }
    return(0);

}

int ssd1681(struct display *ptr, int dcidx, int cmd) {

    if(cmd == DISPLAY_OPEN) {
        if(open_ssd1681()) {
            printf("\nERROR: Cannot open ssd1681\n");
            return(1);
        }
    return(0);
    }
    if(cmd == DISPLAY_UPDATE) {
        if(ssd1681_gram_update(&gs_handle, SSD1681_COLOR_BLACK) != 0) {
            ssd1681_interface_debug_print("ssd1681: update failed.\n");
            return(1);
        }
    return(0);
    }
    if(cmd == DISPLAY_TIME) {
        if(ssd1681_gram_write_string(&gs_handle, SSD1681_COLOR_BLACK, ptr->dc[dcidx].xloc, \
            ptr->dc[dcidx].yloc, display_time, (uint16_t)strlen(display_time), 1, fontoi(ptr->dc[dcidx].font)) != 0) {
            ssd1681_interface_debug_print("ssd1681: time string write failed.\n");
            return(1);
        }
    return(0);
    }
    if(cmd == DISPLAY_DATE) {
        if(ssd1681_gram_write_string(&gs_handle, SSD1681_COLOR_BLACK, ptr->dc[dcidx].xloc, \
            ptr->dc[dcidx].yloc, display_date, (uint16_t)strlen(display_date), 1, fontoi(ptr->dc[dcidx].font)) != 0) {
            ssd1681_interface_debug_print("ssd1681: date string write failed.\n");
            return(1);
        }
    return(0);
    }
    if(cmd == DISPLAY_WRITE) {
        char buffer[48];
        strcpy(buffer, "\0");
        if(!strcmp(ptr->dc[dcidx].type,"N") || !strcmp(ptr->dc[dcidx].type,"NT")) {
            strcat(buffer, ptr->dc[dcidx].data1);
        }
        if(ptr->dc[dcidx].label) {
            strcat(buffer,ptr->dc[dcidx].label);
        }
        if(!strcmp(ptr->dc[dcidx].type,"T") || !strcmp(ptr->dc[dcidx].type,"TN") || !strcmp(ptr->dc[dcidx].type,"NT")) {
            strcat(buffer, ptr->dc[dcidx].data2);
        }
        if(ptr->dc[dcidx].unit) {
            strcat(buffer,ptr->dc[dcidx].unit);
        }
        if(!strcmp(ptr->dc[dcidx].type,"TN")) {
            strcat(buffer, ptr->dc[dcidx].data1);
        }
        if(ssd1681_gram_write_string(&gs_handle, SSD1681_COLOR_BLACK, ptr->dc[dcidx].xloc, \
            ptr->dc[dcidx].yloc, buffer, (uint16_t)strlen(buffer), 1, fontoi(ptr->dc[dcidx].font)) != 0) {
            ssd1681_interface_debug_print("ssd1681: string write failed.\n");
            return(1);
        }
    return(0);
    }
    if(cmd == DISPLAY_SENSOR) {
        char buffer[25];
        char buffer2[7];
        float temp_f = 0;
        strcpy(buffer,"\0");
        if(ptr->dc[dcidx].label) {
            strcat(buffer,ptr->dc[dcidx].label);
        }
        if(!strcmp(ptr->dc[dcidx].type,"C")) {
            strcat(buffer, ptr->dc[dcidx].data1);
        }
        if(!strcmp(ptr->dc[dcidx].type,"F")) {
            sscanf(ptr->dc[dcidx].data1,"%f",&temp_f);
            temp_f = temp_f * 1.8 + 32;
            sprintf(buffer2, "%.2lf", temp_f);
            strcat(buffer, buffer2);
        }
        if(!strcmp(ptr->dc[dcidx].type,"H")) {
            strcat(buffer, ptr->dc[dcidx].data2);
        }
        if(!strcmp(ptr->dc[dcidx].type,"P")) {
            strcat(buffer, ptr->dc[dcidx].data3);
        }
        if(!strcmp(ptr->dc[dcidx].type,"G")) {
            strcat(buffer, ptr->dc[dcidx].data4);
        }
        if(!strcmp(ptr->dc[dcidx].type,"V")) {
            strcat(buffer, ptr->dc[dcidx].data5);
        }
        if(ptr->dc[dcidx].unit) {
            strcat(buffer,ptr->dc[dcidx].unit);
        }
        if(ssd1681_gram_write_string(&gs_handle, SSD1681_COLOR_BLACK, ptr->dc[dcidx].xloc, \
            ptr->dc[dcidx].yloc, buffer, (uint16_t)strlen(buffer), 1, \
                fontoi(ptr->dc[dcidx].font)) != 0) {
            ssd1681_interface_debug_print("ssd1681: bme280 string write failed.\n");
            return(1);
        }
    return(0);
    }
}

int ssd1306(struct display *ptr, int dcidx, int cmd) {

    if(cmd == DISPLAY_OPEN) {
        return(0);
    }
    if(cmd == DISPLAY_UPDATE) {
        return(0);
    }
    if(cmd == DISPLAY_TIME) {
        return(0);
    }
    if(cmd == DISPLAY_DATE) {
        return(0);
    }
    if(cmd == DISPLAY_WRITE) {
        return(0);
    }
    if(cmd == DISPLAY_SENSOR) {
        return(0);
    }
}

int fontoi(char *font_name) {
    if(!strcmp(font_name, "SSD1681_FONT_12")) {
        return(SSD1681_FONT_12);
    }
    if(!strcmp(font_name, "SSD1681_FONT_16")) {
        return(SSD1681_FONT_16);
    }
    if(!strcmp(font_name, "SSD1681_FONT_24")) {
        return(SSD1681_FONT_24);
    }
    if(!strcmp(font_name, "SSD1681_MONOSPACE_28")) {
        return(SSD1681_MONOSPACE_28);
    }
    if(!strcmp(font_name, "SSD1681_MONOSPACE_36")) {
        return(SSD1681_MONOSPACE_36);
    }
    if(!strcmp(font_name, "SSD1681_MONOSPACE_48")) {
        return(SSD1681_MONOSPACE_48);
    }
    if(!strcmp(font_name, "SSD1681_MONOSPACE_72")) {
        return(SSD1681_MONOSPACE_72);
    }
    return(SSD1681_FONT_12);
}