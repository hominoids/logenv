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
 * @file      driver_bme680.c
 * @brief     driver bme680 source file
 * @version   1.0.0
 * @author    Shifeng Li
 * @date      2025-07-15
 *
 * <h3>history</h3>
 * <table>
 * <tr><th>Date        <th>Version  <th>Author      <th>Description
 * <tr><td>2025/07/15  <td>1.0      <td>Shifeng Li  <td>first upload
 * </table>
 */

#include "driver_bme680.h"

/**
 * @brief chip information definition
 */
#define CHIP_NAME                 "Bosch BME680"        /**< chip name */
#define MANUFACTURER_NAME         "Bosch"               /**< manufacturer name */
#define SUPPLY_VOLTAGE_MIN        1.71f                 /**< chip min supply voltage */
#define SUPPLY_VOLTAGE_MAX        3.6f                  /**< chip max supply voltage */
#define MAX_CURRENT               0.849f                /**< chip max current */
#define TEMPERATURE_MIN           -40.0f                /**< chip min operating temperature */
#define TEMPERATURE_MAX           85.0f                 /**< chip max operating temperature */
#define DRIVER_VERSION            1000                  /**< driver version */

/**
 * @brief chip register definition
 */
#define BME680_REG_NVM_PAR_T1_L        0xE9        /**< NVM PAR T1 low register */
#define BME680_REG_NVM_PAR_T1_H        0xEA        /**< NVM PAR T1 high register */
#define BME680_REG_NVM_PAR_T2_L        0x8A        /**< NVM PAR T2 low register */
#define BME680_REG_NVM_PAR_T2_H        0x8B        /**< NVM PAR T2 high register */
#define BME680_REG_NVM_PAR_T3          0x8C        /**< NVM PAR T3 register */
#define BME680_REG_NVM_PAR_P1_L        0x8E        /**< NVM PAR P1 low register */
#define BME680_REG_NVM_PAR_P1_H        0x8F        /**< NVM PAR P1 high register */
#define BME680_REG_NVM_PAR_P2_L        0x90        /**< NVM PAR P2 low register */
#define BME680_REG_NVM_PAR_P2_H        0x91        /**< NVM PAR P2 high register */
#define BME680_REG_NVM_PAR_P3          0x92        /**< NVM PAR P3 register */
#define BME680_REG_NVM_PAR_P4_L        0x94        /**< NVM PAR P4 low register */
#define BME680_REG_NVM_PAR_P4_H        0x95        /**< NVM PAR P4 high register */
#define BME680_REG_NVM_PAR_P5_L        0x96        /**< NVM PAR P5 low register */
#define BME680_REG_NVM_PAR_P5_H        0x97        /**< NVM PAR P5 high register */
#define BME680_REG_NVM_PAR_P6          0x99        /**< NVM PAR P6 register */
#define BME680_REG_NVM_PAR_P7          0x98        /**< NVM PAR P7 register */
#define BME680_REG_NVM_PAR_P8_L        0x9C        /**< NVM PAR P8 low register */
#define BME680_REG_NVM_PAR_P8_H        0x9D        /**< NVM PAR P8 high register */
#define BME680_REG_NVM_PAR_P9_L        0x9E        /**< NVM PAR P9 low register */
#define BME680_REG_NVM_PAR_P9_H        0x9F        /**< NVM PAR P9 high register */
#define BME680_REG_NVM_PAR_P10         0xA0        /**< NVM PAR P10 register */
#define BME680_REG_NVM_PAR_H1_L        0xE2        /**< NVM PAR H1 low register */
#define BME680_REG_NVM_PAR_H1_H        0xE3        /**< NVM PAR H1 high register */
#define BME680_REG_NVM_PAR_H2_L        0xE2        /**< NVM PAR H2 low register */
#define BME680_REG_NVM_PAR_H2_H        0xE1        /**< NVM PAR H2 high register */
#define BME680_REG_NVM_PAR_H3          0xE4        /**< NVM PAR H3 register */
#define BME680_REG_NVM_PAR_H4          0xE5        /**< NVM PAR H4 register */
#define BME680_REG_NVM_PAR_H5          0xE6        /**< NVM PAR H5 register */
#define BME680_REG_NVM_PAR_H6          0xE7        /**< NVM PAR H6 register */
#define BME680_REG_NVM_PAR_H7          0xE8        /**< NVM PAR H7 register */
#define BME680_REG_NVM_PAR_G1          0xED        /**< NVM PAR G1 register */
#define BME680_REG_NVM_PAR_G2_L        0xEB        /**< NVM PAR G2 low register */
#define BME680_REG_NVM_PAR_G2_H        0xEC        /**< NVM PAR G2 high register */
#define BME680_REG_NVM_PAR_G3          0xEE        /**< NVM PAR G3 register */
#define BME680_REG_NVM_RES_HEAT_RANGE  0x02        /**< NVM RES HEAT RANGE register */
#define BME680_REG_NVM_RES_HEAT_VAL    0x00        /**< NVM RES HEAT VAL register */
#define BME680_REG_NVM_RANGE_SWITCH    0x04        /**< NVM RANGE SWITCH ERROR register */
#define BME680_REG_STATUS              0x73        /**< status register */
#define BME680_REG_RESET               0xE0        /**< reset register */
#define BME680_REG_ID                  0xD0        /**< id register */
#define BME680_REG_CONFIG              0x75        /**< config register */
#define BME680_REG_CTRL_MEAS           0x74        /**< control meas register */
#define BME680_REG_CTRL_HUM            0x72        /**< control humidity register */
#define BME680_REG_CTRL_GAS_1          0x71        /**< control gas1 register */
#define BME680_REG_CTRL_GAS_0          0x70        /**< control gas0 register */
#define BME680_REG_GAS_WAIT_X          0x64        /**< gas wait x register */
#define BME680_REG_RES_HEAT_X          0x5A        /**< res heat x register */
#define BME680_REG_IDAC_HEAT_X         0x50        /**< idac heat x register */
#define BME680_REG_GAS_R_LSB           0x2B        /**< gas r lsb register */
#define BME680_REG_GAS_R_MSB           0x2A        /**< gas r msb register */
#define BME680_REG_HUM_LSB             0x26        /**< hum lsb register */
#define BME680_REG_HUM_MSB             0x25        /**< hum msb register */
#define BME680_REG_TEMP_XLSB           0x24        /**< temp xlsb register */
#define BME680_REG_TEMP_LSB            0x23        /**< temp lsb register */
#define BME680_REG_TEMP_MSB            0x22        /**< temp msb register */
#define BME680_REG_PRESS_XLSB          0x21        /**< press xlsb register */
#define BME680_REG_PRESS_LSB           0x20        /**< press lsb register */
#define BME680_REG_PRESS_MSB           0x1F        /**< press msb register */
#define BME680_REG_EAS_STATUS_0        0x1D        /**< eas status 0 register */

/**
 * @brief     change spi page
 * @param[in] *handle pointer to a bme680 handle structure
 * @param[in] page spi page
 * @return    status code
 *            - 0 success
 *            - 1 change failed
 * @note      none
 */
static uint8_t a_bme680_change_spi_page(bme680_handle_t *handle, uint8_t page)
{
    uint8_t reg;
    uint8_t buf[1];

    reg = BME680_REG_STATUS;                        /* set address */
    reg |= 1 << 7;                                  /* set read mode */
    if (handle->spi_read(reg, buf, 1) != 0)         /* spi read */
    {
        return 1;                                   /* return error */
    }
    if (page == 0)                                  /* check page */
    {
        buf[0] &= ~(1 << 4);                        /* set page 0 */
    }
    else
    {
        buf[0] |= 1 << 4;                           /* set page 1 */
    }
    reg &= ~(1 << 7);                               /* write mode */
    if (handle->spi_write(reg, buf, 1) != 0)        /* spi write */
    {
        return 1;                                   /* return error */
    }
    handle->page = page;                            /* save page */
    
    return 0;                                       /* success return 0 */
}

/**
 * @brief      read multiple bytes
 * @param[in]  *handle pointer to a bme680 handle structure
 * @param[in]  reg register address
 * @param[out] *buf pointer to a data buffer
 * @param[in]  len data length
 * @return     status code
 *             - 0 success
 *             - 1 iic spi read failed
 * @note       none
 */
static uint8_t a_bme680_iic_spi_read(bme680_handle_t *handle, uint8_t reg, uint8_t *buf, uint16_t len)
{
    if (handle->iic_spi == BME680_INTERFACE_IIC)                           /* iic interface */
    {
        if (handle->iic_read(handle->iic_addr, reg, buf, len) != 0)        /* iic read */
        {
            return 1;                                                      /* return error */
        }

        return 0;                                                          /* success return 0 */
    }
    else                                                                   /* spi interface */
    {
        if (reg > 0x7F)                                                    /* check address */
        {
            if (handle->page != 0)                                         /* check page */
            {
                if (a_bme680_change_spi_page(handle, 0) != 0)              /* change spi change */
                {
                    return 1;                                              /* return error */
                }
            }
            reg -= 0x80;                                                   /* set address */
            reg |= 1 << 7;                                                 /* set read mode */
            if (handle->spi_read(reg, buf, len) != 0)                      /* spi read */
            {
                return 1;                                                  /* return error */
            }
        }
        else
        {
            if (handle->page == 0)                                         /* check page */
            {
                if (a_bme680_change_spi_page(handle, 1) != 0)              /* change spi change */
                {
                    return 1;                                              /* return error */
                }
            }
            reg |= 1 << 7;                                                 /* set read mode */
            if (handle->spi_read(reg, buf, len) != 0)                      /* spi read */
            {
                return 1;                                                  /* return error */
            }
        }

        return 0;                                                          /* success return 0 */
    }
}

/**
 * @brief     write multiple bytes
 * @param[in] *handle pointer to a bme680 handle structure
 * @param[in] reg register address
 * @param[in] *buf pointer to a data buffer
 * @param[in] len data length
 * @return    status code
 *            - 0 success
 *            - 1 iic spi write failed
 * @note      none
 */
static uint8_t a_bme680_iic_spi_write(bme680_handle_t *handle, uint8_t reg, uint8_t *buf, uint16_t len)
{
    if (handle->iic_spi == BME680_INTERFACE_IIC)                           /* iic interface */
    {
        if (handle->iic_write(handle->iic_addr, reg, buf, len) != 0)       /* iic write */
        {
            return 1;                                                      /* return error */
        }

        return 0;                                                          /* success return 0 */
    }
    else                                                                   /* spi interface */
    {
        if (reg > 0x7F)                                                    /* check address */
        {
            if (handle->page != 0)                                         /* check page */
            {
                if (a_bme680_change_spi_page(handle, 0) != 0)              /* change spi change */
                {
                    return 1;                                              /* return error */
                }
            }
            reg -= 0x80;                                                   /* set address */
            reg &= ~(1 << 7);                                              /* write mode */
            if (handle->spi_write(reg, buf, len) != 0)                     /* spi write */
            {
                return 1;                                                  /* return error */
            }
        }
        else
        {
            if (handle->page == 0)                                         /* check page */
            {
                if (a_bme680_change_spi_page(handle, 1) != 0)              /* change spi change */
                {
                    return 1;                                              /* return error */
                }
            }
            reg &= ~(1 << 7);                                              /* write mode */
            if (handle->spi_write(reg, buf, len) != 0)                     /* spi write */
            {
                return 1;                                                  /* return error */
            }
        }

        return 0;                                                          /* success return 0 */
    }
}

/**
 * @brief     get nvm calibration
 * @param[in] *handle pointer to a bme680 handle structure
 * @return    status code
 *            - 0 success
 *            - 1 get calibration data failed
 * @note      none
 */
