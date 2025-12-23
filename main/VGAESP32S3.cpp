#include "Arduino.h"
#include "VGAESP32S3.h"
#include "esp_lcd_panel_rgb.h"
#include "esp_lcd_panel_ops.h"
#include "esp_mac.h"
#include "esp_cache.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_log.h"

void VGAESP32S3::init() {
    
    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_rgb_panel_config_t panel_config = {
        .clk_src = LCD_CLK_SRC_PLL160M,
         // The timing parameters should refer to your LCD spec
        .timings = {
            .pclk_hz = 25000000,
            .h_res = 640,
            .v_res = 480,
            .hsync_pulse_width = 96,
            .hsync_back_porch = 48,
            .hsync_front_porch = 16,
            .vsync_pulse_width = 2,
            .vsync_back_porch = 33,
            .vsync_front_porch = 10,
            .flags = {
                .hsync_idle_low = 0,
                .vsync_idle_low = 0,
                .pclk_active_neg = 1,
            }
        },
        .data_width = 8, // RGB565 in parallel mode, thus 16 bits in width
        .bits_per_pixel = 8,
        .num_fbs = 2,
        .bounce_buffer_size_px = 640*10,
        .sram_trans_align = 8,
        .psram_trans_align = 64,
        .hsync_gpio_num = GPIO_NUM_1,
        .vsync_gpio_num = GPIO_NUM_2,
        .de_gpio_num = -1,
        .pclk_gpio_num = 10,
        .disp_gpio_num = -1,
        .data_gpio_nums = {
            5,
            6,
            7,
            8,
            9,
            10,
            14,
            21,
            
            // GPIOs in row
        },
        .flags = {
            .fb_in_psram = 1,
            .double_fb = 0,
        },
    }
}