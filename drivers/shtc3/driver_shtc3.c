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
 * @file      driver_shtc3.c
 * @brief     driver shtc3 source file
 * @version   1.0.0
 * @author    Shifeng Li
 * @date      2025-11-03
 *
 * <h3>history</h3>
 * <table>
 * <tr><th>Date        <th>Version  <th>Author      <th>Description
 * <tr><td>2025/11/03  <td>1.0      <td>Shifeng Li  <td>first upload
 * </table>
 */

#include "driver_shtc3.h"

/**
 * @brief chip information definition
 */
#define CHIP_NAME                 "Sensirion SHTC3"        /**< chip name */
#define MANUFACTURER_NAME         "Sensirion"              /**< manufacturer name */
#define SUPPLY_VOLTAGE_MIN        1.62f                    /**< chip min supply voltage */
#define SUPPLY_VOLTAGE_MAX        3.6f                     /**< chip max supply voltage */
#define MAX_CURRENT               0.9f                     /**< chip max current */
#define TEMPERATURE_MIN           -40.0f                   /**< chip min operating temperature */
#define TEMPERATURE_MAX           125.0f                   /**< chip max operating temperature */
#define DRIVER_VERSION            1000                     /**< driver version */

/**
 * @brief chip address definition
 */
#define SHTC3_ADDRESS             (0x70 << 1)        /**< iic device address */

/**
 * @brief chip command definition
 */
#define SHTC3_COMMAND_READ_ID                    0xEFC8U        /**< read id register command */
#define SHTC3_COMMAND_SOFT_RESET                 0x805DU        /**< soft reset command */
#define SHTC3_COMMAND_SLEEP                      0xB098U        /**< sleep command */
#define SHTC3_COMMAND_WAKEUP                     0x3517U        /**< wakeup command */
#define SHTC3_COMMAND_MEAS_T_POLLING             0x7866U        /**< read t first, clock stretching disabled command */
#define SHTC3_COMMAND_MEAS_T_CLOCKSTR            0x7CA2U        /**< read t first, clock stretching enabled command */
#define SHTC3_COMMAND_MEAS_RH_POLLING            0x58E0U        /**< read rh first, clock stretching disabled command */
#define SHTC3_COMMAND_MEAS_RH_CLOCKSTR           0x5C24U        /**< read rh first, clock stretching enabled command */
#define SHTC3_COMMAND_LP_MEAS_T_POLLING          0x609CU        /**< read t first, clock stretching disabled command */
#define SHTC3_COMMAND_LP_MEAS_T_CLOCKSTR         0x6458U        /**< read t first, clock stretching enabled command */
#define SHTC3_COMMAND_LP_MEAS_RH_POLLING         0x401AU        /**< read rh first, clock stretching disabled command */
#define SHTC3_COMMAND_LP_MEAS_RH_CLOCKSTR        0x44DEU        /**< read rh first, clock stretching enabled command */

/**
 * @brief     write the command
 * @param[in] *handle pointer to a shtc3 handle structure
 * @param[in] cmd written command
 * @return    status code
 *            - 0 success
 *            - 1 write failed
 * @note      none
 */
static uint8_t a_shtc3_write(shtc3_handle_t *handle, uint16_t cmd)
{
    if (handle->iic_write_address16(SHTC3_ADDRESS, cmd, NULL, 0) != 0)        /* iic write */
    {
        return 1;                                                             /* return error */
    }
    
    return 0;                                                                 /* success return 0 */
}

/**
 * @brief      read bytes
 * @param[in]  *handle pointer to a shtc3 handle structure
 * @param[in]  reg iic register address
 * @param[out] *data pointer to a data buffer
 * @param[in]  len data length
 * @return     status code
 *             - 0 success
 *             - 1 read failed
 * @note       none
 */
static uint8_t a_shtc3_read(shtc3_handle_t *handle, uint16_t reg, uint8_t *data, uint16_t len)
{
    if (handle->iic_read_address16(SHTC3_ADDRESS, reg, data, len) != 0)        /* iic read */
    {
        return 1;                                                              /* return error */
    }
    
    return 0;                                                                  /* success return 0 */
}

/**
 * @brief     calculate the crc
 * @param[in] *data pointer to a data buffer
 * @param[in] len data length
 * @return    crc
 * @note      none
 */