static uint8_t a_bme680_get_nvm_calibration(bme680_handle_t *handle)
{
    uint8_t reg;
    uint8_t buf[2];

    if (a_bme680_iic_spi_read(handle, BME680_REG_NVM_PAR_T1_L, (uint8_t *)buf, 2) != 0)        /* read t1 */
    {
        handle->debug_print("bme680: get calibration data failed.\n");                         /* get calibration data failed */

        return 1;                                                                              /* return error */
    }
    handle->par_t1 = (uint16_t)buf[1] << 8 | buf[0];                                           /* set par t1 */
    if (a_bme680_iic_spi_read(handle, BME680_REG_NVM_PAR_T2_L, (uint8_t *)buf, 2) != 0)        /* read t2 */
    {
        handle->debug_print("bme680: get calibration data failed.\n");                         /* get calibration data failed */

        return 1;                                                                              /* return error */
    }
    handle->par_t2 = (int16_t)((uint16_t)buf[1] << 8 | buf[0]);                                /* set par t2 */
    if (a_bme680_iic_spi_read(handle, BME680_REG_NVM_PAR_T3, (uint8_t *)&reg, 1) != 0)         /* read t3 */
    {
        handle->debug_print("bme680: get calibration data failed.\n");                         /* get calibration data failed */

        return 1;                                                                              /* return error */
    }
    handle->par_t3 = (int8_t)(reg);                                                            /* set par t3 */
    if (a_bme680_iic_spi_read(handle, BME680_REG_NVM_PAR_P1_L, (uint8_t *)buf, 2) != 0)        /* read p1 */
    {
        handle->debug_print("bme680: get calibration data failed.\n");                         /* get calibration data failed */

        return 1;                                                                              /* return error */
    }
    handle->par_p1 = (uint16_t)buf[1] <<8 | buf[0];                                            /* set par p1 */
    if (a_bme680_iic_spi_read(handle, BME680_REG_NVM_PAR_P2_L, (uint8_t *)buf, 2) != 0)        /* read p2 */
    {
        handle->debug_print("bme680: get calibration data failed.\n");                         /* get calibration data failed */

        return 1;                                                                              /* return error */
    }
    handle->par_p2 = (int16_t)((uint16_t)buf[1] << 8 | buf[0]);                                /* set par p2 */
    if (a_bme680_iic_spi_read(handle, BME680_REG_NVM_PAR_P3, (uint8_t *)&reg, 1) != 0)         /* read p3 */
    {
        handle->debug_print("bme680: get calibration data failed.\n");                         /* get calibration data failed */

        return 1;                                                                              /* return error */
    }
    handle->par_p3 = (int8_t)(reg);                                                            /* set par p3 */
    if (a_bme680_iic_spi_read(handle, BME680_REG_NVM_PAR_P4_L, (uint8_t *)buf, 2) != 0)        /* read p4 */
    {
        handle->debug_print("bme680: get calibration data failed.\n");                         /* get calibration data failed */

        return 1;                                                                              /* return error */
    }
    handle->par_p4 = (int16_t)((uint16_t)buf[1] << 8 | buf[0]);                                /* set par p4 */
    if (a_bme680_iic_spi_read(handle, BME680_REG_NVM_PAR_P5_L, (uint8_t *)buf, 2) != 0)        /* read p5 */
    {
        handle->debug_print("bme680: get calibration data failed.\n");                         /* get calibration data failed */

        return 1;                                                                              /* return error */
    }
    handle->par_p5 = (int16_t)((uint16_t)buf[1] << 8 | buf[0]);                                /* set par p5 */
    if (a_bme680_iic_spi_read(handle, BME680_REG_NVM_PAR_P6, (uint8_t *)&reg, 1) != 0)         /* read p6 */
    {
        handle->debug_print("bme680: get calibration data failed.\n");                         /* get calibration data failed */

        return 1;                                                                              /* return error */
    }
    handle->par_p6 = (int8_t)(reg);                                                            /* set par p6 */
    if (a_bme680_iic_spi_read(handle, BME680_REG_NVM_PAR_P7, (uint8_t *)&reg, 1) != 0)         /* read p7 */
    {
        handle->debug_print("bme680: get calibration data failed.\n");                         /* get calibration data failed */

        return 1;                                                                              /* return error */
    }
    handle->par_p7 = (int8_t)(reg);                                                            /* set par p7 */
    if (a_bme680_iic_spi_read(handle, BME680_REG_NVM_PAR_P8_L, (uint8_t *)buf, 2) != 0)        /* read p8 */
    {
        handle->debug_print("bme680: get calibration data failed.\n");                         /* get calibration data failed */

        return 1;                                                                              /* return error */
    }
    handle->par_p8 = (int16_t)((uint16_t)buf[1] << 8 | buf[0]);                                /* set par p8 */
    if (a_bme680_iic_spi_read(handle, BME680_REG_NVM_PAR_P9_L, (uint8_t *)buf, 2) != 0)        /* read p9 */
    {
        handle->debug_print("bme680: get calibration data failed.\n");                         /* get calibration data failed */

        return 1;                                                                              /* return error */
    }
    handle->par_p9 = (int16_t)((uint16_t)buf[1] << 8 | buf[0]);                                /* set par p9 */
    if (a_bme680_iic_spi_read(handle, BME680_REG_NVM_PAR_P10, (uint8_t *)&reg, 1) != 0)        /* read p10 */
    {
        handle->debug_print("bme680: get calibration data failed.\n");                         /* get calibration data failed */

        return 1;                                                                              /* return error */
    }
    handle->par_p10 = (uint8_t)(reg);                                                          /* set par p10 */
    if (a_bme680_iic_spi_read(handle, BME680_REG_NVM_PAR_H1_L, (uint8_t *)buf, 2) != 0)        /* read h1 */
    {
        handle->debug_print("bme680: get calibration data failed.\n");                         /* get calibration data failed */

        return 1;                                                                              /* return error */
    }
    handle->par_h1 = (uint16_t)((uint16_t)buf[1] << 4 | buf[0] & 0xF);                         /* set h1 */
    if (a_bme680_iic_spi_read(handle, BME680_REG_NVM_PAR_H2_H, (uint8_t *)buf, 2) != 0)        /* read h2 */
    {
        handle->debug_print("bme680: get calibration data failed.\n");                         /* get calibration data failed */

        return 1;                                                                              /* return error */
    }
    handle->par_h2 = (uint16_t)((uint16_t)buf[0] << 4 | ((buf[1] >> 4) & 0xF));                /* set h2 */
    if (a_bme680_iic_spi_read(handle, BME680_REG_NVM_PAR_H3, (uint8_t *)&reg, 1) != 0)         /* read h3 */
    {
        handle->debug_print("bme680: get calibration data failed.\n");                         /* get calibration data failed */

        return 1;                                                                              /* return error */
    }
    handle->par_h3 = (int8_t)reg;                                                              /* set h3 */
    if (a_bme680_iic_spi_read(handle, BME680_REG_NVM_PAR_H4, (uint8_t *)&reg, 1) != 0)         /* read h4 */
    {
        handle->debug_print("bme680: get calibration data failed.\n");                         /* get calibration data failed */

        return 1;                                                                              /* return error */
    }
    handle->par_h4 = (int8_t)reg;                                                              /* set h4 */
    if (a_bme680_iic_spi_read(handle, BME680_REG_NVM_PAR_H5, (uint8_t *)&reg, 1) != 0)         /* read h5 */
    {
        handle->debug_print("bme680: get calibration data failed.\n");                         /* get calibration data failed */

        return 1;                                                                              /* return error */
    }
    handle->par_h5 = (int8_t)reg;                                                              /* set h5 */
    if (a_bme680_iic_spi_read(handle, BME680_REG_NVM_PAR_H6, (uint8_t *)&reg, 1) != 0)         /* read h6 */
    {
        handle->debug_print("bme680: get calibration data failed.\n");                         /* get calibration data failed */

        return 1;                                                                              /* return error */
    }
    handle->par_h6 = (int8_t)reg;                                                              /* set h6 */
    if (a_bme680_iic_spi_read(handle, BME680_REG_NVM_PAR_H7, (uint8_t *)&reg, 1) != 0)         /* read h7 */
    {
        handle->debug_print("bme680: get calibration data failed.\n");                         /* get calibration data failed */

        return 1;                                                                              /* return error */
    }
    handle->par_h7 = (int8_t)reg;                                                              /* set h7 */
    if (a_bme680_iic_spi_read(handle, BME680_REG_NVM_PAR_G1, (uint8_t *)&reg, 1) != 0)         /* read gh1 */
    {
        handle->debug_print("bme680: get calibration data failed.\n");                         /* get calibration data failed */

        return 1;                                                                              /* return error */
    }
    handle->par_gh1 = (int8_t)reg;                                                             /* set par gh1 */
    if (a_bme680_iic_spi_read(handle, BME680_REG_NVM_PAR_G2_L, (uint8_t *)buf, 2) != 0)        /* read gh2 */
    {
        handle->debug_print("bme680: get calibration data failed.\n");                         /* get calibration data failed */

        return 1;                                                                              /* return error */
    }
    handle->par_gh2 = (int16_t)((uint16_t)buf[1] << 8 | buf[0]);                               /* set par gh2 */
    if (a_bme680_iic_spi_read(handle, BME680_REG_NVM_PAR_G3, (uint8_t *)&reg, 1) != 0)         /* read gh3 */
    {
        handle->debug_print("bme680: get calibration data failed.\n");                         /* get calibration data failed */

        return 1;                                                                              /* return error */
    }
    handle->par_gh3 = (int8_t)reg;                                                             /* set par gh3 */
    if (a_bme680_iic_spi_read(handle, BME680_REG_NVM_RES_HEAT_RANGE, (uint8_t *)&reg, 1) != 0) /* read res heat range */
    {
        handle->debug_print("bme680: get calibration data failed.\n");                         /* get calibration data failed */

        return 1;                                                                              /* return error */
    }
    handle->res_heat_range = (uint8_t)((reg >> 4) & 3);                                        /* set res heat range */
    if (a_bme680_iic_spi_read(handle, BME680_REG_NVM_RES_HEAT_VAL, (uint8_t *)&reg, 1) != 0)   /* read res heat val */
    {
        handle->debug_print("bme680: get calibration data failed.\n");                         /* get calibration data failed */

        return 1;                                                                              /* return error */
    }
    handle->res_heat_val = (int8_t)(reg);                                                      /* set res heat val */
    if (a_bme680_iic_spi_read(handle, BME680_REG_NVM_RANGE_SWITCH, (uint8_t *)&reg, 1) != 0)   /* read range switch error */
    {
        handle->debug_print("bme680: get calibration data failed.\n");                         /* get calibration data failed */

        return 1;                                                                              /* return error */
    }
    handle->range_sw_err = (int8_t)((reg >> 4) & 0xF);                                         /* set range switch error */
    handle->t_fine = 0.0f;                                                                     /* init 0 */
    handle->amb_temp = 25;                                                                     /* set 25c */

    return 0;                                                                                  /* success return 0 */
}

/**
 * @brief      compensate temperature
 * @param[in]  *handle pointer to a bme680 handle structure
 * @param[in]  raw raw data
 * @param[out] *output pointer to an output buffer
 * @return     status code
 *             - 0 success
 *             - 1 compensate temperature failed
 * @note       none
 */
static uint8_t a_bme680_compensate_temperature(bme680_handle_t *handle, uint32_t raw, float *output)
{
    float var1;
    float var2;
    
    var1 = ((((float)raw / 16384.0f) - ((float)handle->par_t1 / 1024.0f)) * ((float)handle->par_t2));                 /* calculate var1 data */
    var2 = (((((float)raw / 131072.0f) - ((float)handle->par_t1 / 8192.0f)) *
           (((float)raw / 131072.0f) - ((float)handle->par_t1 / 8192.0f))) * ((float)handle->par_t3 * 16.0f));        /* calculate var2 data */
    handle->t_fine = (var1 + var2);                                                                                   /* t_fine value */
    *output = ((handle->t_fine) / 5120.0f);                                                                           /* compensated temperature data */
    
    return 0;                                                                                                         /* success return 0 */
}

/**
 * @brief      compensate pressure
 * @param[in]  *handle pointer to a bme680 handle structure
 * @param[in]  raw raw data
 * @param[out] *output pointer to an output buffer
 * @return     status code
 *             - 0 success
 *             - 1 compensate pressure failed
 * @note       none
 */
