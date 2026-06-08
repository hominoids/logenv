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

*/

#include <stdint.h>
#include <stdio.h>
#include "displays.h"

uint8_t ssd1681(struct display *ptr, uint8_t dcidx, uint8_t cmd) {

    char buffer[127] = "\0";

    if(cmd == DISPLAY_OPEN) {
        if(ssd1681_basic_init()) {
            printf("\nERROR: Cannot open ssd1681\n");
            return(1);
        }
        return(0);
    }

    if(cmd == DISPLAY_UPDATE) {
        if(ssd1681_gram_update(&ssd1681_handle, SSD1681_COLOR_BLACK) != 0) {
            ssd1681_interface_debug_print("ssd1681: update failed.\n");
            return(1);
        }
        return(0);
    }

    if(cmd == DISPLAY_TIME) {

        int16_t pad = ptr->dc[dcidx].prevlen - strlen(ptr->dc[dcidx].data1);
        ptr->dc[dcidx].prevlen = strlen(ptr->dc[dcidx].data1);

        strcpy(buffer, ptr->dc[dcidx].data1);

        if(pad > 0) {
            while (pad > 0, pad--) {
                strcat(buffer, " ");
            }
        }

        if(ssd1681_gram_write_string(&ssd1681_handle, SSD1681_COLOR_BLACK, ptr->dc[dcidx].xloc, \
            ptr->dc[dcidx].yloc, buffer, (uint16_t)strlen(buffer), 1, fontoi(ptr->dc[dcidx].font)) != 0) {
            ssd1681_interface_debug_print("ssd1681: time string write failed.\n");
            return(1);
        }
        return(0);
    }

    if(cmd == DISPLAY_DATE) {

        int16_t pad = ptr->dc[dcidx].prevlen - strlen(ptr->dc[dcidx].data1);
        ptr->dc[dcidx].prevlen = strlen(ptr->dc[dcidx].data1);

        strcpy(buffer, ptr->dc[dcidx].data1);

        if(pad > 0) {
            while (pad > 0, pad--) {
                strcat(buffer, " ");
            }
        }

        if(ssd1681_gram_write_string(&ssd1681_handle, SSD1681_COLOR_BLACK, ptr->dc[dcidx].xloc, \
            ptr->dc[dcidx].yloc, buffer, (uint16_t)strlen(buffer), 1, fontoi(ptr->dc[dcidx].font)) != 0) {
            ssd1681_interface_debug_print("ssd1681: date string write failed.\n");
            return(1);
        }
        return(0);
    }

    if(cmd == DISPLAY_WRITE) {

        int16_t pad = ptr->dc[dcidx].prevlen - strlen(ptr->dc[dcidx].data1);
        ptr->dc[dcidx].prevlen = strlen(ptr->dc[dcidx].data1);

        if(!strcmp(ptr->dc[dcidx].dtype,"N") || !strcmp(ptr->dc[dcidx].dtype,"ND")) {
            strcat(buffer, ptr->dc[dcidx].data1);
        }
        if(ptr->dc[dcidx].label) {
            strcat(buffer,ptr->dc[dcidx].label);
        }
        if(!strcmp(ptr->dc[dcidx].dtype,"D") || !strcmp(ptr->dc[dcidx].dtype,"DN") || !strcmp(ptr->dc[dcidx].dtype,"ND")) {
            strcat(buffer, ptr->dc[dcidx].data2);
        }
        if(!strcmp(ptr->dc[dcidx].cmd,"governor")) {
            strcat(buffer, ptr->dc[dcidx].data1);
        }
        if(!strcmp(ptr->dc[dcidx].cmd,"disk")) {
            strcat(buffer, ptr->dc[dcidx].data1);
        }
        if(!strcmp(ptr->dc[dcidx].cmd,"swap")) {
            strcat(buffer, ptr->dc[dcidx].data1);
        }
        if(!strcmp(ptr->dc[dcidx].cmd,"uptime")) {
            strcat(buffer, ptr->dc[dcidx].data1);
        }
        if(!strcmp(ptr->dc[dcidx].cmd,"sysload")) {
            strcat(buffer, ptr->dc[dcidx].data1);
        }
        if(!strcmp(ptr->dc[dcidx].cmd,"hostname")) {
            strcat(buffer, ptr->dc[dcidx].data1);
        }
        if(!strcmp(ptr->dc[dcidx].cmd,"kernel")) {
            strcat(buffer, ptr->dc[dcidx].data1);
        }
        if(!strcmp(ptr->dc[dcidx].cmd,"iio")) {
            strcat(buffer, ptr->dc[dcidx].data1);
        }
        if(!strcmp(ptr->dc[dcidx].cmd,"text")) {
            strcat(buffer, ptr->dc[dcidx].device);
        }
        if(ptr->dc[dcidx].unit) {
            strcat(buffer,ptr->dc[dcidx].unit);
        }
        if(!strcmp(ptr->dc[dcidx].dtype,"DN")) {
            strcat(buffer, ptr->dc[dcidx].data1);
        }

        if(pad > 0) {
            while (pad > 0, pad--) {
                strcat(buffer, " ");
            }
        }

        if(ssd1681_gram_write_string(&ssd1681_handle, SSD1681_COLOR_BLACK, ptr->dc[dcidx].xloc, \
            ptr->dc[dcidx].yloc, buffer, (uint16_t)strlen(buffer), 1, fontoi(ptr->dc[dcidx].font)) != 0) {
            ssd1681_interface_debug_print("ssd1681: string write failed.\n");
            return(1);
        }
        return(0);
    }

    if(cmd == DISPLAY_SENSOR) {

        char buffer2[7] = "\0";
        float temp_f = 0;

        int16_t pad = ptr->dc[dcidx].prevlen - strlen(ptr->dc[dcidx].data1);
        ptr->dc[dcidx].prevlen = strlen(ptr->dc[dcidx].data1);

        if(ptr->dc[dcidx].label) {
            strcat(buffer,ptr->dc[dcidx].label);
        }
        if(!strcmp(ptr->dc[dcidx].dtype,"C")) {
            strcat(buffer, ptr->dc[dcidx].data1);
        }
        if(!strcmp(ptr->dc[dcidx].dtype,"F")) {
            sscanf(ptr->dc[dcidx].data1,"%f",&temp_f);
            temp_f = temp_f * 1.8 + 32;
            sprintf(buffer2, "%.1lf", temp_f);
            strcat(buffer, buffer2);
        }
        if(!strcmp(ptr->dc[dcidx].dtype,"H")) {
            strcat(buffer, ptr->dc[dcidx].data2);
        }
        if(!strcmp(ptr->dc[dcidx].dtype,"P")) {
            strcat(buffer, ptr->dc[dcidx].data3);
        }
        if(!strcmp(ptr->dc[dcidx].dtype,"G")) {
            strcat(buffer, ptr->dc[dcidx].data4);
        }
        if(!strcmp(ptr->dc[dcidx].dtype,"V")) {
            strcat(buffer, ptr->dc[dcidx].data5);
        }
        if(ptr->dc[dcidx].unit) {
            strcat(buffer,ptr->dc[dcidx].unit);
        }

        if(pad > 0) {
            while (pad > 0, pad--) {
                strcat(buffer, " ");
            }
        }

        if(ssd1681_gram_write_string(&ssd1681_handle, SSD1681_COLOR_BLACK, ptr->dc[dcidx].xloc, \
            ptr->dc[dcidx].yloc, buffer, (uint16_t)strlen(buffer), 1, \
                fontoi(ptr->dc[dcidx].font)) != 0) {
            ssd1681_interface_debug_print("ssd1681: sensor string write failed.\n");
            return(1);
        }
        return(0);
    }
}


