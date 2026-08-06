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
 * @file      driver_pmsa003i_basic.h
 * @brief     driver pmsa003i basic header file
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

#ifndef DRIVER_PMSA003I_BASIC_H
#define DRIVER_PMSA003I_BASIC_H

#include "driver_pmsa003i_interface.h"

#ifdef __cplusplus
extern "C"{
#endif

/**
 * @defgroup pmsa003i_example_driver pmsa003i example driver function
 * @brief    pmsa003i example driver modules
 * @ingroup  pmsa003i_driver
 * @{
 */

/**
 * @brief pmsa003i basic example default definition
 */
#define PMSA003I_BASIC_DEFAULT_MODE        PMSA003I_MODE_ACTIVE        /**< passive mode */

/**
 * @brief  basic example init
 * @return status code
 *         - 0 success
 *         - 1 init failed
 * @note   none
 */
uint8_t pmsa003i_basic_init(void);

/**
 * @brief  basic example deinit
 * @return status code
 *         - 0 success
 *         - 1 deinit failed
 * @note   none
 */
uint8_t pmsa003i_basic_deinit(void);

/**
 * @brief      basic example read
 * @param[out] *data pointer to a data structure
 * @return     status code
 *             - 0 success
 *             - 1 read failed
 * @note       none
 */
uint8_t pmsa003i_basic_read(pmsa003i_data_t *data);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
