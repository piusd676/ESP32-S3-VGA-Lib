#include "Arduino.h"
#include "VGAESP32S3.h"
#include "VGARes.h"
#include "VGADisplayManager.h"

void VGAESP32S3::init(VGARes VGAResMode) {
    
    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_rgb_panel_config_t panel_config = {
        .clk_src = LCD_CLK_SRC_PLL160M,
         // The timing parameters should refer to your LCD spec
        .timings = {
            .pclk_hz = VGAResMode.pixelclockhz,
            .h_res = VGAResMode.hres,
            .v_res = VGAResMode.vres,
            .hsync_pulse_width = VGAResMode.hsyncpulsewidth,
            .hsync_back_porch = VGAResMode.hsyncbackwidth,
            .hsync_front_porch = VGAResMode.hsyncfrontwidth,
            .vsync_pulse_width = VGAResMode.vsyncpulsewidth,
            .vsync_back_porch = VGAResMode.vsyncbackwidth,
            .vsync_front_porch = VGAResMode.vsyncfrontwidth,
            .flags = {
                .hsync_idle_low = VGAResMode.hsyncidlelow,
                .vsync_idle_low = VGAResMode.vsyncidlelow,
                .pclk_active_neg = VGAResMode.pclkactivenegative,
            }
        },
        .data_width = VGAResMode.datawidth,
        .bits_per_pixel = VGAResMode.bitsperpixel,
        .num_fbs = VGAResMode.num_frbfs,
        .bounce_buffer_size_px = VGAResMode.bouncebfrsize,
        .sram_trans_align = VGAResMode.sramtransalign,
        .psram_trans_align = VGAResMode.psramtransalign,
        .hsync_gpio_num = VGAResMode.hsyncgpio,
        .vsync_gpio_num = VGAResMode.vsyncgpio,
        .de_gpio_num = -1,
        .pclk_gpio_num = -1,
        .disp_gpio_num = -1,
        .data_gpio_nums = {
            VGAResMode.datagpios[0],
            VGAResMode.datagpios[1],
            VGAResMode.datagpios[2],
            VGAResMode.datagpios[3],
            VGAResMode.datagpios[4],
            VGAResMode.datagpios[5],
            VGAResMode.datagpios[6],
            VGAResMode.datagpios[7],
            VGAResMode.datagpios[8],
            VGAResMode.datagpios[9],
            VGAResMode.datagpios[10],
            VGAResMode.datagpios[11],
            VGAResMode.datagpios[12],
            VGAResMode.datagpios[13],
            VGAResMode.datagpios[14],
            VGAResMode.datagpios[15],
            // GPIOs in row
        },
        .flags = {
            .fb_in_psram = 1,
            .double_fb = VGAResMode.dbfb,
        },
    }

    esp_lcd_panel_init(panel_handle);

    esp_lcd_rgb_panel_get_frame_buffer(panel_handle, 1, &framebuffer);

    uint16_t *pixels = (uint16_t *)framebuffer;
    uint8_t *pixels8b = (uint8_t *)framebuffer;

    esp_cache_msync(framebuffer, 640*480, ESP_CACHE_MSYNC_FLAG_DIR_C2M);
}