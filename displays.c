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

extern ssd1681_handle_t gs_handle;

int displays(int (*op)(struct display *, int), struct display *ptr, int cmd) {
    if(op(ptr, cmd)) {
        return(1);
    }
    return(0);

}

int ssd1681(struct display *ptr, int cmd) {

    if(cmd == DISPLAY_OPEN) {
        if(open_ssd1681()) {
            printf("\nERROR: Cannot open ssd1681\n");
            return(1);
        }
printf("%s opened...\n", ptr->name);
    return(0);
    }
    if(cmd == DISPLAY_UPDATE) {
        if(ssd1681_gram_update(&gs_handle, SSD1681_COLOR_BLACK) != 0) {
            ssd1681_interface_debug_print("ssd1681: update failed.\n");
            return(1);
        }
printf("%s updated...\n", ptr->name);
    return(0);
    }
    if(cmd == DISPLAY_TIME) {
        if(ssd1681_gram_write_string(&gs_handle, SSD1681_COLOR_BLACK, ptr->dc[0].xloc, \
            ptr->dc[0].yloc, display_time, (uint16_t)strlen(display_time), 1, SSD1681_MONOSPACE_48) != 0) {
            ssd1681_interface_debug_print("ssd1681: update failed.\n");
            return(1);
        }
printf("%s time cmd...\n", ptr->name);
    return(0);
    }
    if(cmd == DISPLAY_DATE) {
        if(ssd1681_gram_write_string(&gs_handle, SSD1681_COLOR_BLACK, ptr->dc[0].xloc, \
            ptr->dc[0].yloc, display_date, (uint16_t)strlen(display_date), 1, SSD1681_FONT_16) != 0) {
            ssd1681_interface_debug_print("ssd1681: update failed.\n");
            return(1);
        }
printf("%s date cmd...\n", ptr->name);
    return(0);
    }

}

int ssd1306(struct display *ptr, int cmd) {

    if(cmd == DISPLAY_OPEN) {
printf("%s opened...\n", ptr->name);
        return(0);
    }
    if(cmd == DISPLAY_UPDATE) {
printf("%s updated...\n", ptr->name);
        return(0);
    }

    if(cmd == DISPLAY_TIME) {
printf("%s time cmd...\n", ptr->name);
        return(0);
    }

    if(cmd == DISPLAY_DATE) {
printf("%s date cmd...\n", ptr->name);
        return(0);
    }

}