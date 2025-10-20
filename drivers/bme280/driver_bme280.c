/**
 * Copyright (c) 2015 - present LibDriver All rights reserved
 *
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * @file      driver_bme280.c
 * @brief     driver bme280 source file
 * @version   1.0.0
 * @author    Shifeng Li
 * @date      2024-01-15
 *
 * <h3>history</h3>
 * <table>
 * <tr><th>Date        <th>Version  <th>Author      <th>Description
 * <tr><td>2024/01/15  <td>1.0      <td>Shifeng Li  <td>first upload
 * </table>
 */

#include "driver_bme280.h"

/**
 * @brief chip information definition
 */
#define CHIP_NAME                 "Bosch BME280"        /**< chip name */
#define MANUFACTURER_NAME         "Bosch"               /**< manufacturer name */
#define SUPPLY_VOLTAGE_MIN        1.71f                 /**< chip min supply voltage */
#define SUPPLY_VOLTAGE_MAX        3.6f                  /**< chip max supply voltage */
#define MAX_CURRENT               0.714f                /**< chip max current */
#define TEMPERATURE_MIN           -40.0f                /**< chip min operating temperature */
#define TEMPERATURE_MAX           85.0f                 /**< chip max operating temperature */
#define DRIVER_VERSION            1000                  /**< driver version */

/**
 * @brief chip register definition
 */
#define BME280_REG_NVM_PAR_T1_L        0x88        /**< NVM PAR T1 low register */
#define BME280_REG_NVM_PAR_T1_H        0x89        /**< NVM PAR T1 high register */
#define BME280_REG_NVM_PAR_T2_L        0x8A        /**< NVM PAR T2 low register */
#define BME280_REG_NVM_PAR_T2_H        0x8B        /**< NVM PAR T2 high register */
#define BME280_REG_NVM_PAR_T3_L        0x8C        /**< NVM PAR T3 low register */
#define BME280_REG_NVM_PAR_T3_H        0x8D        /**< NVM PAR T3 high register */
#define BME280_REG_NVM_PAR_P1_L        0x8E        /**< NVM PAR P1 low register */
#define BME280_REG_NVM_PAR_P1_H        0x8F        /**< NVM PAR P1 high register */
#define BME280_REG_NVM_PAR_P2_L        0x90        /**< NVM PAR P2 low register */
#define BME280_REG_NVM_PAR_P2_H        0x91        /**< NVM PAR P2 high register */
#define BME280_REG_NVM_PAR_P3_L        0x92        /**< NVM PAR P3 low register */
#define BME280_REG_NVM_PAR_P3_H        0x93        /**< NVM PAR P3 high register */
#define BME280_REG_NVM_PAR_P4_L        0x94        /**< NVM PAR P4 low register */
#define BME280_REG_NVM_PAR_P4_H        0x95        /**< NVM PAR P4 high register */
#define BME280_REG_NVM_PAR_P5_L        0x96        /**< NVM PAR P5 low register */
#define BME280_REG_NVM_PAR_P5_H        0x97        /**< NVM PAR P5 high register */
#define BME280_REG_NVM_PAR_P6_L        0x98        /**< NVM PAR P6 low register */
#define BME280_REG_NVM_PAR_P6_H        0x99        /**< NVM PAR P6 high register */
#define BME280_REG_NVM_PAR_P7_L        0x9A        /**< NVM PAR P7 low register */
#define BME280_REG_NVM_PAR_P7_H        0x9B        /**< NVM PAR P7 high register */
#define BME280_REG_NVM_PAR_P8_L        0x9C        /**< NVM PAR P8 low register */
#define BME280_REG_NVM_PAR_P8_H        0x9D        /**< NVM PAR P8 high register */
#define BME280_REG_NVM_PAR_P9_L        0x9E        /**< NVM PAR P9 low register */
#define BME280_REG_NVM_PAR_P9_H        0x9F        /**< NVM PAR P9 high register */
#define BME280_REG_NVM_PAR_H1          0xA1        /**< NVM PAR H1 register */
#define BME280_REG_NVM_PAR_H2_L        0xE1        /**< NVM PAR H2 low register */
#define BME280_REG_NVM_PAR_H2_H        0xE2        /**< NVM PAR H2 high register */
#define BME280_REG_NVM_PAR_H3          0xE3        /**< NVM PAR H3 register */
#define BME280_REG_NVM_PAR_H4_L        0xE4        /**< NVM PAR H4 low register */
#define BME280_REG_NVM_PAR_H4_H        0xE5        /**< NVM PAR H4 high register */
#define BME280_REG_NVM_PAR_H5_L        0xE5        /**< NVM PAR H5 low register */
#define BME280_REG_NVM_PAR_H5_H        0xE6        /**< NVM PAR H5 high register */
#define BME280_REG_NVM_PAR_H6          0xE7        /**< NVM PAR H6 register */
#define BME280_REG_HUM_LSB             0xFE        /**< hum lsb register */
#define BME280_REG_HUM_MSB             0xFD        /**< hum msb register */
#define BME280_REG_TEMP_XLSB           0xFC        /**< temp xlsb register */
#define BME280_REG_TEMP_LSB            0xFB        /**< temp lsb register */
#define BME280_REG_TEMP_MSB            0xFA        /**< temp msb register */
#define BME280_REG_PRESS_XLSB          0xF9        /**< press xlsb register */
#define BME280_REG_PRESS_LSB           0xF8        /**< press lsb register */
#define BME280_REG_PRESS_MSB           0xF7        /**< press msb register */
#define BME280_REG_CONFIG              0xF5        /**< config register */
#define BME280_REG_CTRL_MEAS           0xF4        /**< ctrl meas register */
#define BME280_REG_STATUS              0xF3        /**< status register */
#define BME280_REG_CTRL_HUM            0xF2        /**< ctrl hum register */
#define BME280_REG_RESET               0xE0        /**< soft reset register */
#define BME280_REG_ID                  0xD0        /**< chip id register */

/**
 * @brief      read multiple bytes
 * @param[in]  *handle pointer to a bme280 handle structure
 * @param[in]  reg register address
 * @param[out] *buf pointer to a data buffer
 * @param[in]  len data length
 * @return     status code
 *             - 0 success
 *             - 1 iic spi read failed
 * @note       none
 */
static uint8_t a_bme280_iic_spi_read(bme280_handle_t *handle, uint8_t reg, uint8_t *buf, uint16_t len)
{
    if (handle->iic_spi == BME280_INTERFACE_IIC)                           /* iic interface */
    {
        if (handle->iic_read(handle->iic_addr, reg, buf, len) != 0)        /* iic read */
        {
            return 1;                                                      /* return error */
        }

        return 0;                                                          /* success return 0 */
    }
    else                                                                   /* spi interface */
    {
        reg |= 1 << 7;                                                     /* set read mode */
        if (handle->spi_read(reg, buf, len) != 0)                          /* spi read */
        {
            return 1;                                                      /* return error */
        }

        return 0;                                                          /* success return 0 */
    }
}

/**
 * @brief     write multiple bytes
 * @param[in] *handle pointer to a bme280 handle structure
 * @param[in] reg register address
 * @param[in] *buf pointer to a data buffer
 * @param[in] len data length
 * @return    status code
 *            - 0 success
 *            - 1 iic spi write failed
 * @note      none
 */
static uint8_t a_bme280_iic_spi_write(bme280_handle_t *handle, uint8_t reg, uint8_t *buf, uint16_t len)
{
    if (handle->iic_spi == BME280_INTERFACE_IIC)                           /* iic interface */
    {
        if (handle->iic_write(handle->iic_addr, reg, buf, len) != 0)       /* iic write */
        {
            return 1;                                                      /* return error */
        }

        return 0;                                                          /* success return 0 */
    }
    else                                                                   /* spi interface */
    {
        reg &= ~(1 << 7);                                                  /* write mode */
        if (handle->spi_write(reg, buf, len) != 0)                         /* spi write */
        {
            return 1;                                                      /* return error */
        }

        return 0;                                                          /* success return 0 */
    }
}

/**
 * @brief     get nvm calibration
 * @param[in] *handle pointer to a bme280 handle structure
 * @return    status code
 *            - 0 success
 *            - 1 get calibration data failed
 * @note      none
 */
