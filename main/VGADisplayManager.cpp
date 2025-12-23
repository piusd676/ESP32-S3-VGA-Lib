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
#include "VGADisplayManager.h"

void VGADisplayManager::displayRectangle(int color, VGAESP32S3 VGAObj) {
    memset(VGAObj.pixels, color, 640*480);
}