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
int width = 0;
int height = 0;

void VGADisplayManager::initGraphics(void *framebuffer, int pwidth, int pheight, int pbitpp) {
    pixels = (uint16_t *)framebuffer;
    pixels8 = (uint8_t *)framebuffer;
    pixelsv = framebuffer;

    width = pwidth;
    height = pheight;

    bitpp = pbitpp;
}

void VGADisplayManager::displayRectangle(int color, int px, int py, int rwidth, int rheight) {
    if(bitpp == 8) {
        for(int y = py; y < (py + rheight); y++) {
            for(int x = px; x < (px + rwidth); x++) {
                pixels8[y * 640 + x] = color;
            }
        }
    }
    if(bitpp == 16) {
        for(int y = py; y < (py + rheight); y++) {
            for(int x = px; x < (px + rwidth); x++) {
                pixels[y * 640 + x] = color;
            }
        }
    }
}

void VGADisplayManager::setBackground(int color) {
    if(bitpp == 8) {
        for(int y = 0; y < height; y++) {
            for(int x = 0; x < width; x++) {
                pixels8[y * 640 + x] = color;
            }
        }
    }
    if(bitpp == 16) {
        for(int y = 0; y < height; y++) {
            for(int x = 0; x < width; x++) {
                pixels[y * 640 + x] = color;
            }
        }
    }
}

//TODO:Circle, Oval, Line, Loading picture