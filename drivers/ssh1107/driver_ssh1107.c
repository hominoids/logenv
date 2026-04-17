/**
 * Copyright (c) 2026 - present LibDriver All rights reserved
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
 * @file      driver_ssh1107.c
 * @brief     driver ssh1107 source file
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

#include "../fonts/monospace_font.h"
#include "driver_ssh1107.h"

/**
 * @brief chip information definition
 */
#define CHIP_NAME                 "Sino Wealth SSH1107"            /**< chip name */
#define MANUFACTURER_NAME         "Sino Wealth"                    /**< manufacturer name */
#define SUPPLY_VOLTAGE_MIN        1.65f                            /**< chip min supply voltage */
#define SUPPLY_VOLTAGE_MAX        3.5f                             /**< chip max supply voltage */
#define MAX_CURRENT               0.78f                            /**< chip max current */
#define TEMPERATURE_MIN           -40.0f                           /**< chip min operating temperature */
#define TEMPERATURE_MAX           85.0f                            /**< chip max operating temperature */
#define DRIVER_VERSION            1000                             /**< driver version */

/**
 * @brief chip command data definition
 */
#define SSH1107_CMD          0        /**< command */
#define SSH1107_DATA         1        /**< data */

/**
 * @brief chip command definition
 */
#define SSH1107_CMD_LOWER_COLUMN_START_ADDRESS              0x00        /**< command lower column start address */
#define SSH1107_CMD_HIGHER_COLUMN_START_ADDRESS             0x10        /**< command higher column start address */
#define SSH1107_CMD_MEMORY_ADDRESSING_MODE                  0x20        /**< command memory addressing mode */
#define SSH1107_CMD_SET_COLUMN_ADDRESS                      0x21        /**< command set column address */
#define SSH1107_CMD_SET_PAGE_ADDRESS                        0x22        /**< command set page address */
#define SSH1107_CMD_CONTRAST_CONTROL                        0x81        /**< command contrast control */
#define SSH1107_CMD_DCDC_ONOFF_MODE                         0x8A        /**< command DC-DC on off mode */
#define SSH1107_CMD_COLUMN_0_MAPPED_TO_SEG0                 0xA0        /**< command column 0 mapped to seg 0 */
#define SSH1107_CMD_COLUMN_127_MAPPED_TO_SEG0               0xA1        /**< command column 127 mapped to seg 0 */
#define SSH1107_CMD_ENTIRE_DISPLAY_OFF                      0xA4        /**< command entire display off */
#define SSH1107_CMD_ENTIRE_DISPLAY_ON                       0xA5        /**< command entire display on */
#define SSH1107_CMD_NORMAL_DISPLAY                          0xA6        /**< command normal display */
#define SSH1107_CMD_INVERSE_DISPLAY                         0xA7        /**< command inverse display */
#define SSH1107_CMD_MULTIPLEX_RATIO                         0xA8        /**< command multiplex ratio */
#define SSH1107_CMD_DCDC_CONTROL                            0xAD        /**< command DC-DC control */
#define SSH1107_CMD_DISPLAY_OFF                             0xAE        /**< command display off */
#define SSH1107_CMD_DISPLAY_ON                              0xAF        /**< command display on */
#define SSH1107_CMD_PAGE_ADDR                               0xB0        /**< command page address */
#define SSH1107_CMD_SCAN_DIRECTION_COM0_START               0xC0        /**< command scan direction com 0 start */
#define SSH1107_CMD_SCAN_DIRECTION_COMN_1_START             0xC8        /**< command scan direction com n-1 start */
#define SSH1107_CMD_DISPLAY_OFFSET                          0xD3        /**< command display offset */
#define SSH1107_CMD_DISPLAY_CLOCK_DIVIDE                    0xD5        /**< command display clock divide */
#define SSH1107_CMD_PRE_CHARGE_PERIOD                       0xD9        /**< command pre charge period */
#define SSH1107_CMD_COMH_DESLECT_LEVEL                      0xDB        /**< command comh deslect level */
#define SSH1107_CMD_DISPLAY_START_LINE                      0xDC        /**< command display start line */
#define SSH1107_CMD_NOP                                     0xE3        /**< command nop */

/**
 * @brief     write one byte
 * @param[in] *handle pointer to an ssh1107 handle structure
 * @param[in] data written data
 * @param[in] cmd command or data type
 * @return    status code
 *            - 0 success
 *            - 1 write failed
 * @note      none
 */
static uint8_t a_ssh1107_write_byte(ssh1107_handle_t *handle, uint8_t data, uint8_t cmd)
{
    uint8_t res;
    
    if (handle->iic_spi == SSH1107_INTERFACE_IIC)                              /* if iic */
    {
        if (cmd != 0)                                                          /* if data */
        {
            if (handle->iic_write(handle->iic_addr, 0x40, &data, 1) != 0)      /* write data */
            {
                return 1;                                                      /* return error */
            }
            else
            {
                return 0;                                                      /* success return 0 */
            }
        }
        else
        {
            if (handle->iic_write(handle->iic_addr, 0x00, &data, 1) != 0)      /* write command */
            {
                return 1;                                                      /* return error */
            }
            else
            {
                return 0;                                                      /* success return 0 */
            }
        }
    }
    else if (handle->iic_spi == SSH1107_INTERFACE_SPI)                         /* if spi */
    {
        res = handle->spi_cmd_data_gpio_write(cmd);                            /* write data command */
        if (res != 0)                                                          /* check error */
        {
            return 1;                                                          /* return error */
        }
        
        if (handle->spi_write_cmd(&data, 1) != 0)                              /* write command */
        {
            return 1;                                                          /* return error */
        }
        else
        {
            return 0;                                                          /* success return 0 */
        }
    }
    else
    {
        return 1;                                                              /* return error */
    }
}

/**
 * @brief     write multiple bytes
 * @param[in] *handle pointer to an ssh1107 handle structure
 * @param[in] *data pointer to a data buffer
 * @param[in] len data length
 * @param[in] cmd command or data type
 * @return    status code
 *            - 0 success
 *            - 1 write failed
 * @note      none
 */
static uint8_t a_ssh1107_multiple_write_byte(ssh1107_handle_t *handle, uint8_t *data, uint8_t len, uint8_t cmd)
{    
    uint8_t res;
    
    if (handle->iic_spi == SSH1107_INTERFACE_IIC)                               /* if iic */
    {
        if (cmd != 0)                                                           /* if data */
        {
            if (handle->iic_write(handle->iic_addr, 0x40, data, len) != 0)      /* write data */
            {
                return 1;                                                       /* return error */
            }
            else
            {
                return 0;                                                       /* success return 0 */
            }
        }
        else
        {
            if (handle->iic_write(handle->iic_addr, 0x00, data, len) != 0)      /* write command */
            {
                return 1;                                                       /* return error */
            }
            else
            {
                return 0;                                                       /* success return 0 */
            }
        }
    }
    else if (handle->iic_spi == SSH1107_INTERFACE_SPI)                          /* if spi */
    {
        res = handle->spi_cmd_data_gpio_write(cmd);                             /* write data command */
        if (res != 0)                                                           /* check error */
        {
            return 1;                                                           /* return error */
        }
        
        if (handle->spi_write_cmd(data, len) != 0)                              /* write command */
        {
            return 1;                                                           /* return error */
        }
        else
        {
            return 0;                                                           /* success return 0 */
        }
    }
    else
    {
        return 1;                                                               /* return error */
    }
}

