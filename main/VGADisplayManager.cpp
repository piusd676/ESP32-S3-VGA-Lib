#include "esp_lcd_panel_rgb.h"
#include "esp_lcd_panel_ops.h"
#include "esp_mac.h"
#include "esp_cache.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_log.h"
#include <cmath>
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
    if(bitpp == 8) {                                    //Test 8 bit or 16 bit output(anything between 3-8 bit uses 8 bit, anything above uses 16 bit)
        for(int y = py; y < (py + rheight); y++) {
            for(int x = px; x < (px + rwidth); x++) {
                pixels8[y * width + x] = color;
            }
        }
    }
    else{
        for(int y = py; y < (py + rheight); y++) {
            for(int x = px; x < (px + rwidth); x++) {
                pixels[y * width + x] = color;
            }
        }
    }
}

void VGADisplayManager::displayCircle(int color, int centerx, int centery, int radius, bool fill) {

    //Calculate area of circle to figure out angles
    int circircu = 0;           //Circumference for the amount of pixels drawn
    double anglesteps = 0.0;    //Steps the angle (in radians) needs to take to reach those angles
    int pix = 0;                //x coordinate of next point to be drawn
    int piy = 0;                //y coordinate of next point to be drawn
    
    //TODO:OPTIMIZE!!
    circircu = M_PI * 4 * radius;
    anglesteps = (2*M_PI)/circircu;

    if(bitpp == 8) {
        if(fill) {
            for(int radius2 = 0; radius2 <= radius; radius2++) {
                circircu = M_PI * 4 * radius2;
                anglesteps = (2*M_PI)/circircu;
                for(int i = 0; i < circircu; i++) {
                    pix = centerx + radius2 * sin(i*anglesteps);
                    piy = centery + radius2 * cos(i*anglesteps);
                    pixels8[piy * width + pix] = color;
                }
            }
        }
        else{
            for(int i = 0; i < circircu; i++) {
                pix = centerx + radius * sin(i*anglesteps);
                piy = centery + radius * cos(i*anglesteps);
                pixels8[piy * width + pix] = color;
            }   
        }
    }
    else{

    }
}

void VGADisplayManager::setBackground(int color) {
    if(bitpp == 8) {
        for(int y = 0; y < height; y++) {
            for(int x = 0; x < width; x++) {
                pixels8[y * width + x] = color;
            }
        }
    }
    else{
        for(int y = 0; y < height; y++) {
            for(int x = 0; x < width; x++) {
                pixels[y * width + x] = color;
            }
        }
    }
}

//TODO:Circle, Oval, Line, Loading picture, layer management