static uint8_t a_shtc3_crc(uint8_t *data, uint16_t len)
{
    const uint8_t POLYNOMIAL = 0x31;
    uint8_t crc = 0xFF;
    uint16_t i, j;
  
    for (j = len; j != 0; --j)                                              /* length-- */
    {
        crc ^= *data++;                                                     /* xor */
        for (i = 8; i != 0; --i)                                            /* 8 times */
        {
            crc = (crc & 0x80) ? (crc << 1) ^ POLYNOMIAL : (crc<<1);        /* calculate crc */
        }
    }
  
    return crc;                                                             /* return crc */
}

/**
 * @brief     initialize the chip
 * @param[in] *handle pointer to a shtc3 handle structure
 * @return    status code
 *            - 0 success
 *            - 1 iic initialization failed
 *            - 2 handle is NULL
 *            - 3 linked functions is NULL
 *            - 4 read id failed
 *            - 5 id is invalid
 * @note      none
 */
uint8_t shtc3_init(shtc3_handle_t *handle)
{
    uint8_t res;
    uint8_t data[3];
    uint16_t command;
    
    if (handle == NULL)                                                      /* check handle */
    {
        return 2;                                                            /* return error */
    }
    if (handle->debug_print == NULL)                                         /* check debug_print */
    {
        return 3;                                                            /* return error */
    }
    if (handle->iic_init == NULL)                                            /* check iic_init */
    {
        handle->debug_print("shtc3: iic_init is null.\n");                   /* iic_init is null */
       
        return 3;                                                            /* return error */
    }
    if (handle->iic_deinit == NULL)                                          /* check iic_deinit */
    {
        handle->debug_print("shtc3: iic_deinit is null.\n");                 /* iic_deinit is null */
       
        return 3;                                                            /* return error */
    }
    if (handle->iic_read_address16 == NULL)                                  /* check iic_read_address16 */
    {
        handle->debug_print("shtc3: iic_read_address16 is null.\n");         /* iic_read_address16 is null */
       
        return 3;                                                            /* return error */
    }
    if (handle->iic_write_address16 == NULL)                                 /* check iic_write_address16 */
    {
        handle->debug_print("shtc3: iic_write_address16 is null.\n");        /* iic_write_address16 is null */
       
        return 3;                                                            /* return error */
    }
    if (handle->delay_ms == NULL)                                            /* check delay_ms */
    {
        handle->debug_print("shtc3: delay_ms is null.\n");                   /* delay_ms is null */
       
        return 3;                                                            /* return error */
    }
    
    if (handle->iic_init() != 0)                                             /* iic init */
    {
        handle->debug_print("shtc3: iic init failed.\n");                    /* iic init failed */
       
        return 1;                                                            /* return error */
    }
    
    command = SHTC3_COMMAND_SOFT_RESET;                                      /* set command */
    res = a_shtc3_write(handle, command);                                    /* write command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("shtc3: write command failed.\n");               /* write command failed */
        (void)handle->iic_deinit();                                          /* iic deinit */
        
        return 1;                                                            /* return error */
    }
    handle->delay_ms(1);                                                     /* delay 1ms */
    
    command = SHTC3_COMMAND_READ_ID;                                         /* read id command */
    res = a_shtc3_read(handle, command, (uint8_t *)data, 3);                 /* read data */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("shtc3: read data failed.\n");                   /* read data failed */
        (void)handle->iic_deinit();                                          /* iic deinit */
        
        return 4;                                                            /* return error */
    }
    if (a_shtc3_crc((uint8_t *)data, 2) != data[2])                          /* check crc */
    {
        handle->debug_print("shtc3: crc check failed.\n");                   /* crc check failed */
        (void)handle->iic_deinit();                                          /* iic deinit */
        
        return 4;                                                            /* return error */
    }
    if (((data[0] & 0x08) != 0x08) || ((data[1] & 0x3F) != 0x7))             /* check id */
    {
        handle->debug_print("shtc3: id is invalid.\n");                      /* id is invalid */
        (void)handle->iic_deinit();                                          /* iic deinit */
        
        return 5;                                                            /* return error */
    }
    handle->inited = 1;                                                      /* flag finish initialization */
    
    return 0;                                                                /* success return 0 */
}

/**
 * @brief     close the chip
 * @param[in] *handle pointer to a shtc3 handle structure
 * @return    status code
 *            - 0 success
 *            - 1 iic deinit failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 reset failed
 * @note      none
 */