/**
 * @brief     draw a point in gram
 * @param[in] *handle pointer to an ssh1107 handle structure
 * @param[in] x coordinate x
 * @param[in] y coordinate y
 * @param[in] data written data
 * @return    status code
 *            - 0 success
 *            - 1 gram draw point failed
 * @note      none
 */
static uint8_t a_ssh1107_gram_draw_point(ssh1107_handle_t *handle, uint8_t x, uint8_t y, uint8_t data)
{
    uint8_t pos;
    uint8_t bx;
    uint8_t temp = 0;
    
    pos = y / 8;                              /* get y page */
    bx = y % 8;                               /* get y point */
    temp = 1 << bx;                           /* set data */
    if (data != 0)                            /* if 1  */
    {
        handle->gram[x][pos] |= temp;         /* set 1 */
    }
    else
    {
        handle->gram[x][pos] &= ~temp;        /* set 0 */
    }
  
    return 0;                                 /* success return 0 */
}

/**
 * @brief     draw a char in gram
 * @param[in] *handle pointer to an ssh1107 handle structure
 * @param[in] x coordinate x
 * @param[in] y coordinate y
 * @param[in] chr written char
 * @param[in] size char size
 * @param[in] mode display mode
 * @return    status code
 *            - 0 success
 *            - 1 gram show char failed
 * @note      none
 */
static uint8_t a_ssh1107_gram_show_char(ssh1107_handle_t *handle, uint8_t x, uint8_t y, uint8_t chr, uint8_t size, uint8_t mode)
{
    uint8_t temp, t, t1;
    uint8_t y0 = y;
    uint8_t csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2);                 /* get size */
    
    chr = chr - ' ';                                                                /* get index */
    for (t = 0; t < csize; t++)                                                     /* write size */
    {   
        if (size == 12)                                                             /* if size 12 */
        {
            temp = monospace_12[chr][t];                                  /* get ascii 1206 */
        }
        else if (size == 16)                                                        /* if size 16 */
        {
            temp = monospace_16[chr][t];                                  /* get ascii 1608 */
        }
        else if(size == 24)                                                         /* if size 24 */
        {
            temp = monospace_24[chr][t];                                  /* get ascii 2412 */
        }
        else if(size == 32)                                                         /* if size 28 */
        {
            temp = monospace_28[chr][t];
        }
        else if(size == 40)                                                         /* if size 36 */
        {
            temp = monospace_36[chr][t];
        }
        else if(size == 50)                                                         /* if size 48 */
        {
            temp = monospace_48[chr][t];
        }
        else if(size == 74)                                                         /* if size 72 */
        {
            temp = monospace_72[chr][t];
        }
        else
        {
            return 1;                                                               /* return error */
        }
        for (t1 = 0; t1 < 8; t1++)                                                  /* write one line */
        {
            if ((temp & 0x80) != 0)                                                 /* if 1 */
            {
                if (a_ssh1107_gram_draw_point(handle, x, y, mode) != 0)             /* draw point */
                {
                    return 1;                                                       /* return error */
                }
            }
            else 
            {
                if (a_ssh1107_gram_draw_point(handle, x, y, !mode) != 0)            /* draw point */
                {
                    return 1;                                                       /* return error */
                }
            }
            temp <<= 1;                                                             /* left shift 1 */
            y++;
            if ((y - y0) == size)                                                   /* reset size */
            {
                y = y0;                                                             /* set y */
                x++;                                                                /* x++ */
                
                break;                                                              /* break */
            }
        }
    }
  
    return 0;                                                                       /* success return 0 */
}

/**
 * @brief     clear the screen
 * @param[in] *handle pointer to an ssh1107 handle structure
 * @return    status code
 *            - 0 success
 *            - 1 clear failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t ssh1107_clear(ssh1107_handle_t *handle)
{
    uint8_t i;
    uint8_t n;
    
    if (handle == NULL)                                                                               /* check handle */
    {
        return 2;                                                                                     /* return error */
    }
    if (handle->inited != 1)                                                                          /* check handle initialization */
    {
        return 3;                                                                                     /* return error */
    }
    
    for (i = 0; i < 16; i++)                                                                           /* write 8 page */
    {  
        if (a_ssh1107_write_byte(handle, SSH1107_CMD_PAGE_ADDR+i, SSH1107_CMD) != 0)                  /* set page */
        {
            handle->debug_print("ssh1107: write byte failed.\n");                                     /* write byte failed */
            
            return 1;                                                                                 /* return error */
        }
        if (a_ssh1107_write_byte(handle, SSH1107_CMD_LOWER_COLUMN_START_ADDRESS, SSH1107_CMD) != 0)   /* set lower column 0 */
        {
            handle->debug_print("ssh1107: write byte failed.\n");                                     /* write byte failed */
            
            return 1;                                                                                 /* return error */
        }
        if (a_ssh1107_write_byte(handle, SSH1107_CMD_HIGHER_COLUMN_START_ADDRESS, SSH1107_CMD) != 0)  /* set higher column 0 */
        {
            handle->debug_print("ssh1107: write byte failed.\n");                                     /* write byte failed */
            
            return 1;                                                                                 /* return error */
        }
        for (n = 0; n < 128; n++)                                                                     /* write 128 */
        {
            handle->gram[n][i] = 0x00;                                                                /* set black */
            if (a_ssh1107_write_byte(handle, handle->gram[n][i], SSH1107_DATA) != 0)                  /* write data */
            {
                handle->debug_print("ssh1107: write byte failed.\n");                                 /* write byte failed */
                
                return 1;                                                                             /* return error */
            }
        }
    }
    
    return 0;                                                                                         /* success return 0 */
}

/**
 * @brief     update the gram data
 * @param[in] *handle pointer to an ssh1107 handle structure
 * @return    status code
 *            - 0 success
 *            - 1 gram update failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t ssh1107_gram_update(ssh1107_handle_t *handle)
{
    uint8_t i;
    uint8_t n;
    
    if (handle == NULL)                                                                               /* check handle */
    {
        return 2;                                                                                     /* return error */
    }
    if (handle->inited != 1)                                                                          /* check handle initialization */
    {
        return 3;                                                                                     /* return error */
    }
    
    for (i = 0; i < 16; i++)                                                                           /* write 8 page */
    {  
        if (a_ssh1107_write_byte(handle, SSH1107_CMD_PAGE_ADDR+i, SSH1107_CMD) != 0)                  /* set page */
        {
            handle->debug_print("ssh1107: write byte failed.\n");                                     /* write byte failed */
            
            return 1;                                                                                 /* return error */
        }
        if (a_ssh1107_write_byte(handle, SSH1107_CMD_LOWER_COLUMN_START_ADDRESS, SSH1107_CMD) != 0)   /* set lower column 0 */
        {
            handle->debug_print("ssh1107: write byte failed.\n");                                     /* write byte failed */
            
            return 1;                                                                                 /* return error */
        }
        if (a_ssh1107_write_byte(handle, SSH1107_CMD_HIGHER_COLUMN_START_ADDRESS, SSH1107_CMD) != 0)  /* set higher column 0 */
        {
            handle->debug_print("ssh1107: write byte failed.\n");                                     /* write byte failed */
            
            return 1;                                                                                 /* return error */
        }
        for (n = 0; n < 128; n++)                                                                     /* write 128 */
        {
            if (a_ssh1107_write_byte(handle, handle->gram[n][i], SSH1107_DATA) != 0)                  /* write data */
            {
                handle->debug_print("ssh1107: write byte failed.\n");                                 /* write byte failed */
                
                return 1;                                                                             /* return error */
            }
        }
    }
    
    return 0;                                                                                         /* success return 0 */
}

