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
 * @file      driver_pmsa003i.c
 * @brief     driver pmsa003i source file
 * @version   1.0.0
 * @author    Edward Kisiel based on driver pmsx003 by Shifeng Li
 * @date      2026-07-04
 *
 * <h3>history</h3>
 * <table>
 * <tr><th>Date        <th>Version  <th>Author      <th>Description
 * <tr><td>2026/07/04  <td>1.0      <td>Edward Kisiel  <td>first upload
 * </table>
 */

#include "driver_pmsa003i.h"

/**
 * @brief chip information definition
 */
#define CHIP_NAME                 "PLANTOWER PMSA003I"        /**< chip name */
#define MANUFACTURER_NAME         "PLANTOWER"                /**< manufacturer name */
#define SUPPLY_VOLTAGE_MIN        4.5f                       /**< chip min supply voltage */
#define SUPPLY_VOLTAGE_MAX        5.5f                       /**< chip max supply voltage */
#define MAX_CURRENT               100.0f                     /**< chip max current */
#define TEMPERATURE_MIN           -10.0f                     /**< chip min operating temperature */
#define TEMPERATURE_MAX           60.0f                      /**< chip max operating temperature */
#define DRIVER_VERSION            1000                       /**< driver version */

/**
 * @brief chip command definition
 */
#define PMSA003I_COMMAND_READ                       0xE2     /**< read command */
#define PMSA003I_COMMAND_CHANGE_WORKING_MODE        0xE1     /**< change working mode */
#define PMSA003I_COMMAND_CHANGE_CHIP_MODE           0xE4     /**< chip mode */

/**
 * @brief chip address definition
 */
extern uint16_t pmsa003i_iic_addr;                           /**< iic device address */

/**
 * @brief      read bytes
 * @param[in]  *handle pointer to an pmsa003i handle structure
 * @param[out] *data pointer to a data buffer
 * @param[in]  len length of data
 * @return     status code
 *             - 0 success
 *             - 1 read failed
 * @note       none
 */
static uint8_t a_pmsa003i_iic_read(pmsa003i_handle_t *handle, uint8_t *data, uint16_t len)
{
    if (handle->iic_read_cmd(pmsa003i_iic_addr, data, len) != 0)        /* read the register */
    {
        return 1;                                                   /* return error */
    }
    else
    {
        return 0;                                                   /* success return 0 */
    }
}

/**
 * @brief     write bytes
 * @param[in] *handle pointer to an pmsa003i handle structure
 * @param[in] *data pointer to a data buffer
 * @param[in] len length of data
 * @return    status code
 *            - 0 success
 *            - 1 write failed
 * @note      none
 */
static uint8_t a_pmsa003i_iic_write(pmsa003i_handle_t *handle, uint8_t *data, uint16_t len)
{
    if (handle->iic_write_cmd(pmsa003i_iic_addr, data, len) != 0)        /* write the register */
    {
        return 1;                                                    /* return error */
    }
    else
    {
        return 0;                                                    /* success return 0 */
    }
}

/**
 * @brief     calculate the crc
 * @param[in] *data pointer to a data buffer
 * @param[in] len length of data
 * @return    crc
 * @note      none
 */
static uint8_t a_pmsa003i_calc_crc(uint8_t *data, uint8_t len)
{
    uint8_t i;
    uint8_t byte;
    uint8_t crc = 0xFF;

    for (byte = 0; byte < len; byte++)          /* len times */
    {
        crc ^= data[byte];                      /* xor byte */
        for (i = 8; i > 0; --i)                 /* one byte */
        {
            if ((crc & 0x80) != 0)              /* if high*/
            {
                crc = (crc << 1) ^ 0x31;        /* xor 0x31 */
            }
            else
            {
                crc = crc << 1;                 /* skip */
            }
        }
    }

    return crc;                                 /* return crc */
}

/**
 * @brief     initialize the chip
 * @param[in] *handle pointer to a pmsa003i handle structure
 * @return    status code
 *            - 0 success
 *            - 1 iic initialization failed
 *            - 2 handle is NULL
 *            - 3 linked functions is NULL
 * @note      none
 */
