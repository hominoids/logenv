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

   logenv - logs count or time stamp, thermal zone temperature, cpu frequency,
            air temperature(BME280, BMP180, MCP9808),
            volts, amps and watts (HK SmartPower2 or SmartPower3 output)


    void usage (void)
    int16_t itoa(int32_t n, char s[])
    int16_t set_tty_attributes(int16_t fd, int32_t speed, bool canconical)
    void sleep_ms(uint32_t milliseconds)
    static void sig_handler(int _)

*/

#include <arpa/inet.h>
#include <errno.h>
#include <ctype.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <math.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/vfs.h>
#include <termios.h>
#include <threads.h>
#include <time.h>
#include <unistd.h>
#include <cjson/cJSON.h>
#include "drivers/displays.h"
#include "logenv.h"

int main(uint8_t argc, char **argv) {

    signal(SIGINT, sig_handler);

    struct display dp[12];
    memset(dp, 0, sizeof(struct display));
    cJSON *iterator = NULL;

    if(argc == 1) {
        usage();
    }
    /*
     * check if creating gnuplot script or using displays
     */
    uint8_t i = argc;
    while (i-- > 1) {

        if(!strcmp(argv[i], "-g") || !strcmp(argv[i], "--gnuplot")) {
            strcpy(gplotfile, argv[i+1]);

            if((gnuplot_file = fopen(gplotfile, "w")) == NULL) {
                printf("\nERROR: Cannot open gnuplot script file %s\n\n", argv[i+1]);
                usage();
            }
            GNUPLOT_ENABLE = 1;
        }

        /*
         * check for debug output
         */
        if(!strcmp(argv[i], "--debug")) {
            VERBOSE_DEBUG = 1;
        }
        /*
         * check for display and load json configuration
         */
        if(!strcmp(argv[i], "-o")) {
            if((json_file = fopen("./logenv.json", "r")) == NULL) {
                if((json_file = fopen("/etc/logenv/logenv.json", "r")) == NULL) {
                    printf("\nERROR: Cannot open file logenv.json in ./ or /etc/logenv/\n");
                    usage();
                }
            }

            char buffer[16384];
            uint16_t len = fread(buffer, 1, sizeof(buffer), json_file);
            fclose(json_file);
            if(VERBOSE_DEBUG) printf("json file read...\n");

            cJSON *root = cJSON_Parse(buffer);
            if (!cJSON_IsObject(root)) {
                printf("\nERROR: Cannot parse JSON root structure.\n");
                return EXIT_FAILURE;
            }
            if(VERBOSE_DEBUG) printf("json root structure parsed...\n");

            cJSON *display = cJSON_GetObjectItemCaseSensitive(root, "displays");
            cJSON *item = display ? display->child : 0;
            while (item)
            {
                cJSON *name = cJSON_GetObjectItemCaseSensitive(item, "name");
                if (cJSON_IsString(name) && (name->valuestring != NULL)) {
                    strcpy(dp[DISPLAY_ENABLE].name, name->valuestring);
                    if(VERBOSE_DEBUG) printf("Displays: %s ", &dp[DISPLAY_ENABLE].name);
                }
                dp[DISPLAY_ENABLE].dptr = !strcmp(dp[DISPLAY_ENABLE].name,"ssd1681") ? ssd1681 :
                    !strcmp(dp[DISPLAY_ENABLE].name, "ssd1306") ? ssd1306 :
                    !strcmp(dp[DISPLAY_ENABLE].name, "ssh1107") ? ssh1107 :
                    !strcmp(dp[DISPLAY_ENABLE].name, "st7789") ? st7789 : NULL;

                cJSON *device = cJSON_GetObjectItemCaseSensitive(item, "device");
                if (cJSON_IsString(device) && (device->valuestring)) {
                    strcpy(dp[DISPLAY_ENABLE].device, device->valuestring);
                    if(VERBOSE_DEBUG) printf("%s ", &dp[DISPLAY_ENABLE].device);
                }
                cJSON *address = cJSON_GetObjectItemCaseSensitive(item, "address");
                if (cJSON_IsNumber(address)) {
                    dp[DISPLAY_ENABLE].address = address->valueint << 1;
                    if(VERBOSE_DEBUG) printf("%d ", dp[DISPLAY_ENABLE].address);
                }
                cJSON *xsize = cJSON_GetObjectItemCaseSensitive(item, "xsize");
                if (cJSON_IsNumber(xsize)) {
                    dp[DISPLAY_ENABLE].xsize = xsize->valueint;
                    if(VERBOSE_DEBUG) printf("%d ", dp[DISPLAY_ENABLE].xsize);
                }
                cJSON *ysize = cJSON_GetObjectItemCaseSensitive(item, "ysize");
                if (cJSON_IsNumber(ysize)) {
                    dp[DISPLAY_ENABLE].ysize = ysize->valueint;
                    if(VERBOSE_DEBUG) printf("%d ", dp[DISPLAY_ENABLE].ysize);
                }
                cJSON *rotation = cJSON_GetObjectItemCaseSensitive(item, "rotation");
                if (cJSON_IsNumber(rotation)) {
                    dp[DISPLAY_ENABLE].rotation = rotation->valueint;
                    if(VERBOSE_DEBUG) printf("%d ", dp[DISPLAY_ENABLE].rotation);
                }
                cJSON *page = cJSON_GetObjectItemCaseSensitive(item, "page");
                if (cJSON_IsNumber(page)) {
                    dp[DISPLAY_ENABLE].page = page->valueint;
                    pg_count++;
                    if(VERBOSE_DEBUG) printf("%d ", dp[DISPLAY_ENABLE].page);
                }
                cJSON *seconds = cJSON_GetObjectItemCaseSensitive(item, "seconds");
                if (cJSON_IsNumber(seconds)) {
                    dp[DISPLAY_ENABLE].seconds = seconds->valueint;
                    if(VERBOSE_DEBUG) printf("%d\n", dp[DISPLAY_ENABLE].seconds);
                }
                /*
                 * Read display content array entries
                 */
                uint8_t ac = 0;
                cJSON *content = cJSON_GetObjectItemCaseSensitive(item, "content");
                cJSON_ArrayForEach(iterator, content)
                    {

                    cJSON *name = cJSON_GetObjectItemCaseSensitive(iterator, "name");
                    cJSON *device = cJSON_GetObjectItemCaseSensitive(iterator, "device");
                    cJSON *address = cJSON_GetObjectItemCaseSensitive(iterator, "address");
                    cJSON *type = cJSON_GetObjectItemCaseSensitive(iterator, "type");
                    cJSON *xloc = cJSON_GetObjectItemCaseSensitive(iterator, "xloc");
                    cJSON *yloc = cJSON_GetObjectItemCaseSensitive(iterator, "yloc");
                    cJSON *color = cJSON_GetObjectItemCaseSensitive(iterator, "color");
                    cJSON *font = cJSON_GetObjectItemCaseSensitive(iterator, "font");
                    cJSON *label = cJSON_GetObjectItemCaseSensitive(iterator, "label");
                    cJSON *unit = cJSON_GetObjectItemCaseSensitive(iterator, "unit");

                    strcpy(dp[DISPLAY_ENABLE].dc[ac].name, name->valuestring);
                    strcpy(dp[DISPLAY_ENABLE].dc[ac].device, device->valuestring);
                    dp[DISPLAY_ENABLE].dc[ac].address = address->valueint;
                    strcpy(dp[DISPLAY_ENABLE].dc[ac].type, type->valuestring);
                    dp[DISPLAY_ENABLE].dc[ac].xloc = xloc->valueint;
                    dp[DISPLAY_ENABLE].dc[ac].yloc = yloc->valueint;
                    dp[DISPLAY_ENABLE].dc[ac].color = color->valueint;
                    strcpy(dp[DISPLAY_ENABLE].dc[ac].font, font->valuestring);
                    strcpy(dp[DISPLAY_ENABLE].dc[ac].label, label->valuestring);
                    strcpy(dp[DISPLAY_ENABLE].dc[ac].unit, unit->valuestring);

                    if(VERBOSE_DEBUG) {
                        printf("  %s ", &dp[DISPLAY_ENABLE].dc[ac].name);
                        printf("%s ", &dp[DISPLAY_ENABLE].dc[ac].device);
                        printf("%d ", dp[DISPLAY_ENABLE].dc[ac].address);
                        printf("%s ", &dp[DISPLAY_ENABLE].dc[ac].type);
                        printf("%d ", dp[DISPLAY_ENABLE].dc[ac].xloc);
                        printf("%d ", dp[DISPLAY_ENABLE].dc[ac].yloc);
                        printf("%d ", dp[DISPLAY_ENABLE].dc[ac].color);
                        printf("%s ", &dp[DISPLAY_ENABLE].dc[ac].font);
                        printf("%s ", &dp[DISPLAY_ENABLE].dc[ac].label);
                        printf("%s\n", &dp[DISPLAY_ENABLE].dc[ac].unit);
                    }

                    if(!strcmp(dp[DISPLAY_ENABLE].dc[ac].name,"date")) {
                        DP_DATE++;
                    }
                    if(!strcmp(dp[DISPLAY_ENABLE].dc[ac].name,"time")) {
                        DP_TIME++;
                    }
                    if(!strcmp(dp[DISPLAY_ENABLE].dc[ac].name,"frequency")) {
                        DP_FREQ++;
                    }
                    if(!strcmp(dp[DISPLAY_ENABLE].dc[ac].name,"thermal")) {
                        DP_THERMAL++;
                    }
                    if(!strcmp(dp[DISPLAY_ENABLE].dc[ac].name,"governor")) {
                        DP_GOVERNOR++;
                    }
                    if(!strcmp(dp[DISPLAY_ENABLE].dc[ac].name,"memory")) {
                        DP_MEMORY++;
                    }
                    if(!strcmp(dp[DISPLAY_ENABLE].dc[ac].name,"usage")) {
                        DP_USAGE++;
                    }
                    if(!strcmp(dp[DISPLAY_ENABLE].dc[ac].name,"disk")) {
                        DP_DISK++;
                    }
                    if(!strcmp(dp[DISPLAY_ENABLE].dc[ac].name,"uptime")) {
                        DP_UPTIME++;
                    }
                    if(!strcmp(dp[DISPLAY_ENABLE].dc[ac].name,"sysload")) {
                        DP_SYSLOAD++;
                    }
                    if(!strcmp(dp[DISPLAY_ENABLE].dc[ac].name,"sp2")) {
                        DP_SP2++;
                    }
                    if(!strcmp(dp[DISPLAY_ENABLE].dc[ac].name,"sp3-ch1")) {
                        DP_SP3CH1++;
                    }
                    if(!strcmp(dp[DISPLAY_ENABLE].dc[ac].name,"sp3-ch2")) {
                        DP_SP3CH2++;
                    }
                    if(!strcmp(dp[DISPLAY_ENABLE].dc[ac].name,"bmp180")) {
                        strcpy(bmp180_iic_dev, dp[DISPLAY_ENABLE].dc[ac].device);
                        bmp180_iic_addr = dp[DISPLAY_ENABLE].dc[ac].address << 1;
                        if(bmp180_basic_init() != 0) {
                            printf("\nERROR: Cannot open BMP180 at %s\n", interface);
                            exit(1);
                        }
                        bmp180_iic_init = 1;
                        DP_BMP180++;
                    }
                    if(!strcmp(dp[DISPLAY_ENABLE].dc[ac].name,"bmp388")) {
                        strcpy(bmp388_iic_dev, dp[DISPLAY_ENABLE].dc[ac].device);
                        bmp388_iic_addr = dp[DISPLAY_ENABLE].dc[ac].address << 1;
                        if(bmp388_basic_init(BMP388_INTERFACE_IIC, bmp388_iic_addr) != 0) {
                            printf("\nERROR: Cannot open BMP388 at %s\n", interface);
                            exit(1);
                        }
                        bmp388_iic_init = 1;
                        DP_BMP388++;
                    }
                    if(!strcmp(dp[DISPLAY_ENABLE].dc[ac].name,"bmp390")) {
                        strcpy(bmp390_iic_dev, dp[DISPLAY_ENABLE].dc[ac].device);
                        bmp390_iic_addr = dp[DISPLAY_ENABLE].dc[ac].address << 1;
                        if(bmp390_basic_init(BMP390_INTERFACE_IIC, bmp390_iic_addr) != 0) {
                            printf("\nERROR: Cannot open BMP390 at %s\n", interface);
                            exit(1);
                        }
                        bmp390_iic_init = 1;
                        DP_BMP390++;
                    }
                    if(!strcmp(dp[DISPLAY_ENABLE].dc[ac].name,"bme280")) {
                        strcpy(bme280_iic_dev, dp[DISPLAY_ENABLE].dc[ac].device);
                        bme280_iic_addr = dp[DISPLAY_ENABLE].dc[ac].address << 1;
                        if(bme280_basic_init(BME280_INTERFACE_IIC, bme280_iic_addr) != 0) {
                            printf("\nERROR: Cannot open BME280 at %s\n", interface);
                            exit(1);
                        }
                        bme280_iic_init = 1;
                        DP_BME280++;
                    }
                    if(!strcmp(dp[DISPLAY_ENABLE].dc[ac].name,"bme680")) {
                        strcpy(bme680_iic_dev, dp[DISPLAY_ENABLE].dc[ac].device);
                        bme680_iic_addr = dp[DISPLAY_ENABLE].dc[ac].address << 1;
                        if(bme680_gas_init(BME680_INTERFACE_IIC, bme680_iic_addr) != 0) {
                            printf("\nERROR: Cannot open BME680 at %s address %d\n", interface, bme680_iic_addr);
                            exit(1);
                        }
                        bme680_iic_init = 1;
                        DP_BME680++;
                    }
                    if(!strcmp(dp[DISPLAY_ENABLE].dc[ac].name,"mcp9808")) {
                        strcpy(mcp9808_iic_dev, dp[DISPLAY_ENABLE].dc[ac].device);
                        mcp9808_iic_addr = dp[DISPLAY_ENABLE].dc[ac].address;
                        if((mcp9808_in = open(sensor, O_RDWR)) < 0) {
                            printf("\nERROR: Cannot open MCP9808 at %s\n", interface);
                            exit(1);
                        }
                        mcp9808_open();
                        mcp9808_iic_init = 1;
                        DP_MCP9808++;
                    }
                    if(!strcmp(dp[DISPLAY_ENABLE].dc[ac].name,"sht4x")) {
                        strcpy(sht4x_iic_dev, dp[DISPLAY_ENABLE].dc[ac].device);
                        sht4x_iic_addr = dp[DISPLAY_ENABLE].dc[ac].address << 1;
                        if(sht4x_basic_init(sht4x_iic_addr) != 0) {
                            printf("\nERROR: Cannot open SHT4x at %s\n", interface);
                            exit(1);
                        }
                        sht4x_iic_init = 1;
                        DP_SHT4X++;
                    }
                    if(!strcmp(dp[DISPLAY_ENABLE].dc[ac].name,"shtc3")) {
                        strcpy(shtc3_iic_dev, dp[DISPLAY_ENABLE].dc[ac].device);
                        shtc3_iic_addr = dp[DISPLAY_ENABLE].dc[ac].address << 1;
                        if(shtc3_basic_init() != 0) {
                            printf("\nERROR: Cannot open SHTC3 at %s\n", interface);
                            exit(1);
                        }
                        shtc3_iic_init = 1;
                        DP_SHTC3++;
                    }
                    if(!strcmp(dp[DISPLAY_ENABLE].dc[ac].name,"aht20")) {
                        strcpy(aht20_iic_dev, dp[DISPLAY_ENABLE].dc[ac].device);
                        aht20_iic_addr = dp[DISPLAY_ENABLE].dc[ac].address << 1;
                        if(aht20_basic_init() != 0) {
                            printf("\nERROR: Cannot open AHT20 at %s\n", interface);
                            exit(1);
                        }
                        aht20_iic_init = 1;
                        DP_AHT20++;
                    }
                    if(!strcmp(dp[DISPLAY_ENABLE].dc[ac].name,"htu31d")) {
                        strcpy(htu31d_iic_dev, dp[DISPLAY_ENABLE].dc[ac].device);
                        htu31d_iic_addr = dp[DISPLAY_ENABLE].dc[ac].address << 1;
                        if(htu31d_basic_init(htu31d_iic_addr) != 0) {
                            printf("\nERROR: Cannot open HTU31D at %s\n", interface);
                            exit(1);
                        }
                        htu31d_iic_init = 1;
                        DP_HTU31D++;
                    }
                    if(!strcmp(dp[DISPLAY_ENABLE].dc[ac].name,"scd30")) {
                        strcpy(scd30_iic_dev, dp[DISPLAY_ENABLE].dc[ac].device);
                        scd30_iic_addr = dp[DISPLAY_ENABLE].dc[ac].address << 1;
                        if (scd30_basic_init(SCD30_INTERFACE_IIC,0)) {
                            printf("\nERROR: SCD30 Init failed.\n");
                            exit(1);
                        }
                        scd30_iic_init = 1;
                        DP_SCD30++;
                    }
                    if(!strcmp(dp[DISPLAY_ENABLE].dc[ac].name,"scd40") || \
                        !strcmp(dp[DISPLAY_ENABLE].dc[ac].name,"scd41") || \
                            !strcmp(dp[DISPLAY_ENABLE].dc[ac].name,"scd43")) {
                        strcpy(scd4x_iic_dev, dp[DISPLAY_ENABLE].dc[ac].device);
                        scd4x_iic_addr = dp[DISPLAY_ENABLE].dc[ac].address << 1;
                        scd4x_t chip_type;
                        if(!strcmp(dp[DISPLAY_ENABLE].dc[ac].name,"scd40")) {
                            chip_type = SCD40;
                        } 
                        else if (!strcmp(dp[DISPLAY_ENABLE].dc[ac].name,"scd41")) {
                            chip_type = SCD41;
                        }
                        else if (!strcmp(dp[DISPLAY_ENABLE].dc[ac].name,"scd43")) {
                            chip_type = SCD43;
                        }
                        if (scd4x_shot_init(chip_type)) {
                            printf("\nERROR: SCD4x Init failed.\n");
                            exit(1);
                        }
                        scd4x_iic_init = 1;
                        DP_SCD4X++;
                    }
                    if(!strcmp(dp[DISPLAY_ENABLE].dc[ac].name,"sgp30")) {
                        strcpy(sgp30_iic_dev, dp[DISPLAY_ENABLE].dc[ac].device);
                        sgp30_iic_addr = dp[DISPLAY_ENABLE].dc[ac].address << 1;
                        if (sgp30_advance_init()) {
                            printf("\nERROR: SGP30 Init failed.\n");
                            exit(1);
                        }
                        sgp30_iic_init = 1;
                        DP_SGP30++;
                    }
                    if(!strcmp(dp[DISPLAY_ENABLE].dc[ac].name,"text")) {
                        DP_TEXT++;
                    }
                    if(!strcmp(dp[DISPLAY_ENABLE].dc[ac].name,"point")) {
                        DP_POINT++;
                    }
                    if(!strcmp(dp[DISPLAY_ENABLE].dc[ac].name,"line")) {
                        DP_LINE++;
                    }
                    if(!strcmp(dp[DISPLAY_ENABLE].dc[ac].name,"circle")) {
                        DP_CIRCLE++;
                    }
                    if(!strcmp(dp[DISPLAY_ENABLE].dc[ac].name,"rectangle")) {
                        DP_RECTANGLE++;
                    }
                    if(!strcmp(dp[DISPLAY_ENABLE].dc[ac].name,"image")) {
                        DP_IMAGE++;
                    }
                    ac++;
                }
                dp[DISPLAY_ENABLE].dc_count = ac;
                if(!strcmp(dp[DISPLAY_ENABLE].name, "ssd1681") && dp[DISPLAY_ENABLE].page == 0) {
                    strcpy(ssd1681_spi_dev, dp[DISPLAY_ENABLE].device);
                    if(dp[DISPLAY_ENABLE].dptr(&dp[DISPLAY_ENABLE], i, DISPLAY_OPEN)) {
                        printf("%s open failed\n", &dp[DISPLAY_ENABLE].name);
                        exit(0);
                    }
                    if(VERBOSE_DEBUG) printf("Display: %s open.\n", &dp[DISPLAY_ENABLE].name);
                    SSD1681_ENABLE = 1;
                }
                if(!strcmp(dp[DISPLAY_ENABLE].name, "ssd1306") && dp[DISPLAY_ENABLE].page == 0) {
                    if(strchr(dp[DISPLAY_ENABLE].device, 's')) {
                        strcpy(ssd1306_spi_dev, dp[DISPLAY_ENABLE].device);
                    }
                    if(strchr(dp[DISPLAY_ENABLE].device, 'c')) {
                        strcpy(ssd1306_iic_dev, dp[DISPLAY_ENABLE].device);
                    }
                    if(dp[DISPLAY_ENABLE].dptr(&dp[DISPLAY_ENABLE], i, DISPLAY_OPEN)) {
                        printf("%s open failed\n", &dp[DISPLAY_ENABLE].name);
                        exit(0);
                    }
                    if(VERBOSE_DEBUG) printf("Display: %s open.\n", &dp[DISPLAY_ENABLE].name);
                    SSD1306_ENABLE = 1;
                }
                if(!strcmp(dp[DISPLAY_ENABLE].name, "ssh1107") && dp[DISPLAY_ENABLE].page == 0) {
                    if(strchr(dp[DISPLAY_ENABLE].device, 's')) {
                        strcpy(ssh1107_spi_dev, dp[DISPLAY_ENABLE].device);
                    }
                    if(strchr(dp[DISPLAY_ENABLE].device, 'c')) {
                        strcpy(ssh1107_iic_dev, dp[DISPLAY_ENABLE].device);
                    }
                    if(dp[DISPLAY_ENABLE].dptr(&dp[DISPLAY_ENABLE], i, DISPLAY_OPEN)) {
                        printf("%s open failed\n", &dp[DISPLAY_ENABLE].name);
                        exit(0);
                    }
                    if(VERBOSE_DEBUG) printf("Display: %s open.\n", &dp[DISPLAY_ENABLE].name);
                    SSH1107_ENABLE = 1;
                }
                if(!strcmp(dp[DISPLAY_ENABLE].name, "st7789") && dp[DISPLAY_ENABLE].page == 0) {
                    strcpy(st7789_spi_dev, dp[DISPLAY_ENABLE].device);
                    if(dp[DISPLAY_ENABLE].dptr(&dp[DISPLAY_ENABLE], i, DISPLAY_OPEN)) {
                        printf("%s open failed\n", &dp[DISPLAY_ENABLE].name);
                        exit(0);
                    }
                    if(VERBOSE_DEBUG) printf("Display: %s open.\n", &dp[DISPLAY_ENABLE].name);
                    ST7789_ENABLE = 1;
                }
                DISPLAY_ENABLE++;
                if(VERBOSE_DEBUG) printf("Display %d complete...\n\n", DISPLAY_ENABLE);
                item=item->next;
            }
            cJSON_Delete(root);
        }
    }
    /*
     * parse command line options
     */
    i = argc;
    while (i-- > 1) {
        if(!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
            usage();
        }
        if(!strcmp(argv[i], "--version")) {
            printf("\nlogenv version %s\n", version);
            exit(0);
        }
        if(!strcmp(argv[i], "-d") || !strcmp(argv[i], "--date")) {
            DT_ENABLE = 1;
            OPTIONS_COUNT++;
        }
        if(!strcmp(argv[i], "-v") || !strcmp(argv[i], "--verbose")) {
            VERBOSE_ENABLE = 1;
        }
        if(!strcmp(argv[i], "-q") || !strcmp(argv[i], "--quiet")) {
            QUIET_ENABLE = 1;
        }
        if(!strcmp(argv[i], "-u") || !strcmp(argv[i], "--usage") || DP_USAGE != 0) {
            if((cpu_online = fopen(cpuonline, "r")) == NULL) {
                printf("\nERROR: Cannot open %s\n", cpuonline);
                exit(1);
            }
            size_t size = 0;
            char *line = 0;
            ssize_t linesize = getline(&line, &size, cpu_online);
            fclose(cpu_online);
            if(DP_USAGE != 0) {
                DP_USAGE = atoi(&line[2])+1;
            }
            if(!strcmp(argv[i], "-u") || !strcmp(argv[i], "--usage")) {
                USAGE_ENABLE = atoi(&line[2])+1;
                OPTIONS_COUNT++;
            }
        }
        if(!strcmp(argv[i], "-m") || !strcmp(argv[i], "--memory")) {
            MEM_ENABLE = 1;
            OPTIONS_COUNT++;
        }
        if(!strcmp(argv[i], "-i") || !strcmp(argv[i], "--milliseconds")) {
            INTERACTIVE_ENABLE = atoi(argv[i+1]);
            COUNT_ENABLE = 1;
        }
        if(!strcmp(argv[i], "-l") || !strcmp(argv[i], "--log")) {
            strcpy(logfile, argv[i+1]);
            LOG_ENABLE = 1;
        }
        if(!strcmp(argv[i], "-n") || !strcmp(argv[i], "--udp")) {
            if((i+1) < argc && !strncmp(":", argv[i+1], 0)) {
                sscanf(argv[i+1], "%[^':']:%d", udp_name, &udp_port);
            }
            udp_host = (struct hostent *) gethostbyname((char *)udp_name);
            if ((udp_socket = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
                printf("\nERROR: Cannot open UDP socket for %s on port %d\n", udp_name, udp_port);
                exit(1);
            }
            udp_server_addr.sin_family = AF_INET;
            udp_server_addr.sin_port = htons(udp_port);
            udp_server_addr.sin_addr = *((struct in_addr *)udp_host->h_addr);
            memset(&(udp_server_addr.sin_zero), 0, 8);
            sin_size = sizeof(struct sockaddr);
            UDP_ENABLE = 1;
        }
        if(!strcmp(argv[i], "-r") || !strcmp(argv[i], "--raw")) {
            RAW_ENABLE = 1;
        }
        if(!strcmp(argv[i], "--xmtics")) {
            xmtics = atoi(argv[i+1]);
        }
        if(!strcmp(argv[i], "--title")) {
            strcpy(charttitle, argv[i+1]);
        }
        /*
         * CPU frequency command line options
         */
        if(!strcmp(argv[i], "-f") || !strcmp(argv[i], "--frequency") || DP_FREQ >= 1) {
            if((cpu_online = fopen(cpuonline, "r")) == NULL) {
                printf("\nERROR: Cannot open %s\n", cpuonline);
                exit(1);
            }
            size_t size = 0;
            char *line = 0;
            ssize_t linesize = getline(&line, &size, cpu_online);
            fclose(cpu_online);
            FREQ_ENABLE = atoi(&line[2])+1;
            OPTIONS_COUNT++;
        }
        /*
         * thermal zones command line options
         */
        if(!strcmp(argv[i], "-t") || !strcmp(argv[i], "--temperature") || DP_THERMAL >= 1) {

            for (uint16_t c = 0; c <= 1024; c++) {
                char strChar[5] = {0};
                itoa(c,strChar);
                strcpy(thermalzone,thermalzone1);
                strcat(thermalzone,strChar);
                strcat(thermalzone,thermalzone2);
                if((cpu_thermal = fopen(thermalzone, "r")) == NULL) {
                    break;
                }
                fclose(cpu_thermal);
                THERMAL_ENABLE++;
            }
            OPTIONS_COUNT++;
        }
        /*
         * ambient temperature bmp280 command line options
         */
        if(!strcmp(argv[i], "-a") || !strcmp(argv[i], "--bme280")) {
            if(GNUPLOT_ENABLE != 1) {
                if((i+1) < argc && !strncmp("/dev/", argv[i+1], 5)) {
                    strcpy(bme280_iic_dev, interface);
                    interface = argv[i+1];
                }
                if((i+2) < argc && !strncmp("/dev/", argv[i+2], 5)) {
                    interface = argv[i+2];
                    strcpy(bme280_iic_dev, interface);
                }
                if(bme280_iic_init == 0) {
                    if(bme280_basic_init(BME280_INTERFACE_IIC, bme280_iic_addr) != 0) {
                        printf("\nERROR: Cannot open BME280 at %s\n", interface);
                        exit(1);
                    }
                    bme280_iic_init == 1;
                }
            }
            SENSOR_ENABLE = 2;
            OPTIONS_COUNT++;
        }
        /*
         * bme680 command line options
         */
        if(!strcmp(argv[i], "--bme680")) {
            if(GNUPLOT_ENABLE != 1) {
                if((i+1) < argc && !strncmp("/dev/", argv[i+1], 5)) {
                    strcpy(bme680_iic_dev, interface);
                    interface = argv[i+1];
                }
                if((i+2) < argc && !strncmp("/dev/", argv[i+2], 5)) {
                    interface = argv[i+2];
                    strcpy(bme680_iic_dev, interface);
                }
                if(bme680_iic_init == 0) {
                    if(bme680_gas_init(BME680_INTERFACE_IIC, bme680_iic_addr) != 0) {
                        printf("\nERROR: Cannot open BME680 at %s address %d\n", interface, bme680_iic_addr);
                        exit(1);
                    }
                    bme680_iic_init == 1;
                }
            }
            BME680_ENABLE = 1;
            OPTIONS_COUNT++;
        }
        /*
         * bmp180 command line options
         */
        if(!strcmp(argv[i], "--bmp180")) {
            if(GNUPLOT_ENABLE != 1) {
                if((i+1) < argc && !strncmp("/dev/", argv[i+1], 5)) {
                    interface = argv[i+1];
                    strcpy(bmp180_iic_dev, interface);
                }
                if((i+2) < argc && !strncmp("/dev/", argv[i+2], 5)) {
                    interface = argv[i+2];
                    strcpy(bmp180_iic_dev, interface);
                }
                if(bmp180_iic_init == 0) {
                    if(bmp180_basic_init() != 0) {
                        printf("\nERROR: Cannot open BMP180 at %s\n", interface);
                        exit(1);
                    }
                    bmp180_iic_init == 1;
                }
            }
            SENSOR_ENABLE = 1;
            OPTIONS_COUNT++;
        }
        /*
         * bmp388 command line options
         */
        if(!strcmp(argv[i], "-a") || !strcmp(argv[i], "--bmp388")) {
            if(GNUPLOT_ENABLE != 1) {
                if((i+1) < argc && !strncmp("/dev/", argv[i+1], 5)) {
                    interface = argv[i+1];
                    strcpy(bmp388_iic_dev, interface);
                }
                if((i+2) < argc && !strncmp("/dev/", argv[i+2], 5)) {
                    interface = argv[i+2];
                    strcpy(bmp388_iic_dev, interface);
                }
                if(bmp388_iic_init == 0) {
                    if(bmp388_basic_init(BMP388_INTERFACE_IIC, bmp388_iic_addr) != 0) {
                        printf("\nERROR: Cannot open BMP388 at %s\n", interface);
                        exit(1);
                    }
                    bmp388_iic_init == 1;
                }
            }
            BMP388_ENABLE = 1;
            OPTIONS_COUNT++;
        }
        /*
         * bmp390 command line options
         */
        if(!strcmp(argv[i], "-a") || !strcmp(argv[i], "--bmp390")) {
            if(GNUPLOT_ENABLE != 1) {
                if((i+1) < argc && !strncmp("/dev/", argv[i+1], 5)) {
                    interface = argv[i+1];
                    strcpy(bmp390_iic_dev, interface);
                }
                if((i+2) < argc && !strncmp("/dev/", argv[i+2], 5)) {
                    interface = argv[i+2];
                    strcpy(bmp390_iic_dev, interface);
                }
                if(bmp390_iic_init == 0) {
                    if(bmp390_basic_init(BMP390_INTERFACE_IIC, bmp390_iic_addr) != 0) {
                        printf("\nERROR: Cannot open BMP390 at %s\n", interface);
                        exit(1);
                    }
                    bmp390_iic_init == 1;
                }
            }
            BMP390_ENABLE = 1;
            OPTIONS_COUNT++;
        }
        /*
         * mcp9808 command line options
         */
        if(!strcmp(argv[i], "--mcp9808")) {
            if(GNUPLOT_ENABLE != 1) {
                if((i+1) < argc && !strncmp("/dev/", argv[i+1], 5)) {
                    interface = argv[i+1];
                    strcpy(mcp9808_iic_dev, interface);
                }
                if((i+2) < argc && !strncmp("/dev/", argv[i+2], 5)) {
                    interface = argv[i+2];
                    strcpy(mcp9808_iic_dev, interface);
                }
                if(mcp9808_iic_init == 0) {
                    if((mcp9808_in = open(sensor, O_RDWR)) < 0) {
                        printf("\nERROR: Cannot open MCP9808 at %s\n", interface);
                        exit(1);
                    }
                    mcp9808_open();
                    mcp9808_iic_init == 1;
                }
            }
            SENSOR_ENABLE = 3;
            OPTIONS_COUNT++;
        }
        /*
         * sht4x command line options
         */
        if(!strcmp(argv[i], "--sht4x")) {
            if(GNUPLOT_ENABLE != 1) {
                if((i+1) < argc && !strncmp("/dev/", argv[i+1], 5)) {
                    interface = argv[i+1];
                    strcpy(sht4x_iic_dev, interface);
                }
                if((i+2) < argc && !strncmp("/dev/", argv[i+2], 5)) {
                    interface = argv[i+2];
                    strcpy(sht4x_iic_dev, interface);
                }
                if(sht4x_iic_init == 0) {
                    if(sht4x_basic_init(sht4x_iic_addr) != 0) {
                        printf("\nERROR: Cannot open SHT4x at %s\n", interface);
                        exit(1);
                    }
                    sht4x_iic_init == 1;
                }
            }
            SENSOR_ENABLE = 4;
            OPTIONS_COUNT++;
        }
        /*
         * shtc3 command line options
         */
        if(!strcmp(argv[i], "--shtc3")) {
            if(GNUPLOT_ENABLE != 1) {
                if((i+1) < argc && !strncmp("/dev/", argv[i+1], 5)) {
                    interface = argv[i+1];
                    strcpy(shtc3_iic_dev, interface);
                }
                if((i+2) < argc && !strncmp("/dev/", argv[i+2], 5)) {
                    interface = argv[i+2];
                    strcpy(shtc3_iic_dev, interface);
                }
                if(shtc3_iic_init == 0) {
                    if(shtc3_basic_init() != 0) {
                        printf("\nERROR: Cannot open SHTC3 at %s\n", interface);
                        exit(1);
                    }
                    shtc3_iic_init == 1;
                }
            }
            SENSOR_ENABLE = 5;
            OPTIONS_COUNT++;
        }
        /*
         * aht20 command line options
         */
        if(!strcmp(argv[i], "--aht20")) {
            if(GNUPLOT_ENABLE != 1) {
                if((i+1) < argc && !strncmp("/dev/", argv[i+1], 5)) {
                    interface = argv[i+1];
                    strcpy(aht20_iic_dev, interface);
                }
                if((i+2) < argc && !strncmp("/dev/", argv[i+2], 5)) {
                    interface = argv[i+2];
                    strcpy(aht20_iic_dev, interface);
                }
                if(aht20_iic_init == 0) {
                    if(aht20_basic_init() != 0) {
                        printf("\nERROR: Cannot open AHT20 at %s\n", interface);
                        exit(1);
                    }
                    aht20_iic_init == 1;
                }
            }
            SENSOR_ENABLE = 6;
            OPTIONS_COUNT++;
        }
        /*
         * htu31d command line options
         */
        if(!strcmp(argv[i], "--htu31d")) {
            if(GNUPLOT_ENABLE != 1) {
                if((i+1) < argc && !strncmp("/dev/", argv[i+1], 5)) {
                    interface = argv[i+1];
                    strcpy(htu31d_iic_dev, interface);
                }
                if((i+2) < argc && !strncmp("/dev/", argv[i+2], 5)) {
                    interface = argv[i+2];
                    strcpy(htu31d_iic_dev, interface);
                }
                if(htu31d_iic_init == 0) {
                    if(htu31d_basic_init(htu31d_iic_addr) != 0) {
                        printf("\nERROR: Cannot open HTU31D at %s\n", interface);
                        exit(1);
                    }
                    htu31d_iic_init == 1;
                }
            }
            SENSOR_ENABLE = 7;
            OPTIONS_COUNT++;
        }
        /*
         * scd30 command line options
         */
        if(!strcmp(argv[i], "--scd30")) {
            if(GNUPLOT_ENABLE != 1) {
                if((i+1) < argc && !strncmp("/dev/", argv[i+1], 5)) {
                    interface = argv[i+1];
                    strcpy(scd30_iic_dev, interface);
                }
                if((i+2) < argc && !strncmp("/dev/", argv[i+2], 5)) {
                    interface = argv[i+2];
                    strcpy(scd30_iic_dev, interface);
                }
                if(scd30_iic_init == 0) {
                    if (scd30_basic_init(SCD30_INTERFACE_IIC,0)) {
                        printf("\nERROR: SCD30 Init failed.\n");
                        exit(1);
                    }
                    scd30_iic_init == 1;
                }
            }
            SCD30_ENABLE = 1;
            OPTIONS_COUNT++;
        }
        /*
         * scd41 command line options
         */
        if(!strcmp(argv[i], "--scd40") || !strcmp(argv[i], "--scd41") || !strcmp(argv[i], "--scd43")) {
            scd4x_t chip_type;
            if(!strcmp(argv[i], "--scd40")) {
                chip_type = SCD40;
            } 
            else if (!strcmp(argv[i], "--scd41")) {
                chip_type = SCD41;
            }
            else if (!strcmp(argv[i], "--scd43")) {
                chip_type = SCD43;
            }
            if (scd4x_shot_init(chip_type)) {
                printf("\nERROR: SCD4x Init failed.\n");
                exit(1);
            }
            if(GNUPLOT_ENABLE != 1) {
                if((i+1) < argc && !strncmp("/dev/", argv[i+1], 5)) {
                    interface = argv[i+1];
                    strcpy(scd4x_iic_dev, interface);
                }
                if((i+2) < argc && !strncmp("/dev/", argv[i+2], 5)) {
                    interface = argv[i+2];
                    strcpy(scd4x_iic_dev, interface);
                }
                if(scd4x_iic_init == 0) {
                    if (scd4x_shot_init(chip_type)) {
                        printf("\nERROR: SCD41 Init failed.\n");
                        exit(1);
                    }
                    scd4x_iic_init == 1;
                }
            }
            SCD4X_ENABLE = 1;
            OPTIONS_COUNT++;
        }
        /*
         * sgp30 command line options
         */
        if(!strcmp(argv[i], "--sgp30")) {
            if(GNUPLOT_ENABLE != 1) {
                if((i+1) < argc && !strncmp("/dev/", argv[i+1], 5)) {
                    interface = argv[i+1];
                    strcpy(sgp30_iic_dev, interface);
                }
                if((i+2) < argc && !strncmp("/dev/", argv[i+2], 5)) {
                    interface = argv[i+2];
                    strcpy(sgp30_iic_dev, interface);
                }
                if(sgp30_iic_init == 0) {
                    if (sgp30_advance_init()) {
                        printf("\nERROR: SGP30 Init failed.\n");
                        exit(1);
                    }
                    sgp30_iic_init == 1;
                 }
            }
            SGP30_ENABLE = 1;
            OPTIONS_COUNT++;
        }
        /*
         * smartpower options command line options
         */
        if(!strcmp(argv[i], "-p") || !strcmp(argv[i], "--smartpower3-ch1")) {
            if(GNUPLOT_ENABLE != 1) {
                if((i+1) < argc && !strncmp("/dev/", argv[i+1], 5)) {
                    smartpower = argv[i+1];
                }
                if((i+2) < argc && !strncmp("/dev/", argv[i+2], 5)) {
                    smartpower = argv[i+2];
                }
                if((pwr_in = open(smartpower, O_RDONLY | O_NOCTTY | O_SYNC)) < 0) {
                    printf("\nERROR: Cannot open SmartPower at %s\n\n", smartpower);
                    exit(1);
                }
                set_tty_attributes(pwr_in, B115200, true);
                close(pwr_in);
            }
            SP_ENABLE = 31;
            OPTIONS_COUNT++;
        }
        if(!strcmp(argv[i], "--smartpower3-ch2")) {
            if(GNUPLOT_ENABLE != 1) {
                if((i+1) < argc && !strncmp("/dev/", argv[i+1], 5)) {
                    smartpower = argv[i+1];
                }
                if((i+2) < argc && !strncmp("/dev/", argv[i+2], 5)) {
                    smartpower = argv[i+2];
                }
                if((pwr_in = open(smartpower, O_RDONLY | O_NOCTTY | O_SYNC)) < 0) {
                    printf("\nERROR: Cannot open SmartPower at %s\n\n", smartpower);
                    exit(1);
                }
                set_tty_attributes(pwr_in, B115200, true);
                close(pwr_in);
            }
            SP_ENABLE = 32;
            OPTIONS_COUNT++;
        }
        if(!strcmp(argv[i], "--smartpower2")) {
            if(GNUPLOT_ENABLE != 1) {
                if((i+1) < argc && !strncmp("/dev/", argv[i+1], 5)) {
                    smartpower = argv[i+1];
                }
                if((i+2) < argc && !strncmp("/dev/", argv[i+2], 5)) {
                    smartpower = argv[i+2];
                }
                if((pwr_in = open(smartpower, O_RDONLY | O_NOCTTY | O_SYNC)) < 0) {
                    printf("\nERROR: Cannot open SmartPower at %s\n\n", smartpower);
                    exit(1);
                }
                set_tty_attributes(pwr_in, B115200, false);
                close(pwr_in);
           }
           SP_ENABLE = 2;
           OPTIONS_COUNT++;
        }
    }
    if (GNUPLOT_ENABLE == 0) {
        /*
         * primary poll loop
         */
        double i = 0;
        int8_t c = OPTIONS_COUNT;

        if(VERBOSE_DEBUG) printf("Primary Polling Loop\n");

        while(i >= 0 && go != -1) {
            int8_t udp_count = 0;
            /*
             * count or date and time stamp
             */
            if(QUIET_ENABLE == 0 && VERBOSE_ENABLE == 0 && DT_ENABLE == 0 && COUNT_ENABLE == 1) {
                printf("%.3f", i/1000);
            }
            if(QUIET_ENABLE == 0 && VERBOSE_ENABLE == 0 && DT_ENABLE == 1 && COUNT_ENABLE == 1) {
                now = time((time_t *)NULL);
                t = localtime(&now);
                printf("%4d-%02d-%02d %02d:%02d:%02d", t->tm_year+1900, t->tm_mon+1, \
                        t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
            }
            if(QUIET_ENABLE == 0 && VERBOSE_ENABLE == 0 && DT_ENABLE == 1 && COUNT_ENABLE == 0) {
                now = time((time_t *)NULL);
                t = localtime(&now);
                if(OPTIONS_COUNT != 0) {
                    printf("%4d-%02d-%02d %02d:%02d:%02d,", t->tm_year+1900, t->tm_mon+1, \
                        t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
                }
                else {
                    printf("%4d-%02d-%02d %02d:%02d:%02d", t->tm_year+1900, t->tm_mon+1, \
                        t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
                }
                OPTIONS_COUNT--;
            }
            if(DP_TIME  != 0 || DP_DATE  != 0){
                for(uint8_t d = 0; d <= DISPLAY_ENABLE-1; d++) {
                    for(uint8_t i = 0; i <= dp[d].dc_count-1; i++) {
                        if(DISPLAY_ENABLE != 0 && DP_TIME != 0 && !strcmp(dp[d].dc[i].name, "time") && dp[d].page == page) {

                            uint16_t count = 0;
                            now = time((time_t *)NULL);
                            t = localtime(&now);

                            if(!strcmp(dp[d].dc[i].type, "24")) {
                                count = sprintf(display_time,"%02d:%02d",t->tm_hour, t->tm_min);
                            }
                            else if(!strcmp(dp[d].dc[i].type, "12")) {
                                count = strftime(display_time,sizeof(display_time),"%I:%M",t);
                            }
                            else {
                                count = strftime(display_time,sizeof(display_time),"%-I:%M %p",t);
                            }
                            if(dp[d].dptr(&dp[d], i, DISPLAY_TIME)){
                                printf("%s time failed\n", &dp[d].name);
                            }
                        }
                        if(DISPLAY_ENABLE  != 0 && DP_DATE  != 0 && !strcmp(dp[d].dc[i].name, "date")) {

                            uint16_t count = 0;
                            now = time((time_t *)NULL);
                            t = localtime(&now);

                            if(!strcmp(dp[d].dc[i].type, "short")) {
                                count = strftime(display_date,sizeof(display_date),"%a %d-%b-%y",t);
                            }
                            else if(!strcmp(dp[d].dc[i].type, "long")) {
                                count = strftime(display_date,sizeof(display_date),"%A %d %B %Y",t);
                            }
                            else {
                                count = sprintf(display_date,"%02d/%02d/%4d", t->tm_mon+1, t->tm_mday, t->tm_year+1900);
                            }
                            if(dp[d].page == page) {
                                if(dp[d].dptr(&dp[d], i, DISPLAY_DATE)){
                                    printf("%s date failed\n", i, &dp[d].name);
                                }
                            }
                        }
                    }
                }
            }
            if(LOG_ENABLE == 1) {
                if((log_file = fopen(logfile, "a")) == NULL) {
                    printf("\nERROR: Cannot open %s\n\n", logfile);
                exit(1);
                }
                if(COUNT_ENABLE && DT_ENABLE == 0) {
                    fprintf(log_file,"%.3f", i/1000);
                }
                else {
                    now = time((time_t *)NULL);
                    t = localtime(&now);
                    fprintf(log_file,"%4d-%02d-%02d %02d:%02d:%02d", t->tm_year+1900, \
                            t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
                }
            }
            if(UDP_ENABLE == 1) {
                if(COUNT_ENABLE && DT_ENABLE == 0) {
                    udp_count = sprintf(udp_tx_data,"%.3f", i/1000);
                }
                if(DT_ENABLE == 1) {
                    now = time((time_t *)NULL);
                    t = localtime(&now);
                    if(COUNT_ENABLE >= 1 && OPTIONS_COUNT > 1) {
                        udp_count = sprintf(udp_tx_data,"%4d-%02d-%02d %02d:%02d:%02d", t->tm_year+1900, \
                                t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
                    }
                    if(COUNT_ENABLE == 0 && OPTIONS_COUNT == 1) {
                        udp_count = sprintf(udp_tx_data,"%4d-%02d-%02d %02d:%02d:%02d", t->tm_year+1900, \
                                t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
                    }
                    if(COUNT_ENABLE == 0 && OPTIONS_COUNT > 1) {
                        udp_count = sprintf(udp_tx_data,"%4d-%02d-%02d %02d:%02d:%02d,", t->tm_year+1900, \
                                t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
                    }
                }
            }
            /*
             * open and read each core frequency file
             */
            if(FREQ_ENABLE != 0) {
                if(QUIET_ENABLE == 0 && VERBOSE_ENABLE == 1) {
                    printf("\n");
                    for (uint16_t c = 0; c < FREQ_ENABLE; c++) {
                        printf("  Core%d  ", c);
                    }
                    printf("\n");
                }
                for (uint16_t c = 0; c < FREQ_ENABLE; c++) {
                    char strChar[5] = {0};
                    itoa(c,strChar);
                    strcpy(cpufreq,cpufreq1);
                    strcat(cpufreq,strChar);
                    strcat(cpufreq,cpufreq2);
                    if((cpu_freq = fopen(cpufreq, "r")) == NULL) {
                        printf("\nERROR: Cannot open %s\n", cpufreq);
                        exit(1);
                    } 
                    fscanf(cpu_freq, "%d", &freq);
                    fclose(cpu_freq);
                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 1) {
                        printf(",%d", freq);
                    }
                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 0 && VERBOSE_ENABLE == 1) {
                        printf(" %.2lfGHz ", (double)freq/1000000);
                        if(c == FREQ_ENABLE - 1) {
                            printf("\n");
                        }
                    }
                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 0 && COUNT_ENABLE == 1 && VERBOSE_ENABLE == 0) {
                            printf(",%.2lf", (double)freq/1000000);
                    }
                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 0 && COUNT_ENABLE == 0 && VERBOSE_ENABLE == 0) {
                        if(OPTIONS_COUNT >= 1 && c < FREQ_ENABLE-1) {
                            printf("%.2lf,", (double)freq/1000000);
                        }
                        if(OPTIONS_COUNT >= 1 && c == FREQ_ENABLE-1) {
                            if(OPTIONS_COUNT > 1) {
                                printf("%.2lf,", (double)freq/1000000);
                            }
                            else {
                                printf("%.2lf", (double)freq/1000000);
                            }
                        }
                    }
                    if(LOG_ENABLE == 1 && RAW_ENABLE == 1) {
                        fprintf(log_file,",%d", freq);
                    }
                    if(LOG_ENABLE == 1 && RAW_ENABLE == 0) {
                        fprintf(log_file,",%.2lf", (double)freq/1000000);
                    }
                    if(UDP_ENABLE == 1 && RAW_ENABLE == 1 && COUNT_ENABLE == 1) {
                        udp_count += sprintf(udp_tx_data + udp_count,",%d", freq);
                    }
                    if(UDP_ENABLE == 1 && RAW_ENABLE == 1 && COUNT_ENABLE == 0) {
                        if(OPTIONS_COUNT >= 1 && c < FREQ_ENABLE-1) {
                            udp_count += sprintf(udp_tx_data + udp_count,"%d,", freq);
                        }
                        if(OPTIONS_COUNT >= 1 && c == FREQ_ENABLE-1) {
                            if(OPTIONS_COUNT > 1) {
                                udp_count += sprintf(udp_tx_data + udp_count,"%d,", freq);
                            }
                            else {
                                udp_count += sprintf(udp_tx_data + udp_count,"%d", freq);
                            }
                        }
                    }
                    if(UDP_ENABLE == 1 && RAW_ENABLE == 0 && COUNT_ENABLE == 1) {
                        udp_count += sprintf(udp_tx_data + udp_count,",%.2lf", (double)freq/1000000);
                    }
                    if(UDP_ENABLE == 1 && RAW_ENABLE == 0 && COUNT_ENABLE == 0) {
                        if(OPTIONS_COUNT >= 1 && c < FREQ_ENABLE-1) {
                            udp_count += sprintf(udp_tx_data + udp_count,"%.2lf,", (double)freq/1000000);
                        }
                        if(OPTIONS_COUNT >= 1 && c == FREQ_ENABLE-1) {
                            if(OPTIONS_COUNT > 1) {
                                udp_count += sprintf(udp_tx_data + udp_count,"%.2lf,", (double)freq/1000000);
                            }
                            else {
                                udp_count += sprintf(udp_tx_data + udp_count,"%.2lf", (double)freq/1000000);
                            }
                        }
                    }
                }
                OPTIONS_COUNT--;
            }
            if(DP_FREQ != 0) {
                for (uint16_t c = 0; c < FREQ_ENABLE; c++) {
                    char strChar[5] = {0};
                    itoa(c,strChar);
                    strcpy(cpufreq,cpufreq1);
                    strcat(cpufreq,strChar);
                    strcat(cpufreq,cpufreq2);
                    if((cpu_freq = fopen(cpufreq, "r")) == NULL) {
                        printf("\nERROR: Cannot open %s\n", cpufreq);
                        exit(1);
                    } 
                    fscanf(cpu_freq, "%d", &freq);
                    fclose(cpu_freq);

                    for(uint8_t d = 0; d <= DISPLAY_ENABLE-1; d++) {
                        for(uint8_t i = 0; i <= dp[d].dc_count-1; i++) {
                            if(!strcmp(dp[d].dc[i].name, "frequency") && dp[d].page == page) {

                                uint16_t yloc_reset = dp[d].dc[i].yloc;
                                char buffer[25];

                                sprintf(buffer, "core%d ", c);
                                strcpy(dp[d].dc[i].data1, buffer);
                                sprintf(buffer, "%.2lf", (double)freq/1000000);
                                strcpy(dp[d].dc[i].data2, buffer);
                                dp[d].dc[i].yloc =  dp[d].dc[i].yloc + c*(fontoi(dp[d].dc[i].font));

                                if(dp[d].dptr(&dp[d], i, DISPLAY_WRITE)){
                                    printf("%s thermal cmd %d failed\n", &dp[d].name, i);
                                }
                                dp[d].dc[i].yloc = yloc_reset;
                            }
                        }
                    }
                }
            }
            /*
             * open and read each thermal zone file
             */
            if(THERMAL_ENABLE != 0) {
                for (uint16_t c = 0; c < THERMAL_ENABLE; c++) {
                    char strChar[5] = {0};
                    itoa(c,strChar);
                    strcpy(thermalzone,thermalzone1);
                    strcat(thermalzone,strChar);
                    strcat(thermalzone,thermalzone2);
                    if((cpu_thermal = fopen(thermalzone, "r")) == NULL) {
                        break;
                    }
                    fscanf(cpu_thermal, "%f", &coretemp);
                    fclose(cpu_thermal);

                    strcpy(thermaltype,thermalzone1);
                    strcat(thermaltype,strChar);
                    strcat(thermaltype,thermaltype1);
                    if((thermal_type = fopen(thermaltype, "r")) == NULL) {
                        break;
                    }
                    fscanf(thermal_type, "%s", thermalname);
                    fclose(thermal_type);
                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 1) {
                        printf(",%.0f", coretemp);
                    }
                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 0 && VERBOSE_ENABLE == 1) {
                        printf("\n %s = %.2fc", thermalname, coretemp/1000);
                    }
                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 0 && COUNT_ENABLE == 1 && VERBOSE_ENABLE == 0) {
                            printf(",%.2f", coretemp/1000);
                    }
                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 0 && COUNT_ENABLE == 0 && VERBOSE_ENABLE == 0) {
                        if(OPTIONS_COUNT >= 1 && c < THERMAL_ENABLE-1) {
                            printf("%.2f,", coretemp/1000);
                        }
                        if(OPTIONS_COUNT >= 1 && c == THERMAL_ENABLE-1) {
                            if(OPTIONS_COUNT > 1) {
                                printf("%.2f,", coretemp/1000);
                            }
                            else {
                                printf("%.2f", coretemp/1000);
                            }
                        }
                    }

                    if(LOG_ENABLE == 1 && RAW_ENABLE == 1) {
                        fprintf(log_file,",%f", coretemp);
                    }
                    if(LOG_ENABLE == 1 && RAW_ENABLE == 0) {
                        fprintf(log_file,",%.2f", coretemp/1000);
                    }
                    if(UDP_ENABLE == 1 && RAW_ENABLE == 1 && COUNT_ENABLE == 1) {
                        udp_count += sprintf(udp_tx_data + udp_count,",%f", coretemp);
                    }
                    if(UDP_ENABLE == 1 && RAW_ENABLE == 1 && COUNT_ENABLE == 0) {
                        if(OPTIONS_COUNT >= 1 && c < THERMAL_ENABLE-1) {
                            udp_count += sprintf(udp_tx_data + udp_count,"%f,", coretemp);
                        }
                        if(OPTIONS_COUNT >= 1 && c == THERMAL_ENABLE-1) {
                            if(OPTIONS_COUNT > 1) {
                                udp_count += sprintf(udp_tx_data + udp_count,"%f,", coretemp);
                            }
                            else {
                                udp_count += sprintf(udp_tx_data + udp_count,"%f", coretemp);
                            }
                        }
                    }
                    if(UDP_ENABLE == 1 && RAW_ENABLE == 0 && COUNT_ENABLE == 1) {
                        udp_count += sprintf(udp_tx_data + udp_count,",%.2f", coretemp/1000);
                    }
                    if(UDP_ENABLE == 1 && RAW_ENABLE == 0 && COUNT_ENABLE == 0) {
                        if(OPTIONS_COUNT >= 1 && c < THERMAL_ENABLE-1) {
                            udp_count += sprintf(udp_tx_data + udp_count,"%.2f,", coretemp/1000);
                        }
                        if(OPTIONS_COUNT >= 1 && c == THERMAL_ENABLE-1) {
                            if(OPTIONS_COUNT > 1) {
                                udp_count += sprintf(udp_tx_data + udp_count,"%.2f,", coretemp/1000);
                            }
                            else {
                                udp_count += sprintf(udp_tx_data + udp_count,"%.2f", coretemp/1000);
                            }
                        }
                    }
                }
                OPTIONS_COUNT--;
            }
            if(DP_THERMAL != 0) {
                for (uint16_t c = 0; c < THERMAL_ENABLE; c++) {
                    char strChar[5] = {0};
                    itoa(c,strChar);
                    strcpy(thermalzone,thermalzone1);
                    strcat(thermalzone,strChar);
                    strcat(thermalzone,thermalzone2);
                    if((cpu_thermal = fopen(thermalzone, "r")) == NULL) {
                        break;
                    }
                    fscanf(cpu_thermal, "%f", &coretemp);
                    fclose(cpu_thermal);

                    strcpy(thermaltype,thermalzone1);
                    strcat(thermaltype,strChar);
                    strcat(thermaltype,thermaltype1);
                    if((thermal_type = fopen(thermaltype, "r")) == NULL) {
                        break;
                    }
                    fscanf(thermal_type, "%s", thermalname);
                    fclose(thermal_type);

                    for(uint8_t d = 0; d <= DISPLAY_ENABLE-1; d++) {
                        for(uint8_t i = 0; i <= dp[d].dc_count-1; i++) {
                            if(!strcmp(dp[d].dc[i].name, "thermal") && dp[d].page == page) {

                                uint16_t yloc_reset = dp[d].dc[i].yloc;
                                char buffer[25];

                                sprintf(buffer, "%s", thermalname);
                                strcpy(dp[d].dc[i].data1, buffer);
                                sprintf(buffer, "%.2lf", coretemp/1000);
                                strcpy(dp[d].dc[i].data2, buffer);
                                dp[d].dc[i].yloc =  dp[d].dc[i].yloc + c*(fontoi(dp[d].dc[i].font));

                                if(dp[d].dptr(&dp[d], i, DISPLAY_WRITE)){
                                    printf("%s thermal cmd %d failed\n", &dp[d].name, i);
                                }
                                dp[d].dc[i].yloc = yloc_reset;
                            }
                        }
                    }
                }
            }
            /*
             * open and read governor setting
             */
            if(DP_GOVERNOR != 0) {
                for(uint8_t d = 0; d <= DISPLAY_ENABLE-1; d++) {
                    for(uint8_t i = 0; i <= dp[d].dc_count-1; i++) {
                        if(!strcmp(dp[d].dc[i].name, "governor") && dp[d].page == page) {

                            strcpy(governorloc, dp[d].dc[i].device);
                            if((governor_file = fopen(governorloc, "r")) == NULL) {
                                printf("\nERROR: Cannot open governor at %s\n", governorloc);
                                break;
                            }
                            fscanf(governor_file, "%s", &governor);
                            fclose(governor_file);

                            strcpy(dp[d].dc[i].data1, governor);
                            if(dp[d].dptr(&dp[d], i, DISPLAY_WRITE)){
                                printf("%s governor cmd %d failed\n", &dp[d].name, i);
                            }
                        }
                    }
                }
            }
            /*
             * open and read disk info
             */
            if(DP_DISK != 0) {
                for(uint8_t d = 0; d <= DISPLAY_ENABLE-1; d++) {
                    for(uint8_t i = 0; i <= dp[d].dc_count-1; i++) {
                        if(!strcmp(dp[d].dc[i].name, "disk") && dp[d].page == page) {

                            struct statfs stat;
                            char buffer[25] = {0};

                            if(statfs(dp[d].dc[i].device, &stat) != 0) {
                                printf("%s cmd %d failed, path %s not found.\n", &dp[d].name, i, dp[d].dc[i].device);
                                break;;
                            }

                            uint64_t dsize = (stat.f_bsize * stat.f_blocks)/1000000000;
                            uint64_t davail = (stat.f_bsize * stat.f_bavail)/1000000000;
                            uint64_t dused = (stat.f_bsize * (stat.f_blocks-stat.f_bavail))/1000000000;
                            float pused = (float) ((dused/dsize) * 100);
//printf("dsize=%d\n", dsize);
//printf("davail=%d\n", davail);
//printf("dused=%d\n", dused);
//printf("pused=%.2f\n", pused);

                            if(!strcmp(dp[d].dc[i].type, "free")) {
                                sprintf(buffer, "%d", davail);
                                strcpy(dp[d].dc[i].data1, buffer);
                            }
                            else if(!strcmp(dp[d].dc[i].type, "used")) {
                                sprintf(buffer, "%d", dused);
                                strcpy(dp[d].dc[i].data1, buffer);
                            }
                            else if(!strcmp(dp[d].dc[i].type, "percent")) {
                                sprintf(buffer, "%.0f", pused);
                                strcpy(dp[d].dc[i].data1, buffer);
                            }
                            if(dp[d].dptr(&dp[d], i, DISPLAY_WRITE)){
                                printf("%s disk cmd %d failed\n", &dp[d].name, i);
                            }
                        }
                    }
                }
            }
            /*
             * sysinfo uptime
             */
            if(DP_UPTIME != 0) {
                for(uint8_t d = 0; d <= DISPLAY_ENABLE-1; d++) {
                    for(uint8_t i = 0; i <= dp[d].dc_count-1; i++) {
                        if(!strcmp(dp[d].dc[i].name, "uptime") && dp[d].page == page) {

                            struct sysinfo sys_info;
                            char buffer[128] = {0};

                            if(sysinfo(&sys_info) != 0) {
                                printf("%s cmd %d sysinfo failed\n", &dp[d].name, i);
                                break;;
                            }

                            int16_t days = sys_info.uptime / 86400;
                            int16_t hours = (sys_info.uptime / 3600) - (days * 24);
                            int16_t mins = (sys_info.uptime / 60) - (days * 1440) - (hours * 60);

                            if(!strcmp(dp[d].dc[i].type, "short")) {
                                sprintf(buffer, "%d days %d:%d", days, hours, mins);
                                strcpy(dp[d].dc[i].data1, buffer);
                            }
                            else if(!strcmp(dp[d].dc[i].type, "long")) {
                                sprintf(buffer, "%ddays, %dhours, %dminutes", days, hours, mins);
                                strcpy(dp[d].dc[i].data1, buffer);
                            }
                            if(dp[d].dptr(&dp[d], i, DISPLAY_WRITE)){
                                printf("%s uptime cmd %d failed\n", &dp[d].name, i);
                            }
                        }
                    }
                }
            }
            /*
             * sysinfo system load
             */
            if(DP_SYSLOAD != 0) {
                for(uint8_t d = 0; d <= DISPLAY_ENABLE-1; d++) {
                    for(uint8_t i = 0; i <= dp[d].dc_count-1; i++) {
                        if(!strcmp(dp[d].dc[i].name, "sysload") && dp[d].page == page) {

                            struct sysinfo sys_info;
                            char buffer[128] = {0};

                            if(sysinfo(&sys_info) != 0) {
                                printf("%s cmd %d sysinfo failed\n", &dp[d].name, i);
                                break;;
                            }

                            if(!strcmp(dp[d].dc[i].type, "short")) {
                                sprintf(buffer, "[%ld] [%ld] [%ld]", sys_info.loads[0], sys_info.loads[1], sys_info.loads[2]);
                                strcpy(dp[d].dc[i].data1, buffer);
                            }
                            else if(!strcmp(dp[d].dc[i].type, "long")) {
                                sprintf(buffer, "1min(%ld) 5min(%ld) 15min(%ld)", sys_info.loads[0], sys_info.loads[1], sys_info.loads[2]);
                                strcpy(dp[d].dc[i].data1, buffer);
                            }
                            if(dp[d].dptr(&dp[d], i, DISPLAY_WRITE)){
                                printf("%s sysload cmd %d failed\n", &dp[d].name, i);
                            }
                        }
                    }
                }
            }
            /*
             * read bmp180
             */
            if(SENSOR_ENABLE == 1 || DP_BMP180 != 0) {

                float temperature_f;
                uint32_t pressure;

                uint8_t res = bmp180_basic_read((float *)&temperature_f, (uint32_t *)&pressure);
                if (res != 0) {
                    bmp180_basic_deinit();
                    printf("ERROR: bmp180 read failed.\n");
                    return 1;
                }

                if(SENSOR_ENABLE == 1) {

                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 1) {
                        printf(",%f", temperature_f);
                    }
                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 0 && COUNT_ENABLE == 1 && VERBOSE_ENABLE == 1) {
                        printf("\n\n BMP180 Sensor = %.2lf c\n", temperature_f);
                        printf("        Pres = %d hPa\n", pressure/100);
                    }
                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 0 && COUNT_ENABLE == 1 && VERBOSE_ENABLE == 0) {
                        printf(",%.2lf", temperature_f);
                    }
                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 0 && COUNT_ENABLE == 0 && VERBOSE_ENABLE == 0) {
                        if(OPTIONS_COUNT > 1) {
                            printf("%.2lf,", temperature_f);
                        }
                        else {
                            printf("%.2lf,", temperature_f);
                        }
                    }
                    if(UDP_ENABLE == 1 && RAW_ENABLE == 1 && COUNT_ENABLE == 1) {
                        udp_count += sprintf(udp_tx_data + udp_count,",%f", temperature_f);
                    }
                    if(UDP_ENABLE == 1 && RAW_ENABLE == 1 && COUNT_ENABLE == 0) {
                        if(OPTIONS_COUNT > 1) {
                            udp_count += sprintf(udp_tx_data + udp_count,"%f,", temperature_f);
                        }
                        else {
                            udp_count += sprintf(udp_tx_data + udp_count,"%f", temperature_f);
                        }
                    }
                    if(UDP_ENABLE == 1 && RAW_ENABLE == 0 && COUNT_ENABLE == 1) {
                        udp_count += sprintf(udp_tx_data + udp_count,",%.2lf", temperature_f);
                    }
                    if(UDP_ENABLE == 1 && RAW_ENABLE == 0 && COUNT_ENABLE == 0) {
                       if(OPTIONS_COUNT > 1) {
                            udp_count += sprintf(udp_tx_data + udp_count,"%.2lf,", temperature_f);
                        }
                        else {
                            udp_count += sprintf(udp_tx_data + udp_count,"%.2lf", temperature_f);
                        }
                    }
                    OPTIONS_COUNT--;
                }
                if(DP_BMP180 != 0) {

                    for(uint8_t d = 0; d <= DISPLAY_ENABLE-1; d++) {
                        for(uint8_t i = 0; i <= dp[d].dc_count-1; i++) {
                            if(!strcmp(dp[d].dc[i].name, "bmp180") && dp[d].page == page) {

                                char buffer[25];

                                sprintf(buffer, "%.1lf", temperature_f);
                                strcpy(dp[d].dc[i].data1, buffer);
                                sprintf(buffer, "%.0lf", (double) pressure/100);
                                strcpy(dp[d].dc[i].data3, buffer);

                                if(dp[d].dptr(&dp[d], i, DISPLAY_SENSOR)){
                                    printf("%s bmp180 cmd %d failed\n", &dp[d].name, i);
                                }
                            }
                        }
                    }
                }
            }
            /*
             * read bmp388
             */
            if(BMP388_ENABLE == 1 || DP_BMP388 != 0) {

                float temperature_f;
                float pressure;

                uint8_t res = bmp388_basic_read((float *)&temperature_f, (float *)&pressure);
                if (res != 0) {
                    bmp388_basic_deinit();
                    printf("ERROR: bmp388 read failed.\n");
                    return 1;
                }

                if(BMP388_ENABLE == 1) {

                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 1) {
                        printf(",%f", temperature_f);
                    }
                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 0 && COUNT_ENABLE == 1 && VERBOSE_ENABLE == 1) {
                        printf("\n BMP388 Sensor = %.2lf c\n", temperature_f);
                        printf("        Pres = %.2lf hPa\n", pressure/100);
                    }
                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 0 && COUNT_ENABLE == 1 && VERBOSE_ENABLE == 0) {
                        printf(",%.2lf", temperature_f);
                    }
                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 0 && COUNT_ENABLE == 0 && VERBOSE_ENABLE == 0) {
                        if(OPTIONS_COUNT > 1) {
                            printf("%.2lf,", temperature_f);
                        }
                        else {
                            printf("%.2lf,", temperature_f);
                        }
                    }
                    if(UDP_ENABLE == 1 && RAW_ENABLE == 1 && COUNT_ENABLE == 1) {
                        udp_count += sprintf(udp_tx_data + udp_count,",%f", temperature_f);
                    }
                    if(UDP_ENABLE == 1 && RAW_ENABLE == 1 && COUNT_ENABLE == 0) {
                        if(OPTIONS_COUNT > 1) {
                            udp_count += sprintf(udp_tx_data + udp_count,"%f,", temperature_f);
                        }
                        else {
                            udp_count += sprintf(udp_tx_data + udp_count,"%f", temperature_f);
                        }
                    }
                    if(UDP_ENABLE == 1 && RAW_ENABLE == 0 && COUNT_ENABLE == 1) {
                        udp_count += sprintf(udp_tx_data + udp_count,",%.2lf", temperature_f);
                    }
                    if(UDP_ENABLE == 1 && RAW_ENABLE == 0 && COUNT_ENABLE == 0) {
                       if(OPTIONS_COUNT > 1) {
                            udp_count += sprintf(udp_tx_data + udp_count,"%.2lf,", temperature_f);
                        }
                        else {
                            udp_count += sprintf(udp_tx_data + udp_count,"%.2lf", temperature_f);
                        }
                    }
                    OPTIONS_COUNT--;
                }
                if(DP_BMP388 != 0) {

                    for(uint8_t d = 0; d <= DISPLAY_ENABLE-1; d++) {
                        for(uint8_t i = 0; i <= dp[d].dc_count-1; i++) {
                            if(!strcmp(dp[d].dc[i].name, "bmp388") && dp[d].page == page) {

                                char buffer[25];

                                sprintf(buffer, "%.1lf", temperature_f);
                                strcpy(dp[d].dc[i].data1, buffer);
                                sprintf(buffer, "%.2lf", (double) pressure/100);
                                strcpy(dp[d].dc[i].data3, buffer);

                                if(dp[d].dptr(&dp[d], i, DISPLAY_SENSOR)){
                                    printf("%s bmp388 cmd %d failed\n", &dp[d].name, i);
                                }
                            }
                        }
                    }
                }
            }
            /*
             * read bmp390
             */
            if(BMP390_ENABLE == 1 || DP_BMP390 != 0) {

                float temperature_f;
                float pressure;

                uint8_t res = bmp390_basic_read((float *)&temperature_f, (float *)&pressure);
                if (res != 0) {
                    bmp390_basic_deinit();
                    printf("ERROR: bmp390 read failed.\n");
                    return 1;
                }

                if(BMP390_ENABLE == 1) {

                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 1) {
                        printf(",%f", temperature_f);
                    }
                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 0 && COUNT_ENABLE == 1 && VERBOSE_ENABLE == 1) {
                        printf("\n BMP390 Sensor = %.2lf c\n", temperature_f);
                        printf("        Pres = %.2lf hPa\n", pressure/100);
                    }
                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 0 && COUNT_ENABLE == 1 && VERBOSE_ENABLE == 0) {
                        printf(",%.2lf", temperature_f);
                    }
                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 0 && COUNT_ENABLE == 0 && VERBOSE_ENABLE == 0) {
                        if(OPTIONS_COUNT > 1) {
                            printf("%.2lf,", temperature_f);
                        }
                        else {
                            printf("%.2lf,", temperature_f);
                        }
                    }
                    if(UDP_ENABLE == 1 && RAW_ENABLE == 1 && COUNT_ENABLE == 1) {
                        udp_count += sprintf(udp_tx_data + udp_count,",%f", temperature_f);
                    }
                    if(UDP_ENABLE == 1 && RAW_ENABLE == 1 && COUNT_ENABLE == 0) {
                        if(OPTIONS_COUNT > 1) {
                            udp_count += sprintf(udp_tx_data + udp_count,"%f,", temperature_f);
                        }
                        else {
                            udp_count += sprintf(udp_tx_data + udp_count,"%f", temperature_f);
                        }
                    }
                    if(UDP_ENABLE == 1 && RAW_ENABLE == 0 && COUNT_ENABLE == 1) {
                        udp_count += sprintf(udp_tx_data + udp_count,",%.2lf", temperature_f);
                    }
                    if(UDP_ENABLE == 1 && RAW_ENABLE == 0 && COUNT_ENABLE == 0) {
                       if(OPTIONS_COUNT > 1) {
                            udp_count += sprintf(udp_tx_data + udp_count,"%.2lf,", temperature_f);
                        }
                        else {
                            udp_count += sprintf(udp_tx_data + udp_count,"%.2lf", temperature_f);
                        }
                    }
                    OPTIONS_COUNT--;
                }
                if(DP_BMP390 != 0) {

                    for(uint8_t d = 0; d <= DISPLAY_ENABLE-1; d++) {
                        for(uint8_t i = 0; i <= dp[d].dc_count-1; i++) {
                            if(!strcmp(dp[d].dc[i].name, "bmp390") && dp[d].page == page) {

                                char buffer[25];

                                sprintf(buffer, "%.1lf", temperature_f);
                                strcpy(dp[d].dc[i].data1, buffer);
                                sprintf(buffer, "%.2lf", (double) pressure/100);
                                strcpy(dp[d].dc[i].data3, buffer);

                                if(dp[d].dptr(&dp[d], i, DISPLAY_SENSOR)){
                                    printf("%s bmp390 cmd %d failed\n", &dp[d].name, i);
                                }
                            }
                        }
                    }
                }
            }
            /*
             * read bme280
             */
            if(SENSOR_ENABLE == 2 || DP_BME280 != 0) {

                float temperature_f;
                float humidity_f;
                float pressure_f;

                uint8_t res = bme280_basic_read((float *)&temperature_f, (float *)&pressure_f, (float *)&humidity_f);
                if (res != 0) {
                    (void)bme280_basic_deinit();
                    printf("ERROR: bme280 read failed.\n");
                    return 1;
                }

                if(SENSOR_ENABLE == 2) {

                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 1) {
                        printf(",%f", temperature_f);
                    }
                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 0 && VERBOSE_ENABLE == 1) {
                        printf("\n BME280 Sensor = %.2lf c\n", temperature_f);
                        printf("        Humd = %.2lf %\n", humidity_f);
                        printf("        Pres = %.2lf hPa\n", pressure_f/100);
                     }
                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 0 && COUNT_ENABLE == 1 && VERBOSE_ENABLE == 0) {
                            printf(",%.2lf", temperature_f);
                    }
                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 0 && COUNT_ENABLE == 0 && VERBOSE_ENABLE == 0) {
                        if(OPTIONS_COUNT > 1) {
                            printf("%.2lf,", temperature_f);
                        }
                        else {
                            printf("%.2lf", temperature_f);
                        }
                    }
                    if(LOG_ENABLE == 1 && RAW_ENABLE == 1) {
                        fprintf(log_file,",%f", temperature_f);
                    }
                    if(LOG_ENABLE == 1 && RAW_ENABLE == 0) {
                        fprintf(log_file,",%.2lf", temperature_f);
                    }
                    if(UDP_ENABLE == 1 && RAW_ENABLE == 1 && COUNT_ENABLE == 1) {
                        udp_count += sprintf(udp_tx_data + udp_count,",%f", temperature_f);
                    }
                    if(UDP_ENABLE == 1 && RAW_ENABLE == 1 && COUNT_ENABLE == 0) {
                        if(OPTIONS_COUNT > 1) {
                            udp_count += sprintf(udp_tx_data + udp_count,"%f,", temperature_f);
                        }
                        else {
                            udp_count += sprintf(udp_tx_data + udp_count,"%f", temperature_f);
                        }
                    }
                    if(UDP_ENABLE == 1 && RAW_ENABLE == 0 && COUNT_ENABLE == 1) {
                        udp_count += sprintf(udp_tx_data + udp_count,",%.2lf", temperature_f);
                    }
                    if(UDP_ENABLE == 1 && RAW_ENABLE == 0 && COUNT_ENABLE == 0) {
                       if(OPTIONS_COUNT > 1) {
                            udp_count += sprintf(udp_tx_data + udp_count,"%.2lf,", temperature_f);
                        }
                        else {
                            udp_count += sprintf(udp_tx_data + udp_count,"%.2lf", temperature_f);
                        }
                    }
                    OPTIONS_COUNT--;
                }
                if(DP_BME280 != 0) {

                    for(uint8_t d = 0; d <= DISPLAY_ENABLE-1; d++) {
                        for(uint8_t i = 0; i <= dp[d].dc_count-1; i++) {
                            if(!strcmp(dp[d].dc[i].name, "bme280") && dp[d].page == page) {

                                char buffer[25];

                                sprintf(buffer, "%.1lf", temperature_f);
                                strcpy(dp[d].dc[i].data1, buffer);
                                sprintf(buffer, "%.0lf", humidity_f);
                                strcpy(dp[d].dc[i].data2, buffer);
                                sprintf(buffer, "%.2lf", pressure_f/100);
                                strcpy(dp[d].dc[i].data3, buffer);

                                if(dp[d].dptr(&dp[d], i, DISPLAY_SENSOR)){
                                    printf("%s bme280 cmd %d failed\n", &dp[d].name, i);
                                }
                            }
                        }
                    }
                }
            }
            /*
             * read bme680
             */
            if(BME680_ENABLE != 0 || DP_BME680 != 0) {

                float temperature_f;
                float humidity_f;
                float pressure_f;
                float idac_ma = 5.0f;
                float degree_celsius = 200.0f;
                float ohms;
                uint8_t index = 0;
                uint16_t gas_wait_ms = 150;

                uint8_t res = bme680_gas_read(idac_ma, degree_celsius, gas_wait_ms, index, \
                    (float *)&temperature_f, (float *)&pressure_f, (float *)&humidity_f,(float *)&ohms);
                if (res != 0) {
                    (void)bme680_gas_deinit();
                    printf("ERROR: bme680 read failed.\n");
                    return 1;
                }
                if(BME680_ENABLE != 0) {

                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 1) {
                        printf(",%f", temperature_f);
                    }
                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 0 && VERBOSE_ENABLE == 1) {
                        printf("\n BME680 Temp = %.2lf c\n", temperature_f);
                        printf("        Humd = %.2lf %\n", humidity_f);
                        printf("        Pres = %.2lf hPa\n", pressure_f/100);
                        printf("         Gas = %d\n", index);
                    }
                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 0 && COUNT_ENABLE == 1 && VERBOSE_ENABLE == 0) {
                            printf(",%.2lf", temperature_f);
                    }
                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 0 && COUNT_ENABLE == 0 && VERBOSE_ENABLE == 0) {
                        if(OPTIONS_COUNT > 1) {
                            printf("%.2lf,", temperature_f);
                        }
                        else {
                            printf("%.2lf", temperature_f);
                        }
                    }
                    if(LOG_ENABLE == 1 && RAW_ENABLE == 1) {
                        fprintf(log_file,",%f", temperature_f);
                    }
                    if(LOG_ENABLE == 1 && RAW_ENABLE == 0) {
                        fprintf(log_file,",%.2lf", temperature_f);
                    }
                    if(UDP_ENABLE == 1 && RAW_ENABLE == 1 && COUNT_ENABLE == 1) {
                        udp_count += sprintf(udp_tx_data + udp_count,",%f", temperature_f);
                    }
                    if(UDP_ENABLE == 1 && RAW_ENABLE == 1 && COUNT_ENABLE == 0) {
                        if(OPTIONS_COUNT > 1) {
                            udp_count += sprintf(udp_tx_data + udp_count,"%f,", temperature_f);
                        }
                        else {
                            udp_count += sprintf(udp_tx_data + udp_count,"%f", temperature_f);
                        }
                    }
                    if(UDP_ENABLE == 1 && RAW_ENABLE == 0 && COUNT_ENABLE == 1) {
                        udp_count += sprintf(udp_tx_data + udp_count,",%.2lf", temperature_f);
                    }
                    if(UDP_ENABLE == 1 && RAW_ENABLE == 0 && COUNT_ENABLE == 0) {
                       if(OPTIONS_COUNT > 1) {
                            udp_count += sprintf(udp_tx_data + udp_count,"%.2lf,", temperature_f);
                        }
                        else {
                            udp_count += sprintf(udp_tx_data + udp_count,"%.2lf", temperature_f);
                        }
                    }
                    OPTIONS_COUNT--;
                }
                if(DP_BME680 != 0) {

                    for(uint8_t d = 0; d <= DISPLAY_ENABLE-1; d++) {
                        for(uint8_t i = 0; i <= dp[d].dc_count-1; i++) {
                            if(!strcmp(dp[d].dc[i].name, "bme680") && dp[d].page == page) {

                                char buffer[25];

                                sprintf(buffer, "%.1lf", temperature_f);
                                strcpy(dp[d].dc[i].data1, buffer);
                                sprintf(buffer, "%.0lf", humidity_f);
                                strcpy(dp[d].dc[i].data2, buffer);
                                sprintf(buffer, "%.2lf", pressure_f/100);
                                strcpy(dp[d].dc[i].data3, buffer);
                                sprintf(buffer, "%d", index);
                                strcpy(dp[d].dc[i].data5, buffer);

                                if(dp[d].dptr(&dp[d], i, DISPLAY_SENSOR)){
                                    printf("%s bme680 cmd %d failed\n", &dp[d].name, i);
                                }
                            }
                        }
                    }
                }
            }
            /*
             * read mcp9808 temperature sensor
             */
            if(SENSOR_ENABLE == 3 || DP_MCP9808 != 0) {

                float temperature = mcp9808_read();

                if(SENSOR_ENABLE == 3) {

                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 1) {
                        printf(",%d", temperature);
                    }
                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 0 && VERBOSE_ENABLE == 1) {
                        printf("\n MCP9808 Sensor = %.2lfc\n", temperature);
                    }
                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 0 && COUNT_ENABLE == 1 && VERBOSE_ENABLE == 0) {
                            printf(",%.2lf", temperature);
                    }
                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 0 && COUNT_ENABLE == 0 && VERBOSE_ENABLE == 0) {
                        if(OPTIONS_COUNT > 1) {
                            printf("%.2lf,", temperature);
                        }
                        else {
                            printf("%.2lf", temperature);
                        }
                    }
                    if(LOG_ENABLE == 1 && RAW_ENABLE == 1) {
                        fprintf(log_file,",%.2lf",temperature);
                    }
                    if(LOG_ENABLE == 1 && RAW_ENABLE == 0) {
                        fprintf(log_file,",%.2lf", temperature);
                    }
                    if(UDP_ENABLE == 1 && COUNT_ENABLE == 1) {
                        udp_count += sprintf(udp_tx_data + udp_count,",%.2lf", temperature);
                    }
                    if(UDP_ENABLE == 1 && COUNT_ENABLE == 0) {
                        if(OPTIONS_COUNT > 1) {
                            udp_count += sprintf(udp_tx_data + udp_count,"%.2lf,", temperature);
                        }
                        else {
                            udp_count += sprintf(udp_tx_data + udp_count,"%.2lf", temperature);
                        }
                    }
                    OPTIONS_COUNT--;
                }
                if(DP_MCP9808 != 0) {

                    for(uint8_t d = 0; d <= DISPLAY_ENABLE-1; d++) {
                        for(uint8_t i = 0; i <= dp[d].dc_count-1; i++) {
                            if(!strcmp(dp[d].dc[i].name, "mcp9808") && dp[d].page == page) {

                                char buffer[6];

                                sprintf(buffer, "%.1lf", temperature);
                                strcpy(dp[d].dc[i].data1, buffer);

                                if(dp[d].dptr(&dp[d], i, DISPLAY_SENSOR)){
                                    printf("%s mcp9808 cmd %d failed\n", &dp[d].name, i);
                                }
                            }
                        }
                    }
                }
            }
            /*
            * SHT4x enabled
            */
            if(SENSOR_ENABLE == 4 || DP_SHT4X != 0) {

                float temperature_f;
                float humidity_f;

                uint8_t res = sht4x_basic_read((float *)&temperature_f, (float *)&humidity_f);
                if (res != 0) {
                    (void)sht4x_basic_deinit();
                    printf("ERROR: sht4x read failed.\n");
                    return 1;
                }

                if(SENSOR_ENABLE == 4) {

                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 1) {
                        printf(",%f", temperature_f);
                    }
                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 0 && VERBOSE_ENABLE == 1) {
                        printf("\n sht4x Sensor = %.2lf c\n", temperature_f);
                        printf("        Humd = %.2lf %\n", humidity_f);
                    }
                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 0 && COUNT_ENABLE == 1 && VERBOSE_ENABLE == 0) {
                            printf(",%.2lf", temperature_f);
                    }
                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 0 && COUNT_ENABLE == 0 && VERBOSE_ENABLE == 0) {
                        if(OPTIONS_COUNT > 1) {
                            printf("%.2lf,", temperature_f);
                        }
                        else {
                            printf("%.2lf", temperature_f);
                        }
                    }
                    if(LOG_ENABLE == 1 && RAW_ENABLE == 1) {
                        fprintf(log_file,",%f", temperature_f);
                    }
                    if(LOG_ENABLE == 1 && RAW_ENABLE == 0) {
                        fprintf(log_file,",%.2lf", temperature_f);
                    }
                    if(UDP_ENABLE == 1 && RAW_ENABLE == 1 && COUNT_ENABLE == 1) {
                        udp_count += sprintf(udp_tx_data + udp_count,",%f", temperature_f);
                    }
                    if(UDP_ENABLE == 1 && RAW_ENABLE == 1 && COUNT_ENABLE == 0) {
                        if(OPTIONS_COUNT > 1) {
                            udp_count += sprintf(udp_tx_data + udp_count,"%f,", temperature_f);
                        }
                        else {
                            udp_count += sprintf(udp_tx_data + udp_count,"%f", temperature_f);
                        }
                    }
                    if(UDP_ENABLE == 1 && RAW_ENABLE == 0 && COUNT_ENABLE == 1) {
                        udp_count += sprintf(udp_tx_data + udp_count,",%.2lf", temperature_f);
                    }
                    if(UDP_ENABLE == 1 && RAW_ENABLE == 0 && COUNT_ENABLE == 0) {
                       if(OPTIONS_COUNT > 1) {
                            udp_count += sprintf(udp_tx_data + udp_count,"%.2lf,", temperature_f);
                        }
                        else {
                            udp_count += sprintf(udp_tx_data + udp_count,"%.2lf", temperature_f);
                        }
                    }
                    OPTIONS_COUNT--;
                }
                if(DP_SHT4X != 0) {

                    for(uint8_t d = 0; d <= DISPLAY_ENABLE-1; d++) {
                        for(uint8_t i = 0; i <= dp[d].dc_count-1; i++) {
                            if(!strcmp(dp[d].dc[i].name, "sht4x") && dp[d].page == page) {

                                char buffer[25];

                                sprintf(buffer, "%.1lf", temperature_f);
                                strcpy(dp[d].dc[i].data1, buffer);
                                sprintf(buffer, "%.0lf", humidity_f);
                                strcpy(dp[d].dc[i].data2, buffer);

                                if(dp[d].dptr(&dp[d], i, DISPLAY_SENSOR)){
                                    printf("%s sht4x cmd %d failed\n", &dp[d].name, i);
                                }
                            }
                        }
                    }
                }
            }
            /*
            * SHTC3 enabled
            */
            if(SENSOR_ENABLE == 5 || DP_SHTC3 != 0) {

                float temperature_f;
                float humidity_f;

                uint8_t res = shtc3_basic_read((float *)&temperature_f, (float *)&humidity_f);
                if (res != 0) {
                    (void)shtc3_basic_deinit();
                    printf("ERROR: shtc3 read failed.\n");
                    return 1;
                }

                if(SENSOR_ENABLE == 5) {
                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 1) {
                        printf(",%f", temperature_f);
                    }
                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 0 && VERBOSE_ENABLE == 1) {
                        printf("\n shtc3 Sensor = %.2lf c\n", temperature_f);
                        printf("        Humd = %.2lf %\n", humidity_f);
                    }
                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 0 && COUNT_ENABLE == 1 && VERBOSE_ENABLE == 0) {
                            printf(",%.2lf", temperature_f);
                    }
                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 0 && COUNT_ENABLE == 0 && VERBOSE_ENABLE == 0) {
                        if(OPTIONS_COUNT > 1) {
                            printf("%.2lf,", temperature_f);
                        }
                        else {
                            printf("%.2lf", temperature_f);
                        }
                    }
                    if(LOG_ENABLE == 1 && RAW_ENABLE == 1) {
                        fprintf(log_file,",%f", temperature_f);
                    }
                    if(LOG_ENABLE == 1 && RAW_ENABLE == 0) {
                        fprintf(log_file,",%.2lf", temperature_f);
                    }
                    if(UDP_ENABLE == 1 && RAW_ENABLE == 1 && COUNT_ENABLE == 1) {
                        udp_count += sprintf(udp_tx_data + udp_count,",%f", temperature_f);
                    }
                    if(UDP_ENABLE == 1 && RAW_ENABLE == 1 && COUNT_ENABLE == 0) {
                        if(OPTIONS_COUNT > 1) {
                            udp_count += sprintf(udp_tx_data + udp_count,"%f,", temperature_f);
                        }
                        else {
                            udp_count += sprintf(udp_tx_data + udp_count,"%f", temperature_f);
                        }
                    }
                    if(UDP_ENABLE == 1 && RAW_ENABLE == 0 && COUNT_ENABLE == 1) {
                        udp_count += sprintf(udp_tx_data + udp_count,",%.2lf", temperature_f);
                    }
                    if(UDP_ENABLE == 1 && RAW_ENABLE == 0 && COUNT_ENABLE == 0) {
                       if(OPTIONS_COUNT > 1) {
                            udp_count += sprintf(udp_tx_data + udp_count,"%.2lf,", temperature_f);
                        }
                        else {
                            udp_count += sprintf(udp_tx_data + udp_count,"%.2lf", temperature_f);
                        }
                    }
                    OPTIONS_COUNT--;
                }
                if(DP_SHTC3 != 0) {

                    for(uint8_t d = 0; d <= DISPLAY_ENABLE-1; d++) {
                        for(uint8_t i = 0; i <= dp[d].dc_count-1; i++) {
                            if(!strcmp(dp[d].dc[i].name, "shtc3") && dp[d].page == page) {

                                char buffer[25];

                                sprintf(buffer, "%.1lf", temperature_f);
                                strcpy(dp[d].dc[i].data1, buffer);
                                sprintf(buffer, "%.0lf", humidity_f);
                                strcpy(dp[d].dc[i].data2, buffer);

                                if(dp[d].dptr(&dp[d], i, DISPLAY_SENSOR)){
                                    printf("%s shtc3 cmd %d failed\n", &dp[d].name, i);
                                }
                            }
                        }
                    }
                }
            }
            /*
            * aht20 enabled
            */
            if(SENSOR_ENABLE == 6 || DP_AHT20 != 0) {

                float temperature_f;
                uint8_t humidity_f;

                uint8_t res = aht20_basic_read((float *)&temperature_f, (uint8_t *)&humidity_f);
                if (res != 0) {
                    (void)aht20_basic_deinit();
                    printf("ERROR: aht20 read failed.\n");
                    return 1;
                }

                if(SENSOR_ENABLE == 6) {
                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 1) {
                        printf(",%f", temperature_f);
                    }
                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 0 && VERBOSE_ENABLE == 1) {
                        printf("\n aht20 Sensor = %.2lf c", temperature_f);
                        printf("        Humd = %.2lf %\n", humidity_f);
                    }
                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 0 && COUNT_ENABLE == 1 && VERBOSE_ENABLE == 0) {
                            printf(",%.2lf", temperature_f);
                    }
                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 0 && COUNT_ENABLE == 0 && VERBOSE_ENABLE == 0) {
                        if(OPTIONS_COUNT > 1) {
                            printf("%.2lf,", temperature_f);
                        }
                        else {
                            printf("%.2lf", temperature_f);
                        }
                    }
                    if(LOG_ENABLE == 1 && RAW_ENABLE == 1) {
                        fprintf(log_file,",%f", temperature_f);
                    }
                    if(LOG_ENABLE == 1 && RAW_ENABLE == 0) {
                        fprintf(log_file,",%.2lf", temperature_f);
                    }
                    if(UDP_ENABLE == 1 && RAW_ENABLE == 1 && COUNT_ENABLE == 1) {
                        udp_count += sprintf(udp_tx_data + udp_count,",%f", temperature_f);
                    }
                    if(UDP_ENABLE == 1 && RAW_ENABLE == 1 && COUNT_ENABLE == 0) {
                        if(OPTIONS_COUNT > 1) {
                            udp_count += sprintf(udp_tx_data + udp_count,"%f,", temperature_f);
                        }
                        else {
                            udp_count += sprintf(udp_tx_data + udp_count,"%f", temperature_f);
                        }
                    }
                    if(UDP_ENABLE == 1 && RAW_ENABLE == 0 && COUNT_ENABLE == 1) {
                        udp_count += sprintf(udp_tx_data + udp_count,",%.2lf", temperature_f);
                    }
                    if(UDP_ENABLE == 1 && RAW_ENABLE == 0 && COUNT_ENABLE == 0) {
                       if(OPTIONS_COUNT > 1) {
                            udp_count += sprintf(udp_tx_data + udp_count,"%.2lf,", temperature_f);
                        }
                        else {
                            udp_count += sprintf(udp_tx_data + udp_count,"%.2lf", temperature_f);
                        }
                    }
                    OPTIONS_COUNT--;
                }
                if(DP_AHT20 != 0) {

                    for(uint8_t d = 0; d <= DISPLAY_ENABLE-1; d++) {
                        for(uint8_t i = 0; i <= dp[d].dc_count-1; i++) {
                            if(!strcmp(dp[d].dc[i].name, "aht20") && dp[d].page == page) {

                                char buffer[25];

                                sprintf(buffer, "%.1lf", temperature_f);
                                strcpy(dp[d].dc[i].data1, buffer);
                                sprintf(buffer, "%d", humidity_f);
                                strcpy(dp[d].dc[i].data2, buffer);

                                if(dp[d].dptr(&dp[d], i, DISPLAY_SENSOR)){
                                    printf("%s aht20 cmd %d failed\n", &dp[d].name, i);
                                }
                            }
                        }
                    }
                }
            }
            /*
            * htu31d enabled
            */
            if(SENSOR_ENABLE == 7 || DP_HTU31D != 0) {

                float temperature_f;
                float humidity_f;

                uint8_t res = htu31d_basic_read((float *)&temperature_f, (float *)&humidity_f);
                if (res != 0) {
                    (void)htu31d_basic_deinit();
                    printf("ERROR: htu31d read failed.\n");
                    return 1;
                }

                if(SENSOR_ENABLE == 7) {
                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 1) {
                        printf(",%f", temperature_f);
                    }
                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 0 && VERBOSE_ENABLE == 1) {
                        printf("\n htu31d Sensor = %.2lf c\n", temperature_f);
                        printf("        Humd = %.2lf %\n", humidity_f);
                    }
                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 0 && COUNT_ENABLE == 1 && VERBOSE_ENABLE == 0) {
                            printf(",%.2lf", temperature_f);
                    }
                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 0 && COUNT_ENABLE == 0 && VERBOSE_ENABLE == 0) {
                        if(OPTIONS_COUNT > 1) {
                            printf("%.2lf,", temperature_f);
                        }
                        else {
                            printf("%.2lf", temperature_f);
                        }
                    }
                    if(LOG_ENABLE == 1 && RAW_ENABLE == 1) {
                        fprintf(log_file,",%f", temperature_f);
                    }
                    if(LOG_ENABLE == 1 && RAW_ENABLE == 0) {
                        fprintf(log_file,",%.2lf", temperature_f);
                    }
                    if(UDP_ENABLE == 1 && RAW_ENABLE == 1 && COUNT_ENABLE == 1) {
                        udp_count += sprintf(udp_tx_data + udp_count,",%f", temperature_f);
                    }
                    if(UDP_ENABLE == 1 && RAW_ENABLE == 1 && COUNT_ENABLE == 0) {
                        if(OPTIONS_COUNT > 1) {
                            udp_count += sprintf(udp_tx_data + udp_count,"%f,", temperature_f);
                        }
                        else {
                            udp_count += sprintf(udp_tx_data + udp_count,"%f", temperature_f);
                        }
                    }
                    if(UDP_ENABLE == 1 && RAW_ENABLE == 0 && COUNT_ENABLE == 1) {
                        udp_count += sprintf(udp_tx_data + udp_count,",%.2lf", temperature_f);
                    }
                    if(UDP_ENABLE == 1 && RAW_ENABLE == 0 && COUNT_ENABLE == 0) {
                       if(OPTIONS_COUNT > 1) {
                            udp_count += sprintf(udp_tx_data + udp_count,"%.2lf,", temperature_f);
                        }
                        else {
                            udp_count += sprintf(udp_tx_data + udp_count,"%.2lf", temperature_f);
                        }
                    }
                    OPTIONS_COUNT--;
                }
                if(DP_HTU31D != 0) {

                    for(uint8_t d = 0; d <= DISPLAY_ENABLE-1; d++) {
                        for(uint8_t i = 0; i <= dp[d].dc_count-1; i++) {
                            if(!strcmp(dp[d].dc[i].name, "htu31d") && dp[d].page == page) {

                                char buffer[25];

                                sprintf(buffer, "%.1lf", temperature_f);
                                strcpy(dp[d].dc[i].data1, buffer);
                                sprintf(buffer, "%.0lf", humidity_f);
                                strcpy(dp[d].dc[i].data2, buffer);

                                if(dp[d].dptr(&dp[d], i, DISPLAY_SENSOR)){
                                    printf("%s htu31d cmd %d failed\n", &dp[d].name, i);
                                }
                            }
                        }
                    }
                }
            }
            /*
            * SCD30 enabled
            */
            if(SCD30_ENABLE != 0 || DP_SCD30 != 0) {

                scd30_data_t data;

                uint8_t res = scd30_basic_read((scd30_data_t *)&data);
                if (res != 0) {
                    (void)scd30_basic_deinit();
                    return 1;
                }
                if(SCD30_ENABLE != 0) {
                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 1) {
                        printf(",%f", data.co2_ppm);
                    }
                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 0 && VERBOSE_ENABLE == 1) {
                        printf("\n scd30 Sensor = %.2lf ppm", data.co2_ppm);
                        printf("        Temp = %.2lf c\n", data.temperature_deg);
                        printf("        Humd = %.2lf %\n", data.humidity_percent);
                    }
                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 0 && COUNT_ENABLE == 1 && VERBOSE_ENABLE == 0) {
                            printf(",%.2lf", data.co2_ppm);
                    }
                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 0 && COUNT_ENABLE == 0 && VERBOSE_ENABLE == 0) {
                        if(OPTIONS_COUNT > 1) {
                            printf("%.2lf,", data.co2_ppm);
                        }
                        else {
                            printf("%.2lf", data.co2_ppm);
                        }
                    }
                    if(LOG_ENABLE == 1 && RAW_ENABLE == 1) {
                        fprintf(log_file,",%f", data.co2_ppm);
                    }
                    if(LOG_ENABLE == 1 && RAW_ENABLE == 0) {
                        fprintf(log_file,",%.2lf", data.co2_ppm);
                    }
                    if(UDP_ENABLE == 1 && RAW_ENABLE == 1 && COUNT_ENABLE == 1) {
                        udp_count += sprintf(udp_tx_data + udp_count,",%f", data.co2_ppm);
                    }
                    if(UDP_ENABLE == 1 && RAW_ENABLE == 1 && COUNT_ENABLE == 0) {
                        if(OPTIONS_COUNT > 1) {
                            udp_count += sprintf(udp_tx_data + udp_count,"%f,", data.co2_ppm);
                        }
                        else {
                            udp_count += sprintf(udp_tx_data + udp_count,"%f", data.co2_ppm);
                        }
                    }
                    if(UDP_ENABLE == 1 && RAW_ENABLE == 0 && COUNT_ENABLE == 1) {
                        udp_count += sprintf(udp_tx_data + udp_count,",%.2lf", data.co2_ppm);
                    }
                    if(UDP_ENABLE == 1 && RAW_ENABLE == 0 && COUNT_ENABLE == 0) {
                       if(OPTIONS_COUNT > 1) {
                            udp_count += sprintf(udp_tx_data + udp_count,"%.2lf,", data.co2_ppm);
                        }
                        else {
                            udp_count += sprintf(udp_tx_data + udp_count,"%.2lf", data.co2_ppm);
                        }
                    }
                    OPTIONS_COUNT--;
                }
                if(DP_SCD30 != 0) {
                    for(uint8_t d = 0; d <= DISPLAY_ENABLE-1; d++) {
                        for(uint8_t i = 0; i <= dp[d].dc_count-1; i++) {
                            if(!strcmp(dp[d].dc[i].name, "scd30") && dp[d].page == page) {

                                char buffer[6];

                                sprintf(buffer, "%.1f", data.temperature_deg);
                                strcpy(dp[d].dc[i].data1, buffer);

                                sprintf(buffer, "%.0f", data.humidity_percent);
                                strcpy(dp[d].dc[i].data2, buffer);

                                sprintf(buffer, "%.0f", data.co2_ppm);
                                strcpy(dp[d].dc[i].data4, buffer);

                                if(dp[d].dptr(&dp[d], i, DISPLAY_SENSOR)){
                                    printf("%s scd30 cmd %d failed\n", &dp[d].name, i);
                                }
                            }
                        }
                    }
                }
            }
            /*
            * SCD4x enabled
            */
            if(SCD4X_ENABLE != 0 || DP_SCD4X != 0) {

                float temperature_f;
                float humidity_f;
                uint16_t co2_ppm;

                uint8_t res = scd4x_shot_read((uint16_t *)&co2_ppm, (float *)&temperature_f, (float *)&humidity_f);
                if (res != 0) {
                    (void)scd4x_shot_deinit();
                    return 1;
                }
                if(SCD4X_ENABLE != 0) {
                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 1) {
                        printf(",%d", co2_ppm);
                    }
                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 0 && VERBOSE_ENABLE == 1) {
                        printf("\n scd4x Sensor = %d ppm\n", co2_ppm);
                        printf("         Temp = %.2lf c\n", temperature_f);
                        printf("         Humd = %.2lf %\n", humidity_f);
                    }
                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 0 && COUNT_ENABLE == 1 && VERBOSE_ENABLE == 0) {
                            printf(",%d", co2_ppm);
                    }
                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 0 && COUNT_ENABLE == 0 && VERBOSE_ENABLE == 0) {
                        if(OPTIONS_COUNT > 1) {
                            printf("%d,", co2_ppm);
                        }
                        else {
                            printf("%d", co2_ppm);
                        }
                    }
                    if(LOG_ENABLE == 1 && RAW_ENABLE == 1) {
                        fprintf(log_file,",%d", co2_ppm);
                    }
                    if(LOG_ENABLE == 1 && RAW_ENABLE == 0) {
                        fprintf(log_file,",%d", co2_ppm);
                    }
                    if(UDP_ENABLE == 1 && RAW_ENABLE == 1 && COUNT_ENABLE == 1) {
                        udp_count += sprintf(udp_tx_data + udp_count,",%d", co2_ppm);
                    }
                    if(UDP_ENABLE == 1 && RAW_ENABLE == 1 && COUNT_ENABLE == 0) {
                        if(OPTIONS_COUNT > 1) {
                            udp_count += sprintf(udp_tx_data + udp_count,"%d,", co2_ppm);
                        }
                        else {
                            udp_count += sprintf(udp_tx_data + udp_count,"%d", co2_ppm);
                        }
                    }
                    if(UDP_ENABLE == 1 && RAW_ENABLE == 0 && COUNT_ENABLE == 1) {
                        udp_count += sprintf(udp_tx_data + udp_count,",%d", co2_ppm);
                    }
                    if(UDP_ENABLE == 1 && RAW_ENABLE == 0 && COUNT_ENABLE == 0) {
                       if(OPTIONS_COUNT > 1) {
                            udp_count += sprintf(udp_tx_data + udp_count,"%d,", co2_ppm);
                        }
                        else {
                            udp_count += sprintf(udp_tx_data + udp_count,"%d", co2_ppm);
                        }
                    }
                    OPTIONS_COUNT--;
                }
                if(DP_SCD4X != 0) {
                    for(uint8_t d = 0; d <= DISPLAY_ENABLE-1; d++) {
                        for(uint8_t i = 0; i <= dp[d].dc_count-1; i++) {
                            if((!strcmp(dp[d].dc[i].name, "scd40") || \
                                !strcmp(dp[d].dc[i].name, "scd41") || \
                                    !strcmp(dp[d].dc[i].name, "scd43")) && dp[d].page == page) {

                                char buffer[6];

                                sprintf(buffer, "%.1f", temperature_f);
                                strcpy(dp[d].dc[i].data1, buffer);

                                sprintf(buffer, "%.0f", humidity_f);
                                strcpy(dp[d].dc[i].data2, buffer);

                                sprintf(buffer, "%d", co2_ppm);
                                strcpy(dp[d].dc[i].data4, buffer);

                                if(dp[d].dptr(&dp[d], i, DISPLAY_SENSOR)){
                                    printf("%s scd41 cmd %d failed\n", &dp[d].name, i);
                                }
                            }
                        }
                    }
                }
            }
            /*
             * SGP30 enabled
             */
            if(SGP30_ENABLE != 0 || DP_SGP30 != 0) {

                uint16_t co2_eq_ppm = 0;
                uint16_t tvoc_ppb = 0;

                uint8_t res = sgp30_advance_read((uint16_t *)&co2_eq_ppm, (uint16_t *)&tvoc_ppb);
                if (res != 0) {
                    (void)sgp30_advance_deinit();
                    return 1;
                }

                if(SGP30_ENABLE != 0) {
                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 1) {
                        printf(",%d", tvoc_ppb);
                    }
                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 0 && VERBOSE_ENABLE == 1) {
                        printf("\n sgp30 Sensor = %d ppm\n", tvoc_ppb);
                        printf(" eCO2 = %d ppm\n", co2_eq_ppm);
                    }
                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 0 && COUNT_ENABLE == 1 && VERBOSE_ENABLE == 0) {
                            printf(",%d", tvoc_ppb);
                    }
                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 0 && COUNT_ENABLE == 0 && VERBOSE_ENABLE == 0) {
                        if(OPTIONS_COUNT > 1) {
                            printf("%d,", tvoc_ppb);
                        }
                        else {
                            printf("%d", tvoc_ppb);
                        }
                    }
                    if(LOG_ENABLE == 1 && RAW_ENABLE == 1) {
                        fprintf(log_file,",%d", tvoc_ppb);
                    }
                    if(LOG_ENABLE == 1 && RAW_ENABLE == 0) {
                        fprintf(log_file,",%d", tvoc_ppb);
                    }
                    if(UDP_ENABLE == 1 && RAW_ENABLE == 1 && COUNT_ENABLE == 1) {
                        udp_count += sprintf(udp_tx_data + udp_count,",%d", tvoc_ppb);
                    }
                    if(UDP_ENABLE == 1 && RAW_ENABLE == 1 && COUNT_ENABLE == 0) {
                        if(OPTIONS_COUNT > 1) {
                            udp_count += sprintf(udp_tx_data + udp_count,"%d,", tvoc_ppb);
                        }
                        else {
                            udp_count += sprintf(udp_tx_data + udp_count,"%d", tvoc_ppb);
                        }
                    }
                    if(UDP_ENABLE == 1 && RAW_ENABLE == 0 && COUNT_ENABLE == 1) {
                        udp_count += sprintf(udp_tx_data + udp_count,",%d", tvoc_ppb);
                    }
                    if(UDP_ENABLE == 1 && RAW_ENABLE == 0 && COUNT_ENABLE == 0) {
                       if(OPTIONS_COUNT > 1) {
                            udp_count += sprintf(udp_tx_data + udp_count,"%d,", tvoc_ppb);
                        }
                        else {
                            udp_count += sprintf(udp_tx_data + udp_count,"%d", tvoc_ppb);
                        }
                    }
                    OPTIONS_COUNT--;
                }
                if(DP_SGP30 != 0) {

                    for(uint8_t d = 0; d <= DISPLAY_ENABLE-1; d++) {
                        for(uint8_t i = 0; i <= dp[d].dc_count-1; i++) {
                            if(!strcmp(dp[d].dc[i].name, "sgp30") && dp[d].page == page) {

                                char buffer[10];

                                sprintf(buffer, "%d", co2_eq_ppm);
                                strcpy(dp[d].dc[i].data4, buffer);
                                sprintf(buffer, "%d", tvoc_ppb);
                                strcpy(dp[d].dc[i].data5, buffer);

                                if(dp[d].dptr(&dp[d], i, DISPLAY_SENSOR)){
                                    printf("%s scd41 cmd %d failed\n", &dp[d].name, i);
                                }
                            }
                        }
                    }
                }
            }
            /*
             * SmartPower enabled
             */
            if(SP_ENABLE != 0) {

                if((pwr_in = open(smartpower, O_RDONLY | O_NOCTTY | O_SYNC)) < 0) {
                    printf("\nERROR: Cannot open SmartPower at %s\n\n", smartpower);
                    exit(1);
                }
                /*
                 * read SmartPower2
                 */
                if(SP_ENABLE == 2) {
                    unsigned char temp[18];
                    int8_t sp_read = 0;
                    if((sp_read = read(pwr_in, temp, sizeof(temp) - 1)) < 0) {
                        printf("Error from read: %d: %s\n", sp_read, strerror(errno));
                    }
                    temp[sp_read] = 0;
                    sscanf(temp, "%f%s %f%s %f%s", &volt, spline, &amp, spline1, &watt, spline2);
                    if(strstr(spline1,"mA")) {
                        if(QUIET_ENABLE == 0 && VERBOSE_ENABLE == 1) {
                            printf("\n\n Volts = %.2f\n Amps = .%.0f\n Watts = %.2f\n\n", volt, amp, watt);
                        }
                        if(QUIET_ENABLE == 0 && RAW_ENABLE == 0 && COUNT_ENABLE == 1 && VERBOSE_ENABLE == 0) {
                                printf(",%.2f,.%.0f,%.2f", volt, amp, watt);
                        }
                        if(QUIET_ENABLE == 0 && RAW_ENABLE == 0 && COUNT_ENABLE == 0 && VERBOSE_ENABLE == 0) {
                            if(OPTIONS_COUNT >= 1) {
                                printf("%.2f,.%.0f,%.2f,", volt, amp, watt);
                            }
                            else {
                                printf("%.2f,.%.0f,%.2f", volt, amp, watt);
                            }
                        }
                        if(LOG_ENABLE == 1) {
                            fprintf(log_file,",%.2f,.%.0f,%.2f", volt, amp, watt);
                        }

                        if(UDP_ENABLE == 1 && COUNT_ENABLE == 1) {
                            udp_count += sprintf(udp_tx_data + udp_count,",%.2f,.%.0f,%.2f", volt, amp, watt);
                        }
                        if(UDP_ENABLE == 1 && COUNT_ENABLE == 0) {
                            if(OPTIONS_COUNT > 1) {
                                udp_count += sprintf(udp_tx_data + udp_count,"%.2f,.%.0f,%.2f,", volt, amp, watt);
                            }
                            else {
                                udp_count += sprintf(udp_tx_data + udp_count,"%.2f,.%.0f,%.2f", volt, amp, watt);
                            }
                        }
                    OPTIONS_COUNT--;
                    }
                    if(strstr(spline1,"A")) {
                        if(QUIET_ENABLE == 0 && VERBOSE_ENABLE == 1) {
                            printf("\n\n Volts = %.2f\n Amps = %.2f\n Watts = %.2f\n\n", volt, amp, watt);
                        }
                        if(QUIET_ENABLE == 0 && RAW_ENABLE == 0 && COUNT_ENABLE == 1 && VERBOSE_ENABLE == 0) {
                                printf(",%.2f,%.2f,%.2f", volt, amp, watt);
                        }
                        if(QUIET_ENABLE == 0 && RAW_ENABLE == 0 && COUNT_ENABLE == 0 && VERBOSE_ENABLE == 0) {
                            if(OPTIONS_COUNT > 1) {
                                printf("%.2f,%.2f,%.2f,", volt, amp, watt);
                            }
                            else {
                                printf("%.2f,%.2f,%.2f", volt, amp, watt);
                            }
                        }
                        if(LOG_ENABLE == 1) {
                            fprintf(log_file,",%.2f,%.2f,%.2f", volt, amp, watt);
                        }

                        if(UDP_ENABLE == 1 && COUNT_ENABLE == 1) {
                            udp_count += sprintf(udp_tx_data + udp_count,",%.2f,%.2f,%.2f", volt, amp, watt);
                        }
                        if(UDP_ENABLE == 1 && COUNT_ENABLE == 0) {
                            if(OPTIONS_COUNT > 1) {
                                udp_count += sprintf(udp_tx_data + udp_count,"%.2f,%.2f,%.2f,", volt, amp, watt);
                            }
                            else {
                                udp_count += sprintf(udp_tx_data + udp_count,"%.2f,%.2f,%.2f", volt, amp, watt);
                            }
                        }
                    }
                    OPTIONS_COUNT--;
                }
                /*
                 * read SmartPower3
                 */
                if(SP_ENABLE == 31 || SP_ENABLE == 32) {

                    unsigned char temp[82];
                    int8_t sp_read = 0;

                    if((sp_read = read(pwr_in, temp, sizeof(temp) - 1)) < 0) {
                        printf("Error from read: %d: %s\n", sp_read, strerror(errno));
                    }
                    temp[sp_read] = 0;
                    sscanf(temp, "%Lf,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%x,%x\n",  \
                        &sp_ms, \
                        &in_mv, &in_ma, &in_w, &in_on, \
                        &ch1_mv, &ch1_ma, &ch1_w, &ch1_on, &ch1_int, \
                        &ch2_mv, &ch2_ma, &ch2_w, &ch2_on, &ch2_int, \
                        &chk_comp, &chk_xor);

                    if(SP_ENABLE == 31) {
                        volt = ch1_mv;
                        amp = ch1_ma;
                        watt = ch1_w;
                    }
                    else {
                        volt = ch2_mv;
                        amp = ch2_ma;
                        watt = ch2_w;
                    }
                    if(QUIET_ENABLE == 0 && VERBOSE_ENABLE == 1) {
                        printf("\n\n Volts = %.2f\n Amps = %.2f\n Watts = %.2f\n\n", volt/1000, amp/1000, watt/1000);
                    }
                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 0 && COUNT_ENABLE == 1 && VERBOSE_ENABLE == 0) {
                        printf(",%.2f,%.2f,%.2f", volt/1000, amp/1000, watt/1000);
                    }
                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 0 && COUNT_ENABLE == 0 && VERBOSE_ENABLE == 0) {
                        if(OPTIONS_COUNT > 1) {
                            printf("%.2f,%.2f,%.2f,", volt/1000, amp/1000, watt/1000);
                        }
                        else {
                            printf("%.2f,%.2f,%.2f", volt/1000, amp/1000, watt/1000);
                        }
                    }
                    if(LOG_ENABLE == 1) {
                        fprintf(log_file,",%.2f,%.2f,%.2f", volt/1000, amp/1000, watt/1000);
                    }

                    if(UDP_ENABLE == 1 && COUNT_ENABLE == 1) {
                        udp_count += sprintf(udp_tx_data + udp_count,",%.2f,%.2f,%.2f", volt/1000, amp/1000, watt/1000);
                    }
                    if(UDP_ENABLE == 1 && COUNT_ENABLE == 0) {
                        if(OPTIONS_COUNT > 1) {
                            udp_count += sprintf(udp_tx_data + udp_count,"%.2f,%.2f,%.2f,", volt/1000, amp/1000, watt/1000);
                        }
                        else {
                            udp_count += sprintf(udp_tx_data + udp_count,"%.2f,%.2f,%.2f", volt/1000, amp/1000, watt/1000);
                        }
                    }
                    OPTIONS_COUNT--;
                }
                close(pwr_in);
            }
            /*
             * read proc stat
             */
            if(USAGE_ENABLE != 0 || DP_USAGE != 0) {

                float r = 0;
                float s = 0;
                float sum = 0;
                uint8_t corecnt = 0;

                if(!USAGE_ENABLE) {
                corecnt = DP_USAGE;
                }
                else {
                corecnt = USAGE_ENABLE;
                }
                if((cpu_use = fopen(cpuusage, "r")) == NULL) {
                    printf("\nERROR: Cannot open %s\n", cpuusage);
                    exit(1);
                }
                for (uint16_t c = 0; c <= corecnt; c++) {
                    char t[20];
                    char us[10][256] = {0};
                    double long u[10][256] = {0};
                    char *endptr;

                    if((fscanf(cpu_use, "%s %s %s %s %s %s %s %s %s %s %s\n", t, &us[0][c], &us[1][c], \
                        &us[2][c], &us[3][c], &us[4][c], &us[5][c], &us[6][c], &us[7][c], &us[8][c], &us[9][c])) != 11) {
                        printf("\nERROR: Reading %s\n", cpuusage);
                        exit(1);
                    }

                    u[0][c] = strtoll(&us[0][c], &endptr, 10);
                    u[1][c] = strtoll(&us[1][c], &endptr, 10);
                    u[2][c] = strtoll(&us[2][c], &endptr, 10);
                    u[3][c] = strtoll(&us[3][c], &endptr, 10);
                    u[4][c] = strtoll(&us[4][c], &endptr, 10);
                    u[5][c] = strtoll(&us[5][c], &endptr, 10);
                    u[6][c] = strtoll(&us[6][c], &endptr, 10);
                    u[7][c] = strtoll(&us[7][c], &endptr, 10);
                    u[8][c] = strtoll(&us[8][c], &endptr, 10);
                    u[9][c] = strtoll(&us[9][c], &endptr, 10);

                    s = u[0][c] + u[1][c] + u[2][c] + u[3][c] + u[4][c] \
                        + u[5][c] + u[6][c] + u[7][c] + u[8][c] + u[9][c];
                    sum = use[0][c] + use[1][c] + use[2][c] + use[3][c] + use[4][c] \
                        + use[5][c] + use[6][c] + use[7][c] + use[8][c] + use[9][c];

                    r = i == 0 && INTERACTIVE_ENABLE != 0 ? (s-sum) : u[3][c] - use[3][c];
                    r /= (s-sum);
                    r = 1 - r;
                    r = r < 0 ? 0 : r * 100;  /* filter out any negative numbers */

                    if(USAGE_ENABLE != 0) {
                        if(QUIET_ENABLE == 0 && RAW_ENABLE == 1) {
                            printf(",%Lf", u[3][c]);
                        }
                        if(QUIET_ENABLE == 0 && RAW_ENABLE == 0 && VERBOSE_ENABLE ==1) {
                            if(c == 0) {
                                printf("CPU = %.2f%% ", r);
                                if(c == USAGE_ENABLE) {
                                    printf("\n");
                                }
                            }
                            else {
                                printf("core%d = %.2f%% ", c, r);
                                if(c == USAGE_ENABLE) {
                                    printf("\n");
                                }
                            }
                        }
                        if(QUIET_ENABLE == 0 && RAW_ENABLE == 0 && COUNT_ENABLE == 1 && VERBOSE_ENABLE == 0) {
                                printf(",%.2f", r);
                        }
                        if(QUIET_ENABLE == 0 && RAW_ENABLE == 0 && COUNT_ENABLE == 0 && VERBOSE_ENABLE == 0) {
                            if(OPTIONS_COUNT >= 1 && c <= USAGE_ENABLE-1) {
                                printf("%.2f,", r);
                            }
                            if(OPTIONS_COUNT >= 1 && c == USAGE_ENABLE-1) {
                                if(OPTIONS_COUNT > 1) {
                                    printf("%.2f,", r);
                                }
                                else {
                                    printf("%.2f", r);
                                }
                            }
                        }
                        if(LOG_ENABLE == 1 && RAW_ENABLE == 1) {
                            fprintf(log_file,",%Lf", u[3][c]);
                        }
                        if(LOG_ENABLE == 1 && RAW_ENABLE == 0) {
                            fprintf(log_file,",%.2f", r);
                        }

                        if(UDP_ENABLE == 1 && RAW_ENABLE == 1 && COUNT_ENABLE == 1) {
                            udp_count += sprintf(udp_tx_data + udp_count,",%Lf", u[3][c]);
                        }
                        if(UDP_ENABLE == 1 && RAW_ENABLE == 1 && COUNT_ENABLE == 0) {
                            if(OPTIONS_COUNT >= 1 && c <= USAGE_ENABLE-1) {
                                udp_count += sprintf(udp_tx_data + udp_count,"%Lf,", u[3][c]);
                            }
                            if(OPTIONS_COUNT >= 1 && c == USAGE_ENABLE-1) {
                                if(OPTIONS_COUNT > 1) {
                                    udp_count += sprintf(udp_tx_data + udp_count,"%Lf,", u[3][c]);
                                }
                                else {
                                    udp_count += sprintf(udp_tx_data + udp_count,"%Lf", u[3][c]);
                                }
                            }
                        }
                        if(UDP_ENABLE == 1 && RAW_ENABLE == 0 && COUNT_ENABLE == 1) {
                            udp_count += sprintf(udp_tx_data + udp_count,",%.2f", r);
                        }
                        if(UDP_ENABLE == 1 && RAW_ENABLE == 0 && COUNT_ENABLE == 0) {
                            if(OPTIONS_COUNT >= 1 && c <= USAGE_ENABLE-1) {
                                udp_count += sprintf(udp_tx_data + udp_count,"%.2f,", r);
                            }
                            if(OPTIONS_COUNT >= 1 && c == USAGE_ENABLE-1) {
                                if(OPTIONS_COUNT > 1) {
                                    udp_count += sprintf(udp_tx_data + udp_count,"%.2f,", r);
                                }
                                else {
                                    udp_count += sprintf(udp_tx_data + udp_count,"%.2f", r);
                                }
                            }
                        }
                    }
                    if(DP_USAGE != 0) {
                        for(uint8_t d = 0; d <= DISPLAY_ENABLE-1; d++) {
                            for(uint8_t i = 0; i <= dp[d].dc_count-1; i++) {
                                if(!strcmp(dp[d].dc[i].name, "usage") && dp[d].page == page) {

                                    uint16_t yloc_reset = dp[d].dc[i].yloc;
                                    char buffer[25];

                                    if(c == 0) {
                                        sprintf(buffer, "CPU ");
                                        strcpy(dp[d].dc[i].data1, buffer);
                                    }
                                    else {
                                        sprintf(buffer, "core%d ", c-1);
                                        strcpy(dp[d].dc[i].data1, buffer);
                                    }
                                    sprintf(buffer, "%.2f", r);
                                    strcpy(dp[d].dc[i].data2, buffer);
                                    dp[d].dc[i].yloc =  dp[d].dc[i].yloc + c*(fontoi(dp[d].dc[i].font));

                                    if(dp[d].dptr(&dp[d], i, DISPLAY_WRITE)){
                                        printf("%s usage cmd %d failed\n", &dp[d].name, i);
                                    }
                                    dp[d].dc[i].yloc = yloc_reset;
                                }
                            }
                        }
                    }
                    use[0][c] = u[0][c];
                    use[1][c] = u[1][c];
                    use[2][c] = u[2][c];
                    use[3][c] = u[3][c];
                    use[4][c] = u[4][c];
                    use[5][c] = u[5][c];
                    use[6][c] = u[6][c];
                    use[7][c] = u[7][c];
                    use[8][c] = u[8][c];
                    use[9][c] = u[9][c];
                }
                if(USAGE_ENABLE != 0) {
                    OPTIONS_COUNT--;
                }
                fclose(cpu_use);
            }
            /*
             * Read memory usage
             */
            if(MEM_ENABLE == 1 || DP_MEMORY != 0) {

                float mt;
                float r = 0;
                float nd;
                char field[30];

                if((mem_load = fopen(memload, "r")) == NULL) {
                    printf("\nERROR: Cannot open %s\n", memload);
                    exit(1);
                }
                while(feof(mem_load) == 0) {
                    fscanf(mem_load, "%29s %f %*s", field, &nd);
                    if (strcmp(mem_total , field) == 0) mt = nd;
                    if (strcmp(mem_avail , field) == 0) r -= nd;
                }
                r /= mt;
                r += 1;
                r *= 100;

                if(MEM_ENABLE == 1) {
                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 1) {
                        printf(",%.3g", r);
                    }
                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 0 && VERBOSE_ENABLE == 1) {
                        printf("\n\n RAM load = %.3g%%", r);
                    }
                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 0 && COUNT_ENABLE == 1 && VERBOSE_ENABLE == 0) {
                            printf(",%.3g", r);
                    }
                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 0 && COUNT_ENABLE == 0 && VERBOSE_ENABLE == 0) {
                        if(OPTIONS_COUNT > 1) {
                            printf("%.3g,", r);
                        }
                        else {
                            printf("%.3g", r);
                        }

                    }
                    if(LOG_ENABLE == 1 && RAW_ENABLE == 1) {
                        fprintf(log_file,",%.3g", r);
                    }
                    if(LOG_ENABLE == 1 && RAW_ENABLE == 0) {
                        fprintf(log_file,",%.3g", r);
                    }
                    if(UDP_ENABLE == 1 && RAW_ENABLE == 1 && COUNT_ENABLE == 1) {
                        udp_count += sprintf(udp_tx_data + udp_count,",%.3g", r);
                    }
                    if(UDP_ENABLE == 1 && RAW_ENABLE == 1 && COUNT_ENABLE == 0) {
                        if(OPTIONS_COUNT > 1) {
                            udp_count += sprintf(udp_tx_data + udp_count,"%.3g,", r);
                        }
                        else {
                            udp_count += sprintf(udp_tx_data + udp_count,"%.3g", r);
                        }
                    }
                    if(UDP_ENABLE == 1 && RAW_ENABLE == 0 && COUNT_ENABLE == 1) {
                        udp_count += sprintf(udp_tx_data + udp_count,",%.3g", r);
                    }
                    if(UDP_ENABLE == 1 && RAW_ENABLE == 0 && COUNT_ENABLE == 0) {
                       if(OPTIONS_COUNT > 1) {
                            udp_count += sprintf(udp_tx_data + udp_count,"%.3g,", r);
                        }
                        else {
                            udp_count += sprintf(udp_tx_data + udp_count,"%.3g", r);
                        }
                    }
                    OPTIONS_COUNT--;
                }
                fclose(mem_load);
                if(DP_MEMORY != 0) {
                    for(uint8_t d = 0; d <= DISPLAY_ENABLE-1; d++) {
                        for(uint8_t i = 0; i <= dp[d].dc_count-1; i++) {
                            if(!strcmp(dp[d].dc[i].name, "memory") && dp[d].page == page) {

                                char buffer[25];

                                sprintf(buffer, "RAM ");
                                strcpy(dp[d].dc[i].data1, buffer);
                                sprintf(buffer, "%.3g", r);
                                strcpy(dp[d].dc[i].data2, buffer);

                                if(dp[d].dptr(&dp[d], i, DISPLAY_WRITE)){
                                    printf("%s memory cmd %d failed\n", &dp[d].name, i);
                                }
                            }
                        }
                    }
                }
            }
            /*
             * eol for stdout and log file
             */
            if(QUIET_ENABLE == 0) {
                printf("\n");
            }

            if(LOG_ENABLE == 1) {
                fprintf(log_file,"\n");
                fclose(log_file);
            }
            if(UDP_ENABLE == 1) {
                udp_count += sprintf(udp_tx_data + udp_count,"\n");
                sendto(udp_socket, udp_tx_data, strlen(udp_tx_data), 0, \
                    (struct sockaddr *)&udp_server_addr, sizeof(struct sockaddr));
            }
            if(DISPLAY_ENABLE != 0) {
                for(uint8_t d = 0; d <= DISPLAY_ENABLE-1; d++) {
                    if(dp[d].page == page) {
                        if(dp[d].dptr(&dp[d], i, DISPLAY_UPDATE)){
                            printf("%s update failed\n", &dp[d].name);
                        }
                    }
                }
            }
            /*
             * break if one and done or sleep
             */
            if(INTERACTIVE_ENABLE == 0) {
                if(DISPLAY_ENABLE == 0 ) {
                    break;
                }
                else {
                    sleep_ms(1000 * dp[page].seconds);
                    if(page < pg_count-1) {
                        page++;     
//                        sleep_ms(1000 * dp[page].seconds);
                    }
                    else {
//                        sleep_ms(1000 * dp[page].seconds);
                        page = 0;
                    }
                    if(SSD1681_ENABLE != 0) {
                        if (ssd1681_gram_clear(&ssd1681_handle, SSD1681_COLOR_BLACK)) {
                            ssd1681_interface_debug_print("ssd1681: gram clear failed.\n");
                            (void)ssd1681_deinit(&ssd1681_handle);
                            return 1;
                        }
                    }
                    if(ST7789_ENABLE != 0) {
                        if (st7789_clear(&st7789_handle)) {
                            st7789_interface_debug_print("st7789: clear failed.\n");
                            (void)st7789_deinit(&st7789_handle);
                            return 1;
                        }
                    }
/*                    if(SSD1306_ENABLE != 0) {
                        if (ssd1306_clear(&ssd1306_handle)) {
                            ssd1306_interface_debug_print("ssd1306: gram clear failed.\n");
                            (void)ssd1306_deinit(&ssd1306_handle);
                            return 1;
                        }
                    }
                    if(SSH1107_ENABLE != 0) {
                        if (ssh1107_clear(&ssh1107_handle)) {
                            ssh1107_interface_debug_print("ssh1107: gram clear failed.\n");
                            (void)ssh1107_deinit(&ssh1107_handle);
                            return 1;
                        }
                    } */
                }
            }
            else {
                OPTIONS_COUNT = c;
                i += (float)INTERACTIVE_ENABLE;
                sleep_ms(INTERACTIVE_ENABLE);
            }
        }
    }
    else {
        /*
         * Build of gnuplot script
         */
        uint16_t i = 0;
        while (i < 11) {
            fprintf(gnuplot_file,"%s",gpscript_start[i]);
            i++;
        }
        /*
         * get thermal zone names and set titles
         */
        char strChar[5] = {0};
        for (uint16_t c = 0; c < THERMAL_ENABLE; c++) {
            itoa(c,strChar);
            strcpy(thermaltype,thermalzone1);
            strcat(thermaltype,strChar);
            strcat(thermaltype,thermaltype1);
            if((thermal_type = fopen(thermaltype, "r")) == NULL) {
                break;
            }
            fscanf(thermal_type, "%s", thermalname);
            fclose(thermal_type);
            fprintf(gnuplot_file,"%s%s\"\n", gpscript_thermal_title[c][0], thermalname);
        }

        if(SENSOR_ENABLE != 0) {
            fprintf(gnuplot_file,"%s%s\"", gpscript_thermal_title[8][0], gpscript_thermal_title[8][1]);
            fprintf(gnuplot_file,"\n%s", gpscript_thermal_title[8][2]);
        }

        i = 0;
        while (i < 18) {
            fprintf(gnuplot_file,"%s",gpscript_mid[i]);
            i++;
        }
        /*
         * set chart layout, size, origin and title
         */
        fprintf(gnuplot_file,"%s",gpscript_layout[0]);

        /* frequency only chart */
        if(SP_ENABLE == 0 && FREQ_ENABLE > 0 && THERMAL_ENABLE == 0 && SENSOR_ENABLE ==0 && USAGE_ENABLE == 0 && MEM_ENABLE ==0) {
            fprintf(gnuplot_file,"%s",one2one);
            strcpy(gpscript_freq1, "set size 1,1\n");
            strcpy(gpscript_freq2, "set origin 0,0\n");
        }
        /* thermal zone only chart */
        if(SP_ENABLE == 0 && FREQ_ENABLE == 0 && (THERMAL_ENABLE > 0 || SENSOR_ENABLE > 0) && USAGE_ENABLE == 0 && MEM_ENABLE ==0) {
            fprintf(gnuplot_file,"%s", one2one);
            strcpy(gpscript_thermal1, "set size 1,1\n");
            strcpy(gpscript_thermal2, "set origin 0,0\n");
        }
        /* usage only chart */
        if(SP_ENABLE == 0 && FREQ_ENABLE == 0 && THERMAL_ENABLE == 0 && SENSOR_ENABLE ==0 && (USAGE_ENABLE > 0 || MEM_ENABLE > 0)) {
            fprintf(gnuplot_file,"%s",one2one);
            strcpy(gpscript_usage1, "set size 1,1\n");
            strcpy(gpscript_usage2, "set origin 0,0\n");
        }
        /* power only chart */
        if(SP_ENABLE > 0 && FREQ_ENABLE == 0 && THERMAL_ENABLE == 0 && SENSOR_ENABLE ==0 && USAGE_ENABLE == 0 && MEM_ENABLE ==0) {
            fprintf(gnuplot_file,"%s",one2one);
            strcpy(gpscript_power1, "set size 1,1\n");
            strcpy(gpscript_power2, "set origin 0,0\n");
        }
        /* frequency and thermal chart*/
        if(SP_ENABLE == 0 && FREQ_ENABLE > 0 && (THERMAL_ENABLE > 0 || SENSOR_ENABLE > 0) && USAGE_ENABLE == 0 && MEM_ENABLE ==0) {
            fprintf(gnuplot_file,"%s", two2one);
            strcpy(gpscript_freq1, "set size 1,.5\n");
            strcpy(gpscript_freq2, "set origin 0,0\n");
            strcpy(gpscript_thermal1, "set size 1,.5\n");
            strcpy(gpscript_thermal2, "set origin 0,.5\n");
        }
        /* frequency and useage chart */
        if(SP_ENABLE == 0 && FREQ_ENABLE > 0 && THERMAL_ENABLE == 0 && SENSOR_ENABLE ==0 && (USAGE_ENABLE > 0 || MEM_ENABLE > 0)) {
            fprintf(gnuplot_file,"%s",two2one);
            strcpy(gpscript_freq1, "set size 1,.5\n");
            strcpy(gpscript_freq2, "set origin 0,.5\n");
            strcpy(gpscript_usage1, "set size 1,.5\n");
            strcpy(gpscript_usage2, "set origin 0,0\n");
        }
        /* frequency and power chart */
        if(SP_ENABLE > 0 && FREQ_ENABLE > 0 && THERMAL_ENABLE == 0 && SENSOR_ENABLE ==0 && USAGE_ENABLE == 0 && MEM_ENABLE ==0) {
            fprintf(gnuplot_file,"%s",two2one);
            strcpy(gpscript_freq1, "set size 1,.5\n");
            strcpy(gpscript_freq2, "set origin 0,.5\n");
            strcpy(gpscript_power1, "set size 1,.5\n");
            strcpy(gpscript_power2, "set origin 0,0\n");
        }
        /* thermal and power chart */
        if(SP_ENABLE > 0 && FREQ_ENABLE == 0 && (THERMAL_ENABLE > 0 || SENSOR_ENABLE > 0) && USAGE_ENABLE == 0 && MEM_ENABLE ==0) {
            fprintf(gnuplot_file,"%s",two2one);
            strcpy(gpscript_thermal1, "set size 1,.5\n");
            strcpy(gpscript_thermal2, "set origin 0,.5\n");
            strcpy(gpscript_power1, "set size 1,.5\n");
            strcpy(gpscript_power2, "set origin 0,0\n");
        }
        /* thermal and useage chart */
        if(SP_ENABLE == 0 && FREQ_ENABLE == 0 && (THERMAL_ENABLE > 0 || SENSOR_ENABLE > 0) && (USAGE_ENABLE > 0 || MEM_ENABLE > 0)) {
            fprintf(gnuplot_file,"%s",two2one);
            strcpy(gpscript_thermal1, "set size 1,.5\n");
            strcpy(gpscript_thermal2, "set origin 0,.5\n");
            strcpy(gpscript_usage1, "set size 1,.5\n");
            strcpy(gpscript_usage2, "set origin 0,0\n");
        }
        /* usage and power chart */
        if(SP_ENABLE > 0 && FREQ_ENABLE == 0 && THERMAL_ENABLE == 0 && SENSOR_ENABLE ==0 && (USAGE_ENABLE > 0 || MEM_ENABLE > 0)) {
            fprintf(gnuplot_file,"%s",two2one);
            strcpy(gpscript_usage1, "set size 1,.5\n");
            strcpy(gpscript_usage2, "set origin 0,.5\n");
            strcpy(gpscript_power1, "set size 1,.5\n");
            strcpy(gpscript_power2, "set origin 0,0\n");
        }
        /* frequency, thermal and power chart */
        if(SP_ENABLE > 0 && FREQ_ENABLE > 0 && (THERMAL_ENABLE > 0 || SENSOR_ENABLE > 0) && USAGE_ENABLE == 0 && MEM_ENABLE ==0) {
            fprintf(gnuplot_file, "%s", three2one);
            strcpy(gpscript_freq1, "set size 1,.2\n");
            strcpy(gpscript_freq2, "set origin 0,.3\n");
            strcpy(gpscript_thermal1, "set size 1,.5\n");
            strcpy(gpscript_thermal2, "set origin 0,.5\n");
            strcpy(gpscript_power1, "set size 1,.3\n");
            strcpy(gpscript_power2, "set origin 0,0\n");
        }
        /* frequency, thermal and usage chart */
        if(SP_ENABLE == 0 && FREQ_ENABLE > 0 && (THERMAL_ENABLE > 0 || SENSOR_ENABLE > 0) && (USAGE_ENABLE > 0 || MEM_ENABLE > 0)) {
            fprintf(gnuplot_file, "%s", three2one);
            strcpy(gpscript_freq1, "set size 1,.2\n");
            strcpy(gpscript_freq2, "set origin 0,.3\n");
            strcpy(gpscript_thermal1, "set size 1,.5\n");
            strcpy(gpscript_thermal2, "set origin 0,.5\n");
            strcpy(gpscript_usage1, "set size 1,.3\n");
            strcpy(gpscript_usage2, "set origin 0,0\n");
        }
        /* frequency, usage and power chart */
        if(SP_ENABLE > 0 && FREQ_ENABLE > 0 && THERMAL_ENABLE == 0 && SENSOR_ENABLE ==0 && (USAGE_ENABLE > 0 || MEM_ENABLE > 0)) {
            fprintf(gnuplot_file, "%s", three2one);
            strcpy(gpscript_freq1, "set size 1,.2\n");
            strcpy(gpscript_freq2, "set origin 0,.3\n");
            strcpy(gpscript_usage1, "set size 1,.5\n");
            strcpy(gpscript_usage2, "set origin 0,.5\n");
            strcpy(gpscript_power1, "set size 1,.3\n");
            strcpy(gpscript_power2, "set origin 0,0\n");
        }
        /* thermal, usage and power chart */
        if(SP_ENABLE > 0 && FREQ_ENABLE > 0 && THERMAL_ENABLE == 0 && SENSOR_ENABLE ==0 && (USAGE_ENABLE > 0 || MEM_ENABLE > 0)) {
            fprintf(gnuplot_file, "%s", three2one);
            strcpy(gpscript_thermal1, "set size 1,.2\n");
            strcpy(gpscript_thermal2, "set origin 0,.3\n");
            strcpy(gpscript_usage1, "set size 1,.5\n");
            strcpy(gpscript_usage2, "set origin 0,.5\n");
            strcpy(gpscript_power1, "set size 1,.3\n");
            strcpy(gpscript_power2, "set origin 0,0\n");
        }
        /* frequency, thermal, usage chart and power chart*/
         if(SP_ENABLE > 0 && FREQ_ENABLE > 0 && (THERMAL_ENABLE > 0 || SENSOR_ENABLE > 0) && (USAGE_ENABLE > 0 || MEM_ENABLE > 0)) {
            fprintf(gnuplot_file, "%s", four2one);
            strcpy(gpscript_thermal1, "set size 1,.35\n");
            strcpy(gpscript_thermal2, "set origin 0,.65\n");
            strcpy(gpscript_freq1, "set size 1,.2\n");
            strcpy(gpscript_freq2, "set origin 0,.45\n");
            strcpy(gpscript_usage1, "set size 1,.15\n");
            strcpy(gpscript_usage2, "set origin 0,.3\n");
            strcpy(gpscript_power1, "set size 1,.3\n");
            strcpy(gpscript_power2, "set origin 0,0\n");
        }

        fprintf(gnuplot_file,"%s",gpscript_layout[1]);
        fprintf(gnuplot_file,"%s",charttitle);
        fprintf(gnuplot_file,"%s",gpscript_layout[2]);
        /*
         * set x axis major tic value
         */
        fprintf(gnuplot_file,"%s",gpscript_xaxis[0]);
        fprintf(gnuplot_file,"%s",gpscript_xaxis[1]);
        fprintf(gnuplot_file,"%s",gpscript_xaxis[2]);
        fprintf(gnuplot_file,"%d",xmtics);
        fprintf(gnuplot_file,"%s",gpscript_xaxis[3]);
        fprintf(gnuplot_file,"%s",gpscript_xaxis[4]);
        /*
         * build thermal zone chart
         */
        if(THERMAL_ENABLE != 0 || SENSOR_ENABLE != 0) {

            i = 0;
            while (i < 9) {
                if(i != 1 && i != 2) {
                    fprintf(gnuplot_file,"%s",gpscript_thermal[i]);
                    i++;
                }
                else {
                    if(i == 1) {
                        fprintf(gnuplot_file,"%s",gpscript_thermal1);
                        i++;
                    }
                    if(i == 2) {
                        fprintf(gnuplot_file,"%s",gpscript_thermal2);
                        i++;
                    }
                }
            }
            i = 0;
            fprintf(gnuplot_file, "plot ");
            if(THERMAL_ENABLE != 0) { 
                fprintf(gnuplot_file, "ARG2 using 1:%d with lines ls %d axes x1y1 title data_title%d", i+(FREQ_ENABLE+2), i+1, i+1);
                i++;
                while (i < THERMAL_ENABLE) {
                    fprintf(gnuplot_file, ", ARG2 using 1:%d with lines ls %d axes x1y1 title data_title%d", i+(FREQ_ENABLE+2), i+1, i+1);
                    i++;
                }
                if(SENSOR_ENABLE != 0) {
                    fprintf(gnuplot_file, ", ARG2 using 1:%d with lines ls 9 axes x1y1 title data_title9", i+(FREQ_ENABLE+2));
                }
            }
            if(THERMAL_ENABLE == 0 && SENSOR_ENABLE != 0) {
                fprintf(gnuplot_file, "ARG2 using 1:%d with lines ls 9 axes x1y1 title data_title9", i+(FREQ_ENABLE+2));
            }

            fprintf(gnuplot_file, "\n\n");
        }
        /*
         * build frequency chart
         */
        if(FREQ_ENABLE != 0) {

            i = 0;
            while (i < 9) {
                if(i != 1 && i != 2) {
                    fprintf(gnuplot_file,"%s",gpscript_freq[i]);
                    i++;
                }
                else {
                    if(i == 1) {
                        fprintf(gnuplot_file,"%s",gpscript_freq1);
                        i++;
                    }
                    if(i == 2) {
                        fprintf(gnuplot_file,"%s",gpscript_freq2);
                        i++;
                    }
                }
            }

            i = 0;
            fprintf(gnuplot_file, "plot ");
            fprintf(gnuplot_file, "ARG2 using 1:%d with lines ls %d axes x1y1 notitle", i+2, i+1);
            i++;
            while (i < FREQ_ENABLE) {
                fprintf(gnuplot_file, ", ARG2 using 1:%d with lines ls %d axes x1y1", i+2, i+1);
                i++;
            }
            fprintf(gnuplot_file, "\n\n");
        }
        /*
         * build power chart
         */
        if(SP_ENABLE != 0) {

            i = 0;
            while (i < 11) {
                if(i != 1 && i != 2) {
                    fprintf(gnuplot_file,"%s",gpscript_power[i]);
                    i++;
                }
                else {
                    if(i == 1) {
                        fprintf(gnuplot_file,"%s",gpscript_power1);
                        i++;
                    }
                    if(i == 2) {
                        fprintf(gnuplot_file,"%s",gpscript_power2);
                        i++;
                    }
                }
            }

            fprintf(gnuplot_file, "plot ");

            if(SENSOR_ENABLE == 0) {
                fprintf(gnuplot_file, "ARG2 using 1:%d with lines ls 4 axes x1y1 notitle", (FREQ_ENABLE+THERMAL_ENABLE+2));
                fprintf(gnuplot_file, ", ARG2 using 1:%d with lines ls 5 axes x1y1", (FREQ_ENABLE+THERMAL_ENABLE+3));
                fprintf(gnuplot_file, ", ARG2 using 1:%d with lines ls 9 axes x1y1\n\n", (FREQ_ENABLE+THERMAL_ENABLE+4));
            }
            else {
                fprintf(gnuplot_file, "ARG2 using 1:%d with lines ls 4 axes x1y1 notitle", (FREQ_ENABLE+THERMAL_ENABLE+3));
                fprintf(gnuplot_file, ", ARG2 using 1:%d with lines ls 5 axes x1y1", (FREQ_ENABLE+THERMAL_ENABLE+4));
                fprintf(gnuplot_file, ", ARG2 using 1:%d with lines ls 9 axes x1y1\n\n", (FREQ_ENABLE+THERMAL_ENABLE+5));
            }
        }
        /*
         * build usage chart
         */
        if(USAGE_ENABLE != 0 || MEM_ENABLE != 0) {

            uint16_t i = 0;
            while (i < 11) {
                if(i != 1 && i != 2) {
                    fprintf(gnuplot_file,"%s",gpscript_usage[i]);
                    i++;
                }
                else {
                    if(i == 1) {
                        fprintf(gnuplot_file,"%s",gpscript_usage1);
                        i++;
                    }
                    if(i == 2) {
                        fprintf(gnuplot_file,"%s",gpscript_usage2);
                        i++;
                    }
                }
            }

            i = 0;
            uint8_t power = SP_ENABLE > 0 ? 3 : 0;
            fprintf(gnuplot_file, "plot ");
            if(USAGE_ENABLE != 0) {
                fprintf(gnuplot_file, "ARG2 using 1:%d with lines ls 9 axes x1y1 notitle", (FREQ_ENABLE+THERMAL_ENABLE+power)+i+3);
                i++;
                while (i < USAGE_ENABLE) {
                    fprintf(gnuplot_file, ", ARG2 using 1:%d with lines ls %d axes x1y1", (FREQ_ENABLE+THERMAL_ENABLE+power)+i+3, i+1);
                    i++;
                }
                if(MEM_ENABLE != 0) {
                    fprintf(gnuplot_file, ", ARG2 using 1:%d with lines ls 9 axes x1y1", (FREQ_ENABLE+THERMAL_ENABLE+power)+i+3);
                }
            }
            if(USAGE_ENABLE == 0 && MEM_ENABLE != 0) {
                fprintf(gnuplot_file, "ARG2 using 1:%d with lines ls 9 axes x1y1", (FREQ_ENABLE+THERMAL_ENABLE+power)+i+2);
            }
            fprintf(gnuplot_file, "\n\n");
        }
        fprintf(gnuplot_file,"%s",gpscript_end);
        fclose(gnuplot_file);
    }
    if (SP_ENABLE != 0) {
        close(pwr_in);
    }
    if (UDP_ENABLE == 1) {
        close(udp_socket);
    }
    if (DISPLAY_ENABLE != 0) {
        if(SSD1681_ENABLE != 0) {
            (void)ssd1681_deinit(&ssd1681_handle);
        }
        if(SSD1306_ENABLE != 0) {
            (void)ssd1306_deinit(&ssd1306_handle);
        }
        if(SSH1107_ENABLE != 0) {
            (void)ssh1107_deinit(&ssh1107_handle);
        }
        if(ST7789_ENABLE != 0) {
            (void)st7789_deinit(&st7789_handle);
        }
    }
    if (SCD30_ENABLE == 1 || DP_SCD30 != 0) {
        (void)scd30_basic_deinit();
    }
    if (SCD4X_ENABLE == 1 || DP_SCD4X != 0) {
        (void)scd4x_shot_deinit();
    }
    if (SGP30_ENABLE == 1 || DP_SGP30 != 0) {
        (void)sgp30_advance_deinit();
    }
    if (SENSOR_ENABLE == 1 || DP_BMP180 != 0) {
        (void)bmp180_basic_deinit();
    }
    if (BMP388_ENABLE == 1 || DP_BMP388 != 0) {
        (void)bmp388_basic_deinit();
    }
    if (BMP390_ENABLE == 1 || DP_BMP390 != 0) {
        (void)bmp390_basic_deinit();
    }
    if (SENSOR_ENABLE == 2 || DP_BME280 != 0) {
        (void)bme280_basic_deinit();
    }
    if (SENSOR_ENABLE == 8 || DP_BME680 != 0) {
        (void)bme680_gas_deinit();
    }
    if (SENSOR_ENABLE == 3 || DP_MCP9808 != 0) {
        close(mcp9808_in);
    }
    if (SENSOR_ENABLE == 4 || DP_SHT4X != 0) {
        (void)sht4x_basic_deinit();
    }
    if (SENSOR_ENABLE == 5 || DP_SHTC3 != 0) {
        (void)shtc3_basic_deinit();
    }
    if (SENSOR_ENABLE == 6 || DP_AHT20 != 0) {
        (void)aht20_basic_deinit();
    }
    if (SENSOR_ENABLE == 7 || DP_HTU31D != 0) {
        (void)htu31d_basic_deinit();
    }
}


