/*
    logenv Copyright 2019,2020, 2024 Edward A. Kisiel
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

   logenv - logs time stamp, cpu temperature, cpu frequency, 
            air temperature(BME280, BMP180),
            volts, amps and watts (HK SmartPower2 or SmartPower3 output)
            
    20200202 Version .97 beta HK SmartPower2
    20241026 Version .98 beta added HK SmartPower3

    void usage (void)
    int itoa(int n, char s[])

*/

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "bme280/bmp180.h"
#include "bme280/bme280.h"
#include "bme280/bme280-i2c.h"
#include "logenv.h"

int main(int argc, char **argv)
{
    /*
    * parse command line options
    */
    if(argc == 1) {
        usage();
    }
    int i = argc;
    while (i--) {
        if(!strcmp(argv[i], "-g") || !strcmp(argv[i], "--gnuplot")) {
            strcpy(gplotfile, argv[i+1]);
            if((gnuplot_file = fopen(gplotfile, "w")) == NULL) {
                printf("\nERROR: Can not open gnuplot script file %s\n\n", argv[i+1]);
                usage();
            }
            GNUPLOT_ENABLE = 1;
        }
    }
    i = argc;
    while (i--) {
        if(!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
            usage();
        }
        if(!strcmp(argv[i], "-v") || !strcmp(argv[i], "--verbose")) {
            VERBOSE_ENABLE = 1;
        }
        if(!strcmp(argv[i], "--version")) {
            printf("\nlogenv version %s\n", version);
            printf("Copyright (C) 2019,2020 Edward Kisiel\n");
            printf("License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>\n");
            printf("This is free software: you are free to change and redistribute it.\n");
            printf("There is NO WARRANTY, to the extent permitted by law.\n");
            COUNT_ENABLE = 0;
        }
        if(!strcmp(argv[i], "-f") || !strcmp(argv[i], "--frequency")) {
            if((cpu_online = fopen(cpuonline, "r")) == NULL) {
                printf("\nERROR: Can not open %s\n", cpuonline);
                exit(0);
            }
            size_t size = 0;
            char *line = 0;
            ssize_t linesize = getline(&line, &size, cpu_online);
            fclose(cpu_online);
            CPU_ENABLE = atoi(&line[2])+1;
        }
        if(!strcmp(argv[i], "-t") || !strcmp(argv[i], "--temperature")) {
            for (int c = 0; c <= 255; c++) {
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
        }
        if(!strcmp(argv[i], "-b") || !strcmp(argv[i], "--bmp280")) {
            if(GNUPLOT_ENABLE != 1) {
                if((i+1) < argc && strlen(argv[i+1]) >= 8) {
                    weatherboard = argv[i+1];
                    }
                if(bme280_begin(weatherboard) < 0) {
                    printf("\nERROR: Can not open WeatherBoard2(BME280) at %s\n", weatherboard);
                    usage();
                }
                if(bme280_set_power_mode(BME280_NORMAL_MODE) < 0) {
                    printf("\nERROR: Can not set power mode for BME280 at %s\n", weatherboard);
                    exit(0);
                }
            }
            WB_ENABLE = 2;
       }
        if(!strcmp(argv[i], "--bmp180")) {
            if(GNUPLOT_ENABLE != 1) {
                if((i+1) < argc && strlen(argv[i+1]) >= 8) {
                    weatherboard = argv[i+1];
                    }
                if(bmp180_begin(weatherboard) < 0) {
                    printf("\nERROR: Can not open WeatherBoard2(BMP180) at %s\n", weatherboard);
                    usage();
                }
            }
            WB_ENABLE = 1;
       }
        if(!strcmp(argv[i], "-p") || !strcmp(argv[i], "--smartpower3-ch1")) {
            if(GNUPLOT_ENABLE != 1) {
                if((i+1) < argc && strlen(argv[i+1]) >= 7) {
                    smartpower = argv[i+1];
                }
                if((pwr_in = fopen(smartpower, "r")) == NULL) {
                    printf("\nERROR: Can not open SmartPower at %s\n\n", smartpower);
                    usage();
                }
            }
            SP_ENABLE = 31;
        }
        if(!strcmp(argv[i], "--smartpower3-ch2")) {
            if(GNUPLOT_ENABLE != 1) {
                if((i+1) < argc && strlen(argv[i+1]) >= 7) {
                    smartpower = argv[i+1];
                }
                if((pwr_in = fopen(smartpower, "r")) == NULL) {
                    printf("\nERROR: Can not open SmartPower at %s\n\n", smartpower);
                    usage();
                }
            }
            SP_ENABLE = 32;
        }
        if(!strcmp(argv[i], "--smartpower2")) {
            if(GNUPLOT_ENABLE != 1) {
                if((i+1) < argc && strlen(argv[i+1]) >= 7) {
                    smartpower = argv[i+1];
                }
                if((pwr_in = fopen(smartpower, "r")) == NULL) {
                    printf("\nERROR: Can not open SmartPower at %s\n\n", smartpower);
                    usage();
                }
           }
            SP_ENABLE = 2;
        }
        if(!strcmp(argv[i], "-q") || !strcmp(argv[i], "--quiet")) {
            QUIET_ENABLE = 1;
        }
        if(!strcmp(argv[i], "-s") || !strcmp(argv[i], "--seconds")) {
            INTERACTIVE_ENABLE = atoi(argv[i+1]);
        }
        if(!strcmp(argv[i], "-l") || !strcmp(argv[i], "--log")) {
            strcpy(logfile, argv[i+1]);
            LOG_ENABLE = 1;
        }
        if(!strcmp(argv[i], "-r") || !strcmp(argv[i], "--raw")) {
            RAW_ENABLE = 1;
        }
        if(!strcmp(argv[i], "-d") || !strcmp(argv[i], "--date")) {
            DT_ENABLE = 1;
            COUNT_ENABLE = 0;
        }
    }
    if (GNUPLOT_ENABLE == 0) {
        /*
         * primary poll loop
         */
        i=0;
        while(i >= 0) {
            if(QUIET_ENABLE == 0 && VERBOSE_ENABLE == 0 && COUNT_ENABLE == 1) {
                printf("%d", i);
            }
            if(QUIET_ENABLE == 0 && VERBOSE_ENABLE == 0 && DT_ENABLE == 1) {
                now = time((time_t *)NULL);
                t = localtime(&now);
                printf("%4d-%02d-%02d %02d:%02d:%02d", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
            }
            if(LOG_ENABLE == 1) {
                if((log_file = fopen(logfile, "a")) == NULL) {
                printf("\nERROR: Can not open %s\n\n", logfile);
                usage();
                }
                if(COUNT_ENABLE) {
                    fprintf(log_file,"%d", i);
                }
                else {
                    now = time((time_t *)NULL);
                    t = localtime(&now);
                    fprintf(log_file,"%4d-%02d-%02d %02d:%02d:%02d", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
                }
            }
            if(QUIET_ENABLE == 0 && VERBOSE_ENABLE == 1) {
                printf("\n");
                for (int c = 0; c < CPU_ENABLE; c++) {
                    printf("  Core%d  ", c);
                }
                printf("\n");
            }
           
            /*
             * open and read each core frequency file
             */
            if(CPU_ENABLE != 0) {
                for (int c = 0; c < CPU_ENABLE; c++) {
                    char strChar[5] = {0};
                    itoa(c,strChar);
                    strcpy(cpufreq,cpufreq1);
                    strcat(cpufreq,strChar);
                    strcat(cpufreq,cpufreq2);
                    if((cpu_freq = fopen(cpufreq, "r")) == NULL) {
                        printf("\nERROR: Can not open %s\n", cpufreq);
                        exit(0);
                    } 
                    fscanf(cpu_freq, "%d", &freq);
                    fclose(cpu_freq);
                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 1) {
                        printf(",%d", freq);
                    }
                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 0) {
                        if(VERBOSE_ENABLE ==1) {
                            printf(" %.2lfGHz ", (double)freq/1000000);
                            if(c == CPU_ENABLE - 1) {
                                printf("\n");
                            }
                        }
                        else {
                            printf(",%.2lf", (double)freq/1000000);
                        }
                    }
                    if(LOG_ENABLE == 1 && RAW_ENABLE == 1) {
                        fprintf(log_file,",%d", freq);
                    }
                    if(LOG_ENABLE == 1 && RAW_ENABLE == 0) {
                        fprintf(log_file,",%.2lf", (double)freq/1000000);
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
                    fscanf(cpu_thermal, "%d", &coretemp);
                    fclose(cpu_thermal);
                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 1) {
                        printf(",%d", coretemp);
                    }
                    if(QUIET_ENABLE == 0 && RAW_ENABLE == 0) {
                        if(VERBOSE_ENABLE == 1) {
                            printf("\n Thermal Zone %d = %dc", c, coretemp/1000);
                        }
                        else {
                            printf(",%d", coretemp/1000);
                        }
                    }
                    if(LOG_ENABLE == 1 && RAW_ENABLE == 1) {
                    fprintf(log_file,",%d", coretemp);
                    }
                    if(LOG_ENABLE == 1 && RAW_ENABLE == 0) {
                    fprintf(log_file,",%d", coretemp/1000);
                    }
                }
            }
            /*
             * read bme280
             */        
            if(WB_ENABLE == 2) {
                bme280_read_pressure_temperature_humidity(&pressure, &temperature, &humidity);
                if(QUIET_ENABLE == 0 && RAW_ENABLE == 1) {
                    printf(",%d", temperature);
                }
                if(QUIET_ENABLE == 0 && RAW_ENABLE == 0) {
                    if(VERBOSE_ENABLE == 1) {
                        printf("\n\n BME280 Sensor = %.2lfc", (double)temperature/100);
                    }
                    else {
                        printf(",%.2lf", (double)temperature/100);
                    }
                }
                if(LOG_ENABLE == 1 && RAW_ENABLE == 1) {
                    fprintf(log_file,",%d", temperature);
                }
                if(LOG_ENABLE == 1 && RAW_ENABLE == 0) {
                    fprintf(log_file,",%.2lf", (double)temperature/100);
                }
            }
            /*
             * read bmp180
             */

            if(WB_ENABLE == 1) {
                temperature = BMP180_readTemperature();
                if(QUIET_ENABLE == 0 && RAW_ENABLE == 1) {
                    printf(",%d", temperature);
                }
                if(QUIET_ENABLE == 0 && RAW_ENABLE == 0) {
                    if(VERBOSE_ENABLE == 1) {
                        printf("\n\n BMP180 Sensor = %.2lfc", (double)temperature/100);
                    }
                    else {
                        printf(",%.2lf", (double)temperature);
                    }
                }
                if(LOG_ENABLE == 1 && RAW_ENABLE == 1) {
                    fprintf(log_file,",%d", temperature);
                }
                if(LOG_ENABLE == 1 && RAW_ENABLE == 0) {
                    fprintf(log_file,",%.2lf", (double)temperature);
                }
            }
            /*
             * read SmartPower port
             */
            if(SP_ENABLE != 0) {
                if((pwr_in = fopen(smartpower, "r")) == NULL) {
                    printf("\nERROR: Can not open SmartPower at %s\n\n", smartpower);
                    usage();
                }
                if(SP_ENABLE == 2) {
                    fscanf(pwr_in, "%fV %s %fW", &volt, spline, &watt);
                    if(strstr(spline,"mA")) {
                        sscanf(spline,"%fmA", &amp);
                        if(QUIET_ENABLE == 0) {
                            if(VERBOSE_ENABLE == 1) {
                                printf("\n\n Volts = %.2f\n Amps = .%.0f\n Watts = %.2f\n\n", volt, amp, watt);
                            }
                            else {
                                printf(",%.2f,.%.0f,%.2f", volt, amp, watt);
                            }
                        }
                        if(LOG_ENABLE == 1) {
                            fprintf(log_file,",%.2f,.%.0f,%.2f", volt, amp, watt);
                        }
                    }
                    else {
                        sscanf(spline,"%fA", &amp);
                        if(QUIET_ENABLE == 0) {
                            if(VERBOSE_ENABLE == 1) {
                                printf("\n\n Volts = %.2f\n Amps = %.2f\n Watts = %.2f\n\n", volt, amp, watt);
                            }
                            else {
                                printf(",%.2f,%.2f,%.2f", volt, amp, watt);
                            }
                        }
                        if(LOG_ENABLE == 1) {
                            fprintf(log_file,",%.2f,%.2f,%.2f", volt, amp, watt);
                        }
                    }
                    fclose(pwr_in);
                }
                if(SP_ENABLE == 31 || SP_ENABLE == 32) {
                    fscanf(pwr_in, "%f,%f,%f,%f,%i,%f,%f,%f,%i,%i,%f,%f,%f,%i,%i,%i,%i", &sp_ms, \
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
                    if(QUIET_ENABLE == 0) {
                        if(VERBOSE_ENABLE == 1) {
                            printf("\n\n Volts = %.2f\n Amps = %.2f\n Watts = %.2f\n\n", volt/1000, amp/1000, watt/1000);
                        }
                        else {
                            printf(",%.2f,%.2f,%.2f", volt/1000, amp/1000, watt/1000);
                        }
                    }
                    if(LOG_ENABLE == 1) {
                        fprintf(log_file,",%.2f,%.2f,%.2f", volt/1000, amp/1000, watt/1000);
                    }
                    fclose(pwr_in);
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
            /*
             * break if one and done or sleep
             */
            if(INTERACTIVE_ENABLE == 0) {
                break;
            }
            i = i+INTERACTIVE_ENABLE;
            sleep(INTERACTIVE_ENABLE);
        }
    }
    else {  // build gnuplot script file
        /*
         * change layout if no power chart
         */
        if(SP_ENABLE == 0) {
            gpscript_layout [21] = 50;
            gpscript_thermal [1][20] = 55;
            gpscript_thermal [2][14] = 51;
            gpscript_freq [1][20] = 51;
            gpscript_freq [2][14] = 48;
            if(INTERACTIVE_ENABLE != 0) {
                itoa(INTERACTIVE_ENABLE, &gpscript_thermal [11][12]);
                strcat(gpscript_thermal [11]," border nomirror out rotate\n");
            }
        }
        /*
         * Beginning of gnuplot script
         */
        i=0;
        while (i < 12) {
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
            fprintf(gnuplot_file,"%s%s\"",gpscript_thermal_title[c][0], thermalname);
            fprintf(gnuplot_file,"\n%s", gpscript_thermal_title[c][2]);
        }
        if(WB_ENABLE != 0) {
            fprintf(gnuplot_file,"%s%s\"",gpscript_thermal_title[8][0],gpscript_thermal_title[8][1]);
            fprintf(gnuplot_file,"\n%s", gpscript_thermal_title[8][2]);
        }
        i=0;
        while (i < 10) {
            fprintf(gnuplot_file,"%s",gpscript_mid[i]);
            i++;
        }
        fprintf(gnuplot_file,"%s",gpscript_layout);
        /*
         * build thermal zone chart
         */
        i=0;
        while (i < 13) {
            fprintf(gnuplot_file,"%s",gpscript_thermal[i]);
            i++;
        }
        i=0;
        fprintf(gnuplot_file, "plot ");
        fprintf(gnuplot_file, "ARG2 using 1:%d with lines ls %d axes x1y1 title data_title%d", i+(CPU_ENABLE+2), i+1, i+1);
        i++;
        while (i < THERMAL_ENABLE) {
            fprintf(gnuplot_file, ", ARG2 using 1:%d with lines ls %d axes x1y1 title data_title%d", i+(CPU_ENABLE+2), i+1, i+1);
            i++;
        }
        if(WB_ENABLE > 0) {
            fprintf(gnuplot_file, ", ARG2 using 1:%d with lines ls 8 axes x1y1 title data_title9", i+(CPU_ENABLE+2));
        }
        fprintf(gnuplot_file, "\n\n");
        /*
         * build frequency chart
         */
        i=0;
        while (i < 12) {
            fprintf(gnuplot_file,"%s",gpscript_freq[i]);
            i++;
        }
        i=0;
        fprintf(gnuplot_file, "plot ");
        fprintf(gnuplot_file, "ARG2 using 1:%d with lines ls %d axes x1y1 notitle", i+2, i+1);
        i++;
        while (i < CPU_ENABLE) {
            fprintf(gnuplot_file, ", ARG2 using 1:%d with lines ls %d axes x1y1", i+2, i+1);
            i++;
        }
        fprintf(gnuplot_file, "\n\n");
        /*
         * build power chart
         */
        if(SP_ENABLE != 0) {
            i=0;
            while (i < 12) {
                fprintf(gnuplot_file,"%s",gpscript_power[i]);
                i++;
            }
        fprintf(gnuplot_file, "plot ");
        fprintf(gnuplot_file, "ARG2 using 1:%d with lines ls 4 axes x1y1 notitle", (CPU_ENABLE+THERMAL_ENABLE+2));
        fprintf(gnuplot_file, ", ARG2 using 1:%d with lines ls 8 axes x1y1", (CPU_ENABLE+THERMAL_ENABLE+3));
        fprintf(gnuplot_file, ", ARG2 using 1:%d with lines ls 5 axes x1y1\n\n", (CPU_ENABLE+THERMAL_ENABLE+4));
        }
        fprintf(gnuplot_file,"%s",gpscript_end);
        fclose(gnuplot_file);
    }
}


void usage (void)
{
        printf("\nlogenv - Version %s Copyright (C) 2019,2020 by Edward Kisiel\n", version);
        printf("logs time, cpu frequency, thermal zones, sensor temperature, volts, amps and watts\n\n");
        printf("usage: logenv [options]\n\n");
        printf("Options:\n");
        printf(" -l,  --log <file>            Log to <file>\n");
        printf(" -s,  --seconds <number>      Poll every <number> seconds\n");        
        printf(" -f,  --frequency             CPU core frequency\n");
        printf(" -t,  --temperature           Thermal zone temperature\n");
        printf(" -b,  --bme280 <device>       BME280 Temperature Sensor(HK Weatherboard 2), default /dev/i2c-1\n");
        printf("      --bmp180 <device>       BMP180 Temperature Sensor(HK Weatherboard 1), default /dev/i2c-1\n");
        printf(" -p,  --smartpower3-ch1 <tty> Volt,Amp,Watt (HK SmartPower3 USBC port), default /dev/ttyUSB0\n");
        printf("      --smartpower3-ch2 <tty> Volt,Amp,Watt (HK SmartPower3 USBC port), default /dev/ttyUSB0\n");
        printf("      --smartpower2 <tty>     Volt,Amp,Watt (HK SmartPower2 microUSB port), default /dev/ttyUSB0\n");
        printf(" -d,  --date                  Date and Time stamp\n");
        printf(" -r,  --raw                   Raw output, no formatting of freq. or temp.  e.g. 35000 instead of 35\n");
        printf(" -v,  --verbose               Readable output\n"); 
        printf(" -q,  --quiet                 No output to stdout\n");
        printf(" -g,  --gnuplot <file>        Gnuplot script generation\n");
        printf("      --version               Version\n");
        printf(" -h,  --help                  Help screen\n\n");
        printf("Example:\n\n");
        printf("Data capture every 2 seconds:\n");
        printf("logenv -l logfile.csv -s 2 -f -t -b /dev/i2c-1 -p /dev/ttyUSB0\n\n");
        printf("Gnuplot script generation for data capture:\n");
        printf("logenv -g gplotscript.gpl -s 2 -f -t -b -p \n\n");
        printf("Gnuplot chart creation:\n");
        printf("gnuplot -c gplotscript.gpl chart.png logfile.csv\n\n");
        exit(0);
}

int itoa(int n, char s[])
{
    int i =  0;

    if(n / 10 != 0)
        i = itoa(n/10, s);
    else if(n < 0)
        s[i++] = '-';

    s[i++] = abs(n % 10) + '0';
    s[i] = '\0';

    return i;
}
