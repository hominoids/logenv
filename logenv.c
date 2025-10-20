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

   logenv - logs count or time stamp, thermal zone temperature, cpu frequency,
            air temperature(BME280, BMP180, MCP9808),
            volts, amps and watts (HK SmartPower2 or SmartPower3 output)


    void usage (void)
    int itoa(int n, char s[])
    int set_tty_attributes(int fd, int speed, bool canconical)
    void sleep_ms(int milliseconds)
    static void sig_handler(int _)

*/
#include <arpa/inet.h>
#include <errno.h>
#include <ctype.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>
#include <cjson/cJSON.h>
#include "drivers/ssd1681/driver_ssd1681_basic.h"
#include "drivers/ssd1681/driver_ssd1681_interface.h"
#include "drivers/bmp180/driver_bmp180_basic.h"
#include "drivers/bme280/driver_bme280_basic.h"
#include "drivers/mcp9808/mcp9808.h"
#include "drivers/scd4x/driver_scd4x_basic.h"
#include "drivers/scd4x/driver_scd4x_shot.h"
#include "drivers/sgp30/driver_sgp30_advance.h"
#include "displays.h"
#include "logenv.h"

int main(int argc, char **argv) {

    signal(SIGINT, sig_handler);

    struct display dp[4];
    cJSON *iterator = NULL;
    scd4x_t chip_type = SCD41;


    if(argc == 1) {
        usage();
    }
    /*
     * check if creating gnuplot script
     */
    int i = argc;
    while (i-- > 1) {

        if(!strcmp(argv[i], "-g") || !strcmp(argv[i], "--gnuplot")) {
            strcpy(gplotfile, argv[i+1]);

            if((gnuplot_file = fopen(gplotfile, "w")) == NULL) {
                printf("\nERROR: Cannot open gnuplot script file %s\n\n", argv[i+1]);
                usage();
            }
            GNUPLOT_ENABLE = 1;
        }
        if(!strcmp(argv[i], "-o")) {
            if((json_file = fopen("logenv.json", "r")) == NULL) {
                printf("\nERROR: Cannot open file logenv.json\n\n");
                usage();
            }

            char buffer[16384];
            int len = fread(buffer, 1, sizeof(buffer), json_file);
            fclose(json_file);
printf("json file read...\n");
            cJSON *root = cJSON_Parse(buffer);
            if (!cJSON_IsObject(root)) {
                printf("\nERROR: Cannot parse JSON root structure.\n");
                return EXIT_FAILURE;
            }
printf("json root structure parsed...\n");
            cJSON *display = cJSON_GetObjectItemCaseSensitive(root, "displays");
            cJSON *item = display ? display->child : 0;
            while (item)
            {
                cJSON *name = cJSON_GetObjectItemCaseSensitive(item, "name");
                if (cJSON_IsString(name) && (name->valuestring != NULL)) {
                    strcpy(dp[DISPLAY_ENABLE].name, name->valuestring);
printf("Displays: %s ", &dp[DISPLAY_ENABLE].name);
                }
                cJSON *device = cJSON_GetObjectItemCaseSensitive(item, "device");
                if (cJSON_IsString(device) && (device->valuestring)) {
                    strcpy(dp[DISPLAY_ENABLE].device, device->valuestring);
printf("%s ", &dp[DISPLAY_ENABLE].device);
                }
                cJSON *address = cJSON_GetObjectItemCaseSensitive(item, "address");
                if (cJSON_IsNumber(address)) {
                    dp[DISPLAY_ENABLE].address = address->valueint;
printf("%d ", dp[DISPLAY_ENABLE].address);
                }
                cJSON *xsize = cJSON_GetObjectItemCaseSensitive(item, "xsize");
                if (cJSON_IsNumber(xsize)) {
                    dp[DISPLAY_ENABLE].xsize = xsize->valueint;
printf("%d ", dp[DISPLAY_ENABLE].xsize);
                }
                cJSON *ysize = cJSON_GetObjectItemCaseSensitive(item, "ysize");
                if (cJSON_IsNumber(ysize)) {
                    dp[DISPLAY_ENABLE].ysize = ysize->valueint;
printf("%d ", dp[DISPLAY_ENABLE].ysize);
                }
                cJSON *rotation = cJSON_GetObjectItemCaseSensitive(item, "rotation");
                if (cJSON_IsNumber(rotation)) {
                    dp[DISPLAY_ENABLE].rotation = rotation->valueint;
printf("%d\n", dp[DISPLAY_ENABLE].rotation);
                }
                int ac = 0;
                cJSON *contents = cJSON_GetObjectItemCaseSensitive(item, "content");
                cJSON_ArrayForEach(iterator, contents)
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

printf("%s ", &dp[DISPLAY_ENABLE].dc[ac].name);
printf("%s ", &dp[DISPLAY_ENABLE].dc[ac].device);
printf("%d ", dp[DISPLAY_ENABLE].dc[ac].address);
printf("%s ", &dp[DISPLAY_ENABLE].dc[ac].type);
printf("%d ", dp[DISPLAY_ENABLE].dc[ac].xloc);
printf("%d ", dp[DISPLAY_ENABLE].dc[ac].yloc);
printf("%d ", dp[DISPLAY_ENABLE].dc[ac].color);
printf("%s ", &dp[DISPLAY_ENABLE].dc[ac].font);
printf("%s ", &dp[DISPLAY_ENABLE].dc[ac].label);
printf("%s\n", &dp[DISPLAY_ENABLE].dc[ac].unit);

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
                    if(!strcmp(dp[DISPLAY_ENABLE].dc[ac].name,"memory")) {
                        DP_MEMORY++;
                    }
                    if(!strcmp(dp[DISPLAY_ENABLE].dc[ac].name,"usage")) {
                        DP_USAGE++;
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
                        DP_BMP180++;
                    }
                    if(!strcmp(dp[DISPLAY_ENABLE].dc[ac].name,"bme280")) {
                        DP_BME280++;
                    }
                    if(!strcmp(dp[DISPLAY_ENABLE].dc[ac].name,"mcp9808")) {
                        DP_MCP9808++;
                    }
                    if(!strcmp(dp[DISPLAY_ENABLE].dc[ac].name,"scd41")) {
                        DP_SCD41++;
                    }
                    if(!strcmp(dp[DISPLAY_ENABLE].dc[ac].name,"sgp30")) {
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
                if(!strcmp(dp[DISPLAY_ENABLE].name, "ssd1681")) {
                    if(displays(ssd1681, &dp[DISPLAY_ENABLE], 0, DISPLAY_OPEN)) {
                        printf("%s open failed\n", &dp[DISPLAY_ENABLE].name);
                        exit(0);
                    }
                }
                if(!strcmp(dp[DISPLAY_ENABLE].name, "ssd1306")) {
                    if(displays(ssd1306, &dp[DISPLAY_ENABLE], 0, DISPLAY_OPEN)) {
                        printf("%s open failed\n", &dp[DISPLAY_ENABLE].name);
                        exit(0);
                    }
                }
                DISPLAY_ENABLE++;
printf("display %d complete...\n", DISPLAY_ENABLE);
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
        if(!strcmp(argv[i], "-u") || !strcmp(argv[i], "--usage")) {
            if((cpu_online = fopen(cpuonline, "r")) == NULL) {
                printf("\nERROR: Cannot open %s\n", cpuonline);
                exit(1);
            }
            size_t size = 0;
            char *line = 0;
            ssize_t linesize = getline(&line, &size, cpu_online);
            fclose(cpu_online);
            USAGE_ENABLE = atoi(&line[2])+1;
            OPTIONS_COUNT++;
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

            for (int c = 0; c <= 1024; c++) {
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
         * ambient temperature command line options
         */
        if(!strcmp(argv[i], "-a") || !strcmp(argv[i], "--bme280") || DP_BME280 >= 1) {
            if(GNUPLOT_ENABLE != 1) {
                if((i+1) < argc && !strncmp("/dev/", argv[i+1], 5)) {
                    interface = argv[i+1];
                }
                if((i+2) < argc && !strncmp("/dev/", argv[i+2], 5)) {
                    interface = argv[i+2];
                }

                if(bme280_basic_init(BME280_INTERFACE_IIC, BME280_ADDRESS_ADO_HIGH) != 0) {
                    printf("\nERROR: Cannot open BME280 at %s\n", interface);
                    exit(1);
                }
            }
            if(!strcmp(argv[i], "-a") || !strcmp(argv[i], "--bme280")) {
                SENSOR_ENABLE = 2;
                OPTIONS_COUNT++;
            }
        }
        if(!strcmp(argv[i], "--bmp180") || DP_BMP180 >= 1) {
            if(GNUPLOT_ENABLE != 1) {
                if((i+1) < argc && !strncmp("/dev/", argv[i+1], 5)) {
                    sensor = argv[i+1];
                }
                if((i+2) < argc && !strncmp("/dev/", argv[i+2], 5)) {
                    sensor = argv[i+2];
                }
                if(bmp180_basic_init() != 0) {
                    printf("\nERROR: Cannot open BMP180 at %s\n", interface);
                    exit(1);
                }
            }
            if(!strcmp(argv[i], "--bmp180")) {
                SENSOR_ENABLE = 1;
                OPTIONS_COUNT++;
            }
        }
        if(!strcmp(argv[i], "--mcp9808") || DP_MCP9808 >= 1) {
            if(GNUPLOT_ENABLE != 1) {
                if((i+1) < argc && !strncmp("/dev/", argv[i+1], 5)) {
                    sensor = argv[i+1];
                }
                if((i+2) < argc && !strncmp("/dev/", argv[i+2], 5)) {
                    sensor = argv[i+2];
                }
                if((sensor_in = open(sensor, O_RDWR)) < 0) {
                    printf("\nERROR: Cannot open MCP9808 at %s\n", sensor);
                    exit(1);
                }
                mcp9808_open();
            }
            if(!strcmp(argv[i], "--mcp9808")) {
                SENSOR_ENABLE = 3;
                OPTIONS_COUNT++;
            }
        }
        /*
         * scd41 command line options
         */
        if(!strcmp(argv[i], "--scd41")) {
            if(GNUPLOT_ENABLE != 1) {
                if((i+1) < argc && !strncmp("/dev/", argv[i+1], 5)) {
                    sensor = argv[i+1];
                }
                if((i+2) < argc && !strncmp("/dev/", argv[i+2], 5)) {
                    sensor = argv[i+2];
                }
                if((scd41_in = open(sensor, O_RDWR)) < 0) {
                    printf("\nERROR: Cannot open SCD41 at %s\n", sensor);
                    exit(1);
                }
            }
            OPTIONS_COUNT++;
        }
        if(DP_SCD41 >= 1) {
            if (scd4x_shot_init(chip_type)) {
                printf("\nERROR: SCD41 Init failed.\n");
                exit(1);
            }
        }
        /*
         * sgp30 command line options
         */
        if(!strcmp(argv[i], "--sgp30")) {
            if(GNUPLOT_ENABLE != 1) {
                if((i+1) < argc && !strncmp("/dev/", argv[i+1], 5)) {
                    sensor = argv[i+1];
                }
                if((i+2) < argc && !strncmp("/dev/", argv[i+2], 5)) {
                    sensor = argv[i+2];
                }
                if((sgp30_in = open(sensor, O_RDWR)) < 0) {
                    printf("\nERROR: Cannot open SGP30 at %s\n", sensor);
                    exit(1);
                }
            }
            OPTIONS_COUNT++;
        }
        if(DP_SGP30 >= 1) {
            if (sgp30_advance_init()) {
                printf("\nERROR: SGP30 Init failed.\n");
                exit(1);
            }
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
        float i = 0;
        int c = OPTIONS_COUNT;
        while(i >= 0 && go != -1) {
            int udp_count = 0;
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
            if(DP_TIME >= 1 || DP_DATE >= 1){
                for(int d = 0; d <= DISPLAY_ENABLE-1; d++) {
                    for(int i = 0; i <= dp[d].dc_count-1; i++) {
                        if(DISPLAY_ENABLE >= 1 && DP_TIME >= 1 && !strcmp(dp[d].dc[i].name, "time")) {
                            int count = 0;
                            int result = 0;
                            now = time((time_t *)NULL);
                            t = localtime(&now);
                            count = sprintf(display_time,"%02d:%02d",t->tm_hour, t->tm_min);
                            if(!strcmp(dp[d].name,"ssd1681")) {
                                if(displays(ssd1681, &dp[d], i, DISPLAY_TIME)){
                                    printf("%s time failed\n", &dp[d].name);
                                }
                            }

                            if(!strcmp(dp[d].name,"ssd1306")) {
                                if(displays(ssd1306, &dp[d], i, DISPLAY_TIME)){
                                    printf("%s time failed\n", &dp[d].name);
                                }
                            }
                        }
                        if(DISPLAY_ENABLE >= 1 && DP_DATE >= 1 && !strcmp(dp[d].dc[i].name, "date")) {
                            int count,result = 0;
                            now = time((time_t *)NULL);
                            t = localtime(&now);
                            count = sprintf(display_date,"%02d/%02d/%4d", t->tm_mon+1, t->tm_mday, t->tm_year+1900);

                            if(!strcmp(dp[d].name,"ssd1681")) {
                                if(displays(ssd1681, &dp[d], i, DISPLAY_DATE)){
                                    printf("%s date failed\n", i, &dp[d].name);
                                }
                            }

                            if(!strcmp(dp[d].name,"ssd1306")) {
                                if(displays(ssd1306, &dp[d], i, DISPLAY_DATE)){
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
                    for (int c = 0; c < FREQ_ENABLE; c++) {
                        printf("  Core%d  ", c);
                    }
                    printf("\n");
                }
                for (int c = 0; c < FREQ_ENABLE; c++) {
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
            if(DP_FREQ >= 1) {
                for (int c = 0; c < FREQ_ENABLE; c++) {
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

                    for(int d = 0; d <= DISPLAY_ENABLE-1; d++) {
                        for(int i = 0; i <= dp[d].dc_count-1; i++) {
                            if(!strcmp(dp[d].dc[i].name, "frequency")) {
                                int yloc_reset = dp[d].dc[i].yloc;
                                char buffer[25];
                                sprintf(buffer, "core%d ", c);
                                strcpy(dp[d].dc[i].data1, buffer);
                                sprintf(buffer, "%.2lf", (double)freq/1000000);
                                strcpy(dp[d].dc[i].data2, buffer);
                                dp[d].dc[i].yloc =  dp[d].dc[i].yloc + c*(fontoi(dp[d].dc[i].font));

                                if(!strcmp(dp[d].name,"ssd1681")) {
                                    if(displays(ssd1681, &dp[d], i, DISPLAY_WRITE)){
                                        printf("%s thermal cmd %d failed\n", &dp[d].name, i);
                                    }
                                }

                                if(!strcmp(dp[d].name,"ssd1306")) {
                                    if(displays(ssd1306, &dp[d], i, DISPLAY_WRITE)){
                                        printf("%s thermal cmd %d failed\n", &dp[d].name, i);
                                    }
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
                for (int c = 0; c < THERMAL_ENABLE; c++) {
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
            if(DP_THERMAL >= 1) {
                for (int c = 0; c < THERMAL_ENABLE; c++) {
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

                    for(int d = 0; d <= DISPLAY_ENABLE-1; d++) {
                        for(int i = 0; i <= dp[d].dc_count-1; i++) {
                            if(!strcmp(dp[d].dc[i].name, "thermal")) {
                                int yloc_reset = dp[d].dc[i].yloc;
                                char buffer[25];
                                sprintf(buffer, "%s", thermalname);
                                strcpy(dp[d].dc[i].data1, buffer);
                                sprintf(buffer, "%.2lf", coretemp/1000);
                                strcpy(dp[d].dc[i].data2, buffer);
                                dp[d].dc[i].yloc =  dp[d].dc[i].yloc + c*(fontoi(dp[d].dc[i].font));

                                if(!strcmp(dp[d].name,"ssd1681")) {
                                    if(displays(ssd1681, &dp[d], i, DISPLAY_WRITE)){
                                        printf("%s thermal cmd %d failed\n", &dp[d].name, i);
                                    }
                                }

                                if(!strcmp(dp[d].name,"ssd1306")) {
                                    if(displays(ssd1306, &dp[d], i, DISPLAY_WRITE)){
                                        printf("%s thermal cmd %d failed\n", &dp[d].name, i);
                                    }
                                }
                                dp[d].dc[i].yloc = yloc_reset;
                            }
                        }
                    }
                }
            }
            /*
             * read mcp9808 temperature sensor
             */
            if(SENSOR_ENABLE == 3 || DP_MCP9808 >= 1) {
                float temperature = mcp9808_read();
                if(SENSOR_ENABLE == 3) {
                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 1) {
                        printf(",%d", temperature);
                    }
                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 0 && VERBOSE_ENABLE == 1) {
                        printf("\n\n MCP9808 Sensor = %.2lfc", temperature);
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
                if(DP_MCP9808 >= 1) {
                    for(int d = 0; d <= DISPLAY_ENABLE-1; d++) {
                        for(int i = 0; i <= dp[d].dc_count-1; i++) {
                            if(!strcmp(dp[d].dc[i].name, "mcp9808")) {
                                char buffer[6];
                                sprintf(buffer, "%.2lf", temperature);
                                strcpy(dp[d].dc[i].data1, buffer);

                                if(!strcmp(dp[d].name,"ssd1681")) {
                                    if(displays(ssd1681, &dp[d], i, DISPLAY_SENSOR)){
                                        printf("%s mcp9808 cmd %d failed\n", &dp[d].name, i);
                                    }
                                }

                                if(!strcmp(dp[d].name,"ssd1306")) {
                                    if(displays(ssd1306, &dp[d], i, DISPLAY_SENSOR)){
                                        printf("%s mcp9808 cmd %d failed\n", &dp[d].name, i);
                                    }
                                }
                            }
                        }
                    }
                }
            }
            /*
             * read bme280
             */
            if(SENSOR_ENABLE == 2) {

                float temperature_f;
                float humidity_f;
                float pressure_f;

                int res = bme280_basic_read((float *)&temperature_f, (float *)&pressure_f, (float *)&humidity_f);
                if (res != 0)
                {
                    (void)bme280_basic_deinit();
                    printf("ERROR: bme280 read failed.\n");
                    return 1;
                }

                if(QUIET_ENABLE == 0 && RAW_ENABLE == 1) {
                    printf(",%f", temperature_f);
                }
                if(QUIET_ENABLE == 0 && RAW_ENABLE == 0 && VERBOSE_ENABLE == 1) {
                    printf("\n\n BME280 Sensor = %.2lfc", temperature_f);
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
            if(DP_BME280 >= 1) {

                float temperature_f;
                float humidity_f;
                float pressure_f;

                int res = bme280_basic_read((float *)&temperature_f, (float *)&pressure_f, (float *)&humidity_f);
                if (res != 0)
                {
                    (void)bme280_basic_deinit();
                    printf("ERROR: bme280 read failed.\n");
                    return 1;
                }

                for(int d = 0; d <= DISPLAY_ENABLE-1; d++) {
                    for(int i = 0; i <= dp[d].dc_count-1; i++) {
                        if(!strcmp(dp[d].dc[i].name, "bme280")) {
                            char buffer[25];
                            sprintf(buffer, "%.2lf", temperature_f);
                            strcpy(dp[d].dc[i].data1, buffer);
                            sprintf(buffer, "%.2lf", humidity_f);
                            strcpy(dp[d].dc[i].data2, buffer);
                            sprintf(buffer, "%.2lf", pressure_f/100);
                            strcpy(dp[d].dc[i].data3, buffer);

                            if(!strcmp(dp[d].name,"ssd1681")) {
                                if(displays(ssd1681, &dp[d], i, DISPLAY_SENSOR)){
                                    printf("%s bme280 cmd %d failed\n", &dp[d].name, i);
                                }
                            }

                            if(!strcmp(dp[d].name,"ssd1306")) {
                                if(displays(ssd1306, &dp[d], i, DISPLAY_SENSOR)){
                                    printf("%s bme280 cmd %d failed\n", &dp[d].name, i);
                                }
                            }
                        }
                    }
                }
            }
            /*
             * read bmp180
             */
            if(SENSOR_ENABLE == 1) {

                float temperature_f;
                float humidity_f;
                uint32_t pressure;

                int res = bmp180_basic_read((float *)&temperature_f, (uint32_t *)&pressure);
                if (res != 0)
                {
                    bmp180_basic_deinit();
                    printf("ERROR: bmp180 read failed.\n");
                    return 1;
                }
                if(QUIET_ENABLE == 0 && RAW_ENABLE == 1) {
                    printf(",%f", temperature_f);
                }
                if(QUIET_ENABLE == 0 && RAW_ENABLE == 0 && COUNT_ENABLE == 1 && VERBOSE_ENABLE == 1) {
                    printf("\n\n BMP180 Sensor = %.2lfc", temperature_f);
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
            if(DP_BMP180 >= 1) {

                float temperature_f;
                float humidity_f;
                uint32_t pressure;

                int res = bmp180_basic_read((float *)&temperature_f, (uint32_t *)&pressure);
                if (res != 0)
                {
                    bmp180_basic_deinit();
                    printf("ERROR: bmp180 read failed.\n");
                    return 1;
                }

                for(int d = 0; d <= DISPLAY_ENABLE-1; d++) {
                    for(int i = 0; i <= dp[d].dc_count-1; i++) {
                        if(!strcmp(dp[d].dc[i].name, "bmp180")) {
                            char buffer[25];
                            sprintf(buffer, "%.2lf", temperature_f);
                            strcpy(dp[d].dc[i].data1, buffer);
                            sprintf(buffer, "%.2lf", (double) pressure/100);
                            strcpy(dp[d].dc[i].data3, buffer);

                            if(!strcmp(dp[d].name,"ssd1681")) {
                                if(displays(ssd1681, &dp[d], i, DISPLAY_SENSOR)){
                                    printf("%s bmp180 cmd %d failed\n", &dp[d].name, i);
                                }
                            }

                            if(!strcmp(dp[d].name,"ssd1306")) {
                                if(displays(ssd1306, &dp[d], i, DISPLAY_SENSOR)){
                                    printf("%s bmp180 cmd %d failed\n", &dp[d].name, i);
                                }
                            }
                        }
                    }
                }
            }
             /*
             * SCD41 enabled
             */
            if(DP_SCD41 >= 1) {
                float temperature_f;
                float humidity_f;
                uint16_t co2_ppm;
                int res = scd4x_shot_read((uint16_t *)&co2_ppm, (float *)&temperature_f, (float *)&humidity_f);
                if (res != 0) {
                    (void)scd4x_shot_deinit();
                    return 1;
                }
                for(int d = 0; d <= DISPLAY_ENABLE-1; d++) {
                    for(int i = 0; i <= dp[d].dc_count-1; i++) {
                        if(!strcmp(dp[d].dc[i].name, "scd41")) {
                            char buffer[6];
                            sprintf(buffer, "%.2f", temperature_f);
                            strcpy(dp[d].dc[i].data1, buffer);

                            sprintf(buffer, "%.2f", humidity_f);
                            strcpy(dp[d].dc[i].data2, buffer);

                            sprintf(buffer, "%.2d", co2_ppm);
                            strcpy(dp[d].dc[i].data4, buffer);

                            if(!strcmp(dp[d].name,"ssd1681")) {
                                if(displays(ssd1681, &dp[d], i, DISPLAY_SENSOR)){
                                    printf("%s scd41 cmd %d failed\n", &dp[d].name, i);
                                }
                            }

                            if(!strcmp(dp[d].name,"ssd1306")) {
                                if(displays(ssd1306, &dp[d], i, DISPLAY_SENSOR)){
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
            if(DP_SGP30 >= 1) {
                uint16_t co2_eq_ppm = 0;
                uint16_t tvoc_ppb = 0;

                int res = sgp30_advance_read((uint16_t *)&co2_eq_ppm, (uint16_t *)&tvoc_ppb);
                if (res != 0) {
                    (void)sgp30_advance_deinit();
                    return 1;
                }
                for(int d = 0; d <= DISPLAY_ENABLE-1; d++) {
                    for(int i = 0; i <= dp[d].dc_count-1; i++) {
                        if(!strcmp(dp[d].dc[i].name, "sgp30")) {
                            char buffer[10];
                            sprintf(buffer, "%d", co2_eq_ppm);
                            strcpy(dp[d].dc[i].data4, buffer);
                            sprintf(buffer, "%d", tvoc_ppb);
                            strcpy(dp[d].dc[i].data5, buffer);

                            if(!strcmp(dp[d].name,"ssd1681")) {
                                if(displays(ssd1681, &dp[d], i, DISPLAY_SENSOR)){
                                    printf("%s scd41 cmd %d failed\n", &dp[d].name, i);
                                }
                            }

                            if(!strcmp(dp[d].name,"ssd1306")) {
                                if(displays(ssd1306, &dp[d], i, DISPLAY_SENSOR)){
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
            if(SP_ENABLE > 0) {

                if((pwr_in = open(smartpower, O_RDONLY | O_NOCTTY | O_SYNC)) < 0) {
                    printf("\nERROR: Cannot open SmartPower at %s\n\n", smartpower);
                    exit(1);
                }
                /*
                 * read SmartPower2
                 */
                if(SP_ENABLE == 2) {
                    unsigned char temp[18];
                    int sp_read = 0;
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
                    int sp_read = 0;

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
            if(USAGE_ENABLE != 0) {

                float r = 0;
                float s = 0;
                float sum = 0;

                if((cpu_use = fopen(cpuusage, "r")) == NULL) {
                    printf("\nERROR: Cannot open %s\n", cpuusage);
                    exit(1);
                }
                for (int c = 0; c <= USAGE_ENABLE; c++) {
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
                OPTIONS_COUNT--;
                fclose(cpu_use);
            }
            /*
             * Read memory usage
             */
            if(MEM_ENABLE == 1) {

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
                fclose(mem_load);
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
            if(DISPLAY_ENABLE >= 1) {
                for(int d = 0; d <= DISPLAY_ENABLE-1; d++) {
                    if(!strcmp(dp[d].name,"ssd1681")) {
                        if(displays(ssd1681, &dp[d], 0, DISPLAY_UPDATE)){
                            printf("%s update failed\n", &dp[d].name);
                        }
                    }

                    if(!strcmp(dp[d].name,"ssd1306")) {
                        if(displays(ssd1306, &dp[d], 0, DISPLAY_UPDATE)){
                            printf("%s update failed\n", &dp[d].name);
                        }
                    }
                }
            }
            /*
             * break if one and done or sleep
             */
            if(INTERACTIVE_ENABLE == 0) {
                break;
            }
            OPTIONS_COUNT = c;
            i += INTERACTIVE_ENABLE;
            sleep_ms(INTERACTIVE_ENABLE);
        }
    }
    else {
        /*
         * Build of gnuplot script
         */
        i = 0;
        while (i < 11) {
            fprintf(gnuplot_file,"%s",gpscript_start[i]);
            i++;
        }
        /*
         * get thermal zone names and set titles
         */
        char strChar[5] = {0};
        for (int c = 0; c < THERMAL_ENABLE; c++) {
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

            i = 0;
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
            int power = SP_ENABLE > 0 ? 3 : 0;
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
    if (SENSOR_ENABLE > 0) {
        close(sensor_in);
    }
    if (SP_ENABLE > 0) {
        close(pwr_in);
    }
    if (UDP_ENABLE == 1) {
        close(udp_socket);
    }
    if (DISPLAY_ENABLE == 1) {
        (void)ssd1681_deinit(&gs_handle);
    }
    if (DP_SCD41 > 0) {
        (void)scd4x_shot_deinit();
    }
    if (DP_SGP30 > 0) {
        (void)sgp30_advance_deinit();
    }
    if (DP_BMP180 > 0) {
        (void)bmp180_basic_deinit();
    }
    if (DP_BME280 > 0) {
        (void)bme280_basic_deinit();
    }
}


void usage (void) {
        printf("\nlogenv - Version %s Copyright (C) 2019,2020,2024 by Edward Kisiel\n", version);
        printf("logs count or time stamp, CPU frequency, thermal zone temperatures,\n");
        printf("external sensor temperature, volts, amps and watts and CPU core usage\n\n");
        printf("usage: logenv [options]\n\n");
        printf("Options:\n");
        printf(" -l,  --log <file>            Log to <file>\n");
        printf(" -i,  --milliseconds <number> Poll Interval <number> in milliseconds\n");
        printf(" -f,  --frequency             CPU core frequency\n");
        printf(" -t,  --temperature           Thermal zone temperature\n");
        printf(" -a,  --bme280 <device>       Ambient Temperature Sensor, BME280 Temperature Sensor default /dev/i2c-0\n");
        printf("      --bmp180 <device>       BMP180 Sensor, default /dev/i2c-0\n");
        printf("      --mcp9808 <device>      MCP9808 Sensor, default /dev/i2c-0\n");
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


int itoa(int n, char s[]) {
    int i =  0;

    if(n / 10 != 0)
        i = itoa(n/10, s);
    else if(n < 0)
        s[i++] = '-';

    s[i++] = abs(n % 10) + '0';
    s[i] = '\0';

    return i;
}


int set_tty_attributes(int fd, int speed, bool canconical) {
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


void sleep_ms(int milliseconds) {
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
}


static void sig_handler(int _)
{
    (void)_;
    go = -1;
}
