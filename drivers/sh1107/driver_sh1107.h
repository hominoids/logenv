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
 * @file      driver_sh1107.h
 * @brief     driver sh1107 header file
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

#ifndef DRIVER_SH1107_H
#define DRIVER_SH1107_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "../fonts/fonts.h"

#ifdef __cplusplus
extern "C"{
#endif

/**
 * @defgroup sh1107_driver sh1107 driver function
 * @brief    sh1107 driver modules
 * @{
 */

/**
 * @addtogroup sh1107_base_driver
 * @{
 */

/**
 * @brief sh1107 interface enumeration definition
 */
typedef enum
{
    SH1107_INTERFACE_IIC = 0x00,        /**< interface iic */
    SH1107_INTERFACE_SPI = 0x01,        /**< interface spi */
} sh1107_interface_t;

/**
 * @brief sh1107 address pin enumeration definition
 */
typedef enum  
{
    SH1107_ADDR_SA0_0 = 0x78,        /**< address pin GND */
    SH1107_ADDR_SA0_1 = 0x7A,        /**< address pin VCC */
} sh1107_address_t;

/**
 * @brief sh1107 memory addressing mode enumeration definition
 */
typedef enum  
{
    SH1107_MEMORY_ADDRESSING_MODE_PAGE       = 0x00,        /**< page addressing mode */
    SH1107_MEMORY_ADDRESSING_MODE_VERTICAL   = 0x01,        /**< vertical addressing mode */
} sh1107_memory_addressing_mode_t;


/**
 * @brief sh1107 segment column remap enumeration definition
 */
typedef enum  
{
    SH1107_SEGMENT_COLUMN_ADDRESS_0   = 0x00,        /**< segment column remap address 0 */
    SH1107_SEGMENT_COLUMN_ADDRESS_127 = 0x01,        /**< segment column remap address 127 */
} sh1107_segment_column_remap_t;

/**
 * @brief sh1107 entire display enumeration definition
 */
typedef enum  
{
    SH1107_ENTIRE_DISPLAY_OFF = 0x00,        /**< entire display off */
    SH1107_ENTIRE_DISPLAY_ON  = 0x01,        /**< entire display on */
} sh1107_entire_display_t;

/**
 * @brief sh1107 display mode enumeration definition
 */
typedef enum  
{
    SH1107_DISPLAY_MODE_NORMAL  = 0x00,        /**< display mode normal */
    SH1107_DISPLAY_MODE_INVERSE = 0x01,        /**< display mode inverse */
} sh1107_display_mode_t;

/**
 * @brief sh1107 display enumeration definition
 */
typedef enum  
{
    SH1107_DISPLAY_OFF = 0x00,        /**< close display */
    SH1107_DISPLAY_ON  = 0x01,        /**< open display */
} sh1107_display_t;

/**
 * @brief sh1107 scan direction enumeration definition
 */
typedef enum  
{
    SH1107_SCAN_DIRECTION_COM0_START   = 0x00,        /**< scan direction com 0 start */
    SH1107_SCAN_DIRECTION_COMN_1_START = 0x01,        /**< scan direction com N-1 start */
} sh1107_scan_direction_t;

/**
 * @brief sh1107 left right remap enumeration definition
 */
typedef enum  
{
    SH1107_LEFT_RIGHT_REMAP_DISABLE = 0x00,        /**< disable left right remap */
    SH1107_LEFT_RIGHT_REMAP_ENABLE  = 0x01,        /**< enable left right remap */
} sh1107_left_right_remap_t;

/**
 * @brief sh1107 deselect level enumeration definition
 */
typedef enum  
{
    SH1107_DESELECT_LEVEL_0P65 = 0x00,        /**< deselect level 0.65 */
    SH1107_DESELECT_LEVEL_0P77 = 0x02,        /**< deselect level 0.77 */
    SH1107_DESELECT_LEVEL_0P83 = 0x03,        /**< deselect level 0.83 */
} sh1107_deselect_level_t;

/**
 * @brief sh1107 handle structure definition
 */
typedef struct sh1107_handle_s
{
    uint8_t (*iic_init)(void);                                                          /**< point to an iic_init function address */
    uint8_t (*iic_deinit)(void);                                                        /**< point to an iic_deinit function address */
    uint8_t (*iic_write)(uint8_t addr, uint8_t reg, uint8_t *buf, uint16_t len);        /**< point to an iic_write function address */
    uint8_t (*spi_init)(void);                                                          /**< point to a spi_init function address */
    uint8_t (*spi_deinit)(void);                                                        /**< point to a spi_deinit function address */
    uint8_t (*spi_write_cmd)(uint8_t *buf, uint16_t len);                               /**< point to a spi_write_cmd function address */
    uint8_t (*spi_cmd_data_gpio_init)(void);                                            /**< point to a spi_cmd_data_gpio_init function address */
    uint8_t (*spi_cmd_data_gpio_deinit)(void);                                          /**< point to a spi_cmd_data_gpio_deinit function address */
    uint8_t (*spi_cmd_data_gpio_write)(uint8_t value);                                  /**< point to a spi_cmd_data_gpio_write function address */
    uint8_t (*reset_gpio_init)(void);                                                   /**< point to a reset_gpio_init function address */
    uint8_t (*reset_gpio_deinit)(void);                                                 /**< point to a reset_gpio_deinit function address */
    uint8_t (*reset_gpio_write)(uint8_t value);                                         /**< point to a reset_gpio_write function address */
    void (*debug_print)(const char *const fmt, ...);                                    /**< point to a debug_print function address */
    void (*delay_ms)(uint32_t ms);                                                      /**< point to a delay_ms function address */
    uint8_t inited;                                                                     /**< inited flag */
    uint8_t iic_addr;                                                                   /**< iic address */
    uint8_t iic_spi;                                                                    /**< iic spi type */
    uint8_t gram[128][16];                                                               /**< gram buffer */
} sh1107_handle_t;

/**
 * @brief sh1107 information structure definition
 */
typedef struct sh1107_info_s
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
} sh1107_info_t;

