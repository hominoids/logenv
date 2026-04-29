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
 * @file      driver_ssh1107_advance.c
 * @brief     driver ssh1107 advance source file
 * @version   1.0.0
 * @author    Edward Kisiel based on ssd1306 by Shifeng Li
 * @date      2026-04-15
 *
 * <h3>history</h3>
 * <table>
 * <tr><th>Date        <th>Version  <th>Author         <th>Description
 * <tr><td>2026/04/15  <td>1.0      <td>Edward Kisiel  <td>first upload
 * </table>
 */

#include "driver_ssh1107_advance.h"

ssh1107_handle_t ssh1107_handle;        /**< ssh1107 handle */

/**
 * @brief     advance example init
 * @param[in] interface interface type
 * @param[in] addr iic device address
 * @return    status code
 *            - 0 success
 *            - 1 init failed
 * @note      none
 */
uint8_t ssh1107_advance_init(ssh1107_interface_t interface, ssh1107_address_t addr)
{
    uint8_t res;
    
    /* link functions */
    DRIVER_SSH1107_LINK_INIT(&ssh1107_handle, ssh1107_handle_t);
    DRIVER_SSH1107_LINK_IIC_INIT(&ssh1107_handle, ssh1107_interface_iic_init);
    DRIVER_SSH1107_LINK_IIC_DEINIT(&ssh1107_handle, ssh1107_interface_iic_deinit);
    DRIVER_SSH1107_LINK_IIC_WRITE(&ssh1107_handle, ssh1107_interface_iic_write);
    DRIVER_SSH1107_LINK_SPI_INIT(&ssh1107_handle, ssh1107_interface_spi_init);
    DRIVER_SSH1107_LINK_SPI_DEINIT(&ssh1107_handle, ssh1107_interface_spi_deinit);
    DRIVER_SSH1107_LINK_SPI_WRITE_COMMAND(&ssh1107_handle, ssh1107_interface_spi_write_cmd);
    DRIVER_SSH1107_LINK_SPI_COMMAND_DATA_GPIO_INIT(&ssh1107_handle, ssh1107_interface_spi_cmd_data_gpio_init);
    DRIVER_SSH1107_LINK_SPI_COMMAND_DATA_GPIO_DEINIT(&ssh1107_handle, ssh1107_interface_spi_cmd_data_gpio_deinit);
    DRIVER_SSH1107_LINK_SPI_COMMAND_DATA_GPIO_WRITE(&ssh1107_handle, ssh1107_interface_spi_cmd_data_gpio_write);
    DRIVER_SSH1107_LINK_RESET_GPIO_INIT(&ssh1107_handle, ssh1107_interface_reset_gpio_init);
    DRIVER_SSH1107_LINK_RESET_GPIO_DEINIT(&ssh1107_handle, ssh1107_interface_reset_gpio_deinit);
    DRIVER_SSH1107_LINK_RESET_GPIO_WRITE(&ssh1107_handle, ssh1107_interface_reset_gpio_write);
    DRIVER_SSH1107_LINK_DELAY_MS(&ssh1107_handle, ssh1107_interface_delay_ms);
    DRIVER_SSH1107_LINK_DEBUG_PRINT(&ssh1107_handle, ssh1107_interface_debug_print);
    
    /* set interface */
    res = ssh1107_set_interface(&ssh1107_handle, interface);
    if (res != 0)
    {
        ssh1107_interface_debug_print("ssh1107: set interface failed.\n");
        
        return 1;
    }
    
    /* set addr pin */
    res = ssh1107_set_addr_pin(&ssh1107_handle, addr);
    if (res != 0)
    {
        ssh1107_interface_debug_print("ssh1107: set addr failed.\n");
        
        return 1;
    }
    
    /* ssh1107 init */
    res = ssh1107_init(&ssh1107_handle);
    if (res != 0)
    {
        ssh1107_interface_debug_print("ssh1107: init failed.\n");
        
        return 1;
    }
    
    /* close display */
    res = ssh1107_set_display(&ssh1107_handle, SSH1107_DISPLAY_OFF);
    if (res != 0)
    {
        ssh1107_interface_debug_print("ssh1107: set display failed.\n");
        (void)ssh1107_deinit(&ssh1107_handle);
        
        return 1;
    }
    
    /* set column address range */
    res = ssh1107_set_column_address_range(&ssh1107_handle, SSH1107_ADVANCE_DEFAULT_COLUMN_ADDRESS_RANGE_START, SSH1107_ADVANCE_DEFAULT_COLUMN_ADDRESS_RANGE_END);
    if (res != 0)
    {
        ssh1107_interface_debug_print("ssh1107: set column address range failed.\n");
        (void)ssh1107_deinit(&ssh1107_handle);
        
        return 1;
    }
    
    /* set page address range */
    res = ssh1107_set_page_address_range(&ssh1107_handle, SSH1107_ADVANCE_DEFAULT_PAGE_ADDRESS_RANGE_START, SSH1107_ADVANCE_DEFAULT_PAGE_ADDRESS_RANGE_END);
    if (res != 0)
    {
        ssh1107_interface_debug_print("ssh1107: set page address range failed.\n");
        (void)ssh1107_deinit(&ssh1107_handle);
        
        return 1;
    }
    
    /* set low column start address */
    res = ssh1107_set_low_column_start_address(&ssh1107_handle, SSH1107_ADVANCE_DEFAULT_LOW_COLUMN_START_ADDRESS);
    if (res != 0)
    {
        ssh1107_interface_debug_print("ssh1107: set low column start address failed.\n");
        (void)ssh1107_deinit(&ssh1107_handle);
        
        return 1;
    }
    
    /* set high column start address */
    res = ssh1107_set_high_column_start_address(&ssh1107_handle, SSH1107_ADVANCE_DEFAULT_HIGH_COLUMN_START_ADDRESS);
    if (res != 0)
    {
        ssh1107_interface_debug_print("ssh1107: set high column start address failed.\n");
        (void)ssh1107_deinit(&ssh1107_handle);
        
        return 1;
    }
    
    /* set display start line */
    res = ssh1107_set_display_start_line(&ssh1107_handle, SSH1107_ADVANCE_DEFAULT_DISPLAY_START_LINE);
    if (res != 0)
    {
        ssh1107_interface_debug_print("ssh1107: set display start line failed.\n");
        (void)ssh1107_deinit(&ssh1107_handle);
        
        return 1;
    }

    /* set contrast */
    res = ssh1107_set_contrast(&ssh1107_handle, SSH1107_ADVANCE_DEFAULT_CONTRAST);
    if (res != 0)
    {
        ssh1107_interface_debug_print("ssh1107: set contrast failed.\n");
        (void)ssh1107_deinit(&ssh1107_handle);
        
        return 1;
    }
    
    /* set segment remap */
    res = ssh1107_set_segment_remap(&ssh1107_handle, SSH1107_ADVANCE_DEFAULT_SEGMENT);
    if (res != 0)
    {
        ssh1107_interface_debug_print("ssh1107: set segment remap failed.\n");
        (void)ssh1107_deinit(&ssh1107_handle);
        
        return 1;
    }
    
    /* set scan direction */
    res = ssh1107_set_scan_direction(&ssh1107_handle, SSH1107_ADVANCE_DEFAULT_SCAN_DIRECTION);
    if (res != 0)
    {
        ssh1107_interface_debug_print("ssh1107: set scan direction failed.\n");
        (void)ssh1107_deinit(&ssh1107_handle);
        
        return 1;
    }
    
    /* set display mode */
    res = ssh1107_set_display_mode(&ssh1107_handle, SSH1107_ADVANCE_DEFAULT_DISPLAY_MODE);
    if (res != 0)
    {
        ssh1107_interface_debug_print("ssh1107: set display mode failed.\n");
        (void)ssh1107_deinit(&ssh1107_handle);
        
        return 1;
    }
    
    /* set multiplex ratio */
    res = ssh1107_set_multiplex_ratio(&ssh1107_handle, SSH1107_ADVANCE_DEFAULT_MULTIPLEX_RATIO);
    if (res != 0)
    {
        ssh1107_interface_debug_print("ssh1107: set multiplex ratio failed.\n");
        (void)ssh1107_deinit(&ssh1107_handle);
        
        return 1;
    }
    
    /* set display offset */
    res = ssh1107_set_display_offset(&ssh1107_handle, SSH1107_ADVANCE_DEFAULT_DISPLAY_OFFSET);
    if (res != 0)
    {
        ssh1107_interface_debug_print("ssh1107: set display offset failed.\n");
        (void)ssh1107_deinit(&ssh1107_handle);
        
        return 1;
    }
    
    /* set display clock */
    res = ssh1107_set_display_clock(&ssh1107_handle, SSH1107_ADVANCE_DEFAULT_OSCILLATOR_FREQUENCY, SSH1107_ADVANCE_DEFAULT_CLOCK_DIVIDE);
    if (res != 0)
    {
        ssh1107_interface_debug_print("ssh1107: set display clock failed.\n");
        (void)ssh1107_deinit(&ssh1107_handle);
        
        return 1;
    }

    /* set pre charge period */
    res = ssh1107_set_precharge_period(&ssh1107_handle, SSH1107_ADVANCE_DEFAULT_PHASE1_PERIOD, SSH1107_ADVANCE_DEFAULT_PHASE2_PERIOD);
    if (res != 0)
    {
        ssh1107_interface_debug_print("ssh1107: set pre charge period failed.\n");
        (void)ssh1107_deinit(&ssh1107_handle);

        return 1;
    }

    /* set deselect level 0.83 */
    res = ssh1107_set_deselect_level(&ssh1107_handle, SSH1107_ADVANCE_DEFAULT_DESELECT_LEVEL);
    if (res != 0)
    {
        ssh1107_interface_debug_print("ssh1107: set deselect level failed.\n");
        (void)ssh1107_deinit(&ssh1107_handle);
        
        return 1;
    }
    
    /* set page memory addressing mode */
    res = ssh1107_set_memory_addressing_mode(&ssh1107_handle, SSH1107_MEMORY_ADDRESSING_MODE_PAGE);
    if (res != 0)
    {
        ssh1107_interface_debug_print("ssh1107: set memory addressing level failed.\n");
        (void)ssh1107_deinit(&ssh1107_handle);
        
        return 1;
    }

    /* entire display off */
    res = ssh1107_set_entire_display(&ssh1107_handle, SSH1107_ENTIRE_DISPLAY_OFF);
    if (res != 0)
    {
        ssh1107_interface_debug_print("ssh1107: set entire display failed.\n");
        (void)ssh1107_deinit(&ssh1107_handle);
        
        return 1;
    }
    
    /* enable display */
    res = ssh1107_set_display(&ssh1107_handle, SSH1107_DISPLAY_ON);
    if (res != 0)
    {
        ssh1107_interface_debug_print("ssh1107: set display failed.\n");
        (void)ssh1107_deinit(&ssh1107_handle);
        
        return 1;
    }
    
    /* clear screen */
    res = ssh1107_clear(&ssh1107_handle);
    if (res != 0)
    {
        ssh1107_interface_debug_print("ssh1107: clear failed.\n");
        (void)ssh1107_deinit(&ssh1107_handle);
        
        return 1;
    }
    
    return 0;
}

