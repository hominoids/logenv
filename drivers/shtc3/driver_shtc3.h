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
 * @file      driver_shtc3.h
 * @brief     driver shtc3 header file
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

#ifndef DRIVER_SHTC3_H
#define DRIVER_SHTC3_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C"{
#endif

/**
 * @defgroup shtc3_driver shtc3 driver function
 * @brief    shtc3 driver modules
 * @{
 */

/**
 * @addtogroup shtc3_base_driver
 * @{
 */

/**
 * @brief shtc3 bool enumeration definition
 */
typedef enum
{
    SHTC3_BOOL_FALSE = 0x00,        /**< disable */
    SHTC3_BOOL_TRUE  = 0x01,        /**< enable */
} shtc3_bool_t;

/**
 * @brief shtc3 handle structure definition
 */
typedef struct shtc3_handle_s
{
    uint8_t (*iic_init)(void);                                                                     /**< point to an iic_init function address */
    uint8_t (*iic_deinit)(void);                                                                   /**< point to an iic_deinit function address */
    uint8_t (*iic_write_address16)(uint8_t addr, uint16_t reg, uint8_t *buf, uint16_t len);        /**< point to an iic_write_address16 function address */
    uint8_t (*iic_read_address16)(uint8_t addr, uint16_t reg, uint8_t *buf, uint16_t len);         /**< point to an iic_read_address16 function address */
    void (*receive_callback)(uint16_t type);                                                       /**< point to a receive_callback function address */
    void (*delay_ms)(uint32_t ms);                                                                 /**< point to a delay_ms function address */
    void (*debug_print)(const char *const fmt, ...);                                               /**< point to a debug_print function address */
    uint8_t inited;                                                                                /**< inited flag */
} shtc3_handle_t;

/**
 * @brief shtc3 information structure definition
 */
typedef struct shtc3_info_s
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
} shtc3_info_t;

/**
 * @}
 */

/**
 * @defgroup shtc3_link_driver shtc3 link driver function
 * @brief    shtc3 link driver modules
 * @ingroup  shtc3_driver
 * @{
 */

/**
 * @brief     initialize shtc3_handle_t structure
 * @param[in] HANDLE pointer to a shtc3 handle structure
 * @param[in] STRUCTURE shtc3_handle_t
 * @note      none
 */
#define DRIVER_SHTC3_LINK_INIT(HANDLE, STRUCTURE)            memset(HANDLE, 0, sizeof(STRUCTURE))

/**
 * @brief     link iic_init function
 * @param[in] HANDLE pointer to a shtc3 handle structure
 * @param[in] FUC pointer to an iic_init function address
 * @note      none
 */
#define DRIVER_SHTC3_LINK_IIC_INIT(HANDLE, FUC)              (HANDLE)->iic_init = FUC

/**
 * @brief     link iic_deinit function
 * @param[in] HANDLE pointer to a shtc3 handle structure
 * @param[in] FUC pointer to an iic_deinit function address
 * @note      none
 */
#define DRIVER_SHTC3_LINK_IIC_DEINIT(HANDLE, FUC)            (HANDLE)->iic_deinit = FUC

/**
 * @brief     link iic_read_address16 function
 * @param[in] HANDLE pointer to a shtc3 handle structure
 * @param[in] FUC pointer to an iic_read_address16 function address
 * @note      none
 */
#define DRIVER_SHTC3_LINK_IIC_READ_ADDRESS16(HANDLE, FUC)    (HANDLE)->iic_read_address16 = FUC

/**
 * @brief     link iic_write_address16 function
 * @param[in] HANDLE pointer to a shtc3 handle structure
 * @param[in] FUC pointer to an iic_write_address16 function address
 * @note      none
 */
#define DRIVER_SHTC3_LINK_IIC_WRITE_ADDRESS16(HANDLE, FUC)   (HANDLE)->iic_write_address16 = FUC

/**
 * @brief     link delay_ms function
 * @param[in] HANDLE pointer to a shtc3 handle structure
 * @param[in] FUC pointer to a delay_ms function address
 * @note      none
 */
#define DRIVER_SHTC3_LINK_DELAY_MS(HANDLE, FUC)              (HANDLE)->delay_ms = FUC

/**
 * @brief     link debug_print function
 * @param[in] HANDLE pointer to a shtc3 handle structure
 * @param[in] FUC pointer to a debug_print function address
 * @note      none
 */
#define DRIVER_SHTC3_LINK_DEBUG_PRINT(HANDLE, FUC)           (HANDLE)->debug_print = FUC

/**
 * @brief     link receive_callback function
 * @param[in] HANDLE pointer to a shtc3 handle structure
 * @param[in] FUC pointer to a receive_callback function address
 * @note      none
 */
#define DRIVER_SHTC3_LINK_RECEIVE_CALLBACK(HANDLE, FUC)      (HANDLE)->receive_callback = FUC

/**
 * @}
 */

/**
 * @defgroup shtc3_base_driver shtc3 base driver function
 * @brief    shtc3 base driver modules
 * @ingroup  shtc3_driver
 * @{
 */

/**
 * @brief      get chip's information
 * @param[out] *info pointer to a shtc3 info structure
 * @return     status code
 *             - 0 success
 *             - 2 handle is NULL
 * @note       none
 */
uint8_t shtc3_info(shtc3_info_t *info);

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
uint8_t shtc3_init(shtc3_handle_t *handle);

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
uint8_t shtc3_deinit(shtc3_handle_t *handle);

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
                   uint16_t *humidity_raw, float *humidity_s);

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
                             uint16_t *humidity_raw, float *humidity_s);

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
uint8_t shtc3_soft_reset(shtc3_handle_t *handle);

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
uint8_t shtc3_sleep(shtc3_handle_t *handle);

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
uint8_t shtc3_wakeup(shtc3_handle_t *handle);

/**
 * @}
 */

/**
 * @defgroup shtc3_extend_driver shtc3 extend driver function
 * @brief    shtc3 extend driver modules
 * @ingroup  shtc3_driver
 * @{
 */

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
uint8_t shtc3_set_reg(shtc3_handle_t *handle, uint16_t command);

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
uint8_t shtc3_get_reg(shtc3_handle_t *handle, uint16_t command, uint8_t *buf, uint16_t len);

/**
 * @}
 */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