/**
 * @}
 */

/**
 * @defgroup sh1107_link_driver sh1107 link driver function
 * @brief    sh1107 link driver modules
 * @ingroup  sh1107_driver
 * @{
 */

/**
 * @brief     initialize sh1107_handle_t structure
 * @param[in] HANDLE pointer to an sh1107 handle structure
 * @param[in] STRUCTURE sh1107_handle_t
 * @note      none
 */
#define DRIVER_SH1107_LINK_INIT(HANDLE, STRUCTURE)                     memset(HANDLE, 0, sizeof(STRUCTURE))

/**
 * @brief     link iic_init function
 * @param[in] HANDLE pointer to an sh1107 handle structure
 * @param[in] FUC pointer to an iic_init function address
 * @note      none
 */
#define DRIVER_SH1107_LINK_IIC_INIT(HANDLE, FUC)                      (HANDLE)->iic_init = FUC

/**
 * @brief     link iic_deinit function
 * @param[in] HANDLE pointer to an sh1107 handle structure
 * @param[in] FUC pointer to an iic_deinit function address
 * @note      none
 */
#define DRIVER_SH1107_LINK_IIC_DEINIT(HANDLE, FUC)                    (HANDLE)->iic_deinit = FUC

/**
 * @brief     link iic_write function
 * @param[in] HANDLE pointer to an sh1107 handle structure
 * @param[in] FUC pointer to an iic_write function address
 * @note      none
 */
#define DRIVER_SH1107_LINK_IIC_WRITE(HANDLE, FUC)                     (HANDLE)->iic_write = FUC

/**
 * @brief     link spi_init function
 * @param[in] HANDLE pointer to an sh1107 handle structure
 * @param[in] FUC pointer to a spi_init function address
 * @note      none
 */
