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
#include "esp_cpu.h"

extern "C" void app_main()
{
    setvbuf(stdout, NULL, _IONBF, 0);

    VGAESP32S3 VGADisplay;
    VGADisplayManager VGADM;

    VGARes VGAResMode = VGARes::R640x480at60hz8bit;
    VGADisplay.init(VGAResMode, VGADM);
    VGADM.initScroller(1280, 480);

    /*
    VGADM.setBackground(0xFF, 0);
    vTaskDelay(pdMS_TO_TICKS(5000));
    VGADM.displayRectangle(0x00, 0, 0, 80, 480, 0);
    VGADM.displayRectangle(0x11, 80, 0, 80, 480, 1);
    VGADM.displayRectangle(0x22, 160, 0, 80, 480, 0);
    VGADM.displayRectangle(0x33, 240, 0, 80, 480, 1);
    VGADM.displayRectangle(0x44, 320, 0, 80, 480, 0);
    VGADM.displayRectangle(0x55, 400, 0, 80, 480, 1);
    VGADM.displayRectangle(0x66, 480, 0, 80, 480, 0);
    VGADM.displayRectangle(0x77, 560, 0, 80, 480, 1);
    VGADM.display(0, 0, 640, 480);
    vTaskDelay(pdMS_TO_TICKS(10000));
    VGADM.loadPicture("H", 50, 50,320 ,240, 640, 480, 0);
    VGADM.display(0, 0, 640, 480);
    //VGADM.displayCircle(0x55, 320, 240, 50, true);
    vTaskDelay(pdMS_TO_TICKS(5000));
    uint32_t start_ccount = esp_cpu_get_cycle_count();
    for(int i = 0; i < 240; i++) {
        VGADM.displayRectangle(0xA6, i*2, i*2, 250, 250, 2);
        VGADM.displayRectangle(0x99, (i*2)+1, (i*2)+1, 250, 250, 0);
        VGADM.display(i*2, i*2, (i*2)+251, (i*2)+251);
    }
    uint32_t end_ccount = esp_cpu_get_cycle_count();
    printf("Drawing frames took %lu", (end_ccount - start_ccount));

    start_ccount = esp_cpu_get_cycle_count();
    for(int i = 0; i < 240; i++) {
        VGADM.displayRectangle(0xA6, i*2, i*2, 250, 250, 4);
        VGADM.displayRectangle(0x99, (i*2)+1, (i*2)+1, 250, 250, 4);
    }
    end_ccount = esp_cpu_get_cycle_count();
    printf("Drawing frames took %lu", (end_ccount - start_ccount));
*/  VGADM.display(0,0,640,480);
    vTaskDelay(pdMS_TO_TICKS(20000));
    VGADM.initScroller(1280, 480);
    VGADM.displayRectangle(0x00, 0, 0, 160, 480, 4, false);
    VGADM.displayRectangle(0x01, 160, 0, 160, 480, 4, false);
    VGADM.displayRectangle(0x02, 320, 0, 160, 480, 4, false);
    VGADM.displayRectangle(0x03, 480, 0, 160, 480, 4, false);
    VGADM.displayRectangle(0x04, 640, 0, 160, 480, 4, false);
    VGADM.displayRectangle(0x05, 800, 0, 160, 480, 4, false);
    VGADM.displayRectangle(0x06, 960, 0, 160, 480, 4, false);
    VGADM.displayRectangle(0x07, 1120, 0, 160, 480, 4, false);
    VGADM.displayscroll(0, 0, 3, 0);
    vTaskDelay(pdMS_TO_TICKS(10000));
    for(int i = 0; i < 640; i++) {
        VGADM.displayscroll(5,0,3,0);
    }
}