/**
 * @brief     write a point
 * @param[in] *handle pointer to an ssh1107 handle structure
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
uint8_t ssh1107_write_point(ssh1107_handle_t *handle, uint8_t x, uint8_t y, uint8_t data)
{
    uint8_t pos;
    uint8_t bx;
    uint8_t temp = 0;
    
    if (handle == NULL)                                                                                        /* check handle */
    {
        return 2;                                                                                              /* return error */
    }
    if (handle->inited != 1)                                                                                   /* check handle initialization */
    {
        return 3;                                                                                              /* return error */
    }
    if ((x > 127) || (y > 127))                                                                                 /* check x, y */
    {
        handle->debug_print("ssh1107: x or y is invalid.\n");                                                  /* x or y is invalid */
        
        return 4;                                                                                              /* return error */
    }
    
    pos = y / 8;                                                                                               /* get y page */
    bx = y % 8;                                                                                                /* get y point */
    temp = 1 << bx;                                                                                            /* set data */
    if (data != 0)                                                                                             /* check the data */
    {
        handle->gram[x][pos] |= temp;                                                                          /* set 1 */
    }
    else
    {
        handle->gram[x][pos] &= ~temp;                                                                         /* set 0 */
    }
    if (a_ssh1107_write_byte(handle, SSH1107_CMD_PAGE_ADDR + pos, SSH1107_CMD) != 0)                           /* write page addr */
    {
        handle->debug_print("ssh1107: write byte failed.\n");                                                  /* write byte failed */
        
        return 1;                                                                                              /* return error */
    }
    if (a_ssh1107_write_byte(handle, SSH1107_CMD_LOWER_COLUMN_START_ADDRESS|(x&0x0F), SSH1107_CMD) != 0)       /* write lower column */
    {
        handle->debug_print("ssh1107: write byte failed.\n");                                                  /* write byte failed */
        
        return 1;                                                                                              /* return error */
    }
    if (a_ssh1107_write_byte(handle, SSH1107_CMD_HIGHER_COLUMN_START_ADDRESS|((x>4)&0x0F), SSH1107_CMD) != 0)  /* write higher column */
    {
        handle->debug_print("ssh1107: write byte failed.\n");                                                  /* write byte failed */
        
        return 1;                                                                                              /* return error */
    }
    if (a_ssh1107_write_byte(handle, handle->gram[x][pos], SSH1107_DATA) != 0)                                 /* write data */
    {
        handle->debug_print("ssh1107: write byte failed.\n");                                                  /* write byte failed */
        
        return 1;                                                                                              /* return error */
    }
    else
    {
       return 0;                                                                                               /* success return 0 */
    }
}

/**
 * @brief      read a point
 * @param[in]  *handle pointer to an ssh1107 handle structure
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
uint8_t ssh1107_read_point(ssh1107_handle_t *handle, uint8_t x, uint8_t y, uint8_t *data)
{
    uint8_t pos;
    uint8_t bx;
    uint8_t temp = 0;
    
    if (handle == NULL)                                              /* check handle */
    {
        return 2;                                                    /* return error */
    }
    if (handle->inited != 1)                                         /* check handle initialization */
    {
        return 3;                                                    /* return error */
    }
    if ((x > 127) || (y > 127))                                       /* check x, y */
    {
        handle->debug_print("ssh1107: x or y is invalid.\n");        /* x or y is invalid */
        
        return 4;                                                    /* return error */
    }
    
    pos = y / 8;                                                     /* get y page */
    bx = y % 8;                                                      /* get y point */
    temp = 1 << bx;                                                  /* set data */
    if ((handle->gram[x][pos] & temp) != 0)                          /* get data */
    {
        *data = 1;                                                   /* set 1 */
    }
    else
    {
        *data = 0;                                                   /* set 0 */
    }
  
    return 0;                                                        /* success return 0 */
}

/**
 * @brief     write a point in the gram
 * @param[in] *handle pointer to an ssh1107 handle structure
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
uint8_t ssh1107_gram_write_point(ssh1107_handle_t *handle, uint8_t x, uint8_t y, uint8_t data)
{
    uint8_t pos;
    uint8_t bx;
    uint8_t temp = 0;
    
    if (handle == NULL)                                              /* check handle */
    {
        return 2;                                                    /* return error */
    }
    if (handle->inited != 1)                                         /* check handle initialization */
    {
        return 3;                                                    /* return error */
    }
    if ((x > 127) || (y > 127))                                       /* check x, y */
    {
        handle->debug_print("ssh1107: x or y is invalid.\n");        /* x or y is invalid */
        
        return 4;                                                    /* return error */
    }
    
    pos = y / 8;                                                     /* get y page */
    bx = y % 8;                                                      /* get y point */
    temp = 1 << bx;                                                  /* set data */
    if (data != 0)                                                   /* if 1 */
    {
        handle->gram[x][pos] |= temp;                                /* set 1 */
    }
    else
    {
        handle->gram[x][pos] &= ~temp;                               /* set 0 */
    }
  
    return 0;                                                        /* success return 0 */
}

/**
 * @brief      read a point from the gram
 * @param[in]  *handle pointer to an ssh1107 handle structure
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
uint8_t ssh1107_gram_read_point(ssh1107_handle_t *handle, uint8_t x, uint8_t y, uint8_t *data)
{
    uint8_t pos;
    uint8_t bx;
    uint8_t temp = 0;
    
    if (handle == NULL)                                              /* check handle */
    {
        return 2;                                                    /* return error */
    }
    if (handle->inited != 1)                                         /* check handle initialization */
    {
        return 3;                                                    /* return error */
    }
    if ((x > 127) || (y > 127))                                       /* check x, y */
    {
        handle->debug_print("ssh1107: x or y is invalid.\n");        /* x or y is invalid */
        
        return 4;                                                    /* return error */
    }
    
    pos = y / 8;                                                     /* get y page */
    bx = y % 8;                                                      /* get y point */
    temp = 1 << bx;                                                  /* set data */
    if ((handle->gram[x][pos] & temp) != 0)                          /* get data */
    {
        *data = 1;                                                   /* set 1 */
    }
    else
    {
        *data = 0;                                                   /* set 0 */
    }
  
    return 0;                                                        /* success return 0 */
}

/**
 * @brief     draw a string in the gram
 * @param[in] *handle pointer to an ssh1107 handle structure
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
uint8_t ssh1107_gram_write_string(ssh1107_handle_t *handle, uint8_t x, uint8_t y, char *str, uint16_t len, uint8_t color, font_t font)
{    
    if (handle == NULL)                                                      /* check handle */
    {
        return 2;                                                            /* return error */
    }
    if (handle->inited != 1)                                                 /* check handle initialization */
    {
        return 3;                                                            /* return error */
    }
    if((x > 127) || (y > 127))                                                /* check x, y */
    {
        handle->debug_print("ssh1107: x or y is invalid.\n");                /* x or y is invalid */
        
        return 4;                                                            /* return error */
    }
    
    while ((len != 0) && (*str <= '~') && (*str >= ' '))                     /* write all string */
    {       
        if (x > (127 - (font / 2)))                                          /* check x point */
        {
            x = 0;                                                           /* set x */
            y += (uint8_t)font;                                              /* set next row */
        }
        if (y > (127 - font))                                                 /* check y point */
        {
            y = x = 0;                                                       /* reset to 0,0 */
        }
        if (a_ssh1107_gram_show_char(handle, x, y, *str, font, color) != 0)  /* show a char */
        {
            return 1;                                                        /* return error */
        }
        x += (uint8_t)(font / 2);                                            /* x + font/2 */
        str++;                                                               /* str address++ */
        len--;                                                               /* str length-- */
    }
    
    return 0;                                                                /* success return 0 */
}