#define DRIVER_SH1107_LINK_SPI_INIT(HANDLE, FUC)                      (HANDLE)->spi_init = FUC

/**
 * @brief     link spi_deinit function
 * @param[in] HANDLE pointer to an sh1107 handle structure
 * @param[in] FUC pointer to a spi_deinit function address
 * @note      none
 */
#define DRIVER_SH1107_LINK_SPI_DEINIT(HANDLE, FUC)                    (HANDLE)->spi_deinit = FUC

/**
 * @brief     link spi_write_cmd function
 * @param[in] HANDLE pointer to an sh1107 handle structure
 * @param[in] FUC pointer to a spi_write_cmd function address
 * @note      none
 */
#define DRIVER_SH1107_LINK_SPI_WRITE_COMMAND(HANDLE, FUC)             (HANDLE)->spi_write_cmd = FUC

/**
 * @brief     link spi_cmd_data_gpio_init function
 * @param[in] HANDLE pointer to an sh1107 handle structure
 * @param[in] FUC pointer to a spi_cmd_data_gpio_init function address
 * @note      none
 */
#define DRIVER_SH1107_LINK_SPI_COMMAND_DATA_GPIO_INIT(HANDLE, FUC)    (HANDLE)->spi_cmd_data_gpio_init = FUC

/**
 * @brief     link spi_cmd_data_gpio_deinit function
 * @param[in] HANDLE pointer to an sh1107 handle structure
 * @param[in] FUC pointer to a spi_cmd_data_gpio_deinit function address
 * @note      none
 */
#define DRIVER_SH1107_LINK_SPI_COMMAND_DATA_GPIO_DEINIT(HANDLE, FUC)  (HANDLE)->spi_cmd_data_gpio_deinit = FUC

/**
 * @brief     link spi_cmd_data_gpio_write function
 * @param[in] HANDLE pointer to an sh1107 handle structure
 * @param[in] FUC pointer to a spi_cmd_data_gpio_write function address
 * @note      none
 */
#define DRIVER_SH1107_LINK_SPI_COMMAND_DATA_GPIO_WRITE(HANDLE, FUC)   (HANDLE)->spi_cmd_data_gpio_write = FUC

/**
 * @brief     link reset_gpio_init function
 * @param[in] HANDLE pointer to an sh1107 handle structure
 * @param[in] FUC pointer to a reset_gpio_init function address
 * @note      none
 */
#define DRIVER_SH1107_LINK_RESET_GPIO_INIT(HANDLE, FUC)               (HANDLE)->reset_gpio_init = FUC

/**
 * @brief     link reset_gpio_deinit function
 * @param[in] HANDLE pointer to an sh1107 handle structure
 * @param[in] FUC pointer to a reset_gpio_deinit function address
 * @note      none
 */
#define DRIVER_SH1107_LINK_RESET_GPIO_DEINIT(HANDLE, FUC)             (HANDLE)->reset_gpio_deinit = FUC

/**
 * @brief     link reset_gpio_write function
 * @param[in] HANDLE pointer to an sh1107 handle structure
 * @param[in] FUC pointer to a reset_gpio_write function address
 * @note      none
 */
#define DRIVER_SH1107_LINK_RESET_GPIO_WRITE(HANDLE, FUC)              (HANDLE)->reset_gpio_write = FUC

/**
 * @brief     link delay_ms function
 * @param[in] HANDLE pointer to an sh1107 handle structure
 * @param[in] FUC pointer to a delay_ms function address
 * @note      none
 */
#define DRIVER_SH1107_LINK_DELAY_MS(HANDLE, FUC)                      (HANDLE)->delay_ms = FUC

/**
 * @brief     link debug_print function
 * @param[in] HANDLE pointer to an sh1107 handle structure
 * @param[in] FUC pointer to a debug_print function address
 * @note      none
 */