static uint8_t a_bme680_compensate_pressure(bme680_handle_t *handle, uint32_t raw, float *output)
{
    float var1;
    float var2;
    float var3;
    float calc_pres;

    var1 = (((float)handle->t_fine / 2.0f) - 64000.0f);                                                        /* set var1 */
    var2 = var1 * var1 * (((float)handle->par_p6) / (131072.0f));                                              /* set var2 */
    var2 = var2 + (var1 * ((float)handle->par_p5) * 2.0f);                                                     /* set var2 */
    var2 = (var2 / 4.0f) + (((float)handle->par_p4) * 65536.0f);                                               /* set var2 */
    var1 = (((((float)handle->par_p3 * var1 * var1) / 16384.0f) + 
           ((float)handle->par_p2 * var1)) / 524288.0f);                                                       /* set var1 */
    var1 = ((1.0f + (var1 / 32768.0f)) * ((float)handle->par_p1));                                             /* set var1 */
    calc_pres = (1048576.0f - ((float)raw));                                                                   /* set pressure */
    if ((int32_t)var1 != 0)                                                                                    /* avoid exception caused by division by zero */
    {
        calc_pres = (((calc_pres - (var2 / 4096.0f)) * 6250.0f) / var1);                                       /* set pressure */
        var1 = (((float)handle->par_p9) * calc_pres * calc_pres) / 2147483648.0f;                              /* set var1 */
        var2 = calc_pres * (((float)handle->par_p8) / 32768.0f);                                               /* set var2 */
        var3 = ((calc_pres / 256.0f) * (calc_pres / 256.0f) * (calc_pres / 256.0f) * 
               ((float)handle->par_p10 / 131072.0f));                                                          /* set var3 */
        calc_pres = (calc_pres + (var1 + var2 + var3 + ((float)handle->par_p7 * 128.0f)) / 16.0f);             /* set pressure */
    }
    else
    {
        calc_pres = 0.0f;                                                                                      /* set 0 */
    }
    *output= calc_pres;                                                                                        /* save to output */
    
    return 0;                                                                                                  /* success return 0 */
}

/**
 * @brief      compensate humidity
 * @param[in]  *handle pointer to a bme680 handle structure
 * @param[in]  raw raw data
 * @param[out] *output pointer to an output buffer
 * @return     status code
 *             - 0 success
 *             - 1 compensate humidity failed
 * @note       none
 */
static uint8_t a_bme680_compensate_humidity(bme680_handle_t *handle, uint16_t raw, float *output)
{
    float calc_hum;
    float var1;
    float var2;
    float var3;
    float var4;
    float temp_comp;
    
    temp_comp = ((handle->t_fine) / 5120.0f);                                                       /* compensated temperature data*/
    var1 = (float)((float)raw) -
           (((float)handle->par_h1 * 16.0f) + (((float)handle->par_h3 / 2.0f) * temp_comp));        /* set var1 */
    var2 = var1 * ((float)(((float)handle->par_h2 / 262144.0f) *
           (1.0f + (((float)handle->par_h4 / 16384.0f) * temp_comp) +
           (((float)handle->par_h5 / 1048576.0f) * temp_comp * temp_comp))));                       /* set var2 */
    var3 = (float)handle->par_h6 / 16384.0f;                                                        /* set var3 */
    var4 = (float)handle->par_h7 / 2097152.0f;                                                      /* set var4 */
    calc_hum = var2 + ((var3 + (var4 * temp_comp)) * var2 * var2);                                  /* set humidity */
    if (calc_hum > 100.0f)                                                                          /* check max range */
    {
        calc_hum = 100.0f;                                                                          /* set max */
    }
    else if (calc_hum < 0.0f)                                                                       /* check min range */
    {
        calc_hum = 0.0f;                                                                            /* set min */
    }
    else
    {
                                                                                                    /* do nothing */
    }
    *output = calc_hum;                                                                             /* save to output */
    
    return 0;                                                                                       /* success return 0 */
}

/**
 * @brief      compensate heat
 * @param[in]  *handle pointer to a bme680 handle structure
 * @param[in]  temp heat temp
 * @param[out] *reg pointer to a register buffer
 * @return     status code
 *             - 0 success
 *             - 1 compensate heat failed
 * @note       none
 */
static uint8_t a_bme680_compensate_heat(bme680_handle_t *handle, float temp, uint8_t *reg)
{
    float var1;
    float var2;
    float var3;
    float var4;
    float var5;
    uint8_t res_heat;

    if (temp > 400.0f)                                                                      /* cap temperature */
    {
        temp = 400.0f;                                                                      /* set 400 */
    }
    var1 = (((float)handle->par_gh1 / (16.0f)) + 49.0f);                                    /* set var1 */
    var2 = ((((float)handle->par_gh2 / (32768.0f)) * (0.0005f)) + 0.00235f);                /* set var2 */
    var3 = ((float)handle->par_gh3 / (1024.0f));                                            /* set var3 */
    var4 = (var1 * (1.0f + (var2 * (float)temp)));                                          /* set var4 */
    var5 = (var4 + (var3 * (float)handle->amb_temp));                                       /* set var5 */
    res_heat = (uint8_t)(3.4f * ((var5 * (4 / (4 + (float)handle->res_heat_range)) *
               (1 / (1 + ((float)handle->res_heat_val * 0.002f)))) - 25));                  /* set res heat */
    *reg =res_heat;                                                                         /* set res */
    
    return 0;                                                                               /* success return 0 */
}

/**
 * @brief      compensate gas resistance
 * @param[in]  *handle pointer to a bme680 handle structure
 * @param[in]  gas_res_adc gas resistance adc
 * @param[in]  gas_range gas range
 * @param[out] *ohms pointer to an ohms buffer
 * @return     status code
 *             - 0 success
 *             - 1 compensate gas resistance failed
 * @note       none
 */
static uint8_t a_bme680_compensate_gas_resistance(bme680_handle_t *handle, uint16_t gas_res_adc, uint8_t gas_range, float *ohms)
{
    float calc_gas_res;
    float var1;
    float var2;
    float var3;
    float gas_res_f = (float)gas_res_adc;
    float gas_range_f = (float)(1U << gas_range);
    const float lookup_k1_range[16] =
    {
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, -0.8f, 0.0f, 0.0f, -0.2f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f
    };
    const float lookup_k2_range[16] =
    {
        0.0f, 0.0f, 0.0f, 0.0f, 0.1f, 0.7f, 0.0f, -0.8f, -0.1f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f
    };

    var1 = (1340.0f + (5.0f * handle->range_sw_err));                          /* set var1 */
    var2 = (var1) * (1.0f + lookup_k1_range[gas_range] / 100.0f);              /* set var2 */
    var3 = 1.0f + (lookup_k2_range[gas_range] / 100.0f);                       /* set var3 */
    calc_gas_res = 1.0f / (float)(var3 * (0.000000125f) * gas_range_f * 
                   (((gas_res_f - 512.0f) / var2) + 1.0f));                    /* set gas resistance */
    *ohms = calc_gas_res;                                                      /* save gas resistance to ohms */
    
    return 0;                                                                  /* success return 0 */
}

/**
 * @brief     set the iic address pin
 * @param[in] *handle pointer to a bme680 handle structure
 * @param[in] addr_pin iic address pin
 * @return    status code
 *            - 0 success
 *            - 2 handle is NULL
 * @note      none
 */
uint8_t bme680_set_addr_pin(bme680_handle_t *handle, bme680_address_t addr_pin)
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
 * @param[in]  *handle pointer to a bme680 handle structure
 * @param[out] *addr_pin pointer to an iic address pin buffer
 * @return     status code
 *             - 0 success
 *             - 2 handle is NULL
 * @note       none
 */
uint8_t bme680_get_addr_pin(bme680_handle_t *handle, bme680_address_t *addr_pin)
{
    if (handle == NULL)                                    /* check handle */
    {
        return 2;                                          /* return error */
    }

    *addr_pin = (bme680_address_t)handle->iic_addr;        /* get iic address */

    return 0;                                              /* success return 0 */
}

/**
 * @brief     set the interface
 * @param[in] *handle pointer to a bme680 handle structure
 * @param[in] interface chip interface
 * @return    status code
 *            - 0 success
 *            - 2 handle is NULL
 * @note      none
 */
uint8_t bme680_set_interface(bme680_handle_t *handle, bme680_interface_t interface)
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
 * @param[in]  *handle pointer to a bme680 handle structure
 * @param[out] *interface pointer to a chip interface buffer
 * @return     status code
 *             - 0 success
 *             - 2 handle is NULL
 * @note       none
 */
uint8_t bme680_get_interface(bme680_handle_t *handle, bme680_interface_t *interface)
{
    if (handle == NULL)                                        /* check handle */
    {
        return 2;                                              /* return error */
    }

    *interface = (bme680_interface_t)(handle->iic_spi);        /* get interface */

    return 0;                                                  /* success return 0 */
}

/**
 * @brief     initialize the chip
 * @param[in] *handle pointer to a bme680 handle structure
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
uint8_t bme680_init(bme680_handle_t *handle)
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
        handle->debug_print("bme680: iic_init is null.\n");                          /* iic_init is nul */

        return 3;                                                                    /* return error */
    }
    if (handle->iic_deinit == NULL)                                                  /* check iic_deinit */
    {
        handle->debug_print("bme680: iic_deinit is null.\n");                        /* iic_deinit is null */

        return 3;                                                                    /* return error */
    }
    if (handle->iic_read == NULL)                                                    /* check iic_read */
    {
        handle->debug_print("bme680: iic_read is null.\n");                          /* iic_read is null */

        return 3;                                                                    /* return error */
    }
    if (handle->iic_write == NULL)                                                   /* check iic_write */
    {
        handle->debug_print("bme680: iic_write is null.\n");                         /* iic_write is null */

        return 3;                                                                    /* return error */
    }
    if (handle->spi_init == NULL)                                                    /* check spi_init */
    {
        handle->debug_print("bme680: spi_init is null.\n");                          /* spi_init is nul */

        return 3;                                                                    /* return error */
    }
    if (handle->spi_deinit == NULL)                                                  /* check spi_deinit */
    {
        handle->debug_print("bme680: spi_deinit is null.\n");                        /* spi_deinit is nul */

        return 3;                                                                    /* return error */
    }
    if (handle->spi_read == NULL)                                                    /* check spi_read */
    {
        handle->debug_print("bme680: spi_read is null.\n");                          /* spi_read is nul */

        return 3;                                                                    /* return error */
    }
    if (handle->spi_write == NULL)                                                   /* check spi_write */
    {
        handle->debug_print("bme680: spi_write is null.\n");                         /* spi_write is nul */

        return 3;                                                                    /* return error */
    }
    if (handle->delay_ms == NULL)                                                    /* check delay_ms */
    {
        handle->debug_print("bme680: delay_ms is null.\n");                          /* delay_ms is null */

        return 3;                                                                    /* return error */
    }

    if (handle->iic_spi == BME680_INTERFACE_IIC)                                     /* iic interface */
    {
        if (handle->iic_init() != 0)                                                 /* iic init */
        {
            handle->debug_print("bme680: iic init failed.\n");                       /* iic init failed */

            return 1;                                                                /* return error */
        }
    }
    else                                                                             /* spi interface */
    {
        if (handle->spi_init() != 0)                                                 /* spi init */
        {
            handle->debug_print("bme680: spi init failed.\n");                       /* spi init failed */

            return 1;                                                                /* return error */
        }
        if (a_bme680_change_spi_page(handle, 0) != 0)                                /* change spi page 0 */
        {
            handle->debug_print("bme680: change spi page failed.\n");                /* change spi page failed */

            return 1;                                                                /* return error */
        }
    }

    if (a_bme680_iic_spi_read(handle, BME680_REG_ID, (uint8_t *)&id, 1) != 0)        /* read chip id */
    {
        handle->debug_print("bme680: read id failed.\n");                            /* read id failed */
        (void)handle->iic_deinit();                                                  /* iic deinit */

        return 4;                                                                    /* return error */
    }
    if (id != 0x61)                                                                  /* check id */
    {
        handle->debug_print("bme680: id is error.\n");                               /* id is error */
        (void)handle->iic_deinit();                                                  /* iic deinit */

        return 4;                                                                    /* return error */
    }
    reg = 0xB6;                                                                      /* set the reset value */
    if (a_bme680_iic_spi_write(handle, BME680_REG_RESET, &reg, 1) != 0)              /* reset the chip */
    {
        handle->debug_print("bme680: reset failed.\n");                              /* reset failed */
        (void)handle->iic_deinit();                                                  /* iic deinit */

        return 5;                                                                    /* return error */
    }
    handle->delay_ms(5);                                                             /* delay 5ms */
    if (a_bme680_get_nvm_calibration(handle) != 0)                                   /* get nvm calibration */
    {
        (void)handle->iic_deinit();                                                  /* iic deinit */

        return 6;                                                                    /* return error */
    }
    handle->inited = 1;                                                              /* flag finish initialization */

    return 0;                                                                        /* success return 0 */
}