/**
 * @brief     fill a rectangle in the gram
 * @param[in] *handle pointer to an ssh1107 handle structure
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
uint8_t ssh1107_gram_fill_rect(ssh1107_handle_t *handle, uint8_t left, uint8_t top, uint8_t right, uint8_t bottom, uint8_t color)
{
    uint8_t x, y;  
    
    if (handle == NULL)                                                         /* check handle */
    {
        return 2;                                                               /* return error */
    }
    if (handle->inited != 1)                                                    /* check handle initialization */
    {
        return 3;                                                               /* return error */
    }
    if ((left > 127) || (top > 127))                                             /* check left top */
    {
        handle->debug_print("ssh1107: left or top is invalid.\n");              /* left or top is invalid */
        
        return 4;                                                               /* return error */
    }
    if ((right > 127) || (bottom > 127))                                         /* check right bottom */
    {
        handle->debug_print("ssh1107: right or bottom is invalid.\n");          /* right or bottom is invalid */
        
        return 5;                                                               /* return error */
    }
    if ((left > right) || (top > bottom))                                       /* check left right top bottom */
    {
        handle->debug_print("ssh1107: left > right or top > bottom.\n");        /* left > right or top > bottom */
        
        return 6;                                                               /* return error */
    }
    
    for (x = left; x <= right; x++)                                             /* write x */
    {
        for (y = top; y <= bottom; y++)                                         /* write y */
        {
            if (a_ssh1107_gram_draw_point(handle, x, y, color) != 0)            /* draw point */
            {
                return 1;                                                       /* return error */
            }
        }
    }
    
    return 0;                                                                   /* return error */
}

/**
 * @brief     draw a picture in the gram
 * @param[in] *handle pointer to an ssh1107 handle structure
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
uint8_t ssh1107_gram_draw_picture(ssh1107_handle_t *handle, uint8_t left, uint8_t top, uint8_t right, uint8_t bottom, uint8_t *img)
{    
    uint8_t x, y;  
    
    if (handle == NULL)                                                         /* check handle */
    {
        return 2;                                                               /* return error */
    }
    if (handle->inited != 1)                                                    /* check handle initialization */
    {
        return 3;                                                               /* return error */
    }
    if ((left > 127) || (top > 127))                                             /* check left top */
    {
        handle->debug_print("ssh1107: left or top is invalid.\n");              /* left or top is invalid */
        
        return 4;                                                               /* return error */
    }
    if ((right > 127) || (bottom > 127))                                         /* check right bottom */
    {
        handle->debug_print("ssh1107: right or bottom is invalid.\n");          /* right or bottom is invalid */
        
        return 5;                                                               /* return error */
    }
    if ((left > right) || (top > bottom))                                       /* check left right top bottom */
    {
        handle->debug_print("ssh1107: left > right or top > bottom.\n");        /* left > right or top > bottom */
        
        return 6;                                                               /* return error */
    }
    
    for (x = left; x <= right; x++)                                             /* write x */
    {
        for (y = top; y <= bottom; y++)                                         /* write y */
        {
            if (a_ssh1107_gram_draw_point(handle, x, y, *img) != 0)             /* draw point */
            {
                return 1;                                                       /* return error */
            }
            img++;                                                              /* img++ */
        }
    }
    
    return 0;                                                                   /* succeed return 0 */
}

/**
 * @brief     initialize the chip
 * @param[in] *handle pointer to an ssh1107 handle structure
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
uint8_t ssh1107_init(ssh1107_handle_t *handle)
{
    if (handle == NULL)                                                             /* check handle */
    {
        return 2;                                                                   /* return error */
    }
    if (handle->debug_print == NULL)                                                /* check debug_print */
    {
        return 3;                                                                   /* return error */
    }
    if (handle->iic_init == NULL)                                                   /* check iic_init */
    {
        handle->debug_print("ssh1107: iic_init is null.\n");                        /* iic_init is null */
        
        return 3;                                                                   /* return error */
    }
    if (handle->iic_deinit == NULL)                                                 /* check iic_deinit */
    {
        handle->debug_print("ssh1107: iic_deinit is null.\n");                      /* iic_deinit is null */
       
        return 3;                                                                   /* return error */
    }
    if (handle->iic_write == NULL)                                                  /* check iic_write */
    {
        handle->debug_print("ssh1107: iic_write is null.\n");                       /* iic_write is null */
       
        return 3;                                                                   /* return error */
    }
    if (handle->spi_init == NULL)                                                   /* check spi_init */
    {
        handle->debug_print("ssh1107: spi_init is null.\n");                        /* spi_init is null */
        
        return 3;                                                                   /* return error */
    }
    if (handle->spi_deinit == NULL)                                                 /* check spi_deinit */
    {
        handle->debug_print("ssh1107: spi_deinit is null.\n");                      /* spi_deinit is null */
       
        return 3;                                                                   /* return error */
    }
    if (handle->spi_write_cmd == NULL)                                              /* check spi_write_cmd */
    {
        handle->debug_print("ssh1107: spi_write_cmd is null.\n");                   /* spi_write_cmd is null */
       
        return 3;                                                                   /* return error */
    }
    if (handle->delay_ms == NULL)                                                   /* check delay_ms */
    {
        handle->debug_print("ssh1107: delay_ms is null.\n");                        /* delay_ms is null */
        
        return 3;                                                                   /* return error */
    }
    if (handle->spi_cmd_data_gpio_init == NULL)                                     /* check spi_cmd_data_gpio_init */
    {
        handle->debug_print("ssh1107: spi_cmd_data_gpio_init is null.\n");          /* spi_cmd_data_gpio_init is null */
       
        return 3;                                                                   /* return error */
    }
    if (handle->spi_cmd_data_gpio_deinit == NULL)                                   /* check spi_cmd_data_gpio_deinit */
    {
        handle->debug_print("ssh1107: spi_cmd_data_gpio_deinit is null.\n");        /* spi_cmd_data_gpio_deinit is null */
       
        return 3;                                                                   /* return error */
    }
    if (handle->spi_cmd_data_gpio_write == NULL)                                    /* check spi_cmd_data_gpio_write */
    {
        handle->debug_print("ssh1107: spi_cmd_data_gpio_write is null.\n");         /* spi_cmd_data_gpio_write is null */
        
        return 3;                                                                   /* return error */
    }
    if (handle->reset_gpio_init == NULL)                                            /* check reset_gpio_init */
    {
        handle->debug_print("ssh1107: reset_gpio_init is null.\n");                 /* reset_gpio_init is null */
        
        return 3;                                                                   /* return error */
    }
    if (handle->reset_gpio_deinit == NULL)                                          /* check reset_gpio_deinit */
    {
        handle->debug_print("ssh1107: reset_gpio_deinit is null.\n");               /* reset_gpio_deinit is null */
        
        return 3;                                                                   /* return error */
    }
    if(handle->reset_gpio_write == NULL)                                            /* check reset_gpio_write */
    {
        handle->debug_print("ssh1107: reset_gpio_write is null.\n");                /* reset_gpio_write is null */
        
        return 3;                                                                   /* return error */
    }
    
//    if (handle->spi_cmd_data_gpio_init() != 0)                                      /* check spi_cmd_data_gpio_init */
//    {
//        handle->debug_print("ssh1107: spi cmd data gpio init failed.\n");           /* spi cmd data gpio init failed */
        