uint8_t pmsa003i_init(pmsa003i_handle_t *handle)
{
    uint8_t res;

    if (handle == NULL)                                                     /* check handle */
    {
        return 2;                                                           /* return error */
    }
    if (handle->debug_print == NULL)                                        /* check debug_print */
    {
        return 3;                                                           /* return error */
    }
    if (handle->iic_init == NULL)                                          /* check iic_init */
    {
        handle->debug_print("pmsa003i: iic_init is null.\n");               /* iic_init is null */

        return 3;                                                           /* return error */
    }
    if (handle->iic_deinit == NULL)                                        /* check iic_deinit */
    {
        handle->debug_print("pmsa003i: iic_deinit is null.\n");             /* iic_deinit is null */

        return 3;                                                           /* return error */
    }
    if (handle->iic_read_cmd == NULL)                                          /* check iic_read */
    {
        handle->debug_print("pmsa003i: iic_read is null.\n");               /* iic_read is null */

        return 3;                                                           /* return error */
    }
    if (handle->delay_ms == NULL)                                           /* check delay_ms */
    {
        handle->debug_print("pmsa003i: delay_ms is null.\n");                /* delay_ms is null */

        return 3;                                                           /* return error */
    }

    if (handle->iic_init() != 0)                                           /* iic init */
    {
        handle->debug_print("pmsa003i: iic init failed.\n");                /* iic init failed */

        return 1;                                                           /* return error */
    }

    handle->mode = (uint8_t)PMSA003I_MODE_ACTIVE;                            /* init 1 */
    handle->inited = 1;                                                     /* flag finish initialization */

    return 0;                                                               /* success return 0 */
}

/**
 * @brief     close the chip
 * @param[in] *handle pointer to an pmsa003i handle structure
 * @return    status code
 *            - 0 success
 *            - 1 deinit failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t pmsa003i_deinit(pmsa003i_handle_t *handle)
{
    if (handle == NULL)                                            /* check handle */
    {
        return 2;                                                  /* return error */
    }
    if (handle->inited != 1)                                       /* check handle initialization */
    {
        return 3;                                                  /* return error */
    }

    if (handle->iic_deinit() != 0)                                 /* iic deinit */
    {
        handle->debug_print("pmsa003i: iic deinit failed.\n");        /* iic deinit failed */

        return 1;                                                  /* return error */
    }
    handle->inited = 0;                                            /* set closed flag */

    return 0;                                                      /* success return 0 */
}

/**
 * @brief      make frame
 * @param[in]  command input command
 * @param[in]  data input data
 * @param[out] *output pointer to an output buffer
 * @return     status code
 *             - 0 success
 *             - 1 make frame failed
 * @note       none
 */
static uint8_t a_pmsa003i_make_frame(uint8_t command, uint8_t data, uint8_t output[7])
{
    uint8_t i;
    uint16_t lrc;
    
    output[0] = 0x42;                     /* set 0x42 */
    output[1] = 0x4D;                     /* set 0x4D */
    output[2] = command;                  /* set command */
    output[3] = 0x00;                     /* set 0x00 */
    output[4] = data;                     /* set data */
    
    lrc = 0;                              /* init 0 */
    for (i = 0; i < 5; i++)               /* add all */
    {
        lrc += output[i];                 /* sum */
    }
    output[5] = (lrc >> 8) & 0xFF;        /* set lrc msb */
    output[6] = (lrc >> 0) & 0xFF;        /* set lrc lsb */
    
    return 0;                             /* success return 0 */
}

/**
 * @brief      parse frame
 * @param[in]  *handle pointer to a pmsa003i handle structure
 * @param[in]  *input pointer to an input buffer '
 * @param[in]  command input command
 * @param[out] *data pointer to a data buffer
 * @return     status code
 *             - 0 success
 *             - 1 parse frame failed
 * @note       none
 */