static uint8_t a_bme280_get_nvm_calibration(bme280_handle_t *handle)
{
    uint8_t buf[2];

    if (a_bme280_iic_spi_read(handle, BME280_REG_NVM_PAR_T1_L, (uint8_t *)buf, 2) != 0)        /* read t1 */
    {
        handle->debug_print("bme280: get calibration data failed.\n");                         /* get calibration data failed */

        return 1;                                                                              /* return error */
    }
    handle->t1 = (uint16_t)buf[1] << 8 | buf[0];                                               /* set t1 */
    if (a_bme280_iic_spi_read(handle, BME280_REG_NVM_PAR_T2_L, (uint8_t *)buf, 2) != 0)        /* read t2 */
    {
        handle->debug_print("bme280: get calibration data failed.\n");                         /* get calibration data failed */

        return 1;                                                                              /* return error */
    }
    handle->t2 = (int16_t)((uint16_t)buf[1] << 8 | buf[0]);                                    /* set t2 */
    if (a_bme280_iic_spi_read(handle, BME280_REG_NVM_PAR_T3_L, (uint8_t *)buf, 2) != 0)        /* read t3 */
    {
        handle->debug_print("bme280: get calibration data failed.\n");                         /* get calibration data failed */

        return 1;                                                                              /* return error */
    }
    handle->t3 = (int16_t)((uint16_t)buf[1] << 8 | buf[0]);                                    /* set t3 */
    if (a_bme280_iic_spi_read(handle, BME280_REG_NVM_PAR_P1_L, (uint8_t *)buf, 2) != 0)        /* read p1 */
    {
        handle->debug_print("bme280: get calibration data failed.\n");                         /* get calibration data failed */

        return 1;                                                                              /* return error */
    }
    handle->p1 = (uint16_t)buf[1] <<8 | buf[0];                                                /* set p1 */
    if (a_bme280_iic_spi_read(handle, BME280_REG_NVM_PAR_P2_L, (uint8_t *)buf, 2) != 0)        /* read p2 */
    {
        handle->debug_print("bme280: get calibration data failed.\n");                         /* get calibration data failed */

        return 1;                                                                              /* return error */
    }
    handle->p2 = (int16_t)((uint16_t)buf[1] << 8 | buf[0]);                                    /* set p2 */
    if (a_bme280_iic_spi_read(handle, BME280_REG_NVM_PAR_P3_L, (uint8_t *)buf, 2) != 0)        /* read p3 */
    {
        handle->debug_print("bme280: get calibration data failed.\n");                         /* get calibration data failed */

        return 1;                                                                              /* return error */
    }
    handle->p3 = (int16_t)((uint16_t)buf[1] << 8 | buf[0]);                                    /* set p3 */
    if (a_bme280_iic_spi_read(handle, BME280_REG_NVM_PAR_P4_L, (uint8_t *)buf, 2) != 0)        /* read p4 */
    {
        handle->debug_print("bme280: get calibration data failed.\n");                         /* get calibration data failed */

        return 1;                                                                              /* return error */
    }
    handle->p4 = (int16_t)((uint16_t)buf[1] << 8 | buf[0]);                                    /* set p4 */
    if (a_bme280_iic_spi_read(handle, BME280_REG_NVM_PAR_P5_L, (uint8_t *)buf, 2) != 0)        /* read p5 */
    {
        handle->debug_print("bme280: get calibration data failed.\n");                         /* get calibration data failed */

        return 1;                                                                              /* return error */
    }
    handle->p5 = (int16_t)((uint16_t)buf[1] << 8 | buf[0]);                                    /* set p5 */
    if (a_bme280_iic_spi_read(handle, BME280_REG_NVM_PAR_P6_L, (uint8_t *)buf, 2) != 0)        /* read p6 */
    {
        handle->debug_print("bme280: get calibration data failed.\n");                         /* get calibration data failed */

        return 1;                                                                              /* return error */
    }
    handle->p6 = (int16_t)((uint16_t)buf[1] << 8 | buf[0]);                                    /* set p6 */
    if (a_bme280_iic_spi_read(handle, BME280_REG_NVM_PAR_P7_L, (uint8_t *)buf, 2) != 0)        /* read p7 */
    {
        handle->debug_print("bme280: get calibration data failed.\n");                         /* get calibration data failed */

        return 1;                                                                              /* return error */
    }
    handle->p7 = (int16_t)((uint16_t)buf[1] << 8 | buf[0]);                                    /* set p7 */
    if (a_bme280_iic_spi_read(handle, BME280_REG_NVM_PAR_P8_L, (uint8_t *)buf, 2) != 0)        /* read p8 */
    {
        handle->debug_print("bme280: get calibration data failed.\n");                         /* get calibration data failed */

        return 1;                                                                              /* return error */
    }
    handle->p8 = (int16_t)((uint16_t)buf[1] << 8 | buf[0]);                                    /* set p8 */
    if (a_bme280_iic_spi_read(handle, BME280_REG_NVM_PAR_P9_L, (uint8_t *)buf, 2) != 0)        /* read p9 */
    {
        handle->debug_print("bme280: get calibration data failed.\n");                         /* get calibration data failed */

        return 1;                                                                              /* return error */
    }
    handle->p9 = (int16_t)((uint16_t)buf[1] << 8 | buf[0]);                                    /* set p9 */
    if (a_bme280_iic_spi_read(handle, BME280_REG_NVM_PAR_H1, (uint8_t *)buf, 1) != 0)          /* read h1 */
    {
        handle->debug_print("bme280: get calibration data failed.\n");                         /* get calibration data failed */

        return 1;                                                                              /* return error */
    }
    handle->h1 = buf[0];                                                                       /* set h1 */
    if (a_bme280_iic_spi_read(handle, BME280_REG_NVM_PAR_H2_L, (uint8_t *)buf, 2) != 0)        /* read h2 */
    {
        handle->debug_print("bme280: get calibration data failed.\n");                         /* get calibration data failed */

        return 1;                                                                              /* return error */
    }
    handle->h2 = (int16_t)((uint16_t)buf[1] <<8 | buf[0]);                                     /* set h2 */
    if (a_bme280_iic_spi_read(handle, BME280_REG_NVM_PAR_H3, (uint8_t *)buf, 1) != 0)          /* read h3 */
    {
        handle->debug_print("bme280: get calibration data failed.\n");                         /* get calibration data failed */

        return 1;                                                                              /* return error */
    }
    handle->h3 = buf[0];                                                                       /* set h3 */
    if (a_bme280_iic_spi_read(handle, BME280_REG_NVM_PAR_H4_L, (uint8_t *)buf, 2) != 0)        /* read h4 */
    {
        handle->debug_print("bme280: get calibration data failed.\n");                         /* get calibration data failed */

        return 1;                                                                              /* return error */
    }
    handle->h4 = (int16_t)((uint16_t)buf[0] << 4 | (buf[1] & 0xF));                            /* set h4 */
    if (a_bme280_iic_spi_read(handle, BME280_REG_NVM_PAR_H5_L, (uint8_t *)buf, 2) != 0)        /* read h5 */
    {
        handle->debug_print("bme280: get calibration data failed.\n");                         /* get calibration data failed */

        return 1;                                                                              /* return error */
    }
    handle->h5 = (int16_t)((uint16_t)buf[1] << 4 | ((buf[0] >> 4) & 0xF));                     /* set h5 */
    if (a_bme280_iic_spi_read(handle, BME280_REG_NVM_PAR_H6, (uint8_t *)buf, 1) != 0)          /* read h6 */
    {
        handle->debug_print("bme280: get calibration data failed.\n");                         /* get calibration data failed */

        return 1;                                                                              /* return error */
    }
    handle->h6 = (int8_t)buf[0];                                                               /* set h6 */
    handle->t_fine = 0;                                                                        /* init 0 */

    return 0;                                                                                  /* success return 0 */
}

/**
 * @brief      compensate temperature
 * @param[in]  *handle pointer to a bme280 handle structure
 * @param[in]  raw raw data
 * @param[out] *output pointer to an output buffer
 * @return     status code
 *             - 0 success
 *             - 1 compensate temperature failed
 * @note       none
 */
static uint8_t a_bme280_compensate_temperature(bme280_handle_t *handle, uint32_t raw, float *output)
{
    uint8_t res;
    float var1;
    float var2;
    float temperature;
    float temperature_min = -40.0f;
    float temperature_max = 85.0f;

    var1 = (((float)raw) / 16384.0f - ((float)handle->t1) / 1024.0f);         /* set var1 */
    var1 = var1 * ((float)handle->t2);                                        /* set var1 */
    var2 = (((float)raw) / 131072.0f - ((float)handle->t1) / 8192.0f);        /* set var2 */
    var2 = (var2 * var2) * ((float)handle->t3);                               /* set var2 */
    handle->t_fine = (int32_t)(var1 + var2);                                  /* set t_fine */
    temperature = (var1 + var2) / 5120.0f;                                    /* set temperature */
    *output = temperature;                                                    /* set output */
    res = 0;                                                                  /* init 0 */
    if (temperature < temperature_min)                                        /* if min */
    {
        temperature = temperature_min;                                        /* set temperature min */
        *output = temperature;                                                /* set output */
        res = 1;                                                              /* set failed */
    }
    else if (temperature > temperature_max)                                   /* if max */
    {
        temperature = temperature_max;                                        /* set temperature max */
        *output = temperature;                                                /* set output */
        res = 1;                                                              /* set failed */
    }
    else
    {
                                                                              /* do nothing */
    }
    
    return res;                                                               /* return result */
}

/**
 * @brief      compensate pressure
 * @param[in]  *handle pointer to a bme280 handle structure
 * @param[in]  raw raw data
 * @param[out] *output pointer to an output buffer
 * @return     status code
 *             - 0 success
 *             - 1 compensate pressure failed
 * @note       none
 */