void usage (void) {
        printf("\nlogenv - Version %s Copyright (C) 2019,2020,2024,2025,2026 by Edward A. Kisiel\n", version);
        printf("logs count or time stamp, CPU frequency, thermal zone temperatures,\n");
        printf("external sensor temperature, volts, amps and watts and CPU core usage\n\n");
        printf("usage: logenv [options]\n\n");
        printf("Options:\n");
        printf(" -l,  --log <file>            Log to <file>\n");
        printf(" -i,  --milliseconds <number> Poll Interval <number> in milliseconds\n");
        printf(" -f,  --frequency             CPU core frequency\n");
        printf(" -t,  --temperature           Thermal zone temperature\n");
        printf(" -a,  --bme280 <device>       Temperature, Humidity, Pressure Sensor I2C 0x76 or 0x77 default /dev/i2c-0\n");
        printf("      --bmp180 <device>       Barometric Pressure, Altitude & Temperature Sensor default /dev/i2c-0\n");
        printf("      --bmp388 <device>       Barometric Pressure, Altitude & Temperature Sensor I2C 0x76 or 0x77\n");
        printf("      --bmp390 <device>       Barometric Pressure, Altitude & Temperature Sensor I2C 0x76 or 0x77\n");
        printf("      --mcp9808 <device>      High Accuracy Temperature Sensor I2C 0x18 default /dev/i2c-0\n");
        printf("      --sht4x <device>        Temperature and Humidity I2C 0x44 default /dev/i2c-0\n");
        printf("      --shtc3 <device>        Temperature and Humidity I2C 0x70 default /dev/i2c-0\n");
        printf("      --aht20 <device>        Temperature and Humidity I2C 0x70 default /dev/i2c-0\n");
        printf("      --htu31d <device>       Temperature and Humidity I2C 0x40 default /dev/i2c-0\n");
        printf(" -p,  --smartpower3-ch1 <tty> Volt, Amp, Watt (HK SmartPower3 USBC port), default /dev/ttyUSB0\n");
        printf("      --smartpower3-ch2 <tty>\n");
        printf("      --smartpower2 <tty>     Volt, Amp, Watt (HK SmartPower2 microUSB port), default /dev/ttyUSB0\n");
        printf(" -u,  --usage                 CPU core usage, aggregate and core 0 to core n-1\n");
        printf(" -m,  --memory                Physical memory usage (total - available, see man free)\n");
        printf(" -d,  --date                  Date and Time stamp\n");
        printf(" -r,  --raw                   Raw output, no formatting of freq. or temp.  e.g. 35000 instead of 35\n");
        printf(" -v,  --verbose               Readable dashboard output\n"); 
        printf(" -q,  --quiet                 No output to stdout\n");
        printf(" -o,                          Output to eInk/Oled/LCD display using logenv.json\n");
        printf(" -n,  --udp <host>:<port>     UDP output to <host>:<port>\n");
        printf(" -s,  --sgp30 <device>        VOC and eCO2 Sensor I2C 0x58 default /dev/i2c-0\n");
        printf("      --scd30 <device>        CO2 Temperature and Humidity Sensor I2C 0x61 default /dev/i2c-0\n");
        printf("      --scd41 <device>        CO2 Temperature and Humidity Sensor I2C 0x62 default /dev/i2c-0\n");
        printf(" -g,  --gnuplot <file>        Gnuplot script generation\n");
        printf("      --title <string>        Chart title <string>\n");
        printf("      --xmtics <number>       Chart x-axis major second tics <number>\n");
        printf("      --version               Version\n");
        printf(" -h,  --help                  Help screen\n\n");
        printf("Example:\n\n");
        printf("Data capture every 2 seconds for frequency, thermal zones, ambient temperature and SmartPower3-ch1:\n");
        printf("logenv -l logfile.csv -i 2000 -f -t -a /dev/i2c-1 -p\n\n");
        printf("Gnuplot script generation for data capture:\n");
        printf("logenv -g gplotscript.gpl --title \"logenv GNUPlot Chart\" --xmtics 60 -i -f -t -a -p \n\n");
        printf("Gnuplot chart creation:\n");
        printf("gnuplot -c gplotscript.gpl chart.png logfile.csv\n\n");
        exit(0);
}


