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
 * @file      driver_bme680.h
 * @brief     driver bme680 header file
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

#ifndef DRIVER_BME680_H
#define DRIVER_BME680_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C"{
#endif

/**
 * @defgroup bme680_driver bme680 driver function
 * @brief    bme680 driver modules
 * @{
 */

/**
 * @addtogroup bme680_base_driver
 * @{
 */

/**
 * @brief bme680 bool enumeration definition
 */
typedef enum
{
    BME680_BOOL_FALSE = 0x00,        /**< false */
    BME680_BOOL_TRUE  = 0x01,        /**< true */
} bme680_bool_t;

/**
 * @brief bme680 interface enumeration definition
 */
typedef enum
{
    BME680_INTERFACE_IIC = 0x00,        /**< iic interface */
    BME680_INTERFACE_SPI = 0x01,        /**< spi interface */
} bme680_interface_t;

/**
 * @brief bme680 address enumeration definition
 */
typedef enum
{
    BME680_ADDRESS_ADO_LOW  = (0x76 << 1),        /**< ADO is low */
    BME680_ADDRESS_ADO_HIGH = (0x77 << 1),        /**< ADO is HIGH */
} bme680_address_t;

/**
 * @brief bme680 oversampling enumeration definition
 */
typedef enum
{
    BME680_OVERSAMPLING_SKIP = 0x00,        /**< oversampling x1 */
    BME680_OVERSAMPLING_x1   = 0x01,        /**< oversampling x1 */
    BME680_OVERSAMPLING_x2   = 0x02,        /**< oversampling x2 */
    BME680_OVERSAMPLING_x4   = 0x03,        /**< oversampling x4 */
    BME680_OVERSAMPLING_x8   = 0x04,        /**< oversampling x8 */
    BME680_OVERSAMPLING_x16  = 0x05,        /**< oversampling x16 */
} bme680_oversampling_t;

/**
 * @brief bme680 mode enumeration definition
 */
typedef enum
{
    BME680_MODE_SLEEP  = 0x00,        /**< sleep mode */
    BME680_MODE_FORCED = 0x01,        /**< forced mode */
} bme680_mode_t;

/**
 * @brief bme680 filter enumeration definition
 */
typedef enum
{
    BME680_FILTER_OFF       = 0x00,        /**< off */
    BME680_FILTER_COEFF_1   = 0x01,        /**< coeff 1 */
    BME680_FILTER_COEFF_3   = 0x02,        /**< coeff 3 */
    BME680_FILTER_COEFF_7   = 0x03,        /**< coeff 7 */
    BME680_FILTER_COEFF_15  = 0x04,        /**< coeff 15 */
    BME680_FILTER_COEFF_31  = 0x05,        /**< coeff 31 */
    BME680_FILTER_COEFF_63  = 0x06,        /**< coeff 63 */
    BME680_FILTER_COEFF_127 = 0x07,        /**< coeff 127 */
} bme680_filter_t;

/**
 * @brief bme680 spi wire enumeration definition
 */
typedef enum
{
    BME680_SPI_WIRE_4 = 0x00,        /**< 4 wire */
    BME680_SPI_WIRE_3 = 0x01,        /**< 3 wire */
} bme680_spi_wire_t;

/**
 * @brief bme680 handle structure definition
 */