static uint8_t a_bme280_compensate_pressure(bme280_handle_t *handle, uint32_t raw, float *output)
{
    uint8_t res;
    float var1;
    float var2;
    float var3;
    float pressure;
    float pressure_min = 30000.0f;
    float pressure_max = 110000.0f;

    var1 = ((float)handle->t_fine / 2.0f) - 64000.0f;                             /* set var1 */
    var2 = var1 * var1 * ((float)handle->p6) / 32768.0f;                          /* set var2 */
    var2 = var2 + var1 * ((float)handle->p5) * 2.0f;                              /* set var2 */
    var2 = (var2 / 4.0f) + (((float)handle->p4) * 65536.0f);                      /* set var2 */
    var3 = ((float)handle->p3) * var1 * var1 / 524288.0f;                         /* set var3 */
    var1 = (var3 + ((float)handle->p2) * var1) / 524288.0f;                       /* set var1 */
    var1 = (1.0f + var1 / 32768.0f) * ((float)handle->p1);                        /* set var1 */
    res = 0;                                                                      /* init 0 */
    if (var1 > (0.0f))                                                            /* if over zero */
    {
        pressure = 1048576.0f - (float)raw;                                       /* set pressure */
        pressure = (pressure - (var2 / 4096.0f)) * 6250.0f / var1;                /* set pressure */
        var1 = ((float)handle->p9) * pressure * pressure / 2147483648.0f;         /* set var1 */
        var2 = pressure * ((float)handle->p8) / 32768.0f;                         /* set var2 */
        pressure = pressure + (var1 + var2 + ((float)handle->p7)) / 16.0f;        /* set pressure */
        if (pressure < pressure_min)                                              /* if min */
        {
            pressure = pressure_min;                                              /* set pressure */
            res = 1;                                                              /* set failed */
        }
        else if (pressure > pressure_max)                                         /* if max */
        {
            pressure = pressure_max;                                              /* set pressure */
            res = 1;                                                              /* set failed */
        }
        else
        {
                                                                                  /* do nothing */
        }
        *output = pressure;                                                       /* set output */
    }
    else
    {
        pressure = pressure_min;                                                  /* set pressure */
        *output = pressure;                                                       /* set output */
        res = 1;                                                                  /* set failed */
    }

    return res;                                                                   /* return result */
}

/**
 * @brief      compensate humidity
 * @param[in]  *handle pointer to a bme280 handle structure
 * @param[in]  raw raw data
 * @param[out] *output pointer to an output buffer
 * @return     status code
 *             - 0 success
 *             - 1 compensate humidity failed
 * @note       none
 */
static uint8_t a_bme280_compensate_humidity(bme280_handle_t *handle, uint32_t raw, float *output)
{
    uint8_t res;
    float var1;
    float var2;
    float var3;
    float var4;
    float var5;
    float var6;
    float humidity;
    float humidity_min = 0.0f;
    float humidity_max = 100.0f;
    
    var1 = ((float)handle->t_fine) - 76800.0f;                                             /* set var1 */
    var2 = (((float)handle->h4) * 64.0f + (((float)handle->h5) / 16384.0f) * var1);        /* set var2 */
    var3 = (float)raw - var2;                                                              /* set var3 */
    var4 = ((float)handle->h2) / 65536.0f;                                                 /* set var4 */
    var5 = (1.0f + (((float)handle->h3) / 67108864.0f) * var1);                            /* set var5 */
    var6 = 1.0f + (((float)handle->h6) / 67108864.0f) * var1 * var5;                       /* set var6 */
    var6 = var3 * var4 * (var5 * var6);                                                    /* set var6 */
    humidity = var6 * (1.0f - ((float)handle->h1) * var6 / 524288.0f);                     /* set humidity */
    *output = humidity;                                                                    /* set output */
    res = 0;                                                                               /* init 0 */
    if (humidity > humidity_max)                                                           /* if max */
    {
        humidity = humidity_max;                                                           /* set humidity */
        *output = humidity;                                                                /* set output */
        res = 1;                                                                           /* set failed */
    }
    else if (humidity < humidity_min)                                                      /* if min */
    {
        humidity = humidity_min;                                                           /* set humidity */
        *output = humidity;                                                                /* set output */
        res = 1;                                                                           /* set failed */
    }
    else
    {
                                                                                           /* do nothing */
    }
    
    return res;                                                                            /* return result */
}

/**
 * @brief     set the iic address pin
 * @param[in] *handle pointer to a bme280 handle structure
 * @param[in] addr_pin iic address pin
 * @return    status code
 *            - 0 success
 *            - 2 handle is NULL
 * @note      none
 */
uint8_t bme280_set_addr_pin(bme280_handle_t *handle, bme280_address_t addr_pin)
{
    if (handle == NULL)                          /* check handle */
    {
        return 2;                                /* return error */
    }

    handle->iic_addr = (uint8_t)addr_pin;        /* set iic address */

    return 0;                                    /* success return 0 */
}

/**
 * @brief      get the iic address pin
 * @param[in]  *handle pointer to a bme280 handle structure
 * @param[out] *addr_pin pointer to an iic address pin buffer
 * @return     status code
 *             - 0 success
 *             - 2 handle is NULL
 * @note       none
 */
uint8_t bme280_get_addr_pin(bme280_handle_t *handle, bme280_address_t *addr_pin)
{
    if (handle == NULL)                                    /* check handle */
    {
        return 2;                                          /* return error */
    }

    *addr_pin = (bme280_address_t)handle->iic_addr;        /* get iic address */

    return 0;                                              /* success return 0 */
}

/**
 * @brief     set the interface
 * @param[in] *handle pointer to a bme280 handle structure
 * @param[in] interface chip interface
 * @return    status code
 *            - 0 success
 *            - 2 handle is NULL
 * @note      none
 */
uint8_t bme280_set_interface(bme280_handle_t *handle, bme280_interface_t interface)
{
    if (handle == NULL)                        /* check handle */
    {
        return 2;                              /* return error */
    }

    handle->iic_spi = (uint8_t)interface;      /* set interface */

    return 0;                                  /* success return 0 */
}

/**
 * @brief      get the interface
 * @param[in]  *handle pointer to a bme280 handle structure
 * @param[out] *interface pointer to a chip interface buffer
 * @return     status code
 *             - 0 success
 *             - 2 handle is NULL
 * @note       none
 */
uint8_t bme280_get_interface(bme280_handle_t *handle, bme280_interface_t *interface)
{
    if (handle == NULL)                                        /* check handle */
    {
        return 2;                                              /* return error */
    }

    *interface = (bme280_interface_t)(handle->iic_spi);        /* get interface */

    return 0;                                                  /* success return 0 */
}

/**
 * @brief     initialize the chip
 * @param[in] *handle pointer to a bme280 handle structure
 * @return    status code
 *            - 0 success
 *            - 1 iic or spi initialization failed
 *            - 2 handle is NULL
 *            - 3 linked functions is NULL
 *            - 4 id is error
 *            - 5 get nvm calibration failed
 *            - 6 read calibration failed
 * @note      none
 */
uint8_t bme280_init(bme280_handle_t *handle)
{
    uint8_t id;
    uint8_t reg;

    if (handle == NULL)                                                              /* check handle */
    {
        return 2;                                                                    /* return error */
    }
    if (handle->debug_print == NULL)                                                 /* check debug_print */
    {
        return 3;                                                                    /* return error */
    }
    if (handle->iic_init == NULL)                                                    /* check iic_init */
    {
        handle->debug_print("bme280: iic_init is null.\n");                          /* iic_init is nul */

        return 3;                                                                    /* return error */
    }
    if (handle->iic_deinit == NULL)                                                  /* check iic_deinit */
    {
        handle->debug_print("bme280: iic_deinit is null.\n");                        /* iic_deinit is null */

        return 3;                                                                    /* return error */
    }
    if (handle->iic_read == NULL)                                                    /* check iic_read */
    {
        handle->debug_print("bme280: iic_read is null.\n");                          /* iic_read is null */

        return 3;                                                                    /* return error */
    }
    if (handle->iic_write == NULL)                                                   /* check iic_write */
    {
        handle->debug_print("bme280: iic_write is null.\n");                         /* iic_write is null */

        return 3;                                                                    /* return error */
    }
    if (handle->spi_init == NULL)                                                    /* check spi_init */
    {
        handle->debug_print("bme280: spi_init is null.\n");                          /* spi_init is nul */

        return 3;                                                                    /* return error */
    }
    if (handle->spi_deinit == NULL)                                                  /* check spi_deinit */
    {
        handle->debug_print("bme280: spi_deinit is null.\n");                        /* spi_deinit is nul */

        return 3;                                                                    /* return error */
    }
    if (handle->spi_read == NULL)                                                    /* check spi_read */
    {
        handle->debug_print("bme280: spi_read is null.\n");                          /* spi_read is nul */

        return 3;                                                                    /* return error */
    }
    if (handle->spi_write == NULL)                                                   /* check spi_write */
    {
        handle->debug_print("bme280: spi_write is null.\n");                         /* spi_write is nul */

        return 3;                                                                    /* return error */
    }
    if (handle->delay_ms == NULL)                                                    /* check delay_ms */
    {
        handle->debug_print("bme280: delay_ms is null.\n");                          /* delay_ms is null */

        return 3;                                                                    /* return error */
    }

    if (handle->iic_spi == BME280_INTERFACE_IIC)                                     /* iic interface */
    {
        if (handle->iic_init() != 0)                                                 /* iic init */
        {
            handle->debug_print("bme280: iic init failed.\n");                       /* iic init failed */

            return 1;                                                                /* return error */
        }
    }
    else                                                                             /* spi interface */
    {
        if (handle->spi_init() != 0)                                                 /* spi init */
        {
            handle->debug_print("bme280: spi init failed.\n");                       /* spi init failed */

            return 1;                                                                /* return error */
        }
    }

    if (a_bme280_iic_spi_read(handle, BME280_REG_ID, (uint8_t *)&id, 1) != 0)        /* read chip id */
    {
        handle->debug_print("bme280: read id failed.\n");                            /* read id failed */
        (void)handle->iic_deinit();                                                  /* iic deinit */

        return 4;                                                                    /* return error */
    }
    if (id != 0x60)                                                                  /* check id */
    {
        handle->debug_print("bme280: id is error.\n");                               /* id is error */
        (void)handle->iic_deinit();                                                  /* iic deinit */

        return 4;                                                                    /* return error */
    }
    reg = 0xB6;                                                                      /* set the reset value */
    if (a_bme280_iic_spi_write(handle, BME280_REG_RESET, &reg, 1) != 0)              /* reset the chip */
    {
        handle->debug_print("bme280: reset failed.\n");                              /* reset failed */
        (void)handle->iic_deinit();                                                  /* iic deinit */

        return 5;                                                                    /* return error */
    }
    handle->delay_ms(5);                                                             /* delay 5ms */
    if (a_bme280_get_nvm_calibration(handle) != 0)                                   /* get nvm calibration */
    {
        (void)handle->iic_deinit();                                                  /* iic deinit */

        return 6;                                                                    /* return error */
    }
    handle->inited = 1;                                                              /* flag finish initialization */

    return 0;                                                                        /* success return 0 */
}