#define DRIVER_SH1107_LINK_DEBUG_PRINT(HANDLE, FUC)                   (HANDLE)->debug_print = FUC

/**
 * @}
 */

/**
 * @defgroup sh1107_base_driver sh1107 base driver function
 * @brief    sh1107 base driver modules
 * @ingroup  sh1107_driver
 * @{
 */

/**
 * @brief      get chip's information
 * @param[out] *info pointer to an sh1107 info structure
 * @return     status code
 *             - 0 success
 *             - 2 handle is NULL
 * @note       none
 */
uint8_t sh1107_info(sh1107_info_t *info);

/**
 * @brief     set the chip interface
 * @param[in] *handle pointer to an sh1107 handle structure
 * @param[in] interface chip interface
 * @return    status code
 *            - 0 success
 *            - 2 handle is NULL
 * @note      none
 */
uint8_t sh1107_set_interface(sh1107_handle_t *handle, sh1107_interface_t interface);

/**
 * @brief      get the chip interface
 * @param[in]  *handle pointer to an sh1107 handle structure
 * @param[out] *interface pointer to a chip interface buffer
 * @return     status code
 *             - 0 success
 *             - 2 handle is NULL
 * @note       none
 */
uint8_t sh1107_get_interface(sh1107_handle_t *handle, sh1107_interface_t *interface);

/**
 * @brief     set the chip iic address
 * @param[in] *handle pointer to an sh1107 handle structure
 * @param[in] addr_pin iic address
 * @return    status code
 *            - 0 success
 *            - 2 handle is NULL
 * @note      none
 */
uint8_t sh1107_set_addr_pin(sh1107_handle_t *handle, sh1107_address_t addr_pin);

/**
 * @brief      get the chip iic address
 * @param[in]  *handle pointer to an sh1107 handle structure
 * @param[out] *addr_pin pointer to an iic address buffer
 * @return     status code
 *             - 0 success
 *             - 2 handle is NULL
 * @note       none
 */
uint8_t sh1107_get_addr_pin(sh1107_handle_t *handle, sh1107_address_t *addr_pin);

/**
 * @brief     initialize the chip
 * @param[in] *handle pointer to an sh1107 handle structure
 * @return    status code
 *            - 0 success
 *            - 1 iic or spi initialization failed
 *            - 2 handle is NULL
 *            - 3 linked functions is NULL
 *            - 4 reset failed
 *            - 5 command && data init failed
 *            - 6 interface param is invalid
 * @note      none
 */
uint8_t sh1107_init(sh1107_handle_t *handle);

/**
 * @brief     close the chip
 * @param[in] *handle pointer to an sh1107 handle structure
 * @return    status code
 *            - 0 success
 *            - 1 iic or spi deinit failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 power down failed
 *            - 5 reset gpio deinit failed
 *            - 6 command && data deinit failed
 *            - 7 interface param is invalid
 * @note      none
 */
uint8_t sh1107_deinit(sh1107_handle_t *handle);

/**
 * @brief     clear the screen in the gram
 * @param[in] *handle pointer to an sh1107 handle structure
 * @param[in] color color type
 * @return    status code
 *            - 0 success
 *            - 1 clear failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t sh1107_gram_clear(sh1107_handle_t *handle);

/**
 * @brief     clear the screen
 * @param[in] *handle pointer to an sh1107 handle structure
 * @return    status code
 *            - 0 success
 *            - 1 clear failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t sh1107_clear(sh1107_handle_t *handle);

/**
 * @brief     update the gram data
 * @param[in] *handle pointer to an sh1107 handle structure
 * @return    status code
 *            - 0 success
 *            - 1 gram update failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t sh1107_gram_update(sh1107_handle_t *handle);

/**
 * @brief     write a point
 * @param[in] *handle pointer to an sh1107 handle structure
 * @param[in] x coordinate x
 * @param[in] y coordinate y
 * @param[in] data written data
 * @return    status code
 *            - 0 success
 *            - 1 write point failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 x or y is invalid
 * @note      none
 */