//        return 5;                                                                   /* return error */
//    }
//    if (handle->reset_gpio_init() != 0)                                             /* reset gpio init */
//    {
//        handle->debug_print("ssh1107: reset gpio init failed.\n");                  /* reset gpio init failed */
//        (void)handle->spi_cmd_data_gpio_deinit();                                   /* spi_cmd_data_gpio_deinit */
        
//        return 4;                                                                   /* return error */
//    }
//    if (handle->reset_gpio_write(0) != 0)                                           /* write 0 */
//    {
//        handle->debug_print("ssh1107: reset gpio write failed.\n");                 /* reset gpio write failed */
//        (void)handle->spi_cmd_data_gpio_deinit();                                   /* spi_cmd_data_gpio_deinit */
//        (void)handle->reset_gpio_deinit();                                          /* reset_gpio_deinit */
        
//        return 4;                                                                   /* return error */
//    }
//    handle->delay_ms(100);                                                          /* delay 100 ms */
//    if (handle->reset_gpio_write(1) != 0)                                           /* write 1 */
//    {
//        handle->debug_print("ssh1107: reset gpio write failed.\n");                 /* reset gpio write failed */
//        (void)handle->spi_cmd_data_gpio_deinit();                                   /* spi_cmd_data_gpio_deinit */
//        (void)handle->reset_gpio_deinit();                                          /* reset_gpio_deinit */
        
//        return 4;                                                                   /* return error */
//    }
    if (handle->iic_spi == SSH1107_INTERFACE_IIC)                                   /* if iic interface */
    {
        if (handle->iic_init() != 0)                                                /* iic init */
        {
            handle->debug_print("ssh1107: iic init failed.\n");                     /* iic init failed */
            (void)handle->spi_cmd_data_gpio_deinit();                               /* spi_cmd_data_gpio_deinit */
            (void)handle->reset_gpio_deinit();                                      /* reset_gpio_deinit */
            
            return 1;                                                               /* return error */
        }
    }
    else if (handle->iic_spi == SSH1107_INTERFACE_SPI)                              /* if spi interface */
    {
        if (handle->spi_init() != 0)                                                /* spi init */
        {
            handle->debug_print("ssh1107: spi init failed.\n");                     /* spi init failed */
            (void)handle->spi_cmd_data_gpio_deinit();                               /* spi_cmd_data_gpio_deinit */
            (void)handle->reset_gpio_deinit();                                      /* reset_gpio_deinit */
            
            return 1;                                                               /* return error */
        }
    }
    else
    {
        handle->debug_print("ssh1107: interface is invalid.\n");                    /* interface is invalid */
        (void)handle->spi_cmd_data_gpio_deinit();                                   /* spi_cmd_data_gpio_deinit */
        (void)handle->reset_gpio_deinit();                                          /* reset_gpio_deinit */
        
        return 6;                                                                   /* return error */
    }
    handle->inited = 1;                                                             /* flag inited */
    
    return 0;                                                                       /* success return 0 */
}

/**
 * @brief     close the chip
 * @param[in] *handle pointer to an ssh1107 handle structure
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
uint8_t ssh1107_deinit(ssh1107_handle_t *handle)
{
    uint8_t buf[2];
    
    if (handle == NULL)                                                              /* check handle */
    {
        return 2;                                                                    /* return error */
    }
    if (handle->inited != 1)                                                         /* check handle initialization */
    {
        return 3;                                                                    /* return error */
    }

    
    if (a_ssh1107_write_byte(handle, SSH1107_CMD_DISPLAY_OFF, SSH1107_CMD) != 0)     /* write display off */
    {
        handle->debug_print("ssh1107: write command failed.\n");                     /* write command failed */
            
        return 4;                                                                    /* return error */
    }

//    if (handle->reset_gpio_deinit() != 0)                                            /* reset gpio deinit */
//    {
//        handle->debug_print("ssh1107: reset gpio deinit failed.\n");                 /* reset gpio deinit failed */
            
//        return 5;                                                                    /* return error */
//    }

//    if (handle->spi_cmd_data_gpio_deinit() != 0)                                     /* spi cmd data gpio deinit */
//    {
//        handle->debug_print("ssh1107: spi cmd data gpio deinit failed.\n");          /* spi cmd data gpio deinit failed */
            
//        return 6;                                                                    /* return error */
//    }

    if (handle->iic_spi == SSH1107_INTERFACE_IIC)                                    /* if iic interface */
    {
        if (handle->iic_deinit() != 0)                                               /* iic deinit */
        {
            handle->debug_print("ssh1107: iic deinit failed.\n");                    /* iic deinit failed */
            
            return 1;                                                                /* return error */
        }
    }
    else if (handle->iic_spi == SSH1107_INTERFACE_SPI)                               /* if spi interface */
    {
        if (handle->spi_deinit() != 0)                                               /* spi deinit */
        {
            handle->debug_print("ssh1107: spi deinit failed.\n");                    /* spi deinit failed */
            
            return 1;                                                                /* return error */
        }
    }
    else
    {
        handle->debug_print("ssh1107: interface is invalid.\n");                     /* interface is invalid */
        
        return 7;                                                                    /* return error */
    }
    handle->inited = 0;                                                              /* flag close */
    
    return 0;                                                                        /* success return 0 */
}

/**
 * @brief     set the chip interface
 * @param[in] *handle pointer to an ssh1107 handle structure
 * @param[in] interface chip interface
 * @return    status code
 *            - 0 success
 *            - 2 handle is NULL
 * @note      none
 */
uint8_t ssh1107_set_interface(ssh1107_handle_t *handle, ssh1107_interface_t interface)
{
    if (handle == NULL)                          /* check handle */
    {
        return 2;                                /* return error */
    }
    
    handle->iic_spi = (uint8_t)interface;        /* set interface */
    
    return 0;                                    /* success return 0 */
}

/**
 * @brief      get the chip interface
 * @param[in]  *handle pointer to an ssh1107 handle structure
 * @param[out] *interface pointer to a chip interface buffer
 * @return     status code
 *             - 0 success
 *             - 2 handle is NULL
 * @note       none
 */
uint8_t ssh1107_get_interface(ssh1107_handle_t *handle, ssh1107_interface_t *interface)
{
    if (handle == NULL)                                         /* check handle */
    {
        return 2;                                               /* return error */
    }
    
    *interface = (ssh1107_interface_t)(handle->iic_spi);        /* get interface */
    
    return 0;                                                   /* success return 0 */
}

/**
 * @brief     set the chip iic address
 * @param[in] *handle pointer to an ssh1107 handle structure
 * @param[in] addr_pin iic address
 * @return    status code
 *            - 0 success
 *            - 2 handle is NULL
 * @note      none
 */
uint8_t ssh1107_set_addr_pin(ssh1107_handle_t *handle, ssh1107_address_t addr_pin)
{
    if (handle == NULL)                          /* check handle */
    {
        return 2;                                /* return error */
    }
    
    handle->iic_addr = (uint8_t)addr_pin;        /* set addr pin */
    
    return 0;                                    /* success return 0 */
}

/**
 * @brief      get the chip iic address
 * @param[in]  *handle pointer to an ssh1107 handle structure
 * @param[out] *addr_pin pointer to an iic address buffer
 * @return     status code
 *             - 0 success
 *             - 2 handle is NULL
 * @note       none
 */
