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
 * @file      driver_pmsa003i.h
 * @brief     driver pmsa003i header file
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

#ifndef DRIVER_PMSA003I_H
#define DRIVER_PMSA003I_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C"{
#endif

/**
 * @defgroup pmsa003i_driver pmsa003i driver function
 * @brief    pmsa003i driver modules
 * @{
 */

/**
 * @addtogroup pmsa003i_basic_driver
 * @{
 */

/**
 * @brief pmsa003i mode enumeration definition
 */
typedef enum
{
    PMSA003I_MODE_PASSIVE = 0x00,        /**< passive mode */
    PMSA003I_MODE_ACTIVE  = 0x01,        /**< active mode */
} pmsa003i_mode_t;

/**
 * @brief pmsa003i data structure definition
 */
typedef struct pmsa003i_data_s
{
    uint16_t pm1p0_standard_ug_m3;           /**< pm1.0 concentration unit, standard particle */
    uint16_t pm2p5_standard_ug_m3;           /**< pm2.5 concentration unit, standard particle */
    uint16_t pm10_standard_ug_m3;            /**< pm10 concentration unit, standard particle */
    uint16_t pm1p0_atmospheric_ug_m3;        /**< pm1.0 concentration unit, under atmospheric environment */
    uint16_t pm2p5_atmospheric_ug_m3;        /**< pm2.5 concentration unit, under atmospheric environment */
    uint16_t pm10_atmospheric_ug_m3;         /**< pm10 concentration unit, under atmospheric environment */
    uint16_t beyond_0p3um;                   /**< beyond 0.3 um in 0.1 L of air */
    uint16_t beyond_0p5um;                   /**< beyond 0.5 um in 0.1 L of air */
    uint16_t beyond_1p0um;                   /**< beyond 1.0 um in 0.1 L of air */
    uint16_t beyond_2p5um;                   /**< beyond 2.5 um in 0.1 L of air */
    uint16_t beyond_5p0um;                   /**< beyond 5.0 um in 0.1 L of air */
    uint16_t beyond_10um;                    /**< beyond 10 um in 0.1 L of air */
    uint8_t version;                         /**< version */
    uint8_t error_code;                      /**< error code */
} pmsa003i_data_t;

/**
 * @brief pmsa003i handle structure definition
 */
typedef struct pmsa003i_handle_s
{
    uint8_t (*iic_init)(void);                                                          /**< point to an iic_init function address */
    uint8_t (*iic_deinit)(void);                                                        /**< point to an iic_deinit function address */
    uint8_t (*iic_read_cmd)(uint8_t addr, uint8_t *buf, uint16_t len);                  /**< point to an iic_read_cmd function address */
    uint8_t (*iic_write_cmd)(uint8_t addr, uint8_t *buf, uint16_t len);                 /**< point to an iic_write_cmd function address */
    void (*delay_ms)(uint32_t ms);                                                      /**< point to a delay_ms function address */
    void (*debug_print)(const char *const fmt, ...);                                    /**< point to a debug_print function address */
    uint8_t iic_addr;                                                                   /**< iic address */
    uint8_t mode;                                                                       /**< mode flag */
    uint8_t inited;                                                                     /**< inited flag */
} pmsa003i_handle_t;

/**
 * @brief pmsa003i information structure definition
 */
typedef struct pmsa003i_info_s
{
    char chip_name[32];                /**< chip name */
    char manufacturer_name[32];        /**< manufacturer name */
    char interface[8];                 /**< chip interface name */
    float supply_voltage_min_v;        /**< chip min supply voltage */
    float supply_voltage_max_v;        /**< chip max supply voltage */
    float max_current_ma;              /**< chip max current */
    float temperature_min;             /**< chip min operating temperature */
    float temperature_max;             /**< chip max operating temperature */
    uint32_t driver_version;           /**< driver version */
} pmsa003i_info_t;

/**
 * @}
 */

/**
 * @defgroup pmsa003i_link_driver pmsa003i link driver function
 * @brief    pmsa003i link driver modules
 * @ingroup  pmsa003i_driver
 * @{
 */

/**
 * @brief     initialize pmsa003i_handle_t structure
 * @param[in] HANDLE pointer to a pmsa003i handle structure
 * @param[in] STRUCTURE pmsa003i_handle_t
 * @note      none
 */
#define DRIVER_PMSA003I_LINK_INIT(HANDLE, STRUCTURE)            memset(HANDLE, 0, sizeof(STRUCTURE))

/**
 * @brief     link iic_init function
 * @param[in] HANDLE pointer to a pmsa003i handle structure
 * @param[in] FUC pointer to an iic_init function address
 * @note      none
 */
#define DRIVER_PMSA003I_LINK_IIC_INIT(HANDLE, FUC)            (HANDLE)->iic_init = FUC

/**
 * @brief     link iic_deinit function
 * @param[in] HANDLE pointer to a pmsa003i handle structure
 * @param[in] FUC pointer to an iic_deinit function address
 * @note      none
 */
#define DRIVER_PMSA003I_LINK_IIC_DEINIT(HANDLE, FUC)          (HANDLE)->iic_deinit = FUC

/**
 * @brief     link iic_read function
 * @param[in] HANDLE pointer to a pmsa003i handle structure
 * @param[in] FUC pointer to an iic_read function address
 * @note      none
 */
#define DRIVER_PMSA003I_LINK_IIC_READ_CMD(HANDLE, FUC)            (HANDLE)->iic_read_cmd = FUC

/**
 * @brief     link iic_write function
 * @param[in] HANDLE pointer to a pmsa003i handle structure
 * @param[in] FUC pointer to an iic_write function address
 * @note      none
 */
#define DRIVER_PMSA003I_LINK_IIC_WRITE_CMD(HANDLE, FUC)           (HANDLE)->iic_write_cmd = FUC

/**
 * @brief     link delay_ms function
 * @param[in] HANDLE pointer to a pmsa003i handle structure
 * @param[in] FUC pointer to a delay_ms function address
 * @note      none
 */
#define DRIVER_PMSA003I_LINK_DELAY_MS(HANDLE, FUC)             (HANDLE)->delay_ms = FUC

/**
 * @brief     link debug_print function
 * @param[in] HANDLE pointer to a pmsa003i handle structure
 * @param[in] FUC pointer to a debug_print function address
 * @note      none
 */
#define DRIVER_PMSA003I_LINK_DEBUG_PRINT(HANDLE, FUC)          (HANDLE)->debug_print = FUC

/**
 * @}
 */

/**
 * @defgroup pmsa003i_basic_driver pmsa003i basic driver function
 * @brief    pmsa003i basic driver modules
 * @ingroup  pmsa003i_driver
 * @{
 */

/**
 * @brief      get chip's information
 * @param[out] *info pointer to a pmsa003i info structure
 * @return     status code
 *             - 0 success
 *             - 2 handle is NULL
 * @note       none
 */
uint8_t pmsa003i_info(pmsa003i_info_t *info);

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
uint8_t pmsa003i_init(pmsa003i_handle_t *handle);

/**
 * @brief     close the chip
 * @param[in] *handle pointer to a pmsa003i handle structure
 * @return    status code
 *            - 0 success
 *            - 1 iic deinit failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t pmsa003i_deinit(pmsa003i_handle_t *handle);

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
uint8_t pmsa003i_read(pmsa003i_handle_t *handle, pmsa003i_data_t *data);

/**
 * @brief     reset
 * @param[in] *handle pointer to a pmsa003i handle structure
 * @return    status code
 *            - 0 success
 *            - 1 reset failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t pmsa003i_reset(pmsa003i_handle_t *handle);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
