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

    mcp9808.c

*/

#include <linux/i2c-dev.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "mcp9808.h"

int mcp9808_open(void) {
        ioctl(mcp9808_in, I2C_SLAVE, mcp9808_iic_addr);
        /*
         * Select configuration register(0x01)
         * Continuous conversion mode, Power-up default(0x00, 0x00)
         */
        char config[3] = {0};
        config[0] = 0x01;
        config[1] = 0x00;
        config[2] = 0x00;
        write(mcp9808_in, config, 3);
        /*
         * Select resolution register(0x08)
         * Resolution = +0.0625 / C(0x03)
         */
        config[0] = 0x08;
        config[1] = 0x03;
        write(mcp9808_in, config, 2);
}

float mcp9808_read(void) {
        /*
         * Read 2 bytes of data from register(0x05)
         * temp msb, temp lsb
         */
        char reg[1] = {0x05};
        write(mcp9808_in, reg, 1);
        char data[2] = {0};
        if(read(mcp9808_in, data, 2) != 2) {
            printf("ERROR : MCP9808 Read Error \n");
            exit(1);
        }
        /*
         * Convert the data to 13-bits
         */
        int temp = ((data[0] & 0x1F) * 256 + data[1]);
        if(temp > 4095) {
            temp -= 8192;
        }
        float temperature = temp * 0.0625;
        return(temperature);
}