static uint8_t a_pmsa003i_parse_frame(pmsa003i_handle_t *handle, uint8_t input[8], uint8_t command, uint8_t *data)
{
    uint8_t i;
    uint16_t lrc;
    uint16_t lrc_check;
    
    lrc = 0;                                                       /* init 0 */
    for (i = 0; i < 6; i++)                                        /* add all */
    {
        lrc += input[i];                                           /* sum */
    }
    lrc_check = ((uint16_t)input[6] << 8) | input[7];              /* get lrc */
    if (lrc != lrc_check)                                          /* check lrc */
    {
        handle->debug_print("pmsa003i: lrc check error.\n");        /* lrc check error */
        
        return 1;                                                  /* return error */
    }
    if ((input[0] == 0x42) && (input[1] == 0x4D))                  /* check header */
    {
        uint16_t len;
        
        len = ((uint16_t)input[2] << 8) | input[3];                /* get length */
        if (len != 4)                                              /* check length */
        {
            return 1;                                              /* return error */
        }
        if (input[4] != command)                                   /* check command */
        {
            return 1;                                              /* return error */
        }
        *data = input[5];                                          /* get data */
        
        return 0;                                                  /* success return 0 */
    }
    else
    {
        return 1;                                                  /* return error */
    }
}

/**
 * @brief      parse data
 * @param[in]  *handle pointer to a pmsa003i handle structure
 * @param[in]  *input pointer to an input buffer
 * @param[out] *data pointer to a data structure
 * @return     status code
 *             - 0 success
 *             - 1 parse frame failed
 * @note       none
 */
static uint8_t a_pmsa003i_parse_data(pmsa003i_handle_t *handle, uint8_t input[32], pmsa003i_data_t *data)
{
    uint8_t i;
    uint16_t lrc;
    uint16_t lrc_check;
    
    lrc = 0;                                                                      /* init 0 */
    for (i = 0; i < 30; i++)                                                      /* add all */
    {
        lrc += input[i];                                                          /* sum */
    }
    lrc_check = ((uint16_t)input[30] << 8) | input[31];                           /* get lrc */
    if (lrc != lrc_check)                                                         /* check lrc */
    {
        handle->debug_print("pmsa003i: lrc check error.\n");                       /* lrc check error */
        
        return 1;                                                                 /* return error */
    }
    if ((input[0] == 0x42) && (input[1] == 0x4D))                                 /* check header */
    {
        uint16_t len;
        
        len = ((uint16_t)input[2] << 8) | input[3];                               /* get length */
        if (len != 28)                                                            /* check length */
        {
            return 1;                                                             /* return error */
        }
        
        data->pm1p0_standard_ug_m3 = ((uint16_t)input[4] << 8) | input[5];        /* set pm1p0 standard ug/m3 */
        data->pm2p5_standard_ug_m3 = ((uint16_t)input[6] << 8) | input[7];        /* set pm2p5 standard ug/m3 */
        data->pm10_standard_ug_m3 = ((uint16_t)input[8] << 8) | input[9];         /* set pm10 standard ug/m3 */
        data->pm1p0_atmospheric_ug_m3 = ((uint16_t)input[10] << 8) | input[11];   /* set pm1p0 atmospheric ug/m3 */
        data->pm2p5_atmospheric_ug_m3 = ((uint16_t)input[12] << 8) | input[13];   /* set pm2p5 atmospheric ug/m3 */
        data->pm10_atmospheric_ug_m3 = ((uint16_t)input[14] << 8) | input[15];    /* set pm10 atmospheric ug/m3 */
        data->beyond_0p3um = ((uint16_t)input[16] << 8) | input[17];              /* set beyond 0p3um */
        data->beyond_0p5um = ((uint16_t)input[18] << 8) | input[19];              /* set beyond 0p5um */
        data->beyond_1p0um = ((uint16_t)input[20] << 8) | input[21];              /* set beyond 1p0um */
        data->beyond_2p5um = ((uint16_t)input[22] << 8) | input[23];              /* set beyond 2p5um */
        data->beyond_5p0um = ((uint16_t)input[24] << 8) | input[25];              /* set beyond 5p0um */
        data->beyond_10um = ((uint16_t)input[26] << 8) | input[27];               /* set beyond 10um */
        data->version = input[28];                                                /* set version */
        data->error_code = input[29];                                             /* set error code */
        
        return 0;                                                                 /* success return 0 */
    }
    else
    {
        return 1;                                                                 /* return error */
    }
}

/**
 * @brief      read the data
 * @param[in]  *handle pointer to a pmsa003i handle structure
 * @param[out] *data pointer to a data structure
 * @return     status code
 *             - 0 success
 *             - 1 read failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 frame error
 *             - 5 data error
 * @note       none
 */