uint8_t ssd1306(struct display *ptr, uint8_t dcidx, uint8_t cmd) {

    char buffer[127] = "\0";

    if(cmd == DISPLAY_OPEN) {

        SSD1306_ADVANCE_DEFAULT_CONTRAST = ptr->contrast;
        SSD1306_ADVANCE_DEFAULT_SEGMENT = ptr->segment_column_address;
        SSD1306_ADVANCE_DEFAULT_SCAN_DIRECTION = ptr->scan_direction_start;
        SSD1306_ADVANCE_DEFAULT_LEFT_RIGHT_REMAP = ptr->left_right_remap;
        SSD1306_ADVANCE_DEFAULT_PIN_CONF = ptr->pin_config_alt;
        SSD1306_ADVANCE_DEFAULT_MULTIPLEX_RATIO = ptr->ysize-1;
        SSD1306_ADVANCE_DEFAULT_DESELECT_LEVEL = ptr->vcom_deselect;

        if(ssd1306_advance_init(SSD1306_INTERFACE_IIC, ptr->address)) {
            printf("\nERROR: Cannot open ssd1306 %d %d\n", SSD1306_INTERFACE_IIC, ptr->address);
            return(1);
        }
        return(0);
    }

    if(cmd == DISPLAY_UPDATE) {
        if(ssd1306_gram_update(&ssd1306_handle) != 0) {
            ssd1306_interface_debug_print("ssd1306: update failed.\n");
            return(1);
        }
        return(0);
    }

    if(cmd == DISPLAY_TIME) {

        int16_t pad = ptr->dc[dcidx].prevlen - strlen(ptr->dc[dcidx].data1);
        ptr->dc[dcidx].prevlen = strlen(ptr->dc[dcidx].data1);

        strcpy(buffer, ptr->dc[dcidx].data1);

        if(pad > 0) {
            while (pad > 0, pad--) {
                strcat(buffer, " ");
            }
        }

        if(ssd1306_gram_write_string(&ssd1306_handle, ptr->dc[dcidx].xloc, \
            ptr->dc[dcidx].yloc, buffer, (uint16_t)strlen(buffer), 1, \
            fontoi(ptr->dc[dcidx].font)) != 0) {
            ssd1306_interface_debug_print("ssd1306: time string write failed.\n");
            return(1);
        }
        return(0);
    }

    if(cmd == DISPLAY_DATE) {

        int16_t pad = ptr->dc[dcidx].prevlen - strlen(ptr->dc[dcidx].data1);
        ptr->dc[dcidx].prevlen = strlen(ptr->dc[dcidx].data1);

        strcpy(buffer, ptr->dc[dcidx].data1);

        if(pad > 0) {
            while (pad > 0, pad--) {
                strcat(buffer, " ");
            }
        }

        if(ssd1306_gram_write_string(&ssd1306_handle, ptr->dc[dcidx].xloc, \
            ptr->dc[dcidx].yloc, buffer, (uint16_t)strlen(buffer), 1, \
            fontoi(ptr->dc[dcidx].font)) != 0) {
            ssd1306_interface_debug_print("ssd1306: date string write failed.\n");
            return(1);
        }
        return(0);
    }

    if(cmd == DISPLAY_WRITE) {

        int16_t pad = ptr->dc[dcidx].prevlen - strlen(ptr->dc[dcidx].data1);
        ptr->dc[dcidx].prevlen = strlen(ptr->dc[dcidx].data1);

        if(!strcmp(ptr->dc[dcidx].dtype,"N") || !strcmp(ptr->dc[dcidx].dtype,"ND")) {
            strcat(buffer, ptr->dc[dcidx].data1);
        }
        if(ptr->dc[dcidx].label) {
            strcat(buffer,ptr->dc[dcidx].label);
        }
        if(!strcmp(ptr->dc[dcidx].dtype,"D") || !strcmp(ptr->dc[dcidx].dtype,"DN") || !strcmp(ptr->dc[dcidx].dtype,"ND")) {
            strcat(buffer, ptr->dc[dcidx].data2);
        }
        if(!strcmp(ptr->dc[dcidx].cmd,"governor")) {
            strcat(buffer, ptr->dc[dcidx].data1);
        }
        if(!strcmp(ptr->dc[dcidx].cmd,"disk")) {
            strcat(buffer, ptr->dc[dcidx].data1);
        }
        if(!strcmp(ptr->dc[dcidx].cmd,"swap")) {
            strcat(buffer, ptr->dc[dcidx].data1);
        }
        if(!strcmp(ptr->dc[dcidx].cmd,"uptime")) {
            strcat(buffer, ptr->dc[dcidx].data1);
        }
        if(!strcmp(ptr->dc[dcidx].cmd,"sysload")) {
            strcat(buffer, ptr->dc[dcidx].data1);
        }
        if(!strcmp(ptr->dc[dcidx].cmd,"hostname")) {
            strcat(buffer, ptr->dc[dcidx].data1);
        }
        if(!strcmp(ptr->dc[dcidx].cmd,"kernel")) {
            strcat(buffer, ptr->dc[dcidx].data1);
        }
        if(!strcmp(ptr->dc[dcidx].cmd,"iio")) {
            strcat(buffer, ptr->dc[dcidx].data1);
        }
        if(!strcmp(ptr->dc[dcidx].cmd,"text")) {
            strcat(buffer, ptr->dc[dcidx].device);
        }
        if(ptr->dc[dcidx].unit) {
            strcat(buffer,ptr->dc[dcidx].unit);
        }
        if(!strcmp(ptr->dc[dcidx].dtype,"DN")) {
            strcat(buffer, ptr->dc[dcidx].data1);
        }

        if(pad > 0) {
            while (pad > 0, pad--) {
                strcat(buffer, " ");
            }
        }

        if(ssd1306_gram_write_string(&ssd1306_handle, ptr->dc[dcidx].xloc, \
            ptr->dc[dcidx].yloc, buffer, (uint16_t)strlen(buffer), 1, \
            fontoi(ptr->dc[dcidx].font)) != 0) {
            ssd1306_interface_debug_print("ssd1306: display string write failed.\n");
            return(1);
        }
        return(0);
    }

    if(cmd == DISPLAY_SENSOR) {

        char buffer2[7] = "\0";
        float temp_f = 0;

        int16_t pad = ptr->dc[dcidx].prevlen - strlen(ptr->dc[dcidx].data1);
        ptr->dc[dcidx].prevlen = strlen(ptr->dc[dcidx].data1);


        if(ptr->dc[dcidx].label) {
            strcat(buffer,ptr->dc[dcidx].label);
        }
        if(!strcmp(ptr->dc[dcidx].dtype,"C")) {
            strcat(buffer, ptr->dc[dcidx].data1);
        }
        if(!strcmp(ptr->dc[dcidx].dtype,"F")) {
            sscanf(ptr->dc[dcidx].data1,"%f",&temp_f);
            temp_f = temp_f * 1.8 + 32;
            sprintf(buffer2, "%.1lf", temp_f);
            strcat(buffer, buffer2);
        }
        if(!strcmp(ptr->dc[dcidx].dtype,"H")) {
            strcat(buffer, ptr->dc[dcidx].data2);
        }
        if(!strcmp(ptr->dc[dcidx].dtype,"P")) {
            strcat(buffer, ptr->dc[dcidx].data3);
        }
        if(!strcmp(ptr->dc[dcidx].dtype,"G")) {
            strcat(buffer, ptr->dc[dcidx].data4);
        }
        if(!strcmp(ptr->dc[dcidx].dtype,"V")) {
            strcat(buffer, ptr->dc[dcidx].data5);
        }
        if(ptr->dc[dcidx].unit) {
            strcat(buffer,ptr->dc[dcidx].unit);
        }

        if(pad > 0) {
            while (pad > 0, pad--) {
                strcat(buffer, " ");
            }
        }

        if(ssd1306_gram_write_string(&ssd1306_handle, ptr->dc[dcidx].xloc, \
            ptr->dc[dcidx].yloc, buffer, (uint16_t)strlen(buffer), 1, \
            fontoi(ptr->dc[dcidx].font)) != 0) {
            ssd1306_interface_debug_print("ssd1306: date string write failed.\n");
            return(1);
        }
        return(0);
    }
}