uint8_t sh1107_write_point(sh1107_handle_t *handle, uint8_t x, uint8_t y, uint8_t data);

/**
 * @brief      read a point
 * @param[in]  *handle pointer to an sh1107 handle structure
 * @param[in]  x coordinate x
 * @param[in]  y coordinate y
 * @param[out] *data pointer to a data buffer
 * @return     status code
 *             - 0 success
 *             - 1 read point failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 x or y is invalid
 * @note       none
 */
uint8_t sh1107_read_point(sh1107_handle_t *handle, uint8_t x, uint8_t y, uint8_t *data);

/**
 * @brief     write a point in the gram
 * @param[in] *handle pointer to an sh1107 handle structure
 * @param[in] x coordinate x
 * @param[in] y coordinate y
 * @param[in] data written data
 * @return    status code
 *            - 0 success
 *            - 1 gram write point failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 x or y is invalid
 * @note      none
 */
uint8_t sh1107_gram_write_point(sh1107_handle_t *handle, uint8_t x, uint8_t y, uint8_t data);

/**
 * @brief      read a point from the gram
 * @param[in]  *handle pointer to an sh1107 handle structure
 * @param[in]  x coordinate x
 * @param[in]  y coordinate y
 * @param[out] *data pointer to a data buffer
 * @return     status code
 *             - 0 success
 *             - 1 gram read point failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 x or y is invalid
 * @note       none
 */
uint8_t sh1107_gram_read_point(sh1107_handle_t *handle, uint8_t x, uint8_t y, uint8_t *data);

/**
 * @brief     draw a string in the gram
 * @param[in] *handle pointer to an sh1107 handle structure
 * @param[in] x coordinate x
 * @param[in] y coordinate y
 * @param[in] *str pointer to a write string address
 * @param[in] len length of the string
 * @param[in] color display color
 * @param[in] font display font size
 * @return    status code
 *            - 0 success
 *            - 1 gram write string failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 x or y is invalid
 * @note      none
 */
uint8_t sh1107_gram_write_string(sh1107_handle_t *handle, uint8_t x, uint8_t y, char *str, uint16_t len, uint8_t color, font_t font);

/**
 * @brief     fill a rectangle in the gram
 * @param[in] *handle pointer to an sh1107 handle structure
 * @param[in] left left coordinate x
 * @param[in] top top coordinate y
 * @param[in] right right coordinate x
 * @param[in] bottom bottom coordinate y
 * @param[in] color display color
 * @return    status code
 *            - 0 success
 *            - 1 gram fill rect failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 left or top is invalid
 *            - 5 right or bottom is invalid
 *            - 6 left > right or top > bottom
 * @note      none
 */
uint8_t sh1107_gram_fill_rect(sh1107_handle_t *handle, uint8_t left, uint8_t top, uint8_t right, uint8_t bottom, uint8_t color);

/**
 * @brief     draw a picture in the gram
 * @param[in] *handle pointer to an sh1107 handle structure
 * @param[in] left left coordinate x
 * @param[in] top top coordinate y
 * @param[in] right right coordinate x
 * @param[in] bottom bottom coordinate y
 * @param[in] *img pointer to an image buffer
 * @return    status code
 *            - 0 success
 *            - 1 gram draw picture failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 left or top is invalid
 *            - 5 right or bottom is invalid
 *            - 6 left > right or top > bottom
 * @note      none
 */
uint8_t sh1107_gram_draw_picture(sh1107_handle_t *handle, uint8_t left, uint8_t top, uint8_t right, uint8_t bottom, uint8_t *img);

/**
 * @brief     set the low column start address
 * @param[in] *handle pointer to an sh1107 handle structure
 * @param[in] addr low column start address
 * @return    status code
 *            - 0 success
 *            - 1 set low column start address failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 addr is invalid
 * @note      addr <= 0xF
 */