typedef struct bme680_handle_s
{
    uint8_t iic_addr;                                                                   /**< iic device address */
    uint8_t (*iic_init)(void);                                                          /**< point to an iic_init function address */
    uint8_t (*iic_deinit)(void);                                                        /**< point to an iic_deinit function address */
    uint8_t (*iic_read)(uint8_t addr, uint8_t reg, uint8_t *buf, uint16_t len);         /**< point to an iic_read function address */
    uint8_t (*iic_write)(uint8_t addr, uint8_t reg, uint8_t *buf, uint16_t len);        /**< point to an iic_write function address */
    uint8_t (*spi_init)(void);                                                          /**< point to a spi_init function address */
    uint8_t (*spi_deinit)(void);                                                        /**< point to a spi_deinit function address */
    uint8_t (*spi_read)(uint8_t reg, uint8_t *buf, uint16_t len);                       /**< point to a spi_read function address */
    uint8_t (*spi_write)(uint8_t reg, uint8_t *buf, uint16_t len);                      /**< point to a spi_write function address */
    void (*delay_ms)(uint32_t ms);                                                      /**< point to a delay_ms function address */
    void (*debug_print)(const char *const fmt, ...);                                    /**< point to a debug_print function address */
    uint8_t inited;                                                                     /**< inited flag */
    uint8_t iic_spi;                                                                    /**< iic spi interface */
    uint8_t page;                                                                       /**< spi page */
    uint16_t par_h1;                                                                    /**< calibration coefficient for the humidity sensor */
    uint16_t par_h2;                                                                    /**< calibration coefficient for the humidity sensor */
    int8_t par_h3;                                                                      /**< calibration coefficient for the humidity sensor */
    int8_t par_h4;                                                                      /**< calibration coefficient for the humidity sensor */
    int8_t par_h5;                                                                      /**< calibration coefficient for the humidity sensor */
    uint8_t par_h6;                                                                     /**< calibration coefficient for the humidity sensor */
    int8_t par_h7;                                                                      /**< calibration coefficient for the humidity sensor */
    int8_t par_gh1;                                                                     /**< calibration coefficient for the gas sensor */
    int16_t par_gh2;                                                                    /**< calibration coefficient for the gas sensor */
    int8_t par_gh3;                                                                     /**< calibration coefficient for the gas sensor */
    uint16_t par_t1;                                                                    /**< calibration coefficient for the temperature sensor */
    int16_t par_t2;                                                                     /**< calibration coefficient for the temperature sensor */
    int8_t par_t3;                                                                      /**< calibration coefficient for the temperature sensor */
    uint16_t par_p1;                                                                    /**< calibration coefficient for the pressure sensor */
    int16_t par_p2;                                                                     /**< calibration coefficient for the pressure sensor */
    int8_t par_p3;                                                                      /**< calibration coefficient for the pressure sensor */
    int16_t par_p4;                                                                     /**< calibration coefficient for the pressure sensor */
    int16_t par_p5;                                                                     /**< calibration coefficient for the pressure sensor */
    int8_t par_p6;                                                                      /**< calibration coefficient for the pressure sensor */
    int8_t par_p7;                                                                      /**< calibration coefficient for the pressure sensor */
    int16_t par_p8;                                                                     /**< calibration coefficient for the pressure sensor */
    int16_t par_p9;                                                                     /**< calibration coefficient for the pressure sensor */
    uint8_t par_p10;                                                                    /**< calibration coefficient for the pressure sensor */
    uint8_t res_heat_range;                                                             /**< heater resistance range coefficient */
    int8_t res_heat_val;                                                                /**< heater resistance value coefficient */
    int8_t range_sw_err;                                                                /**< gas resistance range switching error coefficient */
    int8_t amb_temp;                                                                    /**< ambient temperature in degree c */
    float t_fine;                                                                       /**< variable to store the intermediate temperature coefficient */
} bme680_handle_t;

/**
 * @brief bme680 information structure definition
 */
typedef struct bme680_info_s
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
} bme680_info_t;

/**
 * @}
 */

/**
 * @defgroup bme680_link_driver bme680 link driver function
 * @brief    bme680 link driver modules
 * @ingroup  bme680_driver
 * @{
 */

/**
 * @brief     initialize bme680_handle_t structure
 * @param[in] HANDLE pointer to a bme680 handle structure
 * @param[in] STRUCTURE bme680_handle_t
 * @note      none
 */
#define DRIVER_BME680_LINK_INIT(HANDLE, STRUCTURE)          memset(HANDLE, 0, sizeof(STRUCTURE))

/**
 * @brief     link iic_init function
 * @param[in] HANDLE pointer to a bme680 handle structure
 * @param[in] FUC pointer to an iic_init function address
 * @note      none
 */
#define DRIVER_BME680_LINK_IIC_INIT(HANDLE, FUC)           (HANDLE)->iic_init = FUC

/**
 * @brief     link iic_deinit function
 * @param[in] HANDLE pointer to a bme680 handle structure
 * @param[in] FUC pointer to an iic_deinit function address
 * @note      none
 */
#define DRIVER_BME680_LINK_IIC_DEINIT(HANDLE, FUC)         (HANDLE)->iic_deinit = FUC

/**
 * @brief     link iic_read function
 * @param[in] HANDLE pointer to a bme680 handle structure
 * @param[in] FUC pointer to an iic_read function address
 * @note      none
 */
#define DRIVER_BME680_LINK_IIC_READ(HANDLE, FUC)           (HANDLE)->iic_read = FUC

/**
 * @brief     link iic_write function
 * @param[in] HANDLE pointer to a bme680 handle structure
 * @param[in] FUC pointer to an iic_write function address
 * @note      none
 */
#define DRIVER_BME680_LINK_IIC_WRITE(HANDLE, FUC)          (HANDLE)->iic_write = FUC

/**
 * @brief     link spi_init function
 * @param[in] HANDLE pointer to a bme680 handle structure
 * @param[in] FUC pointer to a spi_init function address
 * @note      none
 */