uint8_t pmsa003i_read(pmsa003i_handle_t *handle, pmsa003i_data_t *data)
{
    uint8_t res;
    uint8_t output[7];
    uint8_t input[32];
    uint16_t len;
    
    if (handle == NULL)                                                                 /* check handle */
    {
        return 2;                                                                       /* return error */
    }
    if (handle->inited != 1)                                                            /* check handle initialization */
    {
        return 3;                                                                       /* return error */
    }
    
    if (handle->mode != 0)                                                              /* active mode */
    {
        len = handle->iic_read_cmd(pmsa003i_iic_addr, input, 32);                                             /* iic read */
        if (len != 32)                                                                  /* check length */
        {
            handle->debug_print("pmsa003i: iic read failed.\n");                        /* iic read failed */
            
            return 1;                                                                   /* return error */
        }
        res = a_pmsa003i_parse_data(handle, input, data);                               /* parse data */
        if (res != 0)                                                                   /* check result */
        {
            handle->debug_print("pmsa003i: frame error.\n");                             /* frame error */
            
            return 4;                                                                   /* return error */
        }
        if (data->error_code != 0)                                                      /* check error code */
        {
            handle->debug_print("pmsa003i: data error.\n");                              /* data error */
            
            return 5;                                                                   /* return error */
        }
    }
    else                                                                                /* passive mode */
    {
        (void)a_pmsa003i_make_frame(PMSA003I_COMMAND_READ, (uint8_t)0x00, output);      /* make frame */
        res = handle->iic_write_cmd(pmsa003i_iic_addr, output, 7);                                             /* iic write */
        if (res != 0)                                                                   /* check result */
        {
            handle->debug_print("pmsa003i: iic write failed.\n");                       /* iic write failed */
            
            return 1;                                                                   /* return error */
        }
        handle->delay_ms(100);                                                          /* delay 100ms */
        len = handle->iic_read_cmd(pmsa003i_iic_addr, input, 32);                                             /* iic read */
        if (len != 32)                                                                  /* check length */
        {
            handle->debug_print("pmsa003i: iic read failed.\n");                       /* iic read failed */
            
            return 1;                                                                   /* return error */
        }
        res = a_pmsa003i_parse_data(handle, input, data);                               /* parse data */
        if (res != 0)                                                                   /* check result */
        {
            handle->debug_print("pmsa003i: frame error.\n");                             /* frame error */
            
            return 4;                                                                   /* return error */
        }
        if (data->error_code != 0)                                                      /* check error code */
        {
            handle->debug_print("pmsa003i: data error.\n");                              /* data error */
            
            return 5;                                                                   /* return error */
        }
    }
    
    return 0;                                                                           /* success return 0 */ 
}

/**
 * @brief      get chip's information
 * @param[out] *info pointer to a pmsa003i info structure
 * @return     status code
 *             - 0 success
 *             - 2 handle is NULL
 * @note       none
 */
uint8_t pmsa003i_info(pmsa003i_info_t *info)
{
    if (info == NULL)                                               /* check handle */
    {
        return 2;                                                   /* return error */
    }
    
    memset(info, 0, sizeof(pmsa003i_info_t));                        /* initialize pmsa003i info structure */
    strncpy(info->chip_name, CHIP_NAME, 32);                        /* copy chip name */
    strncpy(info->manufacturer_name, MANUFACTURER_NAME, 32);        /* copy manufacturer name */
    strncpy(info->interface, "IIC", 8);                            /* copy interface name */
    info->supply_voltage_min_v = SUPPLY_VOLTAGE_MIN;                 /* set minimal supply voltage */
    info->supply_voltage_max_v = SUPPLY_VOLTAGE_MAX;                /* set maximum supply voltage */
    info->max_current_ma = MAX_CURRENT;                             /* set maximum current */
    info->temperature_max = TEMPERATURE_MAX;                        /* set minimal temperature */
    info->temperature_min = TEMPERATURE_MIN;                        /* set maximum temperature */
    info->driver_version = DRIVER_VERSION;                          /* set driver version */
    
    return 0;                                                       /* success return 0 */
}