/**
 * @brief     close the chip
 * @param[in] *handle pointer to a bme280 handle structure
 * @return    status code
 *            - 0 success
 *            - 1 iic deinit failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 power down failed
 * @note      none
 */
uint8_t bme280_deinit(bme280_handle_t *handle)
{
    uint8_t prev;

    if (handle == NULL)                                                             /* check handle */
    {
        return 2;                                                                   /* return error */
    }
    if (handle->inited != 1)                                                        /* check handle initialization */
    {
        return 3;                                                                   /* return error */
    }

    if (a_bme280_iic_spi_read(handle, BME280_REG_CTRL_MEAS, &prev, 1) != 0)         /* read ctrl meas */
    {
        handle->debug_print("bme280: read ctrl meas failed.\n");                    /* read ctrl meas failed */

        return 4;                                                                   /* return error */
    }
    prev &= ~(3 << 0);                                                              /* clear settings */
    prev |= 0 << 0;                                                                 /* set sleep mode */
    if (a_bme280_iic_spi_write(handle, BME280_REG_CTRL_MEAS, &prev, 1) != 0)        /* write ctrl meas */
    {
        handle->debug_print("bme280: write ctrl meas failed.\n");                   /* write ctrl meas failed */

        return 4;                                                                   /* return error */
    }
    if (handle->iic_spi == BME280_INTERFACE_IIC)                                    /* iic interface */
    {
        if (handle->iic_deinit() != 0)                                              /* iic deinit */
        {
            handle->debug_print("bme280: iic deinit failed.\n");                    /* iic deinit failed */

            return 1;                                                               /* return error */
        }
    }
    else                                                                            /* spi interface */
    {
        if (handle->spi_deinit() != 0)                                              /* spi deinit */
        {
            handle->debug_print("bme280: spi deinit failed.\n");                    /* spi deinit failed */

            return 1;                                                               /* return error */
        }
    }
    handle->inited = 0;                                                             /* flag close */

    return 0;                                                                       /* success return 0 */
}

/**
 * @brief     soft reset
 * @param[in] *handle pointer to a bme280 handle structure
 * @return    status code
 *            - 0 success
 *            - 1 soft reset failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t bme280_soft_reset(bme280_handle_t *handle)
{
    uint8_t reg;

    if (handle == NULL)                                                        /* check handle */
    {
        return 2;                                                              /* return error */
    }
    if (handle->inited != 1)                                                   /* check handle initialization */
    {
        return 3;                                                              /* return error */
    }

    reg = 0xB6;                                                                /* set the reset value */
    if (a_bme280_iic_spi_write(handle, BME280_REG_RESET, &reg, 1) != 0)        /* reset the chip */
    {
        handle->debug_print("bme280: reset failed.\n");                        /* reset failed */

        return 1;                                                              /* return error */
    }
    handle->delay_ms(5);                                                       /* delay 5ms */

    return 0;                                                                  /* success return 0 */
}

/**
 * @brief      get status
 * @param[in]  *handle pointer to a bme280 handle structure
 * @param[out] *status pointer to a status buffer
 * @return     status code
 *             - 0 success
 *             - 1 get status failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t bme280_get_status(bme280_handle_t *handle, uint8_t *status)
{
    if (handle == NULL)                                                        /* check handle */
    {
        return 2;                                                              /* return error */
    }
    if (handle->inited != 1)                                                   /* check handle initialization */
    {
        return 3;                                                              /* return error */
    }

    if (a_bme280_iic_spi_read(handle, BME280_REG_STATUS, status, 1) != 0)      /* read status */
    {
        handle->debug_print("bme280: read status failed.\n");                  /* read status failed */

        return 1;                                                              /* return error */
    }

    return 0;                                                                  /* success return 0 */
}

/**
 * @brief     set humidity oversampling
 * @param[in] *handle pointer to a bme280 handle structure
 * @param[in] oversampling humidity oversampling
 * @return    status code
 *            - 0 success
 *            - 1 set humidity oversampling failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t bme280_set_humidity_oversampling(bme280_handle_t *handle, bme280_oversampling_t oversampling)
{
    uint8_t prev;

    if (handle == NULL)                                                        /* check handle */
    {
        return 2;                                                              /* return error */
    }
    if (handle->inited != 1)                                                   /* check handle initialization */
    {
        return 3;                                                              /* return error */
    }

    if (a_bme280_iic_spi_read(handle, BME280_REG_CTRL_HUM, &prev, 1) != 0)     /* read ctrl hum */
    {
        handle->debug_print("bme280: read ctrl hum failed.\n");                /* read ctrl hum failed */

        return 1;                                                              /* return error */
    }
    prev &= ~(7 << 0);                                                         /* clear settings */
    prev |= oversampling << 0;                                                 /* set oversampling */
    if (a_bme280_iic_spi_write(handle, BME280_REG_CTRL_HUM, &prev, 1) != 0)    /* write ctrl hum */
    {
        handle->debug_print("bme280: write ctrl hum failed.\n");               /* write ctrl hum failed */

        return 1;                                                              /* return error */
    }

    return 0;                                                                  /* success return 0 */
}

/**
 * @brief      get humidity oversampling
 * @param[in]  *handle pointer to a bme280 handle structure
 * @param[out] *oversampling pointer to a humidity oversampling buffer
 * @return     status code
 *             - 0 success
 *             - 1 get humidity oversampling failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t bme280_get_humidity_oversampling(bme280_handle_t *handle, bme280_oversampling_t *oversampling)
{
    uint8_t prev;

    if (handle == NULL)                                                        /* check handle */
    {
        return 2;                                                              /* return error */
    }
    if (handle->inited != 1)                                                   /* check handle initialization */
    {
        return 3;                                                              /* return error */
    }

    if (a_bme280_iic_spi_read(handle, BME280_REG_CTRL_HUM, &prev, 1) != 0)     /* read ctrl hum */
    {
        handle->debug_print("bme280: read ctrl hum failed.\n");                /* read ctrl hum failed */

        return 1;                                                              /* return error */
    }
    *oversampling = (bme280_oversampling_t)(prev & 0x07);                      /* get oversampling */

    return 0;                                                                  /* success return 0 */
}

/**
 * @brief     set temperatue oversampling
 * @param[in] *handle pointer to a bme280 handle structure
 * @param[in] oversampling temperatue oversampling
 * @return    status code
 *            - 0 success
 *            - 1 set temperatue oversampling failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t bme280_set_temperatue_oversampling(bme280_handle_t *handle, bme280_oversampling_t oversampling)
{
    uint8_t prev;

    if (handle == NULL)                                                        /* check handle */
    {
        return 2;                                                              /* return error */
    }
    if (handle->inited != 1)                                                   /* check handle initialization */
    {
        return 3;                                                              /* return error */
    }

    if (a_bme280_iic_spi_read(handle, BME280_REG_CTRL_MEAS, &prev, 1) != 0)    /* read ctrl meas */
    {
        handle->debug_print("bme280: read ctrl meas failed.\n");               /* read ctrl meas failed */

        return 1;                                                              /* return error */
    }
    prev &= ~(7 << 5);                                                         /* clear settings */
    prev |= oversampling << 5;                                                 /* set oversampling */
    if (a_bme280_iic_spi_write(handle, BME280_REG_CTRL_MEAS, &prev, 1) != 0)   /* write ctrl meas */
    {
        handle->debug_print("bme280: write ctrl meas failed.\n");              /* write ctrl meas failed */

        return 1;                                                              /* return error */
    }

    return 0;                                                                  /* success return 0 */
}

