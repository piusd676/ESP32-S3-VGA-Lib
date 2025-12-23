#include "esp_lcd_panel_rgb.h"
#include "esp_lcd_panel_ops.h"
#include "esp_mac.h"
#include "esp_cache.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "VGADisplayManager.h"
#include "VGAESP32S3.h"

static uint8_t *pixels8 = NULL;
static uint16_t *pixels = NULL;
static void *pixelsv = NULL;

void VGADisplayManager::initGraphics(void *framebuffer) {
    pixels = (uint16_t *)framebuffer;
    pixels8 = (uint8_t *)framebuffer;
    pixelsv = framebuffer;
}

void VGADisplayManager::displayRectangle(int color) {
    memset(pixelsv, color, 640*480);
}