/**
 * @brief     close the chip
 * @param[in] *handle pointer to a bme680 handle structure
 * @return    status code
 *            - 0 success
 *            - 1 iic deinit failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 power down failed
 * @note      none
 */
uint8_t bme680_deinit(bme680_handle_t *handle)
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

    if (a_bme680_iic_spi_read(handle, BME680_REG_CTRL_MEAS, &prev, 1) != 0)         /* read ctrl meas */
    {
        handle->debug_print("bme680: read ctrl meas failed.\n");                    /* read ctrl meas failed */

        return 4;                                                                   /* return error */
    }
    prev &= ~(3 << 0);                                                              /* clear settings */
    prev |= 0 << 0;                                                                 /* set sleep mode */
    if (a_bme680_iic_spi_write(handle, BME680_REG_CTRL_MEAS, &prev, 1) != 0)        /* write ctrl meas */
    {
        handle->debug_print("bme680: write ctrl meas failed.\n");                   /* write ctrl meas failed */

        return 4;                                                                   /* return error */
    }
    if (handle->iic_spi == BME680_INTERFACE_IIC)                                    /* iic interface */
    {
        if (handle->iic_deinit() != 0)                                              /* iic deinit */
        {
            handle->debug_print("bme680: iic deinit failed.\n");                    /* iic deinit failed */

            return 1;                                                               /* return error */
        }
    }
    else                                                                            /* spi interface */
    {
        if (handle->spi_deinit() != 0)                                              /* spi deinit */
        {
            handle->debug_print("bme680: spi deinit failed.\n");                    /* spi deinit failed */

            return 1;                                                               /* return error */
        }
    }
    handle->inited = 0;                                                             /* flag close */

    return 0;                                                                       /* success return 0 */
}

/**
 * @brief     set idac heat
 * @param[in] *handle pointer to a bme680 handle structure
 * @param[in] index input index
 * @param[in] reg input register
 * @return    status code
 *            - 0 success
 *            - 1 set idac heat failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 index > 9
 * @note      none
 */
uint8_t bme680_set_idac_heat(bme680_handle_t *handle, uint8_t index, uint8_t reg)
{
    uint8_t prev;
    
    if (handle == NULL)                                                                       /* check handle */
    {
        return 2;                                                                             /* return error */
    }
    if (handle->inited != 1)                                                                  /* check handle initialization */
    {
        return 3;                                                                             /* return error */
    }
    if (index > 9)                                                                            /* check index */
    {
        handle->debug_print("bme680: index > 9.\n");                                          /* index > 9 */

        return 4;                                                                             /* return error */
    }

    prev = reg;
    if (a_bme680_iic_spi_write(handle, BME680_REG_IDAC_HEAT_X + index, &prev, 1) != 0)        /* write idac heat */
    {
        handle->debug_print("bme680: write idac heat failed.\n");                             /* write idac heat failed */

        return 1;                                                                             /* return error */
    }

    return 0;                                                                                 /* success return 0 */
}

/**
 * @brief      get idac heat
 * @param[in]  *handle pointer to a bme680 handle structure
 * @param[in]  index input index
 * @param[out] *reg pointer to an input register buffer
 * @return     status code
 *             - 0 success
 *             - 1 get idac heat failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 index > 9
 * @note       none
 */
uint8_t bme680_get_idac_heat(bme680_handle_t *handle, uint8_t index, uint8_t *reg)
{
    if (handle == NULL)                                                                    /* check handle */
    {
        return 2;                                                                          /* return error */
    }
    if (handle->inited != 1)                                                               /* check handle initialization */
    {
        return 3;                                                                          /* return error */
    }
    if (index > 9)                                                                         /* check index */
    {
        handle->debug_print("bme680: index > 9.\n");                                       /* index > 9 */

        return 4;                                                                          /* return error */
    }

    if (a_bme680_iic_spi_read(handle, BME680_REG_IDAC_HEAT_X + index, reg, 1) != 0)        /* read idac heat */
    {
        handle->debug_print("bme680: read idac heat failed.\n");                           /* read idac heat failed */

        return 1;                                                                          /* return error */
    }

    return 0;                                                                              /* success return 0 */
}

/**
 * @brief     set resistance heat
 * @param[in] *handle pointer to a bme680 handle structure
 * @param[in] index input index
 * @param[in] reg input register
 * @return    status code
 *            - 0 success
 *            - 1 set resistance heat failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 index > 9
 * @note      none
 */
uint8_t bme680_set_resistance_heat(bme680_handle_t *handle, uint8_t index, uint8_t reg)
{
    uint8_t prev;
    
    if (handle == NULL)                                                                      /* check handle */
    {
        return 2;                                                                            /* return error */
    }
    if (handle->inited != 1)                                                                 /* check handle initialization */
    {
        return 3;                                                                            /* return error */
    }
    if (index > 9)                                                                           /* check index */
    {
        handle->debug_print("bme680: index > 9.\n");                                         /* index > 9 */

        return 4;                                                                            /* return error */
    }

    prev = reg;
    if (a_bme680_iic_spi_write(handle, BME680_REG_RES_HEAT_X + index, &prev, 1) != 0)        /* write resistance heat */
    {
        handle->debug_print("bme680: write resistance heat failed.\n");                      /* write resistance heat failed */

        return 1;                                                                            /* return error */
    }

    return 0;                                                                                /* success return 0 */
}

/**
 * @brief      get resistance heat
 * @param[in]  *handle pointer to a bme680 handle structure
 * @param[in]  index input index
 * @param[out] *reg pointer to an input register buffer
 * @return     status code
 *             - 0 success
 *             - 1 get resistance heat failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 index > 9
 * @note       none
 */
uint8_t bme680_get_resistance_heat(bme680_handle_t *handle, uint8_t index, uint8_t *reg)
{
    if (handle == NULL)                                                                   /* check handle */
    {
        return 2;                                                                         /* return error */
    }
    if (handle->inited != 1)                                                              /* check handle initialization */
    {
        return 3;                                                                         /* return error */
    }
    if (index > 9)                                                                        /* check index */
    {
        handle->debug_print("bme680: index > 9.\n");                                      /* index > 9 */

        return 4;                                                                         /* return error */
    }

    if (a_bme680_iic_spi_read(handle, BME680_REG_RES_HEAT_X + index, reg, 1) != 0)        /* read resistance heat */
    {
        handle->debug_print("bme680: read resistance heat failed.\n");                    /* read resistance heat failed */

        return 1;                                                                         /* return error */
    }

    return 0;                                                                             /* success return 0 */
}

/**
 * @brief     set gas wait
 * @param[in] *handle pointer to a bme680 handle structure
 * @param[in] index input index
 * @param[in] reg input register
 * @return    status code
 *            - 0 success
 *            - 1 set gas wait failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 index > 9
 * @note      none
 */
uint8_t bme680_set_gas_wait(bme680_handle_t *handle, uint8_t index, uint8_t reg)
{
    uint8_t prev;
    
    if (handle == NULL)                                                                      /* check handle */
    {
        return 2;                                                                            /* return error */
    }
    if (handle->inited != 1)                                                                 /* check handle initialization */
    {
        return 3;                                                                            /* return error */
    }
    if (index > 9)                                                                           /* check index */
    {
        handle->debug_print("bme680: index > 9.\n");                                         /* index > 9 */

        return 4;                                                                            /* return error */
    }

    prev = reg;
    if (a_bme680_iic_spi_write(handle, BME680_REG_GAS_WAIT_X + index, &prev, 1) != 0)        /* write gas wait */
    {
        handle->debug_print("bme680: write gas wait failed.\n");                             /* write gas wait failed */

        return 1;                                                                            /* return error */
    }

    return 0;                                                                                /* success return 0 */
}

/**
 * @brief      get gas wait
 * @param[in]  *handle pointer to a bme680 handle structure
 * @param[in]  index input index
 * @param[out] *reg pointer to an input register buffer
 * @return     status code
 *             - 0 success
 *             - 1 get gas wait failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 index > 9
 * @note       none
 */
uint8_t bme680_get_gas_wait(bme680_handle_t *handle, uint8_t index, uint8_t *reg)
{
    if (handle == NULL)                                                                   /* check handle */
    {
        return 2;                                                                         /* return error */
    }
    if (handle->inited != 1)                                                              /* check handle initialization */
    {
        return 3;                                                                         /* return error */
    }
    if (index > 9)                                                                        /* check index */
    {
        handle->debug_print("bme680: index > 9.\n");                                      /* index > 9 */

        return 4;                                                                         /* return error */
    }

    if (a_bme680_iic_spi_read(handle, BME680_REG_GAS_WAIT_X + index, reg, 1) != 0)        /* read gas wait */
    {
        handle->debug_print("bme680: read gas wait failed.\n");                           /* read gas wait failed */

        return 1;                                                                         /* return error */
    }

    return 0;                                                                             /* success return 0 */
}

/**
 * @brief     soft reset
 * @param[in] *handle pointer to a bme680 handle structure
 * @return    status code
 *            - 0 success
 *            - 1 soft reset failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t bme680_soft_reset(bme680_handle_t *handle)
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
    if (a_bme680_iic_spi_write(handle, BME680_REG_RESET, &reg, 1) != 0)        /* reset the chip */
    {
        handle->debug_print("bme680: reset failed.\n");                        /* reset failed */

        return 1;                                                              /* return error */
    }
    handle->delay_ms(5);                                                       /* delay 5ms */

    return 0;                                                                  /* success return 0 */
}

/**
 * @brief      get new data status
 * @param[in]  *handle pointer to a bme680 handle structure
 * @param[out] *enable pointer to a bool buffer
 * @return     status code
 *             - 0 success
 *             - 1 get new data status failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t bme680_get_new_data_status(bme680_handle_t *handle, bme680_bool_t *enable)
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

    if (a_bme680_iic_spi_read(handle, BME680_REG_EAS_STATUS_0, &prev, 1) != 0)      /* read status */
    {
        handle->debug_print("bme680: read status failed.\n");                       /* read status failed */

        return 1;                                                                   /* return error */
    }
    *enable = (bme680_bool_t)((prev >> 7) & 0x01);                                  /* set bool */

    return 0;                                                                       /* success return 0 */
}

/**
 * @brief      get gas measuring status
 * @param[in]  *handle pointer to a bme680 handle structure
 * @param[out] *enable pointer to a bool buffer
 * @return     status code
 *             - 0 success
 *             - 1 get gas measuring status failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t bme680_get_gas_measuring_status(bme680_handle_t *handle, bme680_bool_t *enable)
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

    if (a_bme680_iic_spi_read(handle, BME680_REG_EAS_STATUS_0, &prev, 1) != 0)      /* read status */
    {
        handle->debug_print("bme680: read status failed.\n");                       /* read status failed */

        return 1;                                                                   /* return error */
    }
    *enable = (bme680_bool_t)((prev >> 6) & 0x01);                                  /* set bool */

    return 0;                                                                       /* success return 0 */
}

/**
 * @brief      get measuring status
 * @param[in]  *handle pointer to a bme680 handle structure
 * @param[out] *enable pointer to a bool buffer
 * @return     status code
 *             - 0 success
 *             - 1 get measuring status failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t bme680_get_measuring_status(bme680_handle_t *handle, bme680_bool_t *enable)
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

    if (a_bme680_iic_spi_read(handle, BME680_REG_EAS_STATUS_0, &prev, 1) != 0)      /* read status */
    {
        handle->debug_print("bme680: read status failed.\n");                       /* read status failed */

        return 1;                                                                   /* return error */
    }
    *enable = (bme680_bool_t)((prev >> 5) & 0x01);                                  /* set bool */

    return 0;                                                                       /* success return 0 */
}