uint8_t sh1107_set_low_column_start_address(sh1107_handle_t *handle, uint8_t addr);

/**
 * @brief     set the high column start address
 * @param[in] *handle pointer to an sh1107 handle structure
 * @param[in] addr high column start address
 * @return    status code
 *            - 0 success
 *            - 1 set high column start address failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 addr is invalid
 * @note      addr <= 0xF
 */
uint8_t sh1107_set_high_column_start_address(sh1107_handle_t *handle, uint8_t addr);

/**
 * @brief     set the memory addressing mode
 * @param[in] *handle pointer to an sh1107 handle structure
 * @param[in] mode memory addressing mode
 * @return    status code
 *            - 0 success
 *            - 1 set memory addressing mode failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t sh1107_set_memory_addressing_mode(sh1107_handle_t *handle, sh1107_memory_addressing_mode_t mode);

/**
 * @brief     set the column address range
 * @param[in] *handle pointer to an sh1107 handle structure
 * @param[in] start_addr column start address
 * @param[in] end_addr column end address
 * @return    status code
 *            - 0 success
 *            - 1 set column address range failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 start addr is invalid
 *            - 5 end addr is invalid
 * @note      start addr and end addr can't be over 0x7F
 */
uint8_t sh1107_set_column_address_range(sh1107_handle_t *handle, uint8_t start_addr, uint8_t end_addr);

/**
 * @brief     set the page address range
 * @param[in] *handle pointer to an sh1107 handle structure
 * @param[in] start_addr page start address
 * @param[in] end_addr page end address
 * @return    status code
 *            - 0 success
 *            - 1 set page address range failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 start addr is invalid
 *            - 5 end addr is invalid
 * @note      start addr and end addr can't be over 0x0F
 */
uint8_t sh1107_set_page_address_range(sh1107_handle_t *handle, uint8_t start_addr, uint8_t end_addr);

/**
 * @brief     set the display start line
 * @param[in] *handle pointer to an sh1107 handle structure
 * @param[in] l start line
 * @return    status code
 *            - 0 success
 *            - 1 set display start line failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 line is invalid
 * @note      line <= 0x7F
 */
uint8_t sh1107_set_display_start_line(sh1107_handle_t *handle, uint8_t l);

/**
 * @brief     set the display contrast
 * @param[in] *handle pointer to an sh1107 handle structure
 * @param[in] contrast display contrast
 * @return    status code
 *            - 0 success
 *            - 1 set contrast failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t sh1107_set_contrast(sh1107_handle_t *handle, uint8_t contrast);

/**
 * @brief     set the segment remap
 * @param[in] *handle pointer to an sh1107 handle structure
 * @param[in] remap segment remap param
 * @return    status code
 *            - 0 success
 *            - 1 set segment remap failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t sh1107_set_segment_remap(sh1107_handle_t *handle, uint8_t remap);

/**
 * @brief     enable or disable the entire display
 * @param[in] *handle pointer to an sh1107 handle structure
 * @param[in] enable bool value
 * @return    status code
 *            - 0 success
 *            - 1 set entire display failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t sh1107_set_entire_display(sh1107_handle_t *handle, sh1107_entire_display_t enable);

/**
 * @brief     set the display mode
 * @param[in] *handle pointer to an sh1107 handle structure
 * @param[in] mode display mode
 * @return    status code
 *            - 0 success
 *            - 1 set display mode failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t sh1107_set_display_mode(sh1107_handle_t *handle, sh1107_display_mode_t mode);

/**
 * @brief     set the multiplex ratio
 * @param[in] *handle pointer to an sh1107 handle structure
 * @param[in] multiplex multiplex ratio
 * @return    status code
 *            - 0 success
 *            - 1 set multiplex ratio failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 multiplex is too small
 *            - 5 multiplex is too large
 * @note      multiplex must be over 0x0E and less than 0x40
 */