/**
 * @brief      get temperatue oversampling
 * @param[in]  *handle pointer to a bme280 handle structure
 * @param[out] *oversampling pointer to a oversampling buffer
 * @return     status code
 *             - 0 success
 *             - 1 get temperatue oversampling failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t bme280_get_temperatue_oversampling(bme280_handle_t *handle, bme280_oversampling_t *oversampling)
{
    uint8_t prev;

    if (handle == NULL)                                                        /* check handle */
    {
        return 2;                                                              /* return error */
    }
    if (handle->inited != 1)                                                   /* check handle initialization */
    {
        return 3;                                                              /* return error */
    }

    if (a_bme280_iic_spi_read(handle, BME280_REG_CTRL_MEAS, &prev, 1) != 0)    /* read ctrl meas */
    {
        handle->debug_print("bme280: read ctrl meas failed.\n");               /* read ctrl meas failed */

        return 1;                                                              /* return error */
    }
    *oversampling = (bme280_oversampling_t)((prev >> 5) & 0x7);                /* set oversampling */

    return 0;                                                                  /* success return 0 */
}

/**
 * @brief     set pressure oversampling
 * @param[in] *handle pointer to a bme280 handle structure
 * @param[in] oversampling pressure oversampling
 * @return    status code
 *            - 0 success
 *            - 1 set pressure oversampling failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t bme280_set_pressure_oversampling(bme280_handle_t *handle, bme280_oversampling_t oversampling)
{
    uint8_t prev;

    if (handle == NULL)                                                        /* check handle */
    {
        return 2;                                                              /* return error */
    }
    if (handle->inited != 1)                                                   /* check handle initialization */
    {
        return 3;                                                              /* return error */
    }

    if (a_bme280_iic_spi_read(handle, BME280_REG_CTRL_MEAS, &prev, 1) != 0)    /* read ctrl meas */
    {
        handle->debug_print("bme280: read ctrl meas failed.\n");               /* read ctrl meas failed */

        return 1;                                                              /* return error */
    }
    prev &= ~(7 << 2);                                                         /* clear settings */
    prev |= oversampling << 2;                                                 /* set oversampling */
    if (a_bme280_iic_spi_write(handle, BME280_REG_CTRL_MEAS, &prev, 1) != 0)   /* write ctrl meas */
    {
        handle->debug_print("bme280: write ctrl meas failed.\n");              /* write ctrl meas failed */

        return 1;                                                              /* return error */
    }

    return 0;                                                                  /* success return 0 */
}

/**
 * @brief      get pressure oversampling
 * @param[in]  *handle pointer to a bme280 handle structure
 * @param[out] *oversampling pointer to a oversampling buffer
 * @return     status code
 *             - 0 success
 *             - 1 get pressure oversampling failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t bme280_get_pressure_oversampling(bme280_handle_t *handle, bme280_oversampling_t *oversampling)
{
    uint8_t prev;

    if (handle == NULL)                                                        /* check handle */
    {
        return 2;                                                              /* return error */
    }
    if (handle->inited != 1)                                                   /* check handle initialization */
    {
        return 3;                                                              /* return error */
    }

    if (a_bme280_iic_spi_read(handle, BME280_REG_CTRL_MEAS, &prev, 1) != 0)    /* read ctrl meas */
    {
        handle->debug_print("bme280: read ctrl meas failed.\n");               /* read ctrl meas failed */

        return 1;                                                              /* return error */
    }
    *oversampling = (bme280_oversampling_t)((prev >> 2) & 0x7);                /* set oversampling */

    return 0;                                                                  /* success return 0 */
}

/**
 * @brief     set mode
 * @param[in] *handle pointer to a bme280 handle structure
 * @param[in] mode chip mode
 * @return    status code
 *            - 0 success
 *            - 1 set mode failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t bme280_set_mode(bme280_handle_t *handle, bme280_mode_t mode)
{
    uint8_t prev;

    if (handle == NULL)                                                             /* check handle */
    {
        return 2;                                                                   /* return error */
    }
    if (handle->inited != 1)                                                        /* check handle initialization */
    {
        return 3;                                                                   /* return error */
    }

    if (a_bme280_iic_spi_read(handle, BME280_REG_CTRL_MEAS, &prev, 1) != 0)         /* read ctrl meas */
    {
        handle->debug_print("bme280: read ctrl meas failed.\n");                    /* read ctrl meas failed */

        return 1;                                                                   /* return error */
    }
    prev &= ~(3 << 0);                                                              /* clear settings */
    prev |= mode << 0;                                                              /* set mode */
    if (a_bme280_iic_spi_write(handle, BME280_REG_CTRL_MEAS, &prev, 1) != 0)        /* write ctrl meas */
    {
        handle->debug_print("bme280: write ctrl meas failed.\n");                   /* write ctrl meas failed */

        return 1;                                                                   /* return error */
    }

    return 0;                                                                       /* success return 0 */
}

/**
 * @brief      get mode
 * @param[in]  *handle pointer to a bme280 handle structure
 * @param[out] *mode pointer to a mode buffer
 * @return     status code
 *             - 0 success
 *             - 1 get mode failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t bme280_get_mode(bme280_handle_t *handle, bme280_mode_t *mode)
{
    uint8_t prev;

    if (handle == NULL)                                                        /* check handle */
    {
        return 2;                                                              /* return error */
    }
    if (handle->inited != 1)                                                   /* check handle initialization */
    {
        return 3;                                                              /* return error */
    }

    if (a_bme280_iic_spi_read(handle, BME280_REG_CTRL_MEAS, &prev, 1) != 0)    /* read ctrl meas */
    {
        handle->debug_print("bme280: read ctrl meas failed.\n");               /* read ctrl meas failed */

        return 1;                                                              /* return error */
    }
    *mode = (bme280_mode_t)((prev >> 0) & 0x3);                                /* set mode */

    return 0;                                                                  /* success return 0 */
}

/**
 * @brief     set standby time
 * @param[in] *handle pointer to a bme280 handle structure
 * @param[in] standby_time standby time
 * @return    status code
 *            - 0 success
 *            - 1 set standby time failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t bme280_set_standby_time(bme280_handle_t *handle, bme280_standby_time_t standby_time)
{
    uint8_t prev;

    if (handle == NULL)                                                          /* check handle */
    {
        return 2;                                                                /* return error */
    }
    if (handle->inited != 1)                                                     /* check handle initialization */
    {
        return 3;                                                                /* return error */
    }

    if (a_bme280_iic_spi_read(handle, BME280_REG_CONFIG, &prev, 1) != 0)         /* read config */
    {
        handle->debug_print("bme280: read config failed.\n");                    /* read config failed */

        return 1;                                                                /* return error */
    }
    prev &= ~(7 << 5);                                                           /* clear settings */
    prev |= standby_time << 5;                                                   /* set standby time */
    if (a_bme280_iic_spi_write(handle, BME280_REG_CONFIG, &prev, 1) != 0)        /* write config */
    {
        handle->debug_print("bme280: write config failed.\n");                   /* write config failed */

        return 1;                                                                /* return error */
    }

    return 0;                                                                    /* success return 0 */
}

/**
 * @brief      get standby time
 * @param[in]  *handle pointer to a bme280 handle structure
 * @param[out] *standby_time pointer to a standby time buffer
 * @return     status code
 *             - 0 success
 *             - 1 get standby time failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t bme280_get_standby_time(bme280_handle_t *handle, bme280_standby_time_t *standby_time)
{
    uint8_t prev;

    if (handle == NULL)                                                         /* check handle */
    {
        return 2;                                                               /* return error */
    }
    if (handle->inited != 1)                                                    /* check handle initialization */
    {
        return 3;                                                               /* return error */
    }

    if (a_bme280_iic_spi_read(handle, BME280_REG_CONFIG, &prev, 1) != 0)        /* read config */
    {
        handle->debug_print("bme280: read config failed.\n");                   /* read config failed */

        return 1;                                                               /* return error */
    }
    *standby_time = (bme280_standby_time_t)((prev >> 5) & 0x7);                 /* get standby time */

    return 0;                                                                   /* success return 0 */
}

/**
 * @brief     set filter
 * @param[in] *handle pointer to a bme280 handle structure
 * @param[in] filter input filter
 * @return    status code
 *            - 0 success
 *            - 1 set filter failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t bme280_set_filter(bme280_handle_t *handle, bme280_filter_t filter)
{
    uint8_t prev;

    if (handle == NULL)                                                          /* check handle */
    {
        return 2;                                                                /* return error */
    }
    if (handle->inited != 1)                                                     /* check handle initialization */
    {
        return 3;                                                                /* return error */
    }

    if (a_bme280_iic_spi_read(handle, BME280_REG_CONFIG, &prev, 1) != 0)         /* read config */
    {
        handle->debug_print("bme280: read config failed.\n");                    /* read config failed */

        return 1;                                                                /* return error */
    }
    prev &= ~(7 << 2);                                                           /* clear settings */
    prev |= (filter & 0x07) << 2;                                                /* set filter */
    if (a_bme280_iic_spi_write(handle, BME280_REG_CONFIG, &prev, 1) != 0)        /* write config */
    {
        handle->debug_print("bme280: write config failed.\n");                   /* write config failed */

        return 1;                                                                /* return error */
    }

    return 0;                                                                    /* success return 0 */
}