/**
 * @brief      get gas measuring index
 * @param[in]  *handle pointer to a bme680 handle structure
 * @param[out] *index pointer to an index buffer
 * @return     status code
 *             - 0 success
 *             - 1 get gas measuring index failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t bme680_get_gas_measuring_index(bme680_handle_t *handle, uint8_t *index)
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

    if (a_bme680_iic_spi_read(handle, BME680_REG_EAS_STATUS_0, &prev, 1) != 0)      /* read status */
    {
        handle->debug_print("bme680: read status failed.\n");                       /* read status failed */

        return 1;                                                                   /* return error */
    }
    *index = (prev & 0xF);                                                          /* set index */

    return 0;                                                                       /* success return 0 */
}

/**
 * @brief      get gas valid status
 * @param[in]  *handle pointer to a bme680 handle structure
 * @param[out] *enable pointer to a bool buffer
 * @return     status code
 *             - 0 success
 *             - 1 get gas valid status failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t bme680_get_gas_valid_status(bme680_handle_t *handle, bme680_bool_t *enable)
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

    if (a_bme680_iic_spi_read(handle, BME680_REG_GAS_R_LSB, &prev, 1) != 0)      /* read gas lsb */
    {
        handle->debug_print("bme680: read gas lsb failed.\n");                   /* read gas lsb failed */

        return 1;                                                                /* return error */
    }
    *enable = (bme680_bool_t)((prev >> 5) & 0x01);                               /* set bool */

    return 0;                                                                    /* success return 0 */
}

/**
 * @brief      get heater stability status
 * @param[in]  *handle pointer to a bme680 handle structure
 * @param[out] *enable pointer to a bool buffer
 * @return     status code
 *             - 0 success
 *             - 1 get heater stability status failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t bme680_get_heater_stability_status(bme680_handle_t *handle, bme680_bool_t *enable)
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

    if (a_bme680_iic_spi_read(handle, BME680_REG_GAS_R_LSB, &prev, 1) != 0)      /* read gas lsb */
    {
        handle->debug_print("bme680: read gas lsb failed.\n");                   /* read gas lsb failed */

        return 1;                                                                /* return error */
    }
    *enable = (bme680_bool_t)((prev >> 4) & 0x01);                               /* set bool */

    return 0;                                                                    /* success return 0 */
}

/**
 * @brief     enable or disable spi wire3 data interrupt
 * @param[in] *handle pointer to a bme680 handle structure
 * @param[in] enable bool value
 * @return    status code
 *            - 0 success
 *            - 1 set spi wire 3 data interrupt failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t bme680_set_spi_wire_3_data_interrupt(bme680_handle_t *handle, bme680_bool_t enable)
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

    if (a_bme680_iic_spi_read(handle, BME680_REG_CTRL_HUM, &prev, 1) != 0)     /* read ctrl hum */
    {
        handle->debug_print("bme680: read ctrl hum failed.\n");                /* read ctrl hum failed */

        return 1;                                                              /* return error */
    }
    prev &= ~(1 << 6);                                                         /* clear settings */
    prev |= enable << 6;                                                       /* set bool */
    if (a_bme680_iic_spi_write(handle, BME680_REG_CTRL_HUM, &prev, 1) != 0)    /* write ctrl hum */
    {
        handle->debug_print("bme680: write ctrl hum failed.\n");               /* write ctrl hum failed */

        return 1;                                                              /* return error */
    }

    return 0;                                                                  /* success return 0 */
}

/**
 * @brief      get spi wire3 data interrupt status
 * @param[in]  *handle pointer to a bme680 handle structure
 * @param[out] *enable pointer to a bool value buffer
 * @return     status code
 *             - 0 success
 *             - 1 get spi wire 3 data interrupt failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t bme680_get_spi_wire_3_data_interrupt(bme680_handle_t *handle, bme680_bool_t *enable)
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

    if (a_bme680_iic_spi_read(handle, BME680_REG_CTRL_HUM, &prev, 1) != 0)     /* read ctrl hum */
    {
        handle->debug_print("bme680: read ctrl hum failed.\n");                /* read ctrl hum failed */

        return 1;                                                              /* return error */
    }
    *enable = (bme680_bool_t)((prev >> 6) & 0x01);                             /* get bool */

    return 0;                                                                  /* success return 0 */
}

/**
 * @brief     set humidity oversampling
 * @param[in] *handle pointer to a bme680 handle structure
 * @param[in] oversampling humidity oversampling
 * @return    status code
 *            - 0 success
 *            - 1 set humidity oversampling failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t bme680_set_humidity_oversampling(bme680_handle_t *handle, bme680_oversampling_t oversampling)
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

    if (a_bme680_iic_spi_read(handle, BME680_REG_CTRL_HUM, &prev, 1) != 0)     /* read ctrl hum */
    {
        handle->debug_print("bme680: read ctrl hum failed.\n");                /* read ctrl hum failed */

        return 1;                                                              /* return error */
    }
    prev &= ~(7 << 0);                                                         /* clear settings */
    prev |= oversampling << 0;                                                 /* set oversampling */
    if (a_bme680_iic_spi_write(handle, BME680_REG_CTRL_HUM, &prev, 1) != 0)    /* write ctrl hum */
    {
        handle->debug_print("bme680: write ctrl hum failed.\n");               /* write ctrl hum failed */

        return 1;                                                              /* return error */
    }

    return 0;                                                                  /* success return 0 */
}

/**
 * @brief      get humidity oversampling
 * @param[in]  *handle pointer to a bme680 handle structure
 * @param[out] *oversampling pointer to a humidity oversampling buffer
 * @return     status code
 *             - 0 success
 *             - 1 get humidity oversampling failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t bme680_get_humidity_oversampling(bme680_handle_t *handle, bme680_oversampling_t *oversampling)
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

    if (a_bme680_iic_spi_read(handle, BME680_REG_CTRL_HUM, &prev, 1) != 0)     /* read ctrl hum */
    {
        handle->debug_print("bme680: read ctrl hum failed.\n");                /* read ctrl hum failed */

        return 1;                                                              /* return error */
    }
    *oversampling = (bme680_oversampling_t)(prev & 0x07);                      /* get oversampling */

    return 0;                                                                  /* success return 0 */
}

/**
 * @brief     set temperature oversampling
 * @param[in] *handle pointer to a bme680 handle structure
 * @param[in] oversampling temperature oversampling
 * @return    status code
 *            - 0 success
 *            - 1 set temperature oversampling failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t bme680_set_temperature_oversampling(bme680_handle_t *handle, bme680_oversampling_t oversampling)
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

    if (a_bme680_iic_spi_read(handle, BME680_REG_CTRL_MEAS, &prev, 1) != 0)    /* read ctrl meas */
    {
        handle->debug_print("bme680: read ctrl meas failed.\n");               /* read ctrl meas failed */

        return 1;                                                              /* return error */
    }
    prev &= ~(7 << 5);                                                         /* clear settings */
    prev |= oversampling << 5;                                                 /* set oversampling */
    if (a_bme680_iic_spi_write(handle, BME680_REG_CTRL_MEAS, &prev, 1) != 0)   /* write ctrl meas */
    {
        handle->debug_print("bme680: write ctrl meas failed.\n");              /* write ctrl meas failed */

        return 1;                                                              /* return error */
    }

    return 0;                                                                  /* success return 0 */
}

/**
 * @brief      get temperature oversampling
 * @param[in]  *handle pointer to a bme680 handle structure
 * @param[out] *oversampling pointer to a oversampling buffer
 * @return     status code
 *             - 0 success
 *             - 1 get temperature oversampling failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t bme680_get_temperature_oversampling(bme680_handle_t *handle, bme680_oversampling_t *oversampling)
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

    if (a_bme680_iic_spi_read(handle, BME680_REG_CTRL_MEAS, &prev, 1) != 0)    /* read ctrl meas */
    {
        handle->debug_print("bme680: read ctrl meas failed.\n");               /* read ctrl meas failed */

        return 1;                                                              /* return error */
    }
    *oversampling = (bme680_oversampling_t)((prev >> 5) & 0x7);                /* set oversampling */

    return 0;                                                                  /* success return 0 */
}

/**
 * @brief     set pressure oversampling
 * @param[in] *handle pointer to a bme680 handle structure
 * @param[in] oversampling pressure oversampling
 * @return    status code
 *            - 0 success
 *            - 1 set pressure oversampling failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t bme680_set_pressure_oversampling(bme680_handle_t *handle, bme680_oversampling_t oversampling)
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

    if (a_bme680_iic_spi_read(handle, BME680_REG_CTRL_MEAS, &prev, 1) != 0)    /* read ctrl meas */
    {
        handle->debug_print("bme680: read ctrl meas failed.\n");               /* read ctrl meas failed */

        return 1;                                                              /* return error */
    }
    prev &= ~(7 << 2);                                                         /* clear settings */
    prev |= oversampling << 2;                                                 /* set oversampling */
    if (a_bme680_iic_spi_write(handle, BME680_REG_CTRL_MEAS, &prev, 1) != 0)   /* write ctrl meas */
    {
        handle->debug_print("bme680: write ctrl meas failed.\n");              /* write ctrl meas failed */

        return 1;                                                              /* return error */
    }

    return 0;                                                                  /* success return 0 */
}

/**
 * @brief      get pressure oversampling
 * @param[in]  *handle pointer to a bme680 handle structure
 * @param[out] *oversampling pointer to a oversampling buffer
 * @return     status code
 *             - 0 success
 *             - 1 get pressure oversampling failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t bme680_get_pressure_oversampling(bme680_handle_t *handle, bme680_oversampling_t *oversampling)
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

    if (a_bme680_iic_spi_read(handle, BME680_REG_CTRL_MEAS, &prev, 1) != 0)    /* read ctrl meas */
    {
        handle->debug_print("bme680: read ctrl meas failed.\n");               /* read ctrl meas failed */

        return 1;                                                              /* return error */
    }
    *oversampling = (bme680_oversampling_t)((prev >> 2) & 0x7);                /* set oversampling */

    return 0;                                                                  /* success return 0 */
}

/**
 * @brief     set mode
 * @param[in] *handle pointer to a bme680 handle structure
 * @param[in] mode chip mode
 * @return    status code
 *            - 0 success
 *            - 1 set mode failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t bme680_set_mode(bme680_handle_t *handle, bme680_mode_t mode)
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

    if (a_bme680_iic_spi_read(handle, BME680_REG_CTRL_MEAS, &prev, 1) != 0)         /* read ctrl meas */
    {
        handle->debug_print("bme680: read ctrl meas failed.\n");                    /* read ctrl meas failed */

        return 1;                                                                   /* return error */
    }
    prev &= ~(3 << 0);                                                              /* clear settings */
    prev |= mode << 0;                                                              /* set mode */
    if (a_bme680_iic_spi_write(handle, BME680_REG_CTRL_MEAS, &prev, 1) != 0)        /* write ctrl meas */
    {
        handle->debug_print("bme680: write ctrl meas failed.\n");                   /* write ctrl meas failed */

        return 1;                                                                   /* return error */
    }

    return 0;                                                                       /* success return 0 */
}

/**
 * @brief      get mode
 * @param[in]  *handle pointer to a bme680 handle structure
 * @param[out] *mode pointer to a mode buffer
 * @return     status code
 *             - 0 success
 *             - 1 get mode failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t bme680_get_mode(bme680_handle_t *handle, bme680_mode_t *mode)
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

    if (a_bme680_iic_spi_read(handle, BME680_REG_CTRL_MEAS, &prev, 1) != 0)    /* read ctrl meas */
    {
        handle->debug_print("bme680: read ctrl meas failed.\n");               /* read ctrl meas failed */

        return 1;                                                              /* return error */
    }
    *mode = (bme680_mode_t)((prev >> 0) & 0x3);                                /* set mode */

    return 0;                                                                  /* success return 0 */
}

/**
 * @brief     set filter
 * @param[in] *handle pointer to a bme680 handle structure
 * @param[in] filter input filter
 * @return    status code
 *            - 0 success
 *            - 1 set filter failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t bme680_set_filter(bme680_handle_t *handle, bme680_filter_t filter)
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

    if (a_bme680_iic_spi_read(handle, BME680_REG_CONFIG, &prev, 1) != 0)         /* read config */
    {
        handle->debug_print("bme680: read config failed.\n");                    /* read config failed */

        return 1;                                                                /* return error */
    }
    prev &= ~(7 << 2);                                                           /* clear settings */
    prev |= (filter & 0x07) << 2;                                                /* set filter */
    if (a_bme680_iic_spi_write(handle, BME680_REG_CONFIG, &prev, 1) != 0)        /* write config */
    {
        handle->debug_print("bme680: write config failed.\n");                   /* write config failed */

        return 1;                                                                /* return error */
    }

    return 0;                                                                    /* success return 0 */
}