uint8_t ssh1107_get_addr_pin(ssh1107_handle_t *handle, ssh1107_address_t *addr_pin)
{
    if (handle == NULL)                                       /* check handle */
    {
        return 2;                                             /* return error */
    }
    
    *addr_pin = (ssh1107_address_t)(handle->iic_addr);        /* set address */
    
    return 0;                                                 /* success return 0 */
}

/**
 * @brief     set the low column start address
 * @param[in] *handle pointer to an ssh1107 handle structure
 * @param[in] addr low column start address
 * @return    status code
 *            - 0 success
 *            - 1 set low column start address failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 addr is invalid
 * @note      addr <= 0xF
 */
uint8_t ssh1107_set_low_column_start_address(ssh1107_handle_t *handle, uint8_t addr)
{
    if (handle == NULL)                                                                                         /* check handle */
    {
        return 2;                                                                                               /* return error */
    }
    if (handle->inited != 1)                                                                                    /* check handle initialization */
    {
        return 3;                                                                                               /* return error */
    }
    if (addr > 0x0F)                                                                                            /* check addr */
    {
        handle->debug_print("ssh1107: addr is invalid.\n");                                                     /* addr is invalid */
        
        return 4;                                                                                               /* return error */
    }
  
    return a_ssh1107_write_byte(handle, SSH1107_CMD_LOWER_COLUMN_START_ADDRESS|(addr&0x0F), SSH1107_CMD);       /* write command */
}

/**
 * @brief     set the high column start address
 * @param[in] *handle pointer to an ssh1107 handle structure
 * @param[in] addr high column start address
 * @return    status code
 *            - 0 success
 *            - 1 set high column start address failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 addr is invalid
 * @note      addr <= 0xF
 */
uint8_t ssh1107_set_high_column_start_address(ssh1107_handle_t *handle, uint8_t addr)
{
    if (handle == NULL)                                                                                         /* check handle */
    {
        return 2;                                                                                               /* return error */
    }
    if (handle->inited != 1)                                                                                    /* check handle initialization */
    {
        return 3;                                                                                               /* return error */
    }
    if (addr > 0x0F)                                                                                            /* check addr */
    {
        handle->debug_print("ssh1107: addr is invalid.\n");                                                     /* addr is invalid */
        
        return 4;                                                                                               /* return error */
    }
  
    return a_ssh1107_write_byte(handle, SSH1107_CMD_HIGHER_COLUMN_START_ADDRESS|(addr&0x0F), SSH1107_CMD);      /* write command */
}

/**
 * @brief     set the memory addressing mode
 * @param[in] *handle pointer to an ssh1107 handle structure
 * @param[in] mode memory addressing mode
 * @return    status code
 *            - 0 success
 *            - 1 set memory addressing mode failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t ssh1107_set_memory_addressing_mode(ssh1107_handle_t *handle, ssh1107_memory_addressing_mode_t mode)
{
    uint8_t buf[2];

    if (handle == NULL)                                                                 /* check handle */
    {
        return 2;                                                                       /* return error */
    }
    if (handle->inited != 1)                                                            /* check handle initialization */
    {
        return 3;                                                                       /* return error */
    }
  
    buf[0] = SSH1107_CMD_MEMORY_ADDRESSING_MODE;                                        /* set command mode */
    buf[1] = mode;                                                                      /* set mode */
  
    return a_ssh1107_multiple_write_byte(handle, (uint8_t *)buf, 2, SSH1107_CMD);       /* write command */
}

/**
 * @brief     set the column address range
 * @param[in] *handle pointer to an ssh1107 handle structure
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
uint8_t ssh1107_set_column_address_range(ssh1107_handle_t *handle, uint8_t start_addr, uint8_t end_addr)
{
    uint8_t buf[3];
    
    if (handle == NULL)                                                                 /* check handle */
    {
        return 2;                                                                       /* return error */
    }
    if (handle->inited != 1)                                                            /* check handle initialization */
    {
        return 3;                                                                       /* return error */
    }
    if (start_addr > 0x7F)                                                              /* check start addr */
    {
        handle->debug_print("ssh1107: start addr is invalid.\n");                       /* start addr is invalid */
        
        return 4;                                                                       /* return error */
    }
    if (end_addr > 0x7F)                                                                /* check end addr */
    {
        handle->debug_print("ssh1107: end addr is invalid.\n");                         /* end addr is invalid */
        
        return 5;                                                                       /* return error */
    }
    
    buf[0] = SSH1107_CMD_SET_COLUMN_ADDRESS;                                            /* set command */
    buf[1] = start_addr & 0x7F;                                                         /* set start address */
    buf[2] = end_addr & 0x7F;                                                           /* set end address */
  
    return a_ssh1107_multiple_write_byte(handle, (uint8_t *)buf, 3, SSH1107_CMD);       /* write command */
}

/**
 * @brief     set the page address range
 * @param[in] *handle pointer to an ssh1107 handle structure
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
uint8_t ssh1107_set_page_address_range(ssh1107_handle_t *handle, uint8_t start_addr, uint8_t end_addr)
{
    uint8_t buf[3];

    if (handle == NULL)                                                                 /* check handle */
    {
        return 2;                                                                       /* return error */
    }
    if (handle->inited != 1)                                                            /* check handle initialization */
    {
        return 3;                                                                       /* return error */
    }
    if (start_addr > 0x0F)                                                              /* check start addr */
    {
        handle->debug_print("ssh1107: start addr is invalid.\n");                       /* start addr is invalid */
        
        return 4;                                                                       /* return error */
    }
    if (end_addr > 0x0F)                                                                /* check end addr */
    {
        handle->debug_print("ssh1107: end addr is invalid.\n");                         /* end_addr is invalid */
        
        return 5;                                                                       /* return error */
    }
    
    buf[0] = SSH1107_CMD_SET_PAGE_ADDRESS;                                              /* set command */
    buf[1] = start_addr & 0x0F;                                                         /* set start address */
    buf[2] = end_addr & 0x0F;                                                           /* set end address */
  
    return a_ssh1107_multiple_write_byte(handle, (uint8_t *)buf, 3, SSH1107_CMD);       /* write command */
}

/**
 * @brief     set the display start line
 * @param[in] *handle pointer to an ssh1107 handle structure
 * @param[in] l start line
 * @return    status code
 *            - 0 success
 *            - 1 set display start line failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 line is invalid
 * @note      line <= 0x7F
 */
uint8_t ssh1107_set_display_start_line(ssh1107_handle_t *handle, uint8_t l)
{
    if (handle == NULL)                                                                                 /* check handle */
    {
        return 2;                                                                                       /* return error */
    }
    if (handle->inited != 1)                                                                            /* check handle initialization */
    {
        return 3;                                                                                       /* return error */
    }
    if (l > 0x7F)                                                                                       /* check line */
    {
        handle->debug_print("ssh1107: line is invalid.\n");                                             /* line is invalid */
        
        return 4;                                                                                       /* return error */
    }
  
    return a_ssh1107_write_byte(handle, SSH1107_CMD_DISPLAY_START_LINE|(l&0x7F), SSH1107_CMD);          /* write command */
}

/**
 * @brief     set the display contrast
 * @param[in] *handle pointer to an ssh1107 handle structure
 * @param[in] contrast display contrast
 * @return    status code
 *            - 0 success
 *            - 1 set contrast failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t ssh1107_set_contrast(ssh1107_handle_t *handle, uint8_t contrast)
{
    uint8_t buf[2];
    
    if (handle == NULL)                                                                 /* check handle */
    {
        return 2;                                                                       /* return error */
    }
    if (handle->inited != 1)                                                            /* check handle initialization */
    {
        return 3;                                                                       /* return error */
    }
    
    buf[0] = SSH1107_CMD_CONTRAST_CONTROL;                                              /* set command */
    buf[1] = contrast;                                                                  /* set contrast */
  
    return a_ssh1107_multiple_write_byte(handle, (uint8_t *)buf, 2, SSH1107_CMD);       /* write command */
}