/**
 * @brief  advance example deinit
 * @return status code
 *         - 0 success
 *         - 1 deinit failed
 * @note   none
 */
uint8_t ssh1107_advance_deinit(void)
{
    /* deinit ssh1107 */
    if (ssh1107_deinit(&ssh1107_handle) != 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/**
 * @brief  advance example display on
 * @return status code
 *         - 0 success
 *         - 1 display on failed
 * @note   none
 */
uint8_t ssh1107_advance_display_on(void)
{
    uint8_t res;
    
    /* display on */
    res = ssh1107_set_display(&ssh1107_handle, SSH1107_DISPLAY_ON);
    if (res != 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/**
 * @brief  advance example display off
 * @return status code
 *         - 0 success
 *         - 1 display off failed
 * @note   none
 */
uint8_t ssh1107_advance_display_off(void)
{
    uint8_t res;
    
    /* display off */
    res = ssh1107_set_display(&ssh1107_handle, SSH1107_DISPLAY_OFF);
    if (res != 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/**
 * @brief  advance example clear
 * @return status code
 *         - 0 success
 *         - 1 clear failed
 * @note   none
 */
uint8_t ssh1107_advance_clear(void)
{
    /* clear */
    if (ssh1107_clear(&ssh1107_handle) != 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/**
 * @brief     advance example write a point
 * @param[in] x coordinate x
 * @param[in] y coordinate y
 * @param[in] data written data
 * @return    status code
 *            - 0 success
 *            - 1 write point failed
 * @note      none
 */
uint8_t ssh1107_advance_write_point(uint8_t x, uint8_t y, uint8_t data)
{
    uint8_t res;
    
    /* write point */
    res = ssh1107_write_point(&ssh1107_handle, x, y, data);
    if (res != 0)
    {
        return 1;
    }
    
    return 0;
}

/**
 * @brief      advance example read a point
 * @param[in]  x coordinate x
 * @param[in]  y coordinate y
 * @param[out] *data pointer to a data buffer
 * @return     status code
 *             - 0 success
 *             - 1 read point failed
 * @note       none
 */
uint8_t ssh1107_advance_read_point(uint8_t x, uint8_t y, uint8_t *data)
{
    uint8_t res;
    
    /* read point in gram */
    res = ssh1107_read_point(&ssh1107_handle, x, y, data);
    if (res != 0)
    {
        return 1;
    }
    
    return 0;
}

/**
 * @brief     advance example draw a string
 * @param[in] x coordinate x
 * @param[in] y coordinate y
 * @param[in] *str pointer to a written string address
 * @param[in] len length of the string
 * @param[in] color display color
 * @param[in] font display font size
 * @return    status code
 *            - 0 success
 *            - 1 write string failed
 * @note      none
 */
uint8_t ssh1107_advance_string(uint8_t x, uint8_t y, char *str, uint16_t len, uint8_t color, font_t font)
{
    uint8_t res;
    
    /* write string in gram */
    res = ssh1107_gram_write_string(&ssh1107_handle, x, y, str, len, color, font);
    if (res != 0)
    {
        return 1;
    }
    
    /* update gram */
    if (ssh1107_gram_update(&ssh1107_handle) != 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/**
 * @brief     advance example fill a rectangle
 * @param[in] left left coordinate x
 * @param[in] top top coordinate y
 * @param[in] right right coordinate x
 * @param[in] bottom bottom coordinate y
 * @param[in] color display color
 * @return    status code
 *            - 0 success
 *            - 1 fill rect failed
 * @note      none
 */
uint8_t ssh1107_advance_rect(uint8_t left, uint8_t top, uint8_t right, uint8_t bottom, uint8_t color)
{
    uint8_t res;
    
    /* fill rect in gram */
    res = ssh1107_gram_fill_rect(&ssh1107_handle, left, top, right, bottom, color);
    if (res != 0)
    {
        return 1;
    }
    
    /* update gram */
    if (ssh1107_gram_update(&ssh1107_handle) != 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/**
 * @brief     advance example draw a picture
 * @param[in] left left coordinate x
 * @param[in] top top coordinate y
 * @param[in] right right coordinate x
 * @param[in] bottom bottom coordinate y
 * @param[in] *img pointer to a image buffer
 * @return    status code
 *            - 0 success
 *            - 1 draw picture failed
 * @note      none
 */
uint8_t ssh1107_advance_picture(uint8_t left, uint8_t top, uint8_t right, uint8_t bottom, uint8_t *img)
{
    uint8_t res;
    
    /* draw picture in gram */
    res = ssh1107_gram_draw_picture(&ssh1107_handle, left, top, right, bottom, img);
    if (res != 0)
    {
        return 1;
    }
    
    /* update gram */
    if (ssh1107_gram_update(&ssh1107_handle) != 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