/**
 * @brief      get filter
 * @param[in]  *handle pointer to a bme680 handle structure
 * @param[out] *filter pointer to a filter buffer
 * @return     status code
 *             - 0 success
 *             - 1 get filter failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t bme680_get_filter(bme680_handle_t *handle, bme680_filter_t *filter)
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

    if (a_bme680_iic_spi_read(handle, BME680_REG_CONFIG, &prev, 1) != 0)         /* read config */
    {
        handle->debug_print("bme680: read config failed.\n");                    /* read config failed */

        return 1;                                                                /* return error */
    }
    *filter = (bme680_filter_t)((prev >> 2) & 0x07);                             /* set filter */

    return 0;                                                                    /* success return 0 */
}

/**
 * @brief     set spi wire
 * @param[in] *handle pointer to a bme680 handle structure
 * @param[in] spi spi wire
 * @return    status code
 *            - 0 success
 *            - 1 set spi wire failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t bme680_set_spi_wire(bme680_handle_t *handle, bme680_spi_wire_t spi)
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

    if (a_bme680_iic_spi_read(handle, BME680_REG_CONFIG, &prev, 1) != 0)         /* read config */
    {
        handle->debug_print("bme680: read config failed.\n");                    /* read config failed */

        return 1;                                                                /* return error */
    }
    prev &= ~(1 << 0);                                                           /* clear settings */
    prev |= spi << 0;                                                            /* set spi wire */
    if (a_bme680_iic_spi_write(handle, BME680_REG_CONFIG, &prev, 1) != 0)        /* write config */
    {
        handle->debug_print("bme680: write config failed.\n");                   /* write config failed */

        return 1;                                                                /* return error */
    }

    return 0;                                                                    /* success return 0 */
}

/**
 * @brief      get spi wire
 * @param[in]  *handle pointer to a bme680 handle structure
 * @param[out] *spi pointer to a spi wire buffer
 * @return     status code
 *             - 0 success
 *             - 1 get spi wire failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t bme680_get_spi_wire(bme680_handle_t *handle, bme680_spi_wire_t *spi)
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

    if (a_bme680_iic_spi_read(handle, BME680_REG_CONFIG, &prev, 1) != 0)         /* read config */
    {
        handle->debug_print("bme680: read config failed.\n");                    /* read config failed */

        return 1;                                                                /* return error */
    }
    *spi = (bme680_spi_wire_t)((prev >> 0) & 0x01);                              /* get spi */

    return 0;                                                                    /* success return 0 */
}

/**
 * @brief     enable or disable heat off
 * @param[in] *handle pointer to a bme680 handle structure
 * @param[in] enable bool value
 * @return    status code
 *            - 0 success
 *            - 1 set heat off failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t bme680_set_heat_off(bme680_handle_t *handle, bme680_bool_t enable)
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

    if (a_bme680_iic_spi_read(handle, BME680_REG_CTRL_GAS_0, &prev, 1) != 0)     /* read ctrl gas0 */
    {
        handle->debug_print("bme680: read ctrl gas0 failed.\n");                 /* read ctrl gas0 failed */

        return 1;                                                                /* return error */
    }
    prev &= ~(1 << 3);                                                           /* clear settings */
    prev |= enable << 3;                                                         /* set bool */
    if (a_bme680_iic_spi_write(handle, BME680_REG_CTRL_GAS_0, &prev, 1) != 0)    /* write ctrl gas0 */
    {
        handle->debug_print("bme680: write ctrl gas0 failed.\n");                /* write ctrl gas0 failed */

        return 1;                                                                /* return error */
    }

    return 0;                                                                    /* success return 0 */
}

/**
 * @brief      get heat off status
 * @param[in]  *handle pointer to a bme680 handle structure
 * @param[out] *enable pointer to a bool value buffer
 * @return     status code
 *             - 0 success
 *             - 1 get heat off failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t bme680_get_heat_off(bme680_handle_t *handle, bme680_bool_t *enable)
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

    if (a_bme680_iic_spi_read(handle, BME680_REG_CTRL_GAS_0, &prev, 1) != 0)     /* read ctrl gas0 */
    {
        handle->debug_print("bme680: read ctrl gas0 failed.\n");                 /* read ctrl gas0 failed */

        return 1;                                                                /* return error */
    }
    *enable = (bme680_bool_t)((prev >> 3) & 0x01);                               /* get bool */

    return 0;                                                                    /* success return 0 */
}

/**
 * @brief     enable or disable run gas
 * @param[in] *handle pointer to a bme680 handle structure
 * @param[in] enable bool value
 * @return    status code
 *            - 0 success
 *            - 1 set run gas failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t bme680_set_run_gas(bme680_handle_t *handle, bme680_bool_t enable)
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

    if (a_bme680_iic_spi_read(handle, BME680_REG_CTRL_GAS_1, &prev, 1) != 0)     /* read ctrl gas1 */
    {
        handle->debug_print("bme680: read ctrl gas1 failed.\n");                 /* read ctrl gas1 failed */

        return 1;                                                                /* return error */
    }
    prev &= ~(1 << 4);                                                           /* clear settings */
    prev |= enable << 4;                                                         /* set bool */
    if (a_bme680_iic_spi_write(handle, BME680_REG_CTRL_GAS_1, &prev, 1) != 0)    /* write ctrl gas1 */
    {
        handle->debug_print("bme680: write ctrl gas1 failed.\n");                /* write ctrl gas1 failed */

        return 1;                                                                /* return error */
    }

    return 0;                                                                    /* success return 0 */
}

/**
 * @brief      get run gas status
 * @param[in]  *handle pointer to a bme680 handle structure
 * @param[out] *enable pointer to a bool value buffer
 * @return     status code
 *             - 0 success
 *             - 1 get run gas failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t bme680_get_run_gas(bme680_handle_t *handle, bme680_bool_t *enable)
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

    if (a_bme680_iic_spi_read(handle, BME680_REG_CTRL_GAS_1, &prev, 1) != 0)     /* read ctrl gas1 */
    {
        handle->debug_print("bme680: read ctrl gas1 failed.\n");                 /* read ctrl gas1 failed */

        return 1;                                                                /* return error */
    }
    *enable = (bme680_bool_t)((prev >> 4) & 0x01);                               /* get bool */
    
    return 0;                                                                    /* success return 0 */
}

/**
 * @brief     set convert index
 * @param[in] *handle pointer to a bme680 handle structure
 * @param[in] index convert index
 * @return    status code
 *            - 0 success
 *            - 1 set convert index failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 index > 9
 * @note      none
 */
uint8_t bme680_set_convert_index(bme680_handle_t *handle, uint8_t index)
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
    if (index > 9)                                                               /* check index */
    {
        handle->debug_print("bme680: index > 9.\n");                             /* index > 9 */

        return 4;                                                                /* return error */
    }
    
    if (a_bme680_iic_spi_read(handle, BME680_REG_CTRL_GAS_1, &prev, 1) != 0)     /* read ctrl gas1 */
    {
        handle->debug_print("bme680: read ctrl gas1 failed.\n");                 /* read ctrl gas1 failed */

        return 1;                                                                /* return error */
    }
    prev &= ~(0x0F << 0);                                                        /* clear settings */
    prev |= (index & 0x0F) << 0;                                                 /* set index */
    if (a_bme680_iic_spi_write(handle, BME680_REG_CTRL_GAS_1, &prev, 1) != 0)    /* write ctrl gas1 */
    {
        handle->debug_print("bme680: write ctrl gas1 failed.\n");                /* write ctrl gas1 failed */

        return 1;                                                                /* return error */
    }

    return 0;                                                                    /* success return 0 */
}

/**
 * @brief      get convert index
 * @param[in]  *handle pointer to a bme680 handle structure
 * @param[out] *index pointer to a convert index buffer
 * @return     status code
 *             - 0 success
 *             - 1 get convert index failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t bme680_get_convert_index(bme680_handle_t *handle, uint8_t *index)
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
    
    if (a_bme680_iic_spi_read(handle, BME680_REG_CTRL_GAS_1, &prev, 1) != 0)     /* read ctrl gas1 */
    {
        handle->debug_print("bme680: read ctrl gas1 failed.\n");                 /* read ctrl gas1 failed */

        return 1;                                                                /* return error */
    }
    *index = prev & 0x0F;                                                        /* set index */

    return 0;                                                                    /* success return 0 */
}

/**
 * @brief      convert the resistance heat to the register raw data
 * @param[in]  *handle pointer to a bme680 handle structure
 * @param[in]  degree_celsius input degree celsius
 * @param[out] *reg pointer to a register raw buffer
 * @return     status code
 *             - 0 success
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t bme680_resistance_heat_convert_to_register(bme680_handle_t *handle, float degree_celsius, uint8_t *reg)
{
    if (handle == NULL)                                                 /* check handle */
    {
        return 2;                                                       /* return error */
    }
    if (handle->inited != 1)                                            /* check handle initialization */
    {
        return 3;                                                       /* return error */
    }
    
    (void)a_bme680_compensate_heat(handle, degree_celsius, reg);        /* convert real data to register data */
    
    return 0;                                                           /* success return 0 */
}

/**
 * @brief      convert the idac heat to the register raw data
 * @param[in]  *handle pointer to a bme680 handle structure
 * @param[in]  ma input ma
 * @param[out] *reg pointer to a register raw buffer
 * @return     status code
 *             - 0 success
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t bme680_idac_heat_convert_to_register(bme680_handle_t *handle, float ma, uint8_t *reg)
{
    if (handle == NULL)                         /* check handle */
    {
        return 2;                               /* return error */
    }
    if (handle->inited != 1)                    /* check handle initialization */
    {
        return 3;                               /* return error */
    }
    
    *reg = ((uint8_t)(ma * 8) - 1) << 1;        /* convert real data to register data  */
    
    return 0;                                   /* success return 0 */
}

/**
 * @brief      convert the register raw data to idac heat
 * @param[in]  *handle pointer to a bme680 handle structure
 * @param[in]  reg register raw data
 * @param[out] *ma pointer to a ma buffer
 * @return     status code
 *             - 0 success
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t bme680_idac_heat_convert_to_data(bme680_handle_t *handle, uint8_t reg, float *ma)
{
    if (handle == NULL)                         /* check handle */
    {
        return 2;                               /* return error */
    }
    if (handle->inited != 1)                    /* check handle initialization */
    {
        return 3;                               /* return error */
    }
    
    *ma = (float)(((reg >> 1) + 1) / 8);        /* convert raw data to real data */
    
    return 0;                                   /* success return 0 */
}

/**
 * @brief      convert the gas wait to the register raw data
 * @param[in]  *handle pointer to a bme680 handle structure
 * @param[in]  ms input ms
 * @param[out] *reg pointer to a register raw buffer
 * @return     status code
 *             - 0 success
 *             - 1 ms >= 0x0FC0
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t bme680_gas_wait_convert_to_register(bme680_handle_t *handle, uint16_t ms, uint8_t *reg)
{
    uint8_t factor = 0;
    
    if (handle == NULL)                                        /* check handle */
    {
        return 2;                                              /* return error */
    }
    if (handle->inited != 1)                                   /* check handle initialization */
    {
        return 3;                                              /* return error */
    }
    
    if (ms >= 0xFC0)                                           /* check ms */
    {
        handle->debug_print("bme680: ms >= 0x0FC0.\n");        /* ms >= 0x0FC0 */

        return 1;                                              /* return error */
    }
    else
    {
        while (ms > 0x3F)                                      /* check ms */
        {
            ms = ms / 4;                                       /* div 4 */
            factor += 1;                                       /* factor */
        }
        
        *reg = (uint8_t)(ms + (factor * 64));                  /* set reg */
    }
    
    return 0;                                                  /* success return 0 */
}

/**
 * @brief      read the gas resistance
 * @param[in]  *handle pointer to a bme680 handle structure
 * @param[out] *adc_raw pointer to an adc raw buffer
 * @param[out] *adc_range pointer to an adc range buffer
 * @param[out] *ohms pointer to an ohms buffer
 * @param[out] *index pointer to a heater index buffer
 * @return     status code
 *             - 0 success
 *             - 1 gas resistance read failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 compensate gas resistance failed
 *             - 5 read timeout
 *             - 6 gas is invalid
 *             - 7 heater is not stable
 * @note       none
 */