uint8_t shtc3_deinit(shtc3_handle_t *handle)
{
    uint8_t res;
    uint16_t command;
    
    if (handle == NULL)                                               /* check handle */
    {
        return 2;                                                     /* return error */
    }
    if (handle->inited != 1)                                          /* check handle initialization */
    {
        return 3;                                                     /* return error */
    }
    
    command = SHTC3_COMMAND_SOFT_RESET;                               /* set command */
    res = a_shtc3_write(handle, command);                             /* write command */
    if (res != 0)                                                     /* check result */
    {
        handle->debug_print("shtc3: write command failed.\n");        /* write command failed */
        
        return 4;                                                     /* return error */
    }
    handle->delay_ms(1);                                              /* delay 1ms */
    
    if (handle->iic_deinit() != 0)                                    /* iic deinit */
    {
        handle->debug_print("shtc3: iic deinit failed.\n");           /* iic deinit failed */
        
        return 1;                                                     /* return error */
    }
    handle->inited = 0;                                               /* flag close */
    
    return 0;                                                         /* success return 0 */
}

/**
 * @brief      normal read
 * @param[in]  *handle pointer to a shtc3 handle structure
 * @param[in]  clock_stretching_enable clock stretching bool value
 * @param[out] *temperature_raw pointer to a raw temperature buffer
 * @param[out] *temperature_s pointer to a converted temperature buffer
 * @param[out] *humidity_raw pointer to a raw humidity buffer
 * @param[out] *humidity_s pointer to a converted humidity buffer
 * @return     status code
 *             - 0 success
 *             - 1 single read failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t shtc3_read(shtc3_handle_t *handle, shtc3_bool_t clock_stretching_enable, 
                   uint16_t *temperature_raw, float *temperature_s,
                   uint16_t *humidity_raw, float *humidity_s)
{
    uint8_t res;
    uint16_t command;
    uint8_t data[3];
    
    if (handle == NULL)                                                            /* check handle */
    {
        return 2;                                                                  /* return error */
    }
    if (handle->inited != 1)                                                       /* check handle initialization */
    {
        return 3;                                                                  /* return error */
    }
    
    if (clock_stretching_enable == SHTC3_BOOL_FALSE)                               /* if disable clock stretching */
    {
        command = SHTC3_COMMAND_MEAS_T_POLLING;                                    /* set disable low */
    }
    else                                                                           /* if enable clock stretching */
    {
        command = SHTC3_COMMAND_MEAS_T_CLOCKSTR;                                   /* set enable low */
    }
    res = a_shtc3_read(handle, command, (uint8_t *)data, 3);                       /* read data */
    if (res != 0)                                                                  /* check result */
    {
        handle->debug_print("shtc3: read data failed.\n");                         /* read data failed */
       
        return 1;                                                                  /* return error */
    }
    if (a_shtc3_crc((uint8_t *)data, 2) != data[2])                                /* check crc */
    {
        handle->debug_print("shtc3: crc check failed.\n");                         /* crc check failed */
       
        return 1;                                                                  /* return error */
    }
    *temperature_raw = (uint16_t)((((uint16_t)data[0]) << 8) | data[1]);           /* get raw temperature */
    *temperature_s = (float)(*temperature_raw) / 65536.0f * 175.0f - 45.0f;        /* convert raw temperature */
    
    if (clock_stretching_enable == SHTC3_BOOL_FALSE)                               /* if disable clock stretching */
    {
        command = SHTC3_COMMAND_MEAS_RH_POLLING;                                   /* set disable low */
    }
    else                                                                           /* if enable clock stretching */
    {
        command = SHTC3_COMMAND_MEAS_RH_CLOCKSTR;                                  /* set enable low */
    }
    res = a_shtc3_read(handle, command, (uint8_t *)data, 3);                       /* read data */
    if (res != 0)                                                                  /* check result */
    {
        handle->debug_print("shtc3: read data failed.\n");                         /* read data failed */
       
        return 1;                                                                  /* return error */
    }
    if (a_shtc3_crc((uint8_t *)data, 2) != data[2])                                /* check crc */
    {
        handle->debug_print("shtc3: crc check failed.\n");                         /* crc check failed */
       
        return 1;                                                                  /* return error */
    }
    *humidity_raw = (uint16_t)((((uint16_t)data[0]) << 8) | data[1]);              /* get raw humidity */
    *humidity_s = (float)(*humidity_raw) / 65536.0f * 100.0f;                      /* convert raw humidity */
    
    return 0;                                                                      /* success return 0 */
}

