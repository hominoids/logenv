
#include <sys/types.h>
#include "driver_ssd1681_basic.h"
#include "driver_ssd1681_interface.h"

ssd1681_handle_t ssd1681_handle;

uint8_t gs_lut[153] = {
    0x80, 0x48, 0x40, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x40, 0x48, 0x80, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x80, 0x48, 0x40, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x40, 0x48, 0x80, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0xA, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x8, 0x1, 0x0, 0x8, 0x1, 0x0, 0x2,
    0xA, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x0, 0x0, 0x0,
};

int open_ssd1681(void)
{

    uint8_t res;

    /* link functions */
    DRIVER_SSD1681_LINK_INIT(&ssd1681_handle, ssd1681_handle_t);
    DRIVER_SSD1681_LINK_SPI_INIT(&ssd1681_handle, ssd1681_interface_spi_init);
    DRIVER_SSD1681_LINK_SPI_DEINIT(&ssd1681_handle, ssd1681_interface_spi_deinit);
    DRIVER_SSD1681_LINK_SPI_WRITE_CMD(&ssd1681_handle, ssd1681_interface_spi_write_cmd);
    DRIVER_SSD1681_LINK_SPI_READ_CMD(&ssd1681_handle, ssd1681_interface_spi_read_cmd);
    DRIVER_SSD1681_LINK_SPI_CMD_DATA_GPIO_INIT(&ssd1681_handle, ssd1681_interface_spi_cmd_data_gpio_init);
    DRIVER_SSD1681_LINK_SPI_CMD_DATA_GPIO_DEINIT(&ssd1681_handle, ssd1681_interface_spi_cmd_data_gpio_deinit);
    DRIVER_SSD1681_LINK_SPI_CMD_DATA_GPIO_WRITE(&ssd1681_handle, ssd1681_interface_spi_cmd_data_gpio_write);
    DRIVER_SSD1681_LINK_RESET_GPIO_INIT(&ssd1681_handle, ssd1681_interface_reset_gpio_init);
    DRIVER_SSD1681_LINK_RESET_GPIO_DEINIT(&ssd1681_handle, ssd1681_interface_reset_gpio_deinit);
    DRIVER_SSD1681_LINK_RESET_GPIO_WRITE(&ssd1681_handle, ssd1681_interface_reset_gpio_write);
    DRIVER_SSD1681_LINK_BUSY_GPIO_INIT(&ssd1681_handle, ssd1681_interface_busy_gpio_init);
    DRIVER_SSD1681_LINK_BUSY_GPIO_DEINIT(&ssd1681_handle, ssd1681_interface_busy_gpio_deinit);
    DRIVER_SSD1681_LINK_BUSY_GPIO_READ(&ssd1681_handle, ssd1681_interface_busy_gpio_read);
    DRIVER_SSD1681_LINK_DELAY_MS(&ssd1681_handle, ssd1681_interface_delay_ms);
    DRIVER_SSD1681_LINK_DEBUG_PRINT(&ssd1681_handle, ssd1681_interface_debug_print);

    /* init */
    res = ssd1681_init(&ssd1681_handle);
    if (res != 0)
    {
        ssd1681_interface_debug_print("ssd1681: init failed.\n");

        return 1;
    }

    /* reset the chip */
    res = ssd1681_software_reset(&ssd1681_handle);
    if (res != 0)
    {
        ssd1681_interface_debug_print("ssd1681: software reset failed.\n");
        (void)ssd1681_deinit(&ssd1681_handle);

        return 1;
    }

    /* set the driver output */
    res = ssd1681_set_driver_output(&ssd1681_handle, 0xC7, SSD1681_BOOL_FALSE, SSD1681_BOOL_FALSE, SSD1681_BOOL_FALSE);
    if (res != 0)
    {
        ssd1681_interface_debug_print("ssd1681: set the driver output failed.\n");
        (void)ssd1681_deinit(&ssd1681_handle);

        return 1;
    }

    /* set the data entry mode */
    res = ssd1681_set_data_entry_mode(&ssd1681_handle, SSD1681_BASIC_DEFAULT_ADDRESS_MODE, SSD1681_BASIC_DEFAULT_ADDRESS_DIRECTION);
    if (res != 0)
    {
        ssd1681_interface_debug_print("ssd1681: set data entry mode failed.\n");
        (void)ssd1681_deinit(&ssd1681_handle);

        return 1;
    }

    /* set the internal sensor */
    res = ssd1681_set_temperature_sensor(&ssd1681_handle, SSD1681_TEMPERATURE_SENSOR_INTERNAL);
    if (res != 0)
    {
        ssd1681_interface_debug_print("ssd1681: set temperature sensor failed.\n");
        (void)ssd1681_deinit(&ssd1681_handle);

        return 1;
    }

    /* set the border waveform */
    res = ssd1681_set_border_waveform(&ssd1681_handle, SSD1681_VBD_GS_TRANSITION, SSD1681_VBD_FIX_LEVEL_VSS,
                                      SSD1681_GS_TRANSITION_FOLLOW_LUT, SSD1681_VBD_TRANSITION_LUT1);
    if (res != 0)
    {
        ssd1681_interface_debug_print("ssd1681: set border waveform failed.\n");
        (void)ssd1681_deinit(&ssd1681_handle);

        return 1;
    }

    /* set the x range */
    res = ssd1681_set_ram_x(&ssd1681_handle, 0x00, 0x18);
    if (res != 0)
    {
        ssd1681_interface_debug_print("ssd1681: set ram x failed.\n");
        (void)ssd1681_deinit(&ssd1681_handle);

        return 1;
    }

    /* set the y range */
    res = ssd1681_set_ram_y(&ssd1681_handle, 0xC7, 0x00);
    if (res != 0)
    {
        ssd1681_interface_debug_print("ssd1681: set ram y failed.\n");
        (void)ssd1681_deinit(&ssd1681_handle);

        return 1;
    }

    /* set the ram x address counter */
    res = ssd1681_set_ram_x_address_counter(&ssd1681_handle, 0x00);
    if (res != 0)
    {
        ssd1681_interface_debug_print("ssd1681: set ram x address counter failed.\n");
        (void)ssd1681_deinit(&ssd1681_handle);

        return 1;
    }

    /* set the ram y address counter */
    res = ssd1681_set_ram_y_address_counter(&ssd1681_handle, 0xC7);
    if (res != 0)
    {
        ssd1681_interface_debug_print("ssd1681: set ram y address counter failed.\n");
        (void)ssd1681_deinit(&ssd1681_handle);

        return 1;
    }

    /* set display update control normal mode */
    res = ssd1681_set_display_update_control(&ssd1681_handle, SSD1681_DISPLAY_CONTROL_NORMAL, SSD1681_DISPLAY_CONTROL_NORMAL);
    if (res != 0)
    {
        ssd1681_interface_debug_print("ssd1681: set display update control failed.\n");
        (void)ssd1681_deinit(&ssd1681_handle);

        return 1;
    }

    /* set the sequence 0 */
    res = ssd1681_set_display_sequence(&ssd1681_handle, SSD1681_DISPLAY_SEQUENCE_0);
    if (res != 0)
    {
        ssd1681_interface_debug_print("ssd1681: set display sequence failed.\n");
        (void)ssd1681_deinit(&ssd1681_handle);

        return 1;
    }

    /* set the vcom sense duration 0x00 */
    res = ssd1681_set_vcom_sense_duration(&ssd1681_handle, 0x00);
    if (res != 0)
    {
        ssd1681_interface_debug_print("ssd1681: set vcom sense duration failed.\n");
        (void)ssd1681_deinit(&ssd1681_handle);

        return 1;
    }

    /* enter the vcom sense */
    res = ssd1681_set_enter_vcom_sense(&ssd1681_handle);
    if (res != 0)
    {
        ssd1681_interface_debug_print("ssd1681: set enter vcom sense failed.\n");
        (void)ssd1681_deinit(&ssd1681_handle);

        return 1;
    }

    /* set the vcom control reg */
    res = ssd1681_set_vcom_control_reg(&ssd1681_handle);
    if (res != 0)
    {
        ssd1681_interface_debug_print("ssd1681: set vcom control reg failed.\n");
        (void)ssd1681_deinit(&ssd1681_handle);

        return 1;
    }

    /* set the lut register */
    res = ssd1681_set_lut_register(&ssd1681_handle, gs_lut, 153);
    if (res != 0)
    {
        ssd1681_interface_debug_print("ssd1681: set lut register failed.\n");
        (void)ssd1681_deinit(&ssd1681_handle);

        return 1;
    }

    /* end the option */
    res = ssd1681_set_end_option(&ssd1681_handle, SSD1681_END_OPT_NORMAL);
    if (res != 0)
    {
        ssd1681_interface_debug_print("ssd1681: set end option failed.\n");
        (void)ssd1681_deinit(&ssd1681_handle);

        return 1;
    }

    /* set the gate driving voltage 20V */
    res = ssd1681_set_gate_driving_voltage(&ssd1681_handle, SSD1681_GATE_DRIVING_VOLTAGE_20V);
    if (res != 0)
    {
        ssd1681_interface_debug_print("ssd1681: set gate driving voltage failed.\n");
        (void)ssd1681_deinit(&ssd1681_handle);

        return 1;
    }

    /* set the source driving voltage */
    res = ssd1681_set_source_driving_voltage(&ssd1681_handle, SSD1681_VSH_15V, SSD1681_VSH_5V, SSD1681_VSL_NEGATIVE_15);
    if (res != 0)
    {
        ssd1681_interface_debug_print("ssd1681: set source driving voltage failed.\n");
        (void)ssd1681_deinit(&ssd1681_handle);

        return 1;
    }

    /* set vcom -0.8 */
    res = ssd1681_set_vcom_register(&ssd1681_handle, SSD1681_VCOM_NEGATIVE_0P8);
    if (res != 0)
    {
        ssd1681_interface_debug_print("ssd1681: set vcom register failed.\n");
        (void)ssd1681_deinit(&ssd1681_handle);

        return 1;
    }

    /* height 200 and width 200 */
    res = ssd1681_set_auto_write_red_ram(&ssd1681_handle, SSD1681_STEP_HEIGHT_200, SSD1681_STEP_WIDTH_200);
    if (res != 0)
    {
        ssd1681_interface_debug_print("ssd1681: set auto write red ram failed.\n");
        (void)ssd1681_deinit(&ssd1681_handle);

        return 1;
    }

    /* height 200 and width 200 */
    res = ssd1681_set_auto_write_black_ram(&ssd1681_handle, SSD1681_STEP_HEIGHT_200, SSD1681_STEP_WIDTH_200);
    if (res != 0)
    {
        ssd1681_interface_debug_print("ssd1681: set auto write black ram failed.\n");
        (void)ssd1681_deinit(&ssd1681_handle);

        return 1;
    }

    /* set the hv ready detection */
    res = ssd1681_set_hv_ready_detection(&ssd1681_handle, 0x00, 0x00);
    if (res != 0)
    {
        ssd1681_interface_debug_print("ssd1681: set hv ready detection failed.\n");
        (void)ssd1681_deinit(&ssd1681_handle);

        return 1;
    }

    /* set the vci detection 2.3V */
    res = ssd1681_set_vci_detection(&ssd1681_handle, SSD1681_VCI_LEVEL_2P3V);
    if (res != 0)
    {
        ssd1681_interface_debug_print("ssd1681: set vci detection failed.\n");
        (void)ssd1681_deinit(&ssd1681_handle);

        return 1;
    }

    /* set the booster soft start param */
    res = ssd1681_set_booster_soft_start(&ssd1681_handle,
                                        (uint8_t)(SSD1681_DRIVING_STRENGTH_1 | SSD1681_MIN_OFF_TIME_8P4),
                                        (uint8_t)(SSD1681_DRIVING_STRENGTH_2 | SSD1681_MIN_OFF_TIME_9P8),
                                        (uint8_t)(SSD1681_DRIVING_STRENGTH_2 | SSD1681_MIN_OFF_TIME_3P9),
                                        (uint8_t)((SSD1681_PHASE_DURATION_10MS << 4) | (SSD1681_PHASE_DURATION_40MS << 2) | (SSD1681_PHASE_DURATION_40MS << 0)));
    if (res != 0)
    {
        ssd1681_interface_debug_print("ssd1681: set booster soft start failed.\n");
        (void)ssd1681_deinit(&ssd1681_handle);

        return 1;
    }

    /* set the normal mode */
    res = ssd1681_set_mode(&ssd1681_handle, SSD1681_MODE_NORMAL);
    if (res != 0)
    {
        ssd1681_interface_debug_print("ssd1681: set mode failed.\n");
        (void)ssd1681_deinit(&ssd1681_handle);

        return 1;
    }

    /* clear the black */
    res = ssd1681_clear(&ssd1681_handle, SSD1681_COLOR_BLACK);
    if (res != 0)
    {
        ssd1681_interface_debug_print("ssd1681: clear failed.\n");
        (void)ssd1681_deinit(&ssd1681_handle);

        return 1;
    }

}