uint8_t bme680_read_gas_resistance(bme680_handle_t *handle, uint16_t *adc_raw, uint8_t *adc_range, float *ohms, uint8_t *index)
{
    uint8_t res;
    uint8_t prev;
    uint32_t timeout;
    uint8_t buf[2];

    if (handle == NULL)                                                                    /* check handle */
    {
        return 2;                                                                          /* return error */
    }
    if (handle->inited != 1)                                                               /* check handle initialization */
    {
        return 3;                                                                          /* return error */
    }
    
    if (a_bme680_iic_spi_read(handle, BME680_REG_CTRL_MEAS, &prev, 1) != 0)                /* read ctrl meas */
    {
        handle->debug_print("bme680: read ctrl meas failed.\n");                           /* read ctrl meas failed */

        return 1;                                                                          /* return error */
    }
    prev &= ~(3 << 0);                                                                     /* clear settings */
    prev |= 0x01 << 0;                                                                     /* set forced mode */
    if (a_bme680_iic_spi_write(handle, BME680_REG_CTRL_MEAS, &prev, 1) != 0)               /* write ctrl meas */
    {
        handle->debug_print("bme680: write ctrl meas failed.\n");                          /* write ctrl meas failed */

        return 1;                                                                          /* return error */
    }
    timeout = 10 * 1000;                                                                   /* set timeout */
    while (timeout != 0)                                                                   /* check timeout */
    {
        if (a_bme680_iic_spi_read(handle, BME680_REG_CTRL_MEAS, &prev, 1) != 0)            /* read ctrl meas */
        {
            handle->debug_print("bme680: read ctrl meas failed.\n");                       /* read ctrl meas failed */

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
        handle->debug_print("bme680: read timeout.\n");                                    /* read timeout */

        return 5;                                                                          /* return error */
    }
    res = a_bme680_iic_spi_read(handle, BME680_REG_GAS_R_MSB, buf, 2);                     /* read gas */
    if (res != 0)
    {
        handle->debug_print("bme680: read failed.\n");                                     /* read failed */

        return 1;                                                                          /* return error */
    }
    if ((buf[1] & (1 << 5)) == 0)                                                          /* check gas valid */
    {
        handle->debug_print("bme680: gas is invalid.\n");                                  /* gas is invalid */

        return 6;                                                                          /* return error */
    }
    if ((buf[1] & (1 << 4)) == 0)                                                          /* check heat */
    {
        handle->debug_print("bme680: heater is not stable.\n");                            /* heater is not stable */

        return 7;                                                                          /* return error */
    }
    *adc_raw = (((uint16_t)buf[0]) << 2) | ((buf[1] >> 6) & 0x03);                         /* set adc raw */
    *adc_range = buf[1] & 0x0F;                                                            /* set adc range */
    res = a_bme680_compensate_gas_resistance(handle, *adc_raw , *adc_range, ohms);         /* compensate gas resistance */
    if (res != 0)                                                                          /* check result */
    {
        handle->debug_print("bme680: compensate gas resistance failed.\n");                /* compensate gas resistance failed */

        return 4;                                                                          /* return error */
    }
    if (a_bme680_iic_spi_read(handle, BME680_REG_EAS_STATUS_0, &prev, 1) != 0)             /* read status */
    {
        handle->debug_print("bme680: read status failed.\n");                              /* read status failed */

        return 1;                                                                          /* return error */
    }
    *index = (prev & 0xF);                                                                 /* set index */
    
    return 0;                                                                              /* success return 0 */
}

/**
 * @brief      read the pressure data
 * @param[in]  *handle pointer to a bme680 handle structure
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
uint8_t bme680_read_pressure(bme680_handle_t *handle, uint32_t *pressure_raw, float *pressure_pa)
{
    uint8_t res;
    uint8_t prev;
    uint32_t timeout;
    uint32_t temperature_raw;
    float temperature_c;
    uint8_t buf[6];

    if (handle == NULL)                                                                    /* check handle */
    {
        return 2;                                                                          /* return error */
    }
    if (handle->inited != 1)                                                               /* check handle initialization */
    {
        return 3;                                                                          /* return error */
    }
    
    if (a_bme680_iic_spi_read(handle, BME680_REG_CTRL_MEAS, &prev, 1) != 0)                /* read ctrl meas */
    {
        handle->debug_print("bme680: read ctrl meas failed.\n");                           /* read ctrl meas failed */

        return 1;                                                                          /* return error */
    }
    prev &= ~(3 << 0);                                                                     /* clear settings */
    prev |= 0x01 << 0;                                                                     /* set forced mode */
    if (a_bme680_iic_spi_write(handle, BME680_REG_CTRL_MEAS, &prev, 1) != 0)               /* write ctrl meas */
    {
        handle->debug_print("bme680: write ctrl meas failed.\n");                          /* write ctrl meas failed */

        return 1;                                                                          /* return error */
    }
    timeout = 10 * 1000;                                                                   /* set timeout */
    while (timeout != 0)                                                                   /* check timeout */
    {
        if (a_bme680_iic_spi_read(handle, BME680_REG_CTRL_MEAS, &prev, 1) != 0)            /* read ctrl meas */
        {
            handle->debug_print("bme680: read ctrl meas failed.\n");                       /* read ctrl meas failed */

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
        handle->debug_print("bme680: read timeout.\n");                                    /* read timeout */

        return 5;                                                                          /* return error */
    }
    res = a_bme680_iic_spi_read(handle, BME680_REG_PRESS_MSB, buf, 6);                     /* read temperature and pressure */
    if (res != 0)
    {
        handle->debug_print("bme680: read failed.\n");                                     /* read failed */

        return 1;                                                                          /* return error */
    }
    temperature_raw = ((((uint32_t)(buf[3])) << 12) |
                      (((uint32_t)(buf[4])) << 4) |
                      ((uint32_t)buf[5] >> 4));                                            /* set temperature raw */
    res = a_bme680_compensate_temperature(handle, temperature_raw, &temperature_c);        /* compensate temperature */
    if (res != 0)
    {
        handle->debug_print("bme680: compensate temperature failed.\n");                   /* compensate temperature failed */

        return 4;                                                                          /* return error */
    }
    *pressure_raw = ((((int32_t)(buf[0])) << 12) |
                    (((int32_t)(buf[1])) << 4) |
                    (((int32_t)(buf[2])) >> 4));                                           /* set pressure raw */
    res = a_bme680_compensate_pressure(handle, *pressure_raw, pressure_pa);                /* compensate pressure */
    if (res != 0)
    {
        handle->debug_print("bme680: compensate pressure failed.\n");                      /* compensate pressure failed */

        return 4;                                                                          /* return error */
    }

    return 0;                                                                              /* success return 0 */
}

/**
 * @brief      read the temperature data
 * @param[in]  *handle pointer to a bme680 handle structure
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
uint8_t bme680_read_temperature(bme680_handle_t *handle, uint32_t *temperature_raw, float *temperature_c)
{
    uint8_t res;
    uint8_t prev;
    uint32_t timeout;
    uint8_t buf[6];

    if (handle == NULL)                                                                    /* check handle */
    {
        return 2;                                                                          /* return error */
    }
    if (handle->inited != 1)                                                               /* check handle initialization */
    {
        return 3;                                                                          /* return error */
    }
    
    if (a_bme680_iic_spi_read(handle, BME680_REG_CTRL_MEAS, &prev, 1) != 0)                /* read ctrl meas */
    {
        handle->debug_print("bme680: read ctrl meas failed.\n");                           /* read ctrl meas failed */

        return 1;                                                                          /* return error */
    }
    prev &= ~(3 << 0);                                                                     /* clear settings */
    prev |= 0x01 << 0;                                                                     /* set forced mode */
    if (a_bme680_iic_spi_write(handle, BME680_REG_CTRL_MEAS, &prev, 1) != 0)               /* write ctrl meas */
    {
        handle->debug_print("bme680: write ctrl meas failed.\n");                          /* write ctrl meas failed */

        return 1;                                                                          /* return error */
    }
    timeout = 10 * 1000;                                                                   /* set timeout */
    while (timeout != 0)                                                                   /* check timeout */
    {
        if (a_bme680_iic_spi_read(handle, BME680_REG_CTRL_MEAS, &prev, 1) != 0)            /* read ctrl meas */
        {
            handle->debug_print("bme680: read ctrl meas failed.\n");                       /* read ctrl meas failed */

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
        handle->debug_print("bme680: read timeout.\n");                                    /* read timeout */

        return 5;                                                                          /* return error */
    }
    res = a_bme680_iic_spi_read(handle, BME680_REG_PRESS_MSB, buf, 6);                     /* read temperature and pressure */
    if (res != 0)
    {
        handle->debug_print("bme680: read failed.\n");                                     /* read failed */

        return 1;                                                                          /* return error */
    }
    *temperature_raw = ((((uint32_t)(buf[3])) << 12) |
                       (((uint32_t)(buf[4])) << 4) |
                       ((uint32_t)buf[5] >> 4));                                           /* set temperature raw */
    res = a_bme680_compensate_temperature(handle, *temperature_raw, temperature_c);        /* compensate temperature */
    if (res != 0)
    {
        handle->debug_print("bme680: compensate temperature failed.\n");                   /* compensate temperature failed */

        return 4;                                                                          /* return error */
    }

    return 0;                                                                              /* success return 0 */
}

/**
 * @brief      read the humidity data
 * @param[in]  *handle pointer to a bme680 handle structure
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
uint8_t bme680_read_humidity(bme680_handle_t *handle, uint32_t *humidity_raw, float *humidity_percentage)
{
    uint8_t res;
    uint8_t prev;
    uint32_t timeout;
    uint32_t temperature_raw;
    float temperature_c;
    uint8_t buf[8];

    if (handle == NULL)                                                                    /* check handle */
    {
        return 2;                                                                          /* return error */
    }
    if (handle->inited != 1)                                                               /* check handle initialization */
    {
        return 3;                                                                          /* return error */
    }
    
    if (a_bme680_iic_spi_read(handle, BME680_REG_CTRL_MEAS, &prev, 1) != 0)                /* read ctrl meas */
    {
        handle->debug_print("bme680: read ctrl meas failed.\n");                           /* read ctrl meas failed */

        return 1;                                                                          /* return error */
    }
    prev &= ~(3 << 0);                                                                     /* clear settings */
    prev |= 0x01 << 0;                                                                     /* set forced mode */
    if (a_bme680_iic_spi_write(handle, BME680_REG_CTRL_MEAS, &prev, 1) != 0)               /* write ctrl meas */
    {
        handle->debug_print("bme680: write ctrl meas failed.\n");                          /* write ctrl meas failed */

        return 1;                                                                          /* return error */
    }
    timeout = 10 * 1000;                                                                   /* set timeout */
    while (timeout != 0)                                                                   /* check timeout */
    {
        if (a_bme680_iic_spi_read(handle, BME680_REG_CTRL_MEAS, &prev, 1) != 0)            /* read ctrl meas */
        {
            handle->debug_print("bme680: read ctrl meas failed.\n");                       /* read ctrl meas failed */

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
        handle->debug_print("bme680: read timeout.\n");                                    /* read timeout */

        return 5;                                                                          /* return error */
    }
    res = a_bme680_iic_spi_read(handle, BME680_REG_PRESS_MSB, buf, 8);                     /* read temperature pressure and humidity */
    if (res != 0)
    {
        handle->debug_print("bme680: read failed.\n");                                     /* read failed */

        return 1;                                                                          /* return error */
    }
    temperature_raw = ((((uint32_t)(buf[3])) << 12) |
                      (((uint32_t)(buf[4])) << 4) |
                      ((uint32_t)buf[5] >> 4));                                            /* set temperature raw */
    res = a_bme680_compensate_temperature(handle, temperature_raw, &temperature_c);        /* compensate temperature */
    if (res != 0)
    {
        handle->debug_print("bme680: compensate temperature failed.\n");                   /* compensate temperature failed */

        return 4;                                                                          /* return error */
    }
    *humidity_raw = (uint32_t)buf[6] << 8 | buf[7];                                        /* set humidity raw */
    res = a_bme680_compensate_humidity(handle, *humidity_raw, humidity_percentage);        /* compensate humidity */
    if (res != 0)
    {
        handle->debug_print("bme680: compensate humidity failed.\n");                      /* compensate humidity failed */

        return 4;                                                                          /* return error */
    }

    return 0;                                                                              /* success return 0 */
}

/**
 * @brief      read the temperature pressure and humidity data
 * @param[in]  *handle pointer to a bme680 handle structure
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
 *             - 4 compensate failed
 *             - 5 read timeout
 * @note       none
 */
uint8_t bme680_read_temperature_pressure_humidity(bme680_handle_t *handle, uint32_t *temperature_raw, float *temperature_c,
                                                  uint32_t *pressure_raw, float *pressure_pa,
                                                  uint32_t *humidity_raw, float *humidity_percentage)
{
    uint8_t res;
    uint8_t prev;
    uint32_t timeout;
    uint8_t buf[8];

    if (handle == NULL)                                                                    /* check handle */
    {
        return 2;                                                                          /* return error */
    }
    if (handle->inited != 1)                                                               /* check handle initialization */
    {
        return 3;                                                                          /* return error */
    }
    
    if (a_bme680_iic_spi_read(handle, BME680_REG_CTRL_MEAS, &prev, 1) != 0)                /* read ctrl meas */
    {
        handle->debug_print("bme680: read ctrl meas failed.\n");                           /* read ctrl meas failed */

        return 1;                                                                          /* return error */
    }
    prev &= ~(3 << 0);                                                                     /* clear settings */
    prev |= 0x01 << 0;                                                                     /* set forced mode */
    if (a_bme680_iic_spi_write(handle, BME680_REG_CTRL_MEAS, &prev, 1) != 0)               /* write ctrl meas */
    {
        handle->debug_print("bme680: write ctrl meas failed.\n");                          /* write ctrl meas failed */

        return 1;                                                                          /* return error */
    }
    timeout = 10 * 1000;                                                                   /* set timeout */
    while (timeout != 0)                                                                   /* check timeout */
    {
        if (a_bme680_iic_spi_read(handle, BME680_REG_CTRL_MEAS, &prev, 1) != 0)            /* read ctrl meas */
        {
            handle->debug_print("bme680: read ctrl meas failed.\n");                       /* read ctrl meas failed */

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
        handle->debug_print("bme680: read timeout.\n");                                    /* read timeout */

        return 5;                                                                          /* return error */
    }
    res = a_bme680_iic_spi_read(handle, BME680_REG_PRESS_MSB, buf, 8);                     /* read temperature pressure and humidity */
    if (res != 0)
    {
        handle->debug_print("bme680: read failed.\n");                                     /* read failed */

        return 1;                                                                          /* return error */
    }
    *temperature_raw = ((((uint32_t)(buf[3])) << 12) |
                       (((uint32_t)(buf[4])) << 4) |
                       ((uint32_t)buf[5] >> 4));                                           /* set temperature raw */
    res = a_bme680_compensate_temperature(handle, *temperature_raw, temperature_c);        /* compensate temperature */
    if (res != 0)
    {
        handle->debug_print("bme680: compensate temperature failed.\n");                   /* compensate temperature failed */

        return 4;                                                                          /* return error */
    }
    *pressure_raw = ((((int32_t)(buf[0])) << 12) |
                    (((int32_t)(buf[1])) << 4) |
                    (((int32_t)(buf[2])) >> 4));                                           /* set pressure raw */
    res = a_bme680_compensate_pressure(handle, *pressure_raw, pressure_pa);                /* compensate pressure */
    if (res != 0)
    {
        handle->debug_print("bme680: compensate pressure failed.\n");                      /* compensate pressure failed */

        return 4;                                                                          /* return error */
    }
    *humidity_raw = (uint32_t)buf[6] << 8 | buf[7];                                        /* set humidity raw */
    res = a_bme680_compensate_humidity(handle, *humidity_raw, humidity_percentage);        /* compensate humidity */
    if (res != 0)
    {
        handle->debug_print("bme680: compensate humidity failed.\n");                      /* compensate humidity failed */

        return 4;                                                                          /* return error */
    }

    return 0;                                                                              /* success return 0 */
}

/**
 * @brief      read the temperature pressure humidity and gas resistance
 * @param[in]  *handle pointer to a bme680 handle structure
 * @param[out] *temperature_raw pointer to a raw temperature buffer
 * @param[out] *temperature_c pointer to a converted temperature buffer
 * @param[out] *pressure_raw pointer to a raw pressure buffer
 * @param[out] *pressure_pa pointer to a converted pressure buffer
 * @param[out] *humidity_raw pointer to a raw humidity buffer
 * @param[out] *humidity_percentage pointer to a converted humidity percentage buffer
 * @param[out] *adc_raw pointer to an adc raw buffer
 * @param[out] *adc_range pointer to an adc range buffer
 * @param[out] *ohms pointer to an ohms buffer
 * @param[out] *index pointer to a heater index buffer
 * @return     status code
 *             - 0 success
 *             - 1 read failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 compensate failed
 *             - 5 read timeout
 *             - 6 gas is invalid
 *             - 7 heater is not stable
 * @note       none
 */
uint8_t bme680_read(bme680_handle_t *handle, uint32_t *temperature_raw, float *temperature_c,
                    uint32_t *pressure_raw, float *pressure_pa,
                    uint32_t *humidity_raw, float *humidity_percentage,
                    uint16_t *adc_raw, uint8_t *adc_range, float *ohms, uint8_t *index)
{
    uint8_t res;
    uint8_t prev;
    uint32_t timeout;
    uint8_t buf[8];

    if (handle == NULL)                                                                    /* check handle */
    {
        return 2;                                                                          /* return error */
    }
    if (handle->inited != 1)                                                               /* check handle initialization */
    {
        return 3;                                                                          /* return error */
    }
    
    if (a_bme680_iic_spi_read(handle, BME680_REG_CTRL_MEAS, &prev, 1) != 0)                /* read ctrl meas */
    {
        handle->debug_print("bme680: read ctrl meas failed.\n");                           /* read ctrl meas failed */

        return 1;                                                                          /* return error */
    }
    prev &= ~(3 << 0);                                                                     /* clear settings */
    prev |= 0x01 << 0;                                                                     /* set forced mode */
    if (a_bme680_iic_spi_write(handle, BME680_REG_CTRL_MEAS, &prev, 1) != 0)               /* write ctrl meas */
    {
        handle->debug_print("bme680: write ctrl meas failed.\n");                          /* write ctrl meas failed */

        return 1;                                                                          /* return error */
    }
    timeout = 10 * 1000;                                                                   /* set timeout */
    while (timeout != 0)                                                                   /* check timeout */
    {
        if (a_bme680_iic_spi_read(handle, BME680_REG_CTRL_MEAS, &prev, 1) != 0)            /* read ctrl meas */
        {
            handle->debug_print("bme680: read ctrl meas failed.\n");                       /* read ctrl meas failed */

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
        handle->debug_print("bme680: read timeout.\n");                                    /* read timeout */

        return 5;                                                                          /* return error */
    }
    res = a_bme680_iic_spi_read(handle, BME680_REG_PRESS_MSB, buf, 8);                     /* read temperature pressure and humidity */
    if (res != 0)
    {
        handle->debug_print("bme680: read failed.\n");                                     /* read failed */

        return 1;                                                                          /* return error */
    }
    *temperature_raw = ((((uint32_t)(buf[3])) << 12) |
                       (((uint32_t)(buf[4])) << 4) |
                       ((uint32_t)buf[5] >> 4));                                           /* set temperature raw */
    res = a_bme680_compensate_temperature(handle, *temperature_raw, temperature_c);        /* compensate temperature */
    if (res != 0)
    {
        handle->debug_print("bme680: compensate temperature failed.\n");                   /* compensate temperature failed */

        return 4;                                                                          /* return error */
    }
    *pressure_raw = ((((int32_t)(buf[0])) << 12) |
                    (((int32_t)(buf[1])) << 4) |
                    (((int32_t)(buf[2])) >> 4));                                           /* set pressure raw */
    res = a_bme680_compensate_pressure(handle, *pressure_raw, pressure_pa);                /* compensate pressure */
    if (res != 0)
    {
        handle->debug_print("bme680: compensate pressure failed.\n");                      /* compensate pressure failed */

        return 4;                                                                          /* return error */
    }
    *humidity_raw = (uint32_t)buf[6] << 8 | buf[7];                                        /* set humidity raw */
    res = a_bme680_compensate_humidity(handle, *humidity_raw, humidity_percentage);        /* compensate humidity */
    if (res != 0)
    {
        handle->debug_print("bme680: compensate humidity failed.\n");                      /* compensate humidity failed */

        return 4;                                                                          /* return error */
    }
    
    res = a_bme680_iic_spi_read(handle, BME680_REG_GAS_R_MSB, buf, 2);                     /* read gas */
    if (res != 0)
    {
        handle->debug_print("bme680: read failed.\n");                                     /* read failed */

        return 1;                                                                          /* return error */
    }
    if ((buf[1] & (1 << 5)) == 0)                                                          /* check gas valid */
    {
        handle->debug_print("bme680: gas is invalid.\n");                                  /* gas is invalid */

        return 6;                                                                          /* return error */
    }
    if ((buf[1] & (1 << 4)) == 0)                                                          /* check heat */
    {
        handle->debug_print("bme680: heater is not stable.\n");                            /* heater is not stable */

        return 7;                                                                          /* return error */
    }
    *adc_raw = (((uint16_t)buf[0]) << 2) | ((buf[1] >> 6) & 0x03);                         /* set adc raw */
    *adc_range = buf[1] & 0x0F;                                                            /* set adc range */
    res = a_bme680_compensate_gas_resistance(handle, *adc_raw , *adc_range, ohms);         /* compensate gas resistance */
    if (res != 0)                                                                          /* check result */
    {
        handle->debug_print("bme680: compensate gas resistance failed.\n");                /* compensate gas resistance failed */

        return 4;                                                                          /* return error */
    }
    if (a_bme680_iic_spi_read(handle, BME680_REG_EAS_STATUS_0, &prev, 1) != 0)             /* read status */
    {
        handle->debug_print("bme680: read status failed.\n");                              /* read status failed */

        return 1;                                                                          /* return error */
    }
    *index = (prev & 0xF);                                                                 /* set index */
    
    return 0;                                                                              /* success return 0 */
}

/**
 * @brief     set the chip register
 * @param[in] *handle pointer to a bme680 handle structure
 * @param[in] reg register address
 * @param[in] *buf pointer to a data buffer
 * @param[in] len data length
 * @return    status code
 *            - 0 success
 *            - 1 write failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t bme680_set_reg(bme680_handle_t *handle, uint8_t reg, uint8_t *buf, uint16_t len)
{
    if (handle == NULL)                                         /* check handle */
    {
        return 2;                                               /* return error */
    }
    if (handle->inited != 1)                                    /* check handle initialization */
    {
        return 3;                                               /* return error */
    }

    return a_bme680_iic_spi_write(handle, reg, buf, len);       /* write register */
}

/**
 * @brief      get the chip register
 * @param[in]  *handle pointer to a bme680 handle structure
 * @param[in]  reg register address
 * @param[out] *buf pointer to a data buffer
 * @param[in]  len data length
 * @return     status code
 *             - 0 success
 *             - 1 read failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t bme680_get_reg(bme680_handle_t *handle, uint8_t reg, uint8_t *buf, uint16_t len)
{
    if (handle == NULL)                                        /* check handle */
    {
        return 2;                                              /* return error */
    }
    if (handle->inited != 1)                                   /* check handle initialization */
    {
        return 3;                                              /* return error */
    }

    return a_bme680_iic_spi_read(handle, reg, buf, len);       /* read register */
}

/**
 * @brief      get chip's information
 * @param[out] *info pointer to a bme680 info structure
 * @return     status code
 *             - 0 success
 *             - 2 handle is NULL
 * @note       none
 */
uint8_t bme680_info(bme680_info_t *info)
{
    if (info == NULL)                                               /* check handle */
    {
        return 2;                                                   /* return error */
    }

    memset(info, 0, sizeof(bme680_info_t));                         /* initialize bme680 info structure */
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