int16_t itoa(int32_t n, char s[]) {
    int16_t i =  0;

    if(n / 10 != 0)
        i = itoa(n/10, s);
    else if(n < 0)
        s[i++] = '-';

    s[i++] = abs(n % 10) + '0';
    s[i] = '\0';

    return i;
}


int16_t set_tty_attributes(int16_t fd, int32_t speed, bool canconical) {
    struct termios tty;

    if (tcgetattr(fd, &tty) < 0) {
        printf("Error from tcgetattr: %s\n", strerror(errno));
        return -1;
    }

    cfsetospeed(&tty, (speed_t)speed);
    cfsetispeed(&tty, (speed_t)speed);

    tty.c_cflag |= CLOCAL | CREAD;
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;                       /* 8-bit characters */
    tty.c_cflag &= ~PARENB;                   /* no parity bit */
    tty.c_cflag &= ~CSTOPB;                   /* 1 stop bit */
    tty.c_cflag &= ~CRTSCTS;                  /* no hardware flowcontrol */
    tty.c_lflag |= canconical == true ? ICANON | ISIG : ~ICANON | ISIG;  /* canonical input */
    tty.c_lflag &= ~(ECHO | ECHOE | ECHONL | IEXTEN);
    tty.c_iflag &= ~IGNCR;                    /* preserve carriage return */
    tty.c_iflag &= ~INPCK;
    tty.c_iflag &= ~(INLCR | ICRNL | IUCLC | IMAXBEL);
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);   /* no SW flowcontrol */

    tty.c_oflag &= ~OPOST;

    tty.c_cc[VEOL] = 0;
    tty.c_cc[VEOL2] = 0;
    tty.c_cc[VEOF] = 0x04;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        printf("Error from tcsetattr: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}


void sleep_ms(int32_t milliseconds) {
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
}


static void sig_handler(int _) {
    (void)_;
    go = -1;
}