#define DRIVER_BME680_LINK_SPI_INIT(HANDLE, FUC)           (HANDLE)->spi_init = FUC

/**
 * @brief     link spi_deinit function
 * @param[in] HANDLE pointer to a bme680 handle structure
 * @param[in] FUC pointer to a spi_deinit function address
 * @note      none
 */
#define DRIVER_BME680_LINK_SPI_DEINIT(HANDLE, FUC)         (HANDLE)->spi_deinit = FUC

/**
 * @brief     link spi_read function
 * @param[in] HANDLE pointer to a bme680 handle structure
 * @param[in] FUC pointer to a spi_read function address
 * @note      none
 */
#define DRIVER_BME680_LINK_SPI_READ(HANDLE, FUC)           (HANDLE)->spi_read = FUC

/**
 * @brief     link spi_write function
 * @param[in] HANDLE pointer to a bme680 handle structure
 * @param[in] FUC pointer to a spi_write function address
 * @note      none
 */
#define DRIVER_BME680_LINK_SPI_WRITE(HANDLE, FUC)          (HANDLE)->spi_write = FUC

/**
 * @brief     link delay_ms function
 * @param[in] HANDLE pointer to a bme680 handle structure
 * @param[in] FUC pointer to a delay_ms function address
 * @note      none
 */
#define DRIVER_BME680_LINK_DELAY_MS(HANDLE, FUC)           (HANDLE)->delay_ms = FUC

/**
 * @brief     link debug_print function
 * @param[in] HANDLE pointer to a bme680 handle structure
 * @param[in] FUC pointer to a debug_print function address
 * @note      none
 */
#define DRIVER_BME680_LINK_DEBUG_PRINT(HANDLE, FUC)        (HANDLE)->debug_print = FUC

/**
 * @}
 */

/**
 * @defgroup bme680_base_driver bme680 base driver function
 * @brief    bme680 base driver modules
 * @ingroup  bme680_driver
 * @{
 */

/**
 * @brief      get chip's information
 * @param[out] *info pointer to a bme680 info structure
 * @return     status code
 *             - 0 success
 *             - 2 handle is NULL
 * @note       none
 */
uint8_t bme680_info(bme680_info_t *info);

/**
 * @brief     set the interface
 * @param[in] *handle pointer to a bme680 handle structure
 * @param[in] interface chip interface
 * @return    status code
 *            - 0 success
 *            - 2 handle is NULL
 * @note      none
 */
uint8_t bme680_set_interface(bme680_handle_t *handle, bme680_interface_t interface);

/**
 * @brief      get the interface
 * @param[in]  *handle pointer to a bme680 handle structure
 * @param[out] *interface pointer to a chip interface buffer
 * @return     status code
 *             - 0 success
 *             - 2 handle is NULL
 * @note       none
 */
uint8_t bme680_get_interface(bme680_handle_t *handle, bme680_interface_t *interface);

/**
 * @brief     set the iic address pin
 * @param[in] *handle pointer to a bme680 handle structure
 * @param[in] addr_pin iic address pin
 * @return    status code
 *            - 0 success
 *            - 2 handle is NULL
 * @note      none
 */
uint8_t bme680_set_addr_pin(bme680_handle_t *handle, bme680_address_t addr_pin);

/**
 * @brief      get the iic address pin
 * @param[in]  *handle pointer to a bme680 handle structure
 * @param[out] *addr_pin pointer to an iic address pin buffer
 * @return     status code
 *             - 0 success
 *             - 2 handle is NULL
 * @note       none
 */
