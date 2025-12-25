#include "esp_lcd_panel_rgb.h"
#include "esp_lcd_panel_ops.h"
#include "esp_mac.h"
#include "esp_cache.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "VGAESP32S3.h"

/*
void *framebuffer = NULL;

int width = 640;
int height = 480;
int pxcnter = 0;
*/
extern "C" void app_main()
{
    /*
    //declarations
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
    };
    ESP_ERROR_CHECK(esp_lcd_new_rgb_panel(&panel_config, &panel_handle));

    esp_lcd_panel_init(panel_handle);

    esp_lcd_rgb_panel_get_frame_buffer(panel_handle, 1, &framebuffer);

    uint8_t *pixels = (uint8_t *)framebuffer;

    esp_cache_msync(framebuffer, 640*480, ESP_CACHE_MSYNC_FLAG_DIR_C2M);
    */
    setvbuf(stdout, NULL, _IONBF, 0);

    VGAESP32S3 VGADisplay;
    VGADisplayManager VGADM;

    VGARes VGAResMode = VGARes::R640x480at60hz8bit;
    VGADisplay.init(VGAResMode, VGADM);
    VGADM.setBackground(0xFF);
    vTaskDelay(pdMS_TO_TICKS(5000));
    VGADM.displayRectangle(0x00, 0, 0, 80, 480);
    VGADM.displayRectangle(0x11, 80, 0, 80, 480);
    VGADM.displayRectangle(0x22, 160, 0, 80, 480);
    VGADM.displayRectangle(0x33, 240, 0, 80, 480);
    VGADM.displayRectangle(0x44, 320, 0, 80, 480);
    VGADM.displayRectangle(0x55, 400, 0, 80, 480);
    VGADM.displayRectangle(0x66, 480, 0, 80, 480);
    VGADM.displayRectangle(0x77, 560, 0, 80, 480);
    vTaskDelay(pdMS_TO_TICKS(10000));
    VGADM.loadPicture("H", 1, 1, 1 ,1);
    //VGADM.displayCircle(0x55, 320, 240, 50, true);
    /*
    vTaskDelay(pdMS_TO_TICKS(5000));
    for(int i = 0; i < 240; i++) {
        VGADM.displayRectangle(0xA6, i*2, i*2, 250, 250);
        vTaskDelay(pdMS_TO_TICKS(25));
        VGADM.displayRectangle(0x99, (i*2)+1, (i*2)+1, 250, 250);
        vTaskDelay(pdMS_TO_TICKS(25));
    }
    */

}