/**
 * @brief      get filter
 * @param[in]  *handle pointer to a bme280 handle structure
 * @param[out] *filter pointer to a filter buffer
 * @return     status code
 *             - 0 success
 *             - 1 get filter failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t bme280_get_filter(bme280_handle_t *handle, bme280_filter_t *filter)
{
    uint8_t prev;

    if (handle == NULL)                                                          /* check handle */
    {
        return 2;                                                                /* return error */
    }
    if (handle->inited != 1)                                                     /* check handle initialization */
    {
        return 3;                                                                /* return error */
    }

    if (a_bme280_iic_spi_read(handle, BME280_REG_CONFIG, &prev, 1) != 0)         /* read config */
    {
        handle->debug_print("bme280: read config failed.\n");                    /* read config failed */

        return 1;                                                                /* return error */
    }
    *filter = (bme280_filter_t)((prev >> 2) & 0x07);                             /* set filter */

    return 0;                                                                    /* success return 0 */
}

/**
 * @brief     set spi wire
 * @param[in] *handle pointer to a bme280 handle structure
 * @param[in] spi spi wire
 * @return    status code
 *            - 0 success
 *            - 1 set spi wire failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t bme280_set_spi_wire(bme280_handle_t *handle, bme280_spi_wire_t spi)
{
    uint8_t prev;

    if (handle == NULL)                                                          /* check handle */
    {
        return 2;                                                                /* return error */
    }
    if (handle->inited != 1)                                                     /* check handle initialization */
    {
        return 3;                                                                /* return error */
    }

    if (a_bme280_iic_spi_read(handle, BME280_REG_CONFIG, &prev, 1) != 0)         /* read config */
    {
        handle->debug_print("bme280: read config failed.\n");                    /* read config failed */

        return 1;                                                                /* return error */
    }
    prev &= ~(1 << 0);                                                           /* clear settings */
    prev |= spi << 0;                                                            /* set spi wire */
    if (a_bme280_iic_spi_write(handle, BME280_REG_CONFIG, &prev, 1) != 0)        /* write config */
    {
        handle->debug_print("bme280: write config failed.\n");                   /* write config failed */

        return 1;                                                                /* return error */
    }

    return 0;                                                                    /* success return 0 */
}

/**
 * @brief      get spi wire
 * @param[in]  *handle pointer to a bme280 handle structure
 * @param[out] *spi pointer to a spi wire buffer
 * @return     status code
 *             - 0 success
 *             - 1 get spi wire failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t bme280_get_spi_wire(bme280_handle_t *handle, bme280_spi_wire_t *spi)
{
    uint8_t prev;

    if (handle == NULL)                                                          /* check handle */
    {
        return 2;                                                                /* return error */
    }
    if (handle->inited != 1)                                                     /* check handle initialization */
    {
        return 3;                                                                /* return error */
    }

    if (a_bme280_iic_spi_read(handle, BME280_REG_CONFIG, &prev, 1) != 0)         /* read config */
    {
        handle->debug_print("bme280: read config failed.\n");                    /* read config failed */

        return 1;                                                                /* return error */
    }
    *spi = (bme280_spi_wire_t)((prev >> 0) & 0x01);                              /* get spi */

    return 0;                                                                    /* success return 0 */
}

/**
 * @brief      read the pressure data
 * @param[in]  *handle pointer to a bme280 handle structure
 * @param[out] *pressure_raw pointer to a raw pressure buffer
 * @param[out] *pressure_pa pointer to a converted pressure buffer
 * @return     status code
 *             - 0 success
 *             - 1 pressure read failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 compensate pressure failed
 *             - 5 read timeout
 * @note       none
 */
uint8_t bme280_read_pressure(bme280_handle_t *handle, uint32_t *pressure_raw, float *pressure_pa)
{
    uint8_t res;
    uint8_t prev;
    uint32_t timeout;
    uint32_t temperature_raw;
    float temperature_c;
    uint8_t buf[6];

    if (handle == NULL)                                                                        /* check handle */
    {
        return 2;                                                                              /* return error */
    }
    if (handle->inited != 1)                                                                   /* check handle initialization */
    {
        return 3;                                                                              /* return error */
    }

    if (a_bme280_iic_spi_read(handle, BME280_REG_CTRL_MEAS, &prev, 1) != 0)                    /* read ctrl meas */
    {
        handle->debug_print("bme280: read ctrl meas failed.\n");                               /* read ctrl meas failed */

        return 1;                                                                              /* return error */
    }
    if ((prev & 0x3) == 3)                                                                     /* normal mode */
    {
        res = a_bme280_iic_spi_read(handle, BME280_REG_PRESS_MSB, buf, 6);                     /* read temperature and pressure */
        if (res != 0)
        {
            handle->debug_print("bme280: read failed.\n");                                     /* read failed */

            return 1;                                                                          /* return error */
        }
        temperature_raw = ((((uint32_t)(buf[3])) << 12) |
                          (((uint32_t)(buf[4])) << 4) |
                          ((uint32_t)buf[5] >> 4));                                            /* set temperature raw */
        res = a_bme280_compensate_temperature(handle, temperature_raw, &temperature_c);        /* compensate temperature */
        if (res != 0)
        {
            handle->debug_print("bme280: compensate temperature failed.\n");                   /* compensate temperature failed */

            return 4;                                                                          /* return error */
        }
        *pressure_raw = ((((int32_t)(buf[0])) << 12) |
                        (((int32_t)(buf[1])) << 4) |
                        (((int32_t)(buf[2])) >> 4));                                           /* set pressure raw */
        res = a_bme280_compensate_pressure(handle, *pressure_raw, pressure_pa);                /* compensate pressure */
        if (res != 0)
        {
            handle->debug_print("bme280: compensate pressure failed.\n");                      /* compensate pressure failed */

            return 4;                                                                          /* return error */
        }
    }
    else                                                                                       /* forced mode */
    {
        if (a_bme280_iic_spi_read(handle, BME280_REG_CTRL_MEAS, &prev, 1) != 0)                /* read ctrl meas */
        {
            handle->debug_print("bme280: read ctrl meas failed.\n");                           /* read ctrl meas failed */

            return 1;                                                                          /* return error */
        }
        prev &= ~(3 << 0);                                                                     /* clear settings */
        prev |= 0x01 << 0;                                                                     /* set forced mode */
        if (a_bme280_iic_spi_write(handle, BME280_REG_CTRL_MEAS, &prev, 1) != 0)               /* write ctrl meas */
        {
            handle->debug_print("bme280: write ctrl meas failed.\n");                          /* write ctrl meas failed */

            return 1;                                                                          /* return error */
        }
        timeout = 10 * 1000;                                                                   /* set timeout */
        while (timeout != 0)                                                                   /* check timeout */
        {
            if (a_bme280_iic_spi_read(handle, BME280_REG_CTRL_MEAS, &prev, 1) != 0)            /* read ctrl meas */
            {
                handle->debug_print("bme280: read ctrl meas failed.\n");                       /* read ctrl meas failed */

                return 1;                                                                      /* return error */
            }
            if ((prev & 0x03) == 0)                                                            /* if finished */
            {
                break;                                                                         /* break */
            }
            handle->delay_ms(1);                                                               /* delay 1ms */
            timeout--;                                                                         /* timeout-- */
        }
        if (timeout == 0)                                                                      /* check timeout */
        {
            handle->debug_print("bme280: read timeout.\n");                                    /* read timeout */

            return 5;                                                                          /* return error */
        }
        res = a_bme280_iic_spi_read(handle, BME280_REG_PRESS_MSB, buf, 6);                     /* read temperature and pressure */
        if (res != 0)
        {
            handle->debug_print("bme280: read failed.\n");                                     /* read failed */

            return 1;                                                                          /* return error */
        }
        temperature_raw = ((((uint32_t)(buf[3])) << 12) |
                          (((uint32_t)(buf[4])) << 4) |
                          ((uint32_t)buf[5] >> 4));                                            /* set temperature raw */
        res = a_bme280_compensate_temperature(handle, temperature_raw, &temperature_c);        /* compensate temperature */
        if (res != 0)
        {
            handle->debug_print("bme280: compensate temperature failed.\n");                   /* compensate temperature failed */

            return 4;                                                                          /* return error */
        }
        *pressure_raw = ((((int32_t)(buf[0])) << 12) |
                        (((int32_t)(buf[1])) << 4) |
                        (((int32_t)(buf[2])) >> 4));                                           /* set pressure raw */
        res = a_bme280_compensate_pressure(handle, *pressure_raw, pressure_pa);                /* compensate pressure */
        if (res != 0)
        {
            handle->debug_print("bme280: compensate pressure failed.\n");                      /* compensate pressure failed */

            return 4;                                                                          /* return error */
        }
    }

    return 0;                                                                                  /* success return 0 */
}

/**
 * @brief      read the temperature data
 * @param[in]  *handle pointer to a bme280 handle structure
 * @param[out] *temperature_raw pointer to a raw temperature buffer
 * @param[out] *temperature_c pointer to a converted temperature buffer
 * @return     status code
 *             - 0 success
 *             - 1 temperature read failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 compensate pressure failed
 *             - 5 read timeout
 * @note       none
 */