uint8_t bme680_get_addr_pin(bme680_handle_t *handle, bme680_address_t *addr_pin);

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
uint8_t bme680_init(bme680_handle_t *handle);

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
uint8_t bme680_deinit(bme680_handle_t *handle);

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
                    uint16_t *adc_raw, uint8_t *adc_range, float *ohms, uint8_t *index);

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
                                                  uint32_t *humidity_raw, float *humidity_percentage);

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
uint8_t bme680_read_pressure(bme680_handle_t *handle, uint32_t *pressure_raw, float *pressure_pa);

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
uint8_t bme680_read_temperature(bme680_handle_t *handle, uint32_t *temperature_raw, float *temperature_c);

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
uint8_t bme680_read_humidity(bme680_handle_t *handle, uint32_t *humidity_raw, float *humidity_percentage);

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
uint8_t bme680_read_gas_resistance(bme680_handle_t *handle, uint16_t *adc_raw, uint8_t *adc_range, float *ohms, uint8_t *index);

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
uint8_t bme680_soft_reset(bme680_handle_t *handle);

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
uint8_t bme680_set_humidity_oversampling(bme680_handle_t *handle, bme680_oversampling_t oversampling);

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
uint8_t bme680_get_humidity_oversampling(bme680_handle_t *handle, bme680_oversampling_t *oversampling);

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
uint8_t bme680_set_temperature_oversampling(bme680_handle_t *handle, bme680_oversampling_t oversampling);

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
uint8_t bme680_get_temperature_oversampling(bme680_handle_t *handle, bme680_oversampling_t *oversampling);

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
uint8_t bme680_set_pressure_oversampling(bme680_handle_t *handle, bme680_oversampling_t oversampling);

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
uint8_t bme680_get_pressure_oversampling(bme680_handle_t *handle, bme680_oversampling_t *oversampling);

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
uint8_t bme680_set_mode(bme680_handle_t *handle, bme680_mode_t mode);

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
uint8_t bme680_get_mode(bme680_handle_t *handle, bme680_mode_t *mode);

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
uint8_t bme680_set_filter(bme680_handle_t *handle, bme680_filter_t filter);

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
uint8_t bme680_get_filter(bme680_handle_t *handle, bme680_filter_t *filter);

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
uint8_t bme680_set_spi_wire(bme680_handle_t *handle, bme680_spi_wire_t spi);

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
uint8_t bme680_get_spi_wire(bme680_handle_t *handle, bme680_spi_wire_t *spi);

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
uint8_t bme680_set_spi_wire_3_data_interrupt(bme680_handle_t *handle, bme680_bool_t enable);

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
uint8_t bme680_get_spi_wire_3_data_interrupt(bme680_handle_t *handle, bme680_bool_t *enable);

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
uint8_t bme680_get_new_data_status(bme680_handle_t *handle, bme680_bool_t *enable);

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
uint8_t bme680_get_gas_measuring_status(bme680_handle_t *handle, bme680_bool_t *enable);

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
uint8_t bme680_get_measuring_status(bme680_handle_t *handle, bme680_bool_t *enable);

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
uint8_t bme680_get_gas_measuring_index(bme680_handle_t *handle, uint8_t *index);

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
uint8_t bme680_get_gas_valid_status(bme680_handle_t *handle, bme680_bool_t *enable);

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
uint8_t bme680_get_heater_stability_status(bme680_handle_t *handle, bme680_bool_t *enable);

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
uint8_t bme680_set_heat_off(bme680_handle_t *handle, bme680_bool_t enable);

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
uint8_t bme680_get_heat_off(bme680_handle_t *handle, bme680_bool_t *enable);

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
uint8_t bme680_set_run_gas(bme680_handle_t *handle, bme680_bool_t enable);

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
uint8_t bme680_get_run_gas(bme680_handle_t *handle, bme680_bool_t *enable);

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
uint8_t bme680_set_convert_index(bme680_handle_t *handle, uint8_t index);

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
uint8_t bme680_get_convert_index(bme680_handle_t *handle, uint8_t *index);

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
uint8_t bme680_set_idac_heat(bme680_handle_t *handle, uint8_t index, uint8_t reg);

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
uint8_t bme680_get_idac_heat(bme680_handle_t *handle, uint8_t index, uint8_t *reg);

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
uint8_t bme680_set_resistance_heat(bme680_handle_t *handle, uint8_t index, uint8_t reg);

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
uint8_t bme680_get_resistance_heat(bme680_handle_t *handle, uint8_t index, uint8_t *reg);

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
uint8_t bme680_set_gas_wait(bme680_handle_t *handle, uint8_t index, uint8_t reg);

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
uint8_t bme680_get_gas_wait(bme680_handle_t *handle, uint8_t index, uint8_t *reg);

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
uint8_t bme680_resistance_heat_convert_to_register(bme680_handle_t *handle, float degree_celsius, uint8_t *reg);

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
uint8_t bme680_gas_wait_convert_to_register(bme680_handle_t *handle, uint16_t ms, uint8_t *reg);

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
uint8_t bme680_idac_heat_convert_to_register(bme680_handle_t *handle, float ma, uint8_t *reg);

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
uint8_t bme680_idac_heat_convert_to_data(bme680_handle_t *handle, uint8_t reg, float *ma);

/**
 * @}
 */

/**
 * @defgroup bme680_extern_driver bme680 extern driver function
 * @brief    bme680 extern driver modules
 * @ingroup  bme680_driver
 * @{
 */

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
uint8_t bme680_set_reg(bme680_handle_t *handle, uint8_t reg, uint8_t *buf, uint16_t len);

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
uint8_t bme680_get_reg(bme680_handle_t *handle, uint8_t reg, uint8_t *buf, uint16_t len);

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