/**
 * @brief      read in low power mode
 * @param[in]  *handle pointer to a shtc3 handle structure
 * @param[in]  clock_stretching_enable clock stretching bool value
 * @param[out] *temperature_raw pointer to a raw temperature buffer
 * @param[out] *temperature_s pointer to a converted temperature buffer
 * @param[out] *humidity_raw pointer to a raw humidity buffer
 * @param[out] *humidity_s pointer to a converted humidity buffer
 * @return     status code
 *             - 0 success
 *             - 1 single read failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t shtc3_read_low_power(shtc3_handle_t *handle, shtc3_bool_t clock_stretching_enable, 
                             uint16_t *temperature_raw, float *temperature_s,
                             uint16_t *humidity_raw, float *humidity_s)
{
    uint8_t res;
    uint16_t command;
    uint8_t data[3];
    
    if (handle == NULL)                                                            /* check handle */
    {
        return 2;                                                                  /* return error */
    }
    if (handle->inited != 1)                                                       /* check handle initialization */
    {
        return 3;                                                                  /* return error */
    }
    
    if (clock_stretching_enable == SHTC3_BOOL_FALSE)                               /* if disable clock stretching */
    {
        command = SHTC3_COMMAND_LP_MEAS_T_POLLING;                                 /* set disable low */
    }
    else                                                                           /* if enable clock stretching */
    {
        command = SHTC3_COMMAND_LP_MEAS_T_CLOCKSTR;                                /* set enable low */
    }
    res = a_shtc3_read(handle, command, (uint8_t *)data, 3);                       /* read data */
    if (res != 0)                                                                  /* check result */
    {
        handle->debug_print("shtc3: read data failed.\n");                         /* read data failed */
       
        return 1;                                                                  /* return error */
    }
    if (a_shtc3_crc((uint8_t *)data, 2) != data[2])                                /* check crc */
    {
        handle->debug_print("shtc3: crc check failed.\n");                         /* crc check failed */
       
        return 1;                                                                  /* return error */
    }
    *temperature_raw = (uint16_t)((((uint16_t)data[0]) << 8) | data[1]);           /* get raw temperature */
    *temperature_s = (float)(*temperature_raw) / 65536.0f * 175.0f - 45.0f;        /* convert raw temperature */
    
    if (clock_stretching_enable == SHTC3_BOOL_FALSE)                               /* if disable clock stretching */
    {
        command = SHTC3_COMMAND_LP_MEAS_RH_POLLING;                                /* set disable low */
    }
    else                                                                           /* if enable clock stretching */
    {
        command = SHTC3_COMMAND_LP_MEAS_RH_CLOCKSTR;                               /* set enable low */
    }
    res = a_shtc3_read(handle, command, (uint8_t *)data, 3);                       /* read data */
    if (res != 0)                                                                  /* check result */
    {
        handle->debug_print("shtc3: read data failed.\n");                         /* read data failed */
       
        return 1;                                                                  /* return error */
    }
    if (a_shtc3_crc((uint8_t *)data, 2) != data[2])                                /* check crc */
    {
        handle->debug_print("shtc3: crc check failed.\n");                         /* crc check failed */
       
        return 1;                                                                  /* return error */
    }
    *humidity_raw = (uint16_t)((((uint16_t)data[0]) << 8) | data[1]);              /* get raw humidity */
    *humidity_s = (float)(*humidity_raw) / 65536.0f * 100.0f;                      /* convert raw humidity */
    
    return 0;                                                                      /* success return 0 */
}

/**
 * @brief     soft reset the chip
 * @param[in] *handle pointer to a shtc3 handle structure
 * @return    status code
 *            - 0 success
 *            - 1 soft reset failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t shtc3_soft_reset(shtc3_handle_t *handle)
{
    uint8_t res;
    uint16_t command;
    
    if (handle == NULL)                                               /* check handle */
    {
        return 2;                                                     /* return error */
    }
    if (handle->inited != 1)                                          /* check handle initialization */
    {
        return 3;                                                     /* return error */
    }
    
    command = SHTC3_COMMAND_SOFT_RESET;                               /* set command */
    res = a_shtc3_write(handle, command);                             /* write command */
    if (res != 0)                                                     /* check result */
    {
        handle->debug_print("shtc3: write command failed.\n");        /* write command failed */
           
        return 1;                                                     /* return error */
    }
    handle->delay_ms(1);                                              /* delay 1ms */
    
    return 0;                                                         /* success return 0 */
}