uint8_t sh1107_set_multiplex_ratio(sh1107_handle_t *handle, uint8_t multiplex);

/**
 * @brief     enable or disable the display
 * @param[in] *handle pointer to an sh1107 handle structure
 * @param[in] on_off bool value
 * @return    status code
 *            - 0 success
 *            - 1 set display failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t sh1107_set_display(sh1107_handle_t *handle, sh1107_display_t on_off);

/**
 * @brief     set the page address
 * @param[in] *handle pointer to an sh1107 handle structure
 * @param[in] addr page address
 * @return    status code
 *            - 0 success
 *            - 1 set page address failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 addr is invalid
 * @note      addr <= 0x0F
 */
uint8_t sh1107_set_page_address(sh1107_handle_t *handle, uint8_t addr);

/**
 * @brief     set the scan direction
 * @param[in] *handle pointer to an sh1107 handle structure
 * @param[in] dir scan direction
 * @return    status code
 *            - 0 success
 *            - 1 set scan direction failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t sh1107_set_scan_direction(sh1107_handle_t *handle, uint8_t dir);

/**
 * @brief     set the display offset
 * @param[in] *handle pointer to an sh1107 handle structure
 * @param[in] offset display offset
 * @return    status code
 *            - 0 success
 *            - 1 set display offset failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 offset is invalid
 * @note      offset <= 0x7F
 */
uint8_t sh1107_set_display_offset(sh1107_handle_t *handle, uint8_t offset);

/**
 * @brief     set the display clock
 * @param[in] *handle pointer to an sh1107 handle structure
 * @param[in] oscillator_frequency oscillator frequency
 * @param[in] clock_divide clock divide
 * @return    status code
 *            - 0 success
 *            - 1 set display clock failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 oscillator frequency is invalid
 *            - 5 clock divide is invalid
 * @note      oscillator_frequency <= 0x0F, clock_divide <= 0x0F
 */
uint8_t sh1107_set_display_clock(sh1107_handle_t *handle, uint8_t oscillator_frequency, uint8_t clock_divide);

/**
 * @brief     set the pre charge period
 * @param[in] *handle pointer to an sh1107 handle structure
 * @param[in] phase1_period phase1 period
 * @param[in] phase2_period phase2 period
 * @return    status code
 *            - 0 success
 *            - 1 set pre charge period failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 phase1 period is invalid
 *            - 5 phase2 period is invalid
 * @note      phase1_period <= 0x0F, phase2_period <= 0x0F
 */
uint8_t sh1107_set_precharge_period(sh1107_handle_t *handle, uint8_t phase1_period, uint8_t phase2_period);

/**
 * @brief     set the deselect level
 * @param[in] *handle pointer to an sh1107 handle structure
 * @param[in] level deselect level
 * @return    status code
 *            - 0 success
 *            - 1 set deselect level failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t sh1107_set_deselect_level(sh1107_handle_t *handle, sh1107_deselect_level_t level);

/**
 * @}
 */

/**
 * @defgroup sh1107_extend_driver sh1107 extend driver function
 * @brief    sh1107 extend driver modules
 * @ingroup  sh1107_driver
 * @{
 */

/**
 * @brief     write the register command
 * @param[in] *handle pointer to an sh1107 handle structure
 * @param[in] *buf pointer to a data buffer
 * @param[in] len data length
 * @return    status code
 *            - 0 success
 *            - 1 write failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t sh1107_write_cmd(sh1107_handle_t *handle, uint8_t *buf, uint8_t len);

/**
 * @brief     write the register data
 * @param[in] *handle pointer to an sh1107 handle structure
 * @param[in] *buf pointer to a data buffer
 * @param[in] len data length
 * @return    status code
 *            - 0 success
 *            - 1 write failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t sh1107_write_data(sh1107_handle_t *handle, uint8_t *buf, uint8_t len);

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