/**
 * @brief     set the segment remap
 * @param[in] *handle pointer to an ssh1107 handle structure
 * @param[in] remap segment remap param
 * @return    status code
 *            - 0 success
 *            - 1 set segment remap failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t ssh1107_set_segment_remap(ssh1107_handle_t *handle, ssh1107_segment_column_remap_t remap)
{
    if (handle == NULL)                                                                                /* check handle */
    {
        return 2;                                                                                      /* return error */
    }
    if (handle->inited != 1)                                                                           /* check handle initialization */
    {
        return 3;                                                                                      /* return error */
    }
    
    if (remap != 0)                                                                                    /* check remap */
    {
        return a_ssh1107_write_byte(handle, SSH1107_CMD_COLUMN_127_MAPPED_TO_SEG0, SSH1107_CMD);       /* write remap */
    }
    else
    {
        return a_ssh1107_write_byte(handle, SSH1107_CMD_COLUMN_0_MAPPED_TO_SEG0, SSH1107_CMD);         /* write remap */
    }
}

/**
 * @brief     enable or disable the entire display
 * @param[in] *handle pointer to an ssh1107 handle structure
 * @param[in] enable bool value
 * @return    status code
 *            - 0 success
 *            - 1 set entire display failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t ssh1107_set_entire_display(ssh1107_handle_t *handle, ssh1107_entire_display_t enable)
{
    if (handle == NULL)                                                                         /* check handle */
    {
        return 2;                                                                               /* return error */
    }
    if (handle->inited != 1)                                                                    /* check handle initialization */
    {
        return 3;                                                                               /* return error */
    }
    
    if (enable != 0)                                                                            /* if enable */
    {
        return a_ssh1107_write_byte(handle, SSH1107_CMD_ENTIRE_DISPLAY_ON, SSH1107_CMD);        /* write command */
    }
    else
    {
        return a_ssh1107_write_byte(handle, SSH1107_CMD_ENTIRE_DISPLAY_OFF, SSH1107_CMD);       /* write command */
    }
}

/**
 * @brief     set the display mode
 * @param[in] *handle pointer to an ssh1107 handle structure
 * @param[in] mode display mode
 * @return    status code
 *            - 0 success
 *            - 1 set display mode failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t ssh1107_set_display_mode(ssh1107_handle_t *handle, ssh1107_display_mode_t mode)
{
    if (handle == NULL)                                                                         /* check handle */
    {
        return 2;                                                                               /* return error */
    }
    if (handle->inited != 1)                                                                    /* check handle initialization */
    {
        return 3;                                                                               /* return error */
    }
    
    if (mode != 0)                                                                              /* check mode */
    {
        return a_ssh1107_write_byte(handle, SSH1107_CMD_INVERSE_DISPLAY, SSH1107_CMD);          /* write command */
    }
    else
    {
        return a_ssh1107_write_byte(handle, SSH1107_CMD_NORMAL_DISPLAY, SSH1107_CMD);           /* write command */
    }
}

/**
 * @brief     set the multiplex ratio
 * @param[in] *handle pointer to an ssh1107 handle structure
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
uint8_t ssh1107_set_multiplex_ratio(ssh1107_handle_t *handle, uint8_t multiplex)
{
    uint8_t buf[2];

    if (handle == NULL)                                                                 /* check handle */
    {
        return 2;                                                                       /* return error */
    }
    if (handle->inited != 1)                                                            /* check handle initialization */
    {
        return 3;                                                                       /* return error */
    }
    if (multiplex < 0x0F)                                                               /* check multiplex */
    {
        handle->debug_print("ssh1107: multiplex is too small.\n");                      /* multiplex is too small */
       
        return 4;                                                                       /* return error */
    }
    if (multiplex > 0x7F)                                                               /* check multiplex */
    {
        handle->debug_print("ssh1107: multiplex is too large.\n");                      /* multiplex is too large */
       
        return 5;                                                                       /* return error */
    }
    
    buf[0] = SSH1107_CMD_MULTIPLEX_RATIO ;                                              /* set command */
    buf[1] = multiplex;                                                                 /* set multiplex */
  
    return a_ssh1107_multiple_write_byte(handle, (uint8_t *)buf, 2, SSH1107_CMD);       /* write command */
}

/**
 * @brief     enable or disable the display
 * @param[in] *handle pointer to an ssh1107 handle structure
 * @param[in] on_off bool value
 * @return    status code
 *            - 0 success
 *            - 1 set display failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t ssh1107_set_display(ssh1107_handle_t *handle, ssh1107_display_t on_off)
{
    if (handle == NULL)                                                                  /* check handle */
    {
        return 2;                                                                        /* return error */
    }
    if (handle->inited != 1)                                                             /* check handle initialization */
    {
        return 3;                                                                        /* return error */
    }
    
    if (on_off != 0)                                                                     /* check on off */
    {
        return a_ssh1107_write_byte(handle, SSH1107_CMD_DISPLAY_ON, SSH1107_CMD);        /* write command */
    }
    else
    {
        return a_ssh1107_write_byte(handle, SSH1107_CMD_DISPLAY_OFF, SSH1107_CMD);       /* write command */
    }
}

/**
 * @brief     set the page address
 * @param[in] *handle pointer to an ssh1107 handle structure
 * @param[in] addr page address
 * @return    status code
 *            - 0 success
 *            - 1 set page address failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 addr is invalid
 * @note      addr <= 0x0F
 */
uint8_t ssh1107_set_page_address(ssh1107_handle_t *handle, uint8_t addr)
{
    if (handle == NULL)                                                                        /* check handle */
    {
        return 2;                                                                              /* return error */
    }
    if (handle->inited != 1)                                                                   /* check handle initialization */
    {
        return 3;                                                                              /* return error */
    }
    if (addr > 0x0F)                                                                           /* check addr */
    {
        handle->debug_print("ssh1107: addr is invalid.\n");                                    /* addr is invalid */
        
        return 4;                                                                              /* return error */
    }
    
    return a_ssh1107_write_byte(handle, SSH1107_CMD_PAGE_ADDR|(addr&0x0F), SSH1107_CMD);       /* write command */
}

/**
 * @brief     set the scan direction
 * @param[in] *handle pointer to an ssh1107 handle structure
 * @param[in] dir scan direction
 * @return    status code
 *            - 0 success
 *            - 1 set scan direction failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t ssh1107_set_scan_direction(ssh1107_handle_t *handle, ssh1107_scan_direction_t dir)
{
    if (handle == NULL)                                                                                  /* check handle */
    {
        return 2;                                                                                        /* return error */
    }
    if (handle->inited != 1)                                                                             /* check handle initialization */
    {
        return 3;                                                                                        /* return error */
    }
    
    if (dir != 0)                                                                                        /* choose dir */
    {
        return a_ssh1107_write_byte(handle, SSH1107_CMD_SCAN_DIRECTION_COMN_1_START, SSH1107_CMD);       /* write command */
    }
    else
    {
        return a_ssh1107_write_byte(handle, SSH1107_CMD_SCAN_DIRECTION_COM0_START, SSH1107_CMD);         /* write command */
    }
}