/**
 * @brief     sleep
 * @param[in] *handle pointer to a shtc3 handle structure
 * @return    status code
 *            - 0 success
 *            - 1 sleep failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t shtc3_sleep(shtc3_handle_t *handle)
{
    uint8_t res;
    uint16_t command;
    
    if (handle == NULL)                                               /* check handle */
    {
        return 2;                                                     /* return error */
    }
    if (handle->inited != 1)                                          /* check handle initialization */
    {
        return 3;                                                     /* return error */
    }
    
    command = SHTC3_COMMAND_SLEEP;                                    /* set command */
    res = a_shtc3_write(handle, command);                             /* write command */
    if (res != 0)                                                     /* check result */
    {
        handle->debug_print("shtc3: write command failed.\n");        /* write command failed */
           
        return 1;                                                     /* return error */
    }
    
    return 0;                                                         /* success return 0 */
}

/**
 * @brief     wakeup
 * @param[in] *handle pointer to a shtc3 handle structure
 * @return    status code
 *            - 0 success
 *            - 1 wakeup failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t shtc3_wakeup(shtc3_handle_t *handle)
{
    uint8_t res;
    uint16_t command;
    
    if (handle == NULL)                                               /* check handle */
    {
        return 2;                                                     /* return error */
    }
    if (handle->inited != 1)                                          /* check handle initialization */
    {
        return 3;                                                     /* return error */
    }
    
    command = SHTC3_COMMAND_WAKEUP;                                   /* set command */
    res = a_shtc3_write(handle, command);                             /* write command */
    if (res != 0)                                                     /* check result */
    {
        handle->debug_print("shtc3: write command failed.\n");        /* write command failed */
           
        return 1;                                                     /* return error */
    }
    handle->delay_ms(1);                                              /* delay 1ms */
    
    return 0;                                                         /* success return 0 */
}

/**
 * @brief     set the chip register
 * @param[in] *handle pointer to a shtc3 handle structure
 * @param[in] command chip command
 * @return    status code
 *            - 0 success
 *            - 1 write failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t shtc3_set_reg(shtc3_handle_t *handle, uint16_t command)
{
    if (handle == NULL)                          /* check handle */
    {
        return 2;                                /* return error */
    }
    if (handle->inited != 1)                     /* check handle initialization */
    {
        return 3;                                /* return error */
    }
    
    return a_shtc3_write(handle, command);       /* write command */
}

/**
 * @brief      get the chip register
 * @param[in]  *handle pointer to a shtc3 handle structure
 * @param[in]  command chip command
 * @param[out] *buf pointer to data buffer
 * @param[in]  len data length
 * @return     status code
 *             - 0 success
 *             - 1 read failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t shtc3_get_reg(shtc3_handle_t *handle, uint16_t command, uint8_t *buf, uint16_t len)
{
    if (handle == NULL)                                   /* check handle */
    {
        return 2;                                         /* return error */
    }
    if (handle->inited != 1)                              /* check handle initialization */
    {
        return 3;                                         /* return error */
    }
    
    return a_shtc3_read(handle, command, buf, len);       /* read data */
}

/**
 * @brief      get chip's information
 * @param[out] *info pointer to a shtc3 info structure
 * @return     status code
 *             - 0 success
 *             - 2 handle is NULL
 * @note       none
 */
uint8_t shtc3_info(shtc3_info_t *info)
{
    if (info == NULL)                                               /* check handle */
    {
        return 2;                                                   /* return error */
    }
    
    memset(info, 0, sizeof(shtc3_info_t));                          /* initialize shtc3 info structure */
    strncpy(info->chip_name, CHIP_NAME, 32);                        /* copy chip name */
    strncpy(info->manufacturer_name, MANUFACTURER_NAME, 32);        /* copy manufacturer name */
    strncpy(info->interface, "IIC", 8);                             /* copy interface name */
    info->supply_voltage_min_v = SUPPLY_VOLTAGE_MIN;                /* set minimal supply voltage */
    info->supply_voltage_max_v = SUPPLY_VOLTAGE_MAX;                /* set maximum supply voltage */
    info->max_current_ma = MAX_CURRENT;                             /* set maximum current */
    info->temperature_max = TEMPERATURE_MAX;                        /* set minimal temperature */
    info->temperature_min = TEMPERATURE_MIN;                        /* set maximum temperature */
    info->driver_version = DRIVER_VERSION;                          /* set driver version */
    
    return 0;                                                       /* success return 0 */
}