uint8_t bme280_read_temperature(bme280_handle_t *handle, uint32_t *temperature_raw, float *temperature_c)
{
    uint8_t res;
    uint8_t prev;
    uint32_t timeout;
    uint8_t buf[6];

    if (handle == NULL)                                                                        /* check handle */
    {
        return 2;                                                                              /* return error */
    }
    if (handle->inited != 1)                                                                   /* check handle initialization */
    {
        return 3;                                                                              /* return error */
    }

    if (a_bme280_iic_spi_read(handle, BME280_REG_CTRL_MEAS, &prev, 1) != 0)                    /* read ctrl meas */
    {
        handle->debug_print("bme280: read ctrl meas failed.\n");                               /* read ctrl meas failed */

        return 1;                                                                              /* return error */
    }
    if ((prev & 0x3) == 3)                                                                     /* normal mode */
    {
        res = a_bme280_iic_spi_read(handle, BME280_REG_PRESS_MSB, buf, 6);                     /* read temperature and pressure */
        if (res != 0)
        {
            handle->debug_print("bme280: read failed.\n");                                     /* read failed */

            return 1;                                                                          /* return error */
        }
        *temperature_raw = ((((uint32_t)(buf[3])) << 12) |
                           (((uint32_t)(buf[4])) << 4) |
                           ((uint32_t)buf[5] >> 4));                                           /* set temperature raw */
        res = a_bme280_compensate_temperature(handle, *temperature_raw, temperature_c);        /* compensate temperature */
        if (res != 0)
        {
            handle->debug_print("bme280: compensate temperature failed.\n");                   /* compensate temperature failed */

            return 4;                                                                          /* return error */
        }
    }
    else                                                                                       /* forced mode */
    {
        if (a_bme280_iic_spi_read(handle, BME280_REG_CTRL_MEAS, &prev, 1) != 0)                /* read ctrl meas */
        {
            handle->debug_print("bme280: read ctrl meas failed.\n");                           /* read ctrl meas failed */

            return 1;                                                                          /* return error */
        }
        prev &= ~(3 << 0);                                                                     /* clear settings */
        prev |= 0x01 << 0;                                                                     /* set forced mode */
        if (a_bme280_iic_spi_write(handle, BME280_REG_CTRL_MEAS, &prev, 1) != 0)               /* write ctrl meas */
        {
            handle->debug_print("bme280: write ctrl meas failed.\n");                          /* write ctrl meas failed */

            return 1;                                                                          /* return error */
        }
        timeout = 10 * 1000;                                                                   /* set timeout */
        while (timeout != 0)                                                                   /* check timeout */
        {
            if (a_bme280_iic_spi_read(handle, BME280_REG_CTRL_MEAS, &prev, 1) != 0)            /* read ctrl meas */
            {
                handle->debug_print("bme280: read ctrl meas failed.\n");                       /* read ctrl meas failed */

                return 1;                                                                      /* return error */
            }
            if ((prev & 0x03) == 0)                                                            /* if finished */
            {
                break;                                                                         /* break */
            }
            handle->delay_ms(1);                                                               /* delay 1ms */
            timeout--;                                                                         /* timeout-- */
        }
        if (timeout == 0)                                                                      /* check timeout */
        {
            handle->debug_print("bme280: read timeout.\n");                                    /* read timeout */

            return 5;                                                                          /* return error */
        }
        res = a_bme280_iic_spi_read(handle, BME280_REG_PRESS_MSB, buf, 6);                     /* read temperature and pressure */
        if (res != 0)
        {
            handle->debug_print("bme280: read failed.\n");                                     /* read failed */

            return 1;                                                                          /* return error */
        }
        *temperature_raw = ((((uint32_t)(buf[3])) << 12) |
                           (((uint32_t)(buf[4])) << 4) |
                           ((uint32_t)buf[5] >> 4));                                           /* set temperature raw */
        res = a_bme280_compensate_temperature(handle, *temperature_raw, temperature_c);        /* compensate temperature */
        if (res != 0)
        {
            handle->debug_print("bme280: compensate temperature failed.\n");                   /* compensate temperature failed */

            return 4;                                                                          /* return error */
        }
    }

    return 0;                                                                                  /* success return 0 */
}

/**
 * @brief      read the humidity data
 * @param[in]  *handle pointer to a bme280 handle structure
 * @param[out] *humidity_raw pointer to a raw humidity buffer
 * @param[out] *humidity_percentage pointer to a converted humidity percentage buffer
 * @return     status code
 *             - 0 success
 *             - 1 read failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 compensate pressure failed
 *             - 5 read timeout
 * @note       none
 */
uint8_t bme280_read_humidity(bme280_handle_t *handle, uint32_t *humidity_raw, float *humidity_percentage)
{
    uint8_t res;
    uint8_t prev;
    uint32_t timeout;
    uint32_t temperature_raw;
    float temperature_c;
    uint8_t buf[8];

    if (handle == NULL)                                                                        /* check handle */
    {
        return 2;                                                                              /* return error */
    }
    if (handle->inited != 1)                                                                   /* check handle initialization */
    {
        return 3;                                                                              /* return error */
    }

    if (a_bme280_iic_spi_read(handle, BME280_REG_CTRL_MEAS, &prev, 1) != 0)                    /* read ctrl meas */
    {
        handle->debug_print("bme280: read ctrl meas failed.\n");                               /* read ctrl meas failed */

        return 1;                                                                              /* return error */
    }
    if ((prev & 0x3) == 3)                                                                     /* normal mode */
    {
        res = a_bme280_iic_spi_read(handle, BME280_REG_PRESS_MSB, buf, 8);                     /* read temperature pressure and humidity */
        if (res != 0)
        {
            handle->debug_print("bme280: read failed.\n");                                     /* read failed */

            return 1;                                                                          /* return error */
        }
        temperature_raw = ((((uint32_t)(buf[3])) << 12) |
                          (((uint32_t)(buf[4])) << 4) |
                          ((uint32_t)buf[5] >> 4));                                            /* set temperature raw */
        res = a_bme280_compensate_temperature(handle, temperature_raw, &temperature_c);        /* compensate temperature */
        if (res != 0)
        {
            handle->debug_print("bme280: compensate temperature failed.\n");                   /* compensate temperature failed */

            return 4;                                                                          /* return error */
        }
        *humidity_raw = (uint32_t)buf[6] << 8 | buf[7];                                        /* set humidity raw */
        res = a_bme280_compensate_humidity(handle, *humidity_raw, humidity_percentage);        /* compensate humidity */
        if (res != 0)
        {
            handle->debug_print("bme280: compensate humidity failed.\n");                      /* compensate humidity failed */

            return 4;                                                                          /* return error */
        }
    }
    else                                                                                       /* forced mode */
    {
        if (a_bme280_iic_spi_read(handle, BME280_REG_CTRL_MEAS, &prev, 1) != 0)                /* read ctrl meas */
        {
            handle->debug_print("bme280: read ctrl meas failed.\n");                           /* read ctrl meas failed */

            return 1;                                                                          /* return error */
        }
        prev &= ~(3 << 0);                                                                     /* clear settings */
        prev |= 0x01 << 0;                                                                     /* set forced mode */
        if (a_bme280_iic_spi_write(handle, BME280_REG_CTRL_MEAS, &prev, 1) != 0)               /* write ctrl meas */
        {
            handle->debug_print("bme280: write ctrl meas failed.\n");                          /* write ctrl meas failed */

            return 1;                                                                          /* return error */
        }
        timeout = 10 * 1000;                                                                   /* set timeout */
        while (timeout != 0)                                                                   /* check timeout */
        {
            if (a_bme280_iic_spi_read(handle, BME280_REG_CTRL_MEAS, &prev, 1) != 0)            /* read ctrl meas */
            {
                handle->debug_print("bme280: read ctrl meas failed.\n");                       /* read ctrl meas failed */

                return 1;                                                                      /* return error */
            }
            if ((prev & 0x03) == 0)                                                            /* if finished */
            {
                break;                                                                         /* break */
            }
            handle->delay_ms(1);                                                               /* delay 1ms */
            timeout--;                                                                         /* timeout-- */
        }
        if (timeout == 0)                                                                      /* check timeout */
        {
            handle->debug_print("bme280: read timeout.\n");                                    /* read timeout */

            return 5;                                                                          /* return error */
        }
        res = a_bme280_iic_spi_read(handle, BME280_REG_PRESS_MSB, buf, 8);                     /* read temperature pressure and humidity */
        if (res != 0)
        {
            handle->debug_print("bme280: read failed.\n");                                     /* read failed */

            return 1;                                                                          /* return error */
        }
        temperature_raw = ((((uint32_t)(buf[3])) << 12) |
                          (((uint32_t)(buf[4])) << 4) |
                          ((uint32_t)buf[5] >> 4));                                            /* set temperature raw */
        res = a_bme280_compensate_temperature(handle, temperature_raw, &temperature_c);        /* compensate temperature */
        if (res != 0)
        {
            handle->debug_print("bme280: compensate temperature failed.\n");                   /* compensate temperature failed */

            return 4;                                                                          /* return error */
        }
        *humidity_raw = (uint32_t)buf[6] << 8 | buf[7];                                        /* set humidity raw */
        res = a_bme280_compensate_humidity(handle, *humidity_raw, humidity_percentage);        /* compensate humidity */
        if (res != 0)
        {
            handle->debug_print("bme280: compensate humidity failed.\n");                      /* compensate humidity failed */

            return 4;                                                                          /* return error */
        }
    }

    return 0;                                                                                  /* success return 0 */
}