uint8_t sh1107(struct display *ptr, uint8_t dcidx, uint8_t cmd) {

    char buffer[127] = "\0";

    if(cmd == DISPLAY_OPEN) {

        SH1107_ADVANCE_DEFAULT_CONTRAST = ptr->contrast;
        SH1107_ADVANCE_DEFAULT_SEGMENT = ptr->segment_column_address;
        SH1107_ADVANCE_DEFAULT_SCAN_DIRECTION = ptr->scan_direction_start;
        SH1107_ADVANCE_DEFAULT_LEFT_RIGHT_REMAP = ptr->left_right_remap;
        SH1107_ADVANCE_DEFAULT_MULTIPLEX_RATIO = ptr->ysize-1;
        SH1107_ADVANCE_DEFAULT_DESELECT_LEVEL = ptr->vcom_deselect;

        if(sh1107_advance_init(SSD1306_INTERFACE_IIC, ptr->address)) {
            printf("\nERROR: Cannot open sh1107 %d %d\n", SSD1306_INTERFACE_IIC, ptr->address);
            return(1);
        }
        return(0);
    }

    if(cmd == DISPLAY_UPDATE) {
        if(sh1107_gram_update(&sh1107_handle) != 0) {
            sh1107_interface_debug_print("sh1107: update failed.\n");
            return(1);
        }
        return(0);
    }

    if(cmd == DISPLAY_TIME) {

        int16_t pad = ptr->dc[dcidx].prevlen - strlen(ptr->dc[dcidx].data1);
        ptr->dc[dcidx].prevlen = strlen(ptr->dc[dcidx].data1);

        strcpy(buffer, ptr->dc[dcidx].data1);

        if(pad > 0) {
            while (pad > 0, pad--) {
                strcat(buffer, " ");
            }
        }

        if(sh1107_gram_write_string(&sh1107_handle, ptr->dc[dcidx].xloc, \
            ptr->dc[dcidx].yloc, buffer, (uint16_t)strlen(buffer), 1, \
            fontoi(ptr->dc[dcidx].font)) != 0) {
            sh1107_interface_debug_print("sh1107: time string write failed.\n");
            return(1);
        }
        return(0);
    }

    if(cmd == DISPLAY_DATE) {

        int16_t pad = ptr->dc[dcidx].prevlen - strlen(ptr->dc[dcidx].data1);
        ptr->dc[dcidx].prevlen = strlen(ptr->dc[dcidx].data1);

        strcpy(buffer, ptr->dc[dcidx].data1);

        if(pad > 0) {
            while (pad > 0, pad--) {
                strcat(buffer, " ");
            }
        }

        if(sh1107_gram_write_string(&sh1107_handle, ptr->dc[dcidx].xloc, \
            ptr->dc[dcidx].yloc, buffer, (uint16_t)strlen(buffer), 1, \
            fontoi(ptr->dc[dcidx].font)) != 0) {
            sh1107_interface_debug_print("sh1107: date string write failed.\n");
            return(1);
        }
        return(0);
    }

    if(cmd == DISPLAY_WRITE) {

        int16_t pad = ptr->dc[dcidx].prevlen - strlen(ptr->dc[dcidx].data1);
        ptr->dc[dcidx].prevlen = strlen(ptr->dc[dcidx].data1);

        if(!strcmp(ptr->dc[dcidx].dtype,"N") || !strcmp(ptr->dc[dcidx].dtype,"ND")) {
            strcat(buffer, ptr->dc[dcidx].data1);
        }
        if(ptr->dc[dcidx].label) {
            strcat(buffer,ptr->dc[dcidx].label);
        }
        if(!strcmp(ptr->dc[dcidx].dtype,"D") || !strcmp(ptr->dc[dcidx].dtype,"DN") || !strcmp(ptr->dc[dcidx].dtype,"ND")) {
            strcat(buffer, ptr->dc[dcidx].data2);
        }
        if(!strcmp(ptr->dc[dcidx].cmd,"governor")) {
            strcat(buffer, ptr->dc[dcidx].data1);
        }
        if(!strcmp(ptr->dc[dcidx].cmd,"disk")) {
            strcat(buffer, ptr->dc[dcidx].data1);
        }
        if(!strcmp(ptr->dc[dcidx].cmd,"swap")) {
            strcat(buffer, ptr->dc[dcidx].data1);
        }
        if(!strcmp(ptr->dc[dcidx].cmd,"uptime")) {
            strcat(buffer, ptr->dc[dcidx].data1);
        }
        if(!strcmp(ptr->dc[dcidx].cmd,"sysload")) {
            strcat(buffer, ptr->dc[dcidx].data1);
        }
        if(!strcmp(ptr->dc[dcidx].cmd,"hostname")) {
            strcat(buffer, ptr->dc[dcidx].data1);
        }
        if(!strcmp(ptr->dc[dcidx].cmd,"kernel")) {
            strcat(buffer, ptr->dc[dcidx].data1);
        }
        if(!strcmp(ptr->dc[dcidx].cmd,"iio")) {
            strcat(buffer, ptr->dc[dcidx].data1);
        }
        if(!strcmp(ptr->dc[dcidx].cmd,"text")) {
            strcat(buffer, ptr->dc[dcidx].device);
        }
        if(ptr->dc[dcidx].unit) {
            strcat(buffer,ptr->dc[dcidx].unit);
        }
        if(!strcmp(ptr->dc[dcidx].dtype,"DN")) {
            strcat(buffer, ptr->dc[dcidx].data1);
        }

        if(pad > 0) {
            while (pad > 0, pad--) {
                strcat(buffer, " ");
            }
        }

        if(sh1107_gram_write_string(&sh1107_handle, ptr->dc[dcidx].xloc, \
            ptr->dc[dcidx].yloc, buffer, (uint16_t)strlen(buffer), 1, \
            fontoi(ptr->dc[dcidx].font)) != 0) {
            sh1107_interface_debug_print("sh1107: display string write failed.\n");
            return(1);
        }
        return(0);
    }

    if(cmd == DISPLAY_SENSOR) {

        char buffer2[7] = "\0";
        float temp_f = 0;

        int16_t pad = ptr->dc[dcidx].prevlen - strlen(ptr->dc[dcidx].data1);
        ptr->dc[dcidx].prevlen = strlen(ptr->dc[dcidx].data1);

        if(ptr->dc[dcidx].label) {
            strcat(buffer,ptr->dc[dcidx].label);
        }
        if(!strcmp(ptr->dc[dcidx].dtype,"C")) {
            strcat(buffer, ptr->dc[dcidx].data1);
        }
        if(!strcmp(ptr->dc[dcidx].dtype,"F")) {
            sscanf(ptr->dc[dcidx].data1,"%f",&temp_f);
            temp_f = temp_f * 1.8 + 32;
            sprintf(buffer2, "%.1lf", temp_f);
            strcat(buffer, buffer2);
        }
        if(!strcmp(ptr->dc[dcidx].dtype,"H")) {
            strcat(buffer, ptr->dc[dcidx].data2);
        }
        if(!strcmp(ptr->dc[dcidx].dtype,"P")) {
            strcat(buffer, ptr->dc[dcidx].data3);
        }
        if(!strcmp(ptr->dc[dcidx].dtype,"G")) {
            strcat(buffer, ptr->dc[dcidx].data4);
        }
        if(!strcmp(ptr->dc[dcidx].dtype,"V")) {
            strcat(buffer, ptr->dc[dcidx].data5);
        }
        if(ptr->dc[dcidx].unit) {
            strcat(buffer,ptr->dc[dcidx].unit);
        }

        if(pad > 0) {
            while (pad > 0, pad--) {
                strcat(buffer, " ");
            }
        }

        if(sh1107_gram_write_string(&sh1107_handle, ptr->dc[dcidx].xloc, \
            ptr->dc[dcidx].yloc, buffer, (uint16_t)strlen(buffer), 1, \
            fontoi(ptr->dc[dcidx].font)) != 0) {
            sh1107_interface_debug_print("sh1107: date string write failed.\n");
            return(1);
        }
        return(0);
    }
}


