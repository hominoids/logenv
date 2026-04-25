
#ifndef FONTS_H
#define FONTS_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "libdriver_font.h"
#include "monospace_font.h"

#ifdef __cplusplus
extern "C"{
#endif

/**
 * font enumeration definition
 */
typedef enum  
{
    DEFAULT_12 = 0x0C,             /* libdriver default font 12 */
    DEFAULT_16 = 0x10,             /* libdriver default font 16 */
    DEFAULT_24 = 0x18,             /* libdriver default font 24 */
    MONOSPACE_16 = 0x10,           /* monospace font 16 */
    MONOSPACE_20 = 0x24,           /* monospace font 20 */
    MONOSPACE_24 = 0x38,           /* monospace font 24 */
    MONOSPACE_28 = 0x20,           /* monospace font 28 */
    MONOSPACE_36 = 0x28,           /* monospace font 36 */
    MONOSPACE_48 = 0x32,           /* monospace font 48 */
    MONOSPACE_72 = 0x4A,           /* monospace font 72 */
} font_t;

#ifdef __cplusplus
}
#endif

#endif