/**
 * @brief      read the temperature pressure and humidity data
 * @param[in]  *handle pointer to a bme280 handle structure
 * @param[out] *temperature_raw pointer to a raw temperature buffer
 * @param[out] *temperature_c pointer to a converted temperature buffer
 * @param[out] *pressure_raw pointer to a raw pressure buffer
 * @param[out] *pressure_pa pointer to a converted pressure buffer
 * @param[out] *humidity_raw pointer to a raw humidity buffer
 * @param[out] *humidity_percentage pointer to a converted humidity percentage buffer
 * @return     status code
 *             - 0 success
 *             - 1 read failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 compensate pressure failed
 *             - 5 read timeout
 * @note       none
 */
uint8_t bme280_read_temperature_pressure_humidity(bme280_handle_t *handle, uint32_t *temperature_raw, float *temperature_c,
                                                  uint32_t *pressure_raw, float *pressure_pa,
                                                  uint32_t *humidity_raw, float *humidity_percentage)
{
    uint8_t res;
    uint8_t prev;
    uint32_t timeout;
    uint8_t buf[8];

    if (handle == NULL)                                                                        /* check handle */
    {
        return 2;                                                                              /* return error */
    }
    if (handle->inited != 1)                                                                   /* check handle initialization */
    {
        return 3;                                                                              /* return error */
    }

    if (a_bme280_iic_spi_read(handle, BME280_REG_CTRL_MEAS, &prev, 1) != 0)                    /* read ctrl meas */
    {
        handle->debug_print("bme280: read ctrl meas failed.\n");                               /* read ctrl meas failed */

        return 1;                                                                              /* return error */
    }
    if ((prev & 0x3) == 3)                                                                     /* normal mode */
    {
        res = a_bme280_iic_spi_read(handle, BME280_REG_PRESS_MSB, buf, 8);                     /* read temperature pressure and humidity */
        if (res != 0)
        {
            handle->debug_print("bme280: read failed.\n");                                     /* read failed */

            return 1;                                                                          /* return error */
        }
        *temperature_raw = ((((uint32_t)(buf[3])) << 12) |
                           (((uint32_t)(buf[4])) << 4) |
                           ((uint32_t)buf[5] >> 4));                                           /* set temperature raw */
        res = a_bme280_compensate_temperature(handle, *temperature_raw, temperature_c);        /* compensate temperature */
        if (res != 0)
        {
            handle->debug_print("bme280: compensate temperature failed.\n");                   /* compensate temperature failed */

            return 4;                                                                          /* return error */
        }
        *pressure_raw = ((((int32_t)(buf[0])) << 12) |
                        (((int32_t)(buf[1])) << 4) |
                        (((int32_t)(buf[2])) >> 4));                                           /* set pressure raw */
        res = a_bme280_compensate_pressure(handle, *pressure_raw, pressure_pa);                /* compensate pressure */
        if (res != 0)
        {
            handle->debug_print("bme280: compensate pressure failed.\n");                      /* compensate pressure failed */

            return 4;                                                                          /* return error */
        }
        *humidity_raw = (uint32_t)buf[6] << 8 | buf[7];                                        /* set humidity raw */
        res = a_bme280_compensate_humidity(handle, *humidity_raw, humidity_percentage);        /* compensate humidity */
        if (res != 0)
        {
            handle->debug_print("bme280: compensate humidity failed.\n");                      /* compensate humidity failed */

            return 4;                                                                          /* return error */
        }
    }
    else                                                                                       /* forced mode */
    {
        if (a_bme280_iic_spi_read(handle, BME280_REG_CTRL_MEAS, &prev, 1) != 0)                /* read ctrl meas */
        {
            handle->debug_print("bme280: read ctrl meas failed.\n");                           /* read ctrl meas failed */

            return 1;                                                                          /* return error */
        }
        prev &= ~(3 << 0);                                                                     /* clear settings */
        prev |= 0x01 << 0;                                                                     /* set forced mode */
        if (a_bme280_iic_spi_write(handle, BME280_REG_CTRL_MEAS, &prev, 1) != 0)               /* write ctrl meas */
        {
            handle->debug_print("bme280: write ctrl meas failed.\n");                          /* write ctrl meas failed */

            return 1;                                                                          /* return error */
        }
        timeout = 10 * 1000;                                                                   /* set timeout */
        while (timeout != 0)                                                                   /* check timeout */
        {
            if (a_bme280_iic_spi_read(handle, BME280_REG_CTRL_MEAS, &prev, 1) != 0)            /* read ctrl meas */
            {
                handle->debug_print("bme280: read ctrl meas failed.\n");                       /* read ctrl meas failed */

                return 1;                                                                      /* return error */
            }
            if ((prev & 0x03) == 0)                                                            /* if finished */
            {
                break;                                                                         /* break */
            }
            handle->delay_ms(1);                                                               /* delay 1ms */
            timeout--;                                                                         /* timeout-- */
        }
        if (timeout == 0)                                                                      /* check timeout */
        {
            handle->debug_print("bme280: read timeout.\n");                                    /* read timeout */

            return 5;                                                                          /* return error */
        }
        res = a_bme280_iic_spi_read(handle, BME280_REG_PRESS_MSB, buf, 8);                     /* read temperature pressure and humidity */
        if (res != 0)
        {
            handle->debug_print("bme280: read failed.\n");                                     /* read failed */

            return 1;                                                                          /* return error */
        }
        *temperature_raw = ((((uint32_t)(buf[3])) << 12) |
                           (((uint32_t)(buf[4])) << 4) |
                           ((uint32_t)buf[5] >> 4));                                           /* set temperature raw */
        res = a_bme280_compensate_temperature(handle, *temperature_raw, temperature_c);        /* compensate temperature */
        if (res != 0)
        {
            handle->debug_print("bme280: compensate temperature failed.\n");                   /* compensate temperature failed */

            return 4;                                                                          /* return error */
        }
        *pressure_raw = ((((int32_t)(buf[0])) << 12) |
                        (((int32_t)(buf[1])) << 4) |
                        (((int32_t)(buf[2])) >> 4));                                           /* set pressure raw */
        res = a_bme280_compensate_pressure(handle, *pressure_raw, pressure_pa);                /* compensate pressure */
        if (res != 0)
        {
            handle->debug_print("bme280: compensate pressure failed.\n");                      /* compensate pressure failed */

            return 4;                                                                          /* return error */
        }
        *humidity_raw = (uint32_t)buf[6] << 8 | buf[7];                                        /* set humidity raw */
        res = a_bme280_compensate_humidity(handle, *humidity_raw, humidity_percentage);        /* compensate humidity */
        if (res != 0)
        {
            handle->debug_print("bme280: compensate humidity failed.\n");                      /* compensate humidity failed */

            return 4;                                                                          /* return error */
        }
    }

    return 0;                                                                                  /* success return 0 */
}

/**
 * @brief     set the chip register
 * @param[in] *handle pointer to a bme280 handle structure
 * @param[in] reg iic register address
 * @param[in] value data written to the register
 * @return    status code
 *            - 0 success
 *            - 1 write failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t bme280_set_reg(bme280_handle_t *handle, uint8_t reg, uint8_t value)
{
    if (handle == NULL)                                          /* check handle */
    {
        return 2;                                                /* return error */
    }
    if (handle->inited != 1)                                     /* check handle initialization */
    {
        return 3;                                                /* return error */
    }

    return a_bme280_iic_spi_write(handle, reg, &value, 1);       /* write register */
}

/**
 * @brief      get the chip register
 * @param[in]  *handle pointer to a bme280 handle structure
 * @param[in]  reg iic register address
 * @param[out] *value pointer to a read data buffer
 * @return     status code
 *             - 0 success
 *             - 1 read failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t bme280_get_reg(bme280_handle_t *handle, uint8_t reg, uint8_t *value)
{
    if (handle == NULL)                                        /* check handle */
    {
        return 2;                                              /* return error */
    }
    if (handle->inited != 1)                                   /* check handle initialization */
    {
        return 3;                                              /* return error */
    }

    return a_bme280_iic_spi_read(handle, reg, value, 1);       /* read register */
}

/**
 * @brief      get chip's information
 * @param[out] *info pointer to a bme280 info structure
 * @return     status code
 *             - 0 success
 *             - 2 handle is NULL
 * @note       none
 */
uint8_t bme280_info(bme280_info_t *info)
{
    if (info == NULL)                                               /* check handle */
    {
        return 2;                                                   /* return error */
    }

    memset(info, 0, sizeof(bme280_info_t));                         /* initialize bme280 info structure */
    strncpy(info->chip_name, CHIP_NAME, 32);                        /* copy chip name */
    strncpy(info->manufacturer_name, MANUFACTURER_NAME, 32);        /* copy manufacturer name */
    strncpy(info->interface, "IIC SPI", 8);                         /* copy interface name */
    info->supply_voltage_min_v = SUPPLY_VOLTAGE_MIN;                /* set minimal supply voltage */
    info->supply_voltage_max_v = SUPPLY_VOLTAGE_MAX;                /* set maximum supply voltage */
    info->max_current_ma = MAX_CURRENT;                             /* set maximum current */
    info->temperature_max = TEMPERATURE_MAX;                        /* set minimal temperature */
    info->temperature_min = TEMPERATURE_MIN;                        /* set maximum temperature */
    info->driver_version = DRIVER_VERSION;                          /* set driver version */

    return 0;                                                       /* success return 0 */
}