uint8_t st7789(struct display *ptr, uint8_t dcidx, uint8_t cmd) {

    char buffer[127] = "\0";

    if(cmd == DISPLAY_OPEN) {
        if(st7789_basic_init()) {
            printf("\nERROR: Cannot open st7789\n");
            return(1);
        }
        return(0);
    }

    if(cmd == DISPLAY_TIME) {

        int16_t pad = ptr->dc[dcidx].prevlen - strlen(ptr->dc[dcidx].data1);
        ptr->dc[dcidx].prevlen = strlen(ptr->dc[dcidx].data1);

        strcpy(buffer, ptr->dc[dcidx].data1);

        if(pad > 0) {
            while (pad > 0, pad--) {
                strcat(buffer, " ");
            }
        }
        if(st7789_write_string(&st7789_handle, ptr->dc[dcidx].xloc, ptr->dc[dcidx].yloc, \
            buffer, (uint16_t)strlen(buffer), 0xFFFFU, fontoi(ptr->dc[dcidx].font)) != 0) {
            st7789_interface_debug_print("st7789: time string write failed.\n");
            return(1);
        }
        return(0);
    }

    if(cmd == DISPLAY_DATE) {

        int16_t pad = ptr->dc[dcidx].prevlen - strlen(ptr->dc[dcidx].data1);
        ptr->dc[dcidx].prevlen = strlen(ptr->dc[dcidx].data1);

        strcpy(buffer, ptr->dc[dcidx].data1);

        if(pad > 0) {
            while (pad > 0, pad--) {
                strcat(buffer, " ");
            }
        }

        if(st7789_write_string(&st7789_handle, ptr->dc[dcidx].xloc, ptr->dc[dcidx].yloc, \
            buffer, (uint16_t)strlen(buffer), 0xFFFFU, fontoi(ptr->dc[dcidx].font)) != 0) {
            st7789_interface_debug_print("st7789: date string write failed.\n");
            return(1);
        }
        return(0);
    }

    if(cmd == DISPLAY_WRITE) {

        int16_t pad = ptr->dc[dcidx].prevlen - strlen(ptr->dc[dcidx].data1);
        ptr->dc[dcidx].prevlen = strlen(ptr->dc[dcidx].data1);

        if(!strcmp(ptr->dc[dcidx].dtype,"N") || !strcmp(ptr->dc[dcidx].dtype,"ND")) {
            strcat(buffer, ptr->dc[dcidx].data1);
        }
        if(ptr->dc[dcidx].label) {
            strcat(buffer,ptr->dc[dcidx].label);
        }
        if(!strcmp(ptr->dc[dcidx].dtype,"D") || !strcmp(ptr->dc[dcidx].dtype,"DN") || !strcmp(ptr->dc[dcidx].dtype,"ND")) {
            strcat(buffer, ptr->dc[dcidx].data2);
        }
        if(!strcmp(ptr->dc[dcidx].cmd,"governor")) {
            strcat(buffer, ptr->dc[dcidx].data1);
        }
        if(!strcmp(ptr->dc[dcidx].cmd,"disk")) {
            strcat(buffer, ptr->dc[dcidx].data1);
        }
        if(!strcmp(ptr->dc[dcidx].cmd,"swap")) {
            strcat(buffer, ptr->dc[dcidx].data1);
        }
        if(!strcmp(ptr->dc[dcidx].cmd,"uptime")) {
            strcat(buffer, ptr->dc[dcidx].data1);
        }
        if(!strcmp(ptr->dc[dcidx].cmd,"sysload")) {
            strcat(buffer, ptr->dc[dcidx].data1);
        }
        if(!strcmp(ptr->dc[dcidx].cmd,"hostname")) {
            strcat(buffer, ptr->dc[dcidx].data1);
        }
        if(!strcmp(ptr->dc[dcidx].cmd,"kernel")) {
            strcat(buffer, ptr->dc[dcidx].data1);
        }
        if(!strcmp(ptr->dc[dcidx].cmd,"iio")) {
            strcat(buffer, ptr->dc[dcidx].data1);
        }
        if(!strcmp(ptr->dc[dcidx].cmd,"text")) {
            strcat(buffer, ptr->dc[dcidx].device);
        }
        if(ptr->dc[dcidx].unit) {
            strcat(buffer,ptr->dc[dcidx].unit);
        }
        if(!strcmp(ptr->dc[dcidx].dtype,"DN")) {
            strcat(buffer, ptr->dc[dcidx].data1);
        }

        if(pad > 0) {
            while (pad > 0, pad--) {
                strcat(buffer, " ");
            }
        }

        if(st7789_write_string(&st7789_handle, ptr->dc[dcidx].xloc, ptr->dc[dcidx].yloc, \
            buffer, (uint16_t)strlen(buffer), 0xFFFFU, fontoi(ptr->dc[dcidx].font)) != 0) {
            st7789_interface_debug_print("st7789: string write failed.\n");
            return(1);
        }
        return(0);
    }

    if(cmd == DISPLAY_SENSOR) {

        char buffer2[7] = "\0";
        float temp_f = 0;

        int16_t pad = ptr->dc[dcidx].prevlen - strlen(ptr->dc[dcidx].data1);
        ptr->dc[dcidx].prevlen = strlen(ptr->dc[dcidx].data1);

        if(ptr->dc[dcidx].label) {
            strcat(buffer,ptr->dc[dcidx].label);
        }
        if(!strcmp(ptr->dc[dcidx].dtype,"C")) {
            strcat(buffer, ptr->dc[dcidx].data1);
        }
        if(!strcmp(ptr->dc[dcidx].dtype,"F")) {
            sscanf(ptr->dc[dcidx].data1,"%f",&temp_f);
            temp_f = temp_f * 1.8 + 32;
            sprintf(buffer2, "%.1lf", temp_f);
            strcat(buffer, buffer2);
        }
        if(!strcmp(ptr->dc[dcidx].dtype,"H")) {
            strcat(buffer, ptr->dc[dcidx].data2);
        }
        if(!strcmp(ptr->dc[dcidx].dtype,"P")) {
            strcat(buffer, ptr->dc[dcidx].data3);
        }
        if(!strcmp(ptr->dc[dcidx].dtype,"G")) {
            strcat(buffer, ptr->dc[dcidx].data4);
        }
        if(!strcmp(ptr->dc[dcidx].dtype,"V")) {
            strcat(buffer, ptr->dc[dcidx].data5);
        }
        if(ptr->dc[dcidx].unit) {
            strcat(buffer,ptr->dc[dcidx].unit);
        }

        if(pad > 0) {
            while (pad > 0, pad--) {
                strcat(buffer, " ");
            }
        }

        if(st7789_write_string(&st7789_handle, ptr->dc[dcidx].xloc, ptr->dc[dcidx].yloc, \
            buffer, (uint16_t)strlen(buffer), 0xFFFFU, fontoi(ptr->dc[dcidx].font)) != 0) {
            st7789_interface_debug_print("st7789: sensor string write failed.\n");
            return(1);
        }
        return(0);
    }
}


uint8_t fontoi(char *font_name) {
    if(!strcmp(font_name, "DEFAULT_12")) {
        return(DEFAULT_12);
    }
    if(!strcmp(font_name, "DEFAULT_16")) {
        return(DEFAULT_16);
    }
    if(!strcmp(font_name, "DEFAULT_24")) {
        return(DEFAULT_24);
    }
    if(!strcmp(font_name, "MONOSPACE_12")) {
        return(MONOSPACE_12);
    }
    if(!strcmp(font_name, "MONOSPACE_16")) {
        return(MONOSPACE_16);
    }
    if(!strcmp(font_name, "MONOSPACE_20")) {
        return(MONOSPACE_20);
    }
    if(!strcmp(font_name, "MONOSPACE_24")) {
        return(MONOSPACE_24);
    }
    if(!strcmp(font_name, "MONOSPACE_28")) {
        return(MONOSPACE_28);
    }
    if(!strcmp(font_name, "MONOSPACE_36")) {
        return(MONOSPACE_36);
    }
    if(!strcmp(font_name, "MONOSPACE_48")) {
        return(MONOSPACE_48);
    }
    return(DEFAULT_12);
}
