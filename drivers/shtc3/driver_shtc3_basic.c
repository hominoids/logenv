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
 * @file      driver_shtc3_basic.c
 * @brief     driver shtc3 basic source file
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

#include "driver_shtc3_basic.h"

static shtc3_handle_t gs_handle;        /**< shtc3 handle */

/**
 * @brief  basic example init
 * @return status code
 *         - 0 success
 *         - 1 init failed
 * @note   none
 */
uint8_t shtc3_basic_init(void)
{
    uint8_t res;
    
    /* link functions */
    DRIVER_SHTC3_LINK_INIT(&gs_handle, shtc3_handle_t);
    DRIVER_SHTC3_LINK_IIC_INIT(&gs_handle, shtc3_interface_iic_init);
    DRIVER_SHTC3_LINK_IIC_DEINIT(&gs_handle, shtc3_interface_iic_deinit);
#if (SHTC3_BASIC_DEFAULT_CLOCK_STRETCHING != 1)
    DRIVER_SHTC3_LINK_IIC_READ_ADDRESS16(&gs_handle, shtc3_interface_iic_read_address16);
#else
    DRIVER_SHTC3_LINK_IIC_READ_ADDRESS16(&gs_handle, shtc3_interface_iic_scl_read_address16);
#endif
    DRIVER_SHTC3_LINK_IIC_WRITE_ADDRESS16(&gs_handle, shtc3_interface_iic_write_address16);
    DRIVER_SHTC3_LINK_DELAY_MS(&gs_handle, shtc3_interface_delay_ms);
    DRIVER_SHTC3_LINK_DEBUG_PRINT(&gs_handle, shtc3_interface_debug_print);
    
    /* shtc3 init */
    res = shtc3_init(&gs_handle);
    if (res != 0)
    {
        shtc3_interface_debug_print("shtc3: init failed.\n");
        
        return 1;
    }
    
    return 0;
}

/**
 * @brief      basic example read
 * @param[out] *temperature pointer to a converted temperature buffer
 * @param[out] *humidity pointer to a converted humidity buffer
 * @return     status code
 *             - 0 success
 *             - 1 read failed
 * @note       none
 */
uint8_t shtc3_basic_read(float *temperature, float *humidity)
{
    uint16_t temperature_raw;
    uint16_t humidity_raw;
   
    /* read data */
#if (SHTC3_BASIC_DEFAULT_CLOCK_STRETCHING != 1)
    if (shtc3_read(&gs_handle, SHTC3_BOOL_FALSE, (uint16_t *)&temperature_raw, temperature, (uint16_t *)&humidity_raw, humidity) != 0)
    {
        return 1;
    }
#else
    if (shtc3_read(&gs_handle, SHTC3_BOOL_TRUE, (uint16_t *)&temperature_raw, temperature, (uint16_t *)&humidity_raw, humidity) != 0)
    {
        return 1;
    }
#endif
    
    return 0;
}

/**
 * @brief      basic example read low power
 * @param[out] *temperature pointer to a converted temperature buffer
 * @param[out] *humidity pointer to a converted humidity buffer
 * @return     status code
 *             - 0 success
 *             - 1 read failed
 * @note       none
 */
uint8_t shtc3_basic_read_low_power(float *temperature, float *humidity)
{
    uint16_t temperature_raw;
    uint16_t humidity_raw;
   
    /* read data */
#if (SHTC3_BASIC_DEFAULT_CLOCK_STRETCHING != 1)
    if (shtc3_read_low_power(&gs_handle, SHTC3_BOOL_FALSE, (uint16_t *)&temperature_raw, temperature,
                            (uint16_t *)&humidity_raw, humidity) != 0)
    {
        return 1;
    }
#else
    if (shtc3_read_low_power(&gs_handle, SHTC3_BOOL_TRUE, (uint16_t *)&temperature_raw, temperature,
                            (uint16_t *)&humidity_raw, humidity) != 0)
    {
        return 1;
    }
#endif
    
    return 0;
}

/**
 * @brief  basic example deinit
 * @return status code
 *         - 0 success
 *         - 1 deinit failed
 * @note   none
 */
uint8_t shtc3_basic_deinit(void)
{
    /* close shtc3 */
    if (shtc3_deinit(&gs_handle) != 0)
    {
        return 1;
    }
    
    return 0;
}

/**
 * @brief  basic example soft reset
 * @return status code
 *         - 0 success
 *         - 1 soft reset failed
 * @note   none
 */
uint8_t shtc3_basic_soft_reset(void)
{
    /* soft reset */
    if (shtc3_soft_reset(&gs_handle) != 0)
    {
        return 1;
    }
    
    return 0;
}

/**
 * @brief  basic example sleep
 * @return status code
 *         - 0 success
 *         - 1 sleep failed
 * @note   none
 */
uint8_t shtc3_basic_sleep(void)
{
    /* sleep */
    if (shtc3_sleep(&gs_handle) != 0)
    {
        return 1;
    }
    
    return 0;
}

/**
 * @brief  basic example wakeup
 * @return status code
 *         - 0 success
 *         - 1 wakeup failed
 * @note   none
 */
uint8_t shtc3_basic_wakeup(void)
{
    /* wakeup */
    if (shtc3_wakeup(&gs_handle) != 0)
    {
        return 1;
    }
    
    return 0;
}