/**
 * @brief     set the display offset
 * @param[in] *handle pointer to an ssh1107 handle structure
 * @param[in] offset display offset
 * @return    status code
 *            - 0 success
 *            - 1 set display offset failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 offset is invalid
 * @note      offset <= 0x7F
 */
uint8_t ssh1107_set_display_offset(ssh1107_handle_t *handle, uint8_t offset)
{
    uint8_t buf[2];

    if (handle == NULL)                                                                 /* check handle */
    {
        return 2;                                                                       /* return error */
    }
    if (handle->inited != 1)                                                            /* check handle initialization */
    {
        return 3;                                                                       /* return error */
    }
    if (offset > 0x7F)                                                                  /* check offset */
    {
        handle->debug_print("ssh1107: offset is invalid.\n");                           /* offset is invalid */
       
        return 4;                                                                       /* return error */
    }
    
    buf[0] = SSH1107_CMD_DISPLAY_OFFSET ;                                               /* set command */
    buf[1] = offset;                                                                    /* set offset */
  
    return a_ssh1107_multiple_write_byte(handle, (uint8_t *)buf, 2, SSH1107_CMD);       /* write command */
}

/**
 * @brief     set the display clock
 * @param[in] *handle pointer to an ssh1107 handle structure
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
uint8_t ssh1107_set_display_clock(ssh1107_handle_t *handle, uint8_t oscillator_frequency, uint8_t clock_divide)
{
    uint8_t buf[2];
    
    if (handle == NULL)                                                                 /* check handle */
    {
        return 2;                                                                       /* return error */
    }
    if (handle->inited != 1)                                                            /* check handle initialization */
    {
        return 3;                                                                       /* return error */
    }
    if (oscillator_frequency> 0x0F)                                                     /* check oscillator_frequency */
    {
        handle->debug_print("ssh1107: oscillator frequency is invalid.\n");             /* oscillator frequency is invalid */
        
        return 4;                                                                       /* return error */
    }
    if (clock_divide> 0x0F)                                                             /* check clock_divide */
    {
        handle->debug_print("ssh1107: clock divide is invalid.\n");                     /* clock divide is invalid */
        
        return 5;                                                                       /* return error */
    }
    
    buf[0] = SSH1107_CMD_DISPLAY_CLOCK_DIVIDE ;                                         /* set command */
    buf[1] = (oscillator_frequency<<4) | clock_divide;                                  /* set oscillator frequency and clock divide */
  
    return a_ssh1107_multiple_write_byte(handle, (uint8_t *)buf, 2, SSH1107_CMD);       /* write command */
}

/**
 * @brief     set the pre charge period
 * @param[in] *handle pointer to an ssh1107 handle structure
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
uint8_t ssh1107_set_precharge_period(ssh1107_handle_t *handle, uint8_t phase1_period, uint8_t phase2_period)
{
    uint8_t buf[2];

    if (handle == NULL)                                                                 /* check handle */
    {
        return 2;                                                                       /* return error */
    }
    if (handle->inited != 1)                                                            /* check handle initialization */
    {
        return 3;                                                                       /* return error */
    }
    if (phase1_period> 0x0F)                                                            /* check phase1 period */
    {
        handle->debug_print("ssh1107: phase1 period is invalid.\n");                    /* phase1 period is invalid */

        return 4;                                                                       /* return error */
    }
    if (phase2_period> 0x0F)                                                            /* check phase2 period */
    {
        handle->debug_print("ssh1107: phase2 period is invalid.\n");                    /* phase2 period is invalid */

        return 5;                                                                       /* return error */
    }

    buf[0] = SSH1107_CMD_PRE_CHARGE_PERIOD;                                             /* set command */
    buf[1] = (phase2_period << 4) | phase1_period;                                      /* set period */

    return a_ssh1107_multiple_write_byte(handle, (uint8_t *)buf, 2, SSH1107_CMD);       /* write command */
}

/**
 * @brief     set the deselect level
 * @param[in] *handle pointer to an ssh1107 handle structure
 * @param[in] level deselect level
 * @return    status code
 *            - 0 success
 *            - 1 set deselect level failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t ssh1107_set_deselect_level(ssh1107_handle_t *handle, ssh1107_deselect_level_t level)
{
    uint8_t buf[2];
    
    if (handle == NULL)                                                                 /* check handle */
    {
        return 2;                                                                       /* return error */
    }
    if (handle->inited != 1)                                                            /* check handle initialization */
    {
        return 3;                                                                       /* return error */
    }
    
    buf[0] = SSH1107_CMD_COMH_DESLECT_LEVEL;                                            /* set command */
    buf[1] = (uint8_t)(level << 4);                                                     /* set level */
  
    return a_ssh1107_multiple_write_byte(handle, (uint8_t *)buf, 2, SSH1107_CMD);       /* write command */
}

/**
 * @brief     write the register command
 * @param[in] *handle pointer to an ssh1107 handle structure
 * @param[in] *buf pointer to a data buffer
 * @param[in] len data length
 * @return    status code
 *            - 0 success
 *            - 1 write failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t ssh1107_write_cmd(ssh1107_handle_t *handle, uint8_t *buf, uint8_t len)
{
    if (handle == NULL)                                                                   /* check handle */
    {
        return 2;                                                                         /* return error */
    }
    if (handle->inited != 1)                                                              /* check handle initialization */
    {
        return 3;                                                                         /* return error */
    }
    
    return a_ssh1107_multiple_write_byte(handle, (uint8_t *)buf, len, SSH1107_CMD);       /* write command */
}

/**
 * @brief     write the register data
 * @param[in] *handle pointer to an ssh1107 handle structure
 * @param[in] *buf pointer to a data buffer
 * @param[in] len data length
 * @return    status code
 *            - 0 success
 *            - 1 write failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t ssh1107_write_data(ssh1107_handle_t *handle, uint8_t *buf, uint8_t len)
{
    if (handle == NULL)                                                                    /* check handle */
    {
        return 2;                                                                          /* return error */
    }
    if (handle->inited != 1)                                                               /* check handle initialization */
    {
        return 3;                                                                          /* return error */
    }
  
    return a_ssh1107_multiple_write_byte(handle, (uint8_t *)buf, len, SSH1107_DATA);       /* write data */
}

/**
 * @brief      get chip's information
 * @param[out] *info pointer to an ssh1107 info structure
 * @return     status code
 *             - 0 success
 *             - 2 handle is NULL
 * @note       none
 */
uint8_t ssh1107_info(ssh1107_info_t *info)
{
    if (info == NULL)                                               /* check handle */
    {
        return 2;                                                   /* return error */
    }
    
    memset(info, 0, sizeof(ssh1107_info_t));                        /* initialize ssh1107 info structure */
    strncpy(info->chip_name, CHIP_NAME, 32);                        /* copy chip name */
    strncpy(info->manufacturer_name, MANUFACTURER_NAME, 32);        /* copy manufacturer name */
    strncpy(info->interface, "IIC SPI", 8);                         /* copy interface name */
    info->supply_voltage_min_v = SUPPLY_VOLTAGE_MIN;                /* set minimal supply voltage */
    info->supply_voltage_max_v = SUPPLY_VOLTAGE_MAX;                /* set maximum supply voltage */
    info->max_current_ma = MAX_CURRENT;                             /* set maximum current */
    info->temperature_max = TEMPERATURE_MAX;                        /* set minimal temperature */
    info->temperature_min = TEMPERATURE_MIN;                        /* set maximum temperature */
    info->driver_version = DRIVER_VERSION;                          /* set driver version */
    
    return 0;                                                       /* success return 0 */
}
