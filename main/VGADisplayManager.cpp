#include "esp_lcd_panel_rgb.h"
#include "esp_lcd_panel_ops.h"
#include "esp_mac.h"
#include "esp_cache.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_log.h"
#include <vector>
#include "VGADisplayManager.h"
#include "VGAESP32S3.h"

static uint8_t *pixels8 = NULL;
static uint16_t *pixels = NULL;
static void *pixelsv = NULL;
int bitpp = 0;
std::vector<std::vector<int>> frame;

void VGADisplayManager::initGraphics(void *framebuffer, int width, int height, int pbitpp) {
    pixels = (uint16_t *)framebuffer;
    pixels8 = (uint8_t *)framebuffer;
    pixelsv = framebuffer;

    bitpp = pbitpp;

    frame.resize(height);
    for(int i = 0; i < frame.size(); i++) {
        frame[i].resize(width);
    }
    for(int y = 0; y < frame.size(); y++) {
        for(int x = 0; x < frame[y].size(); x++) {
            frame[y][x] = 0;
        }
    }
}

void VGADisplayManager::displayRectangle(int color, int px, int py, int rwidth, int rheight) {
    for(int y = py; y < (py + rheight); y++) {
        for(int x = px; x < (px + rwidth); x++) {
            pixels8[y * 640 + x] = color;
        }
    }
}