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
 * @file      driver_pmsa003i_basic.c
 * @brief     driver pmsa003i basic source file
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

#include "driver_pmsa003i_basic.h"

static pmsa003i_handle_t gs_handle;        /**< pmsa003i handle */

/**
 * @brief  basic example init
 * @return status code
 *         - 0 success
 *         - 1 init failed
 * @note   none
 */
uint8_t pmsa003i_basic_init(void)
{
    uint8_t res;
    
    /* link interface function */
    DRIVER_PMSA003I_LINK_INIT(&gs_handle, pmsa003i_handle_t);
    DRIVER_PMSA003I_LINK_IIC_INIT(&gs_handle, pmsa003i_interface_iic_init);
    DRIVER_PMSA003I_LINK_IIC_DEINIT(&gs_handle, pmsa003i_interface_iic_deinit);
    DRIVER_PMSA003I_LINK_IIC_READ_CMD(&gs_handle, pmsa003i_interface_iic_read_cmd);
    DRIVER_PMSA003I_LINK_DELAY_MS(&gs_handle, pmsa003i_interface_delay_ms);
    DRIVER_PMSA003I_LINK_DEBUG_PRINT(&gs_handle, pmsa003i_interface_debug_print);
    
    /* pmsa003i init */
    res = pmsa003i_init(&gs_handle);
    if (res != 0)
    {
        pmsa003i_interface_debug_print("pmsa003i: init failed.\n");
        
        return 1;
    }

    return 0;
}

/**
 * @brief      basic example read
 * @param[out] *data pointer to a data structure
 * @return     status code
 *             - 0 success
 *             - 1 read failed
 * @note       none
 */
uint8_t pmsa003i_basic_read(pmsa003i_data_t *data)
{
    uint8_t res;

    /* read */
    res = pmsa003i_read(&gs_handle, data);
    if (res != 0)
    {
        return 1;
    }
    
    return 0;
}

/**
 * @brief  basic example deinit
 * @return status code
 *         - 0 success
 *         - 1 deinit failed
 * @note   none
 */
uint8_t pmsa003i_basic_deinit(void)
{
    /* deinit pmsa003i */
    if (pmsa003i_deinit(&gs_handle) != 0)
    {
        return 1;
    }
    
    return 0;
}
