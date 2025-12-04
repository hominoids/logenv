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
 * @file      driver_bme680_basic.c
 * @brief     driver bme680 basic source file
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

#include "driver_bme680_basic.h"

static bme680_handle_t gs_handle;        /**< bme680 handle */

/**
 * @brief     basic example init
 * @param[in] interface chip interface
 * @param[in] addr_pin chip address pin
 * @return    status code
 *            - 0 success
 *            - 1 init failed
 * @note      none
 */
uint8_t bme680_basic_init(bme680_interface_t interface, bme680_address_t addr_pin)
{
    uint8_t res;
    
    /* link interface function */
    DRIVER_BME680_LINK_INIT(&gs_handle, bme680_handle_t); 
    DRIVER_BME680_LINK_IIC_INIT(&gs_handle, bme680_interface_iic_init);
    DRIVER_BME680_LINK_IIC_DEINIT(&gs_handle, bme680_interface_iic_deinit);
    DRIVER_BME680_LINK_IIC_READ(&gs_handle, bme680_interface_iic_read);
    DRIVER_BME680_LINK_IIC_WRITE(&gs_handle, bme680_interface_iic_write);
    DRIVER_BME680_LINK_SPI_INIT(&gs_handle, bme680_interface_spi_init);
    DRIVER_BME680_LINK_SPI_DEINIT(&gs_handle, bme680_interface_spi_deinit);
    DRIVER_BME680_LINK_SPI_READ(&gs_handle, bme680_interface_spi_read);
    DRIVER_BME680_LINK_SPI_WRITE(&gs_handle, bme680_interface_spi_write);
    DRIVER_BME680_LINK_DELAY_MS(&gs_handle, bme680_interface_delay_ms);
    DRIVER_BME680_LINK_DEBUG_PRINT(&gs_handle, bme680_interface_debug_print);
    
    /* set interface */
    res = bme680_set_interface(&gs_handle, interface);
    if (res != 0)
    {
        bme680_interface_debug_print("bme680: set interface failed.\n");
       
        return 1;
    }
    
    /* set addr pin */
    res = bme680_set_addr_pin(&gs_handle, addr_pin);
    if (res != 0)
    {
        bme680_interface_debug_print("bme680: set addr pin failed.\n");
       
        return 1;
    }
    
    /* bme680 init */
    res = bme680_init(&gs_handle);
    if (res != 0)
    {
        bme680_interface_debug_print("bme680: init failed.\n");
        
        return 1;
    }
    
    /* set default temperature oversampling */
    res = bme680_set_temperature_oversampling(&gs_handle, BME680_BASIC_DEFAULT_TEMPERATURE_OVERSAMPLING);
    if (res != 0)
    {
        bme680_interface_debug_print("bme680: set temperature oversampling failed.\n");
        (void)bme680_deinit(&gs_handle);
        
        return 1;
    }
    
    /* set default pressure oversampling */
    res = bme680_set_pressure_oversampling(&gs_handle, BME680_BASIC_DEFAULT_PRESSURE_OVERSAMPLING);
    if (res != 0)
    {
        bme680_interface_debug_print("bme680: set pressure oversampling failed.\n");
        (void)bme680_deinit(&gs_handle);
        
        return 1;
    }
    
    /* set default humidity oversampling */
    res = bme680_set_humidity_oversampling(&gs_handle, BME680_BASIC_DEFAULT_HUMIDITY_OVERSAMPLING);
    if (res != 0)
    {
        bme680_interface_debug_print("bme680: set humidity oversampling failed.\n");
        (void)bme680_deinit(&gs_handle);
        
        return 1;
    }
    
    /* set default filter */
    res = bme680_set_filter(&gs_handle, BME680_BASIC_DEFAULT_FILTER);
    if (res != 0)
    {
        bme680_interface_debug_print("bme680: set filter failed.\n");
        (void)bme680_deinit(&gs_handle);
        
        return 1;
    }
    
    /* set default spi wire */
    res = bme680_set_spi_wire(&gs_handle, BME680_BASIC_DEFAULT_SPI_WIRE);
    if (res != 0)
    {
        bme680_interface_debug_print("bme680: set spi wire failed.\n");
        (void)bme680_deinit(&gs_handle);
        
        return 1;
    }
    
    /* enable heat off */
    res = bme680_set_heat_off(&gs_handle, BME680_BOOL_TRUE);
    if (res != 0)
    {
        bme680_interface_debug_print("bme680: set heat off failed.\n");
        (void)bme680_deinit(&gs_handle); 
        
        return 1;
    }
    
    /* disable run gas */
    res = bme680_set_run_gas(&gs_handle, BME680_BOOL_FALSE);
    if (res != 0)
    {
        bme680_interface_debug_print("bme680: set run gas failed.\n");
        (void)bme680_deinit(&gs_handle); 
        
        return 1;
    }
    
    /* set sleep mode */
    res = bme680_set_mode(&gs_handle, BME680_MODE_SLEEP);
    if (res != 0)
    {
        bme680_interface_debug_print("bme680: set mode failed.\n");
        (void)bme680_deinit(&gs_handle); 
        
        return 1;
    }
    
    return 0;
}

/**
 * @brief      basic example read
 * @param[out] *temperature pointer to a converted temperature buffer
 * @param[out] *pressure pointer to a converted pressure buffer
 * @param[out] *humidity_percentage pointer to a converted humidity percentage buffer
 * @return     status code
 *             - 0 success
 *             - 1 read failed
 * @note       none
 */
uint8_t bme680_basic_read(float *temperature, float *pressure, float *humidity_percentage)
{
    uint32_t temperature_yaw;
    uint32_t pressure_yaw;
    uint32_t humidity_raw;
    
    /* read temperature pressure and humidity */
    if (bme680_read_temperature_pressure_humidity(&gs_handle, (uint32_t *)&temperature_yaw, 
                                                  temperature, (uint32_t *)&pressure_yaw, pressure,
                                                 (uint32_t *)&humidity_raw, humidity_percentage) != 0)
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
uint8_t bme680_basic_deinit(void)
{
    /* enable heat off */
    if (bme680_set_heat_off(&gs_handle, BME680_BOOL_TRUE) != 0)
    {
        return 1;
    }
    
    /* close bme680 */
    if (bme680_deinit(&gs_handle) != 0)
    {
        return 1;
    }
    
    return 0;
}
