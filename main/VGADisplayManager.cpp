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
#include <esp_heap_caps.h>
#include <string>
#include "VGADisplayManager.h"
#include "VGAESP32S3.h"

static uint8_t *pixels8 = NULL;
static uint16_t *pixels = NULL;
int bitpp = 0;
int width = 0;
int height = 0;

void VGADisplayManager::initGraphics(void *framebuffer, int pwidth, int pheight, int pbitpp) {

    width = pwidth;
    height = pheight;

    pixels = (uint16_t *)framebuffer;
    pixels8 = (uint8_t *)framebuffer;

    bitpp = pbitpp;
}

void VGADisplayManager::displayRectangle(EXT_RAM_BSS_ATTR int color, EXT_RAM_BSS_ATTR int px, EXT_RAM_BSS_ATTR int py, EXT_RAM_BSS_ATTR int rwidth,EXT_RAM_BSS_ATTR int rheight) {
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

    if(bitpp == 8) {                        //Testing for bits
        if(fill) {
            for(int i = 0; i < circircu/4; i++) {           //Filling bottom-right quadrant
                pix = centerx + radius * sin(i*anglesteps);
                piy = centery + radius * cos(i*anglesteps);
                displayRectangle(color, centerx, centery, (pix-centerx), (piy-centery));
            }
            for(int i = circircu/4; i < circircu/2; i++) {  //Filling top-right quadrant
                pix = centerx + radius * sin(i*anglesteps);
                piy = centery + radius * cos(i*anglesteps);
                displayRectangle(color, centerx, piy, (pix-centerx), (centery-piy));
            }                                               //Filling top-left quadrant
            for(int i = circircu/2; i < 3*(circircu/4); i++) {
                pix = centerx + radius * sin(i*anglesteps);
                piy = centery + radius * cos(i*anglesteps);
                displayRectangle(color, pix, piy, (centerx-pix), (centery-piy));
            }                                               //Filling bottom-left quadrant
            for(int i = 3*(circircu/4); i < circircu; i++) {
                pix = centerx + radius * sin(i*anglesteps);
                piy = centery + radius * cos(i*anglesteps);
                displayRectangle(color, pix, centery, (centerx-pix), (piy-centery));
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
        if(fill) {
            for(int i = 0; i < circircu/4; i++) {           //Filling bottom-right quadrant
                pix = centerx + radius * sin(i*anglesteps);
                piy = centery + radius * cos(i*anglesteps);
                displayRectangle(color, centerx, centery, (pix-centerx), (piy-centery));
            }
            for(int i = circircu/4; i < circircu/2; i++) {  //Filling top-right quadrant
                pix = centerx + radius * sin(i*anglesteps);
                piy = centery + radius * cos(i*anglesteps);
                displayRectangle(color, centerx, piy, (pix-centerx), (centery-piy));
            }                                               //Filling top-left quadrant
            for(int i = circircu/2; i < 3*(circircu/4); i++) {
                pix = centerx + radius * sin(i*anglesteps);
                piy = centery + radius * cos(i*anglesteps);
                displayRectangle(color, pix, piy, (centerx-pix), (centery-piy));
            }                                               //Filling bottom-left quadrant
            for(int i = 3*(circircu/4); i < circircu; i++) {
                pix = centerx + radius * sin(i*anglesteps);
                piy = centery + radius * cos(i*anglesteps);
                displayRectangle(color, pix, centery, (centerx-pix), (piy-centery));
            }
        }
        else{
            for(int i = 0; i < circircu; i++) {
                pix = centerx + radius * sin(i*anglesteps);
                piy = centery + radius * cos(i*anglesteps);
                pixels[piy * width + pix] = color;
            }   
        }
    }
}

void VGADisplayManager::displayLine(EXT_RAM_BSS_ATTR int color, EXT_RAM_BSS_ATTR int startx, EXT_RAM_BSS_ATTR int starty, EXT_RAM_BSS_ATTR int endx, EXT_RAM_BSS_ATTR int endy, EXT_RAM_BSS_ATTR int lwidth) {

    //Declaration Variables
    EXT_RAM_BSS_ATTR double slope = 0;  //slope of line
    EXT_RAM_BSS_ATTR double length = 0; //length of line to determine how many points need to be
    EXT_RAM_BSS_ATTR double lengthx = 0;
    EXT_RAM_BSS_ATTR double lengthy = 0;
    EXT_RAM_BSS_ATTR int valuef = 0;

    //Calculating pure axis lengths
    lengthx = endx-startx;
    lengthy = endy-starty;
    //rintf("lengthx is %f, lengthy is %f\n\n", lengthx, lengthy);

    //Check that our line is atleast 1 pixel long
    if((lengthx == 0) & (lengthy == 0)) {
        return;
        //printf("Needless line calculations avoided. Returning.");
    }

    //Calculating slope
    if((lengthx > 0) & (lengthy > 0)) {     //make sure there isnt a division by zero and that our result wont be Infinity
        slope = lengthy/lengthx;
        //printf("Slope is %f\n",slope);

        //Calculating length
        length = sqrt(pow(lengthx, 2) + pow(lengthy, 2));
        //printf("length is %f\n",length);

        //Drawing angled line
        if(bitpp == 8) {
            for(int i = -(lwidth/2); i < lwidth/2; i++) {
                for(int j = 0; j <= lengthx; j++) {
                    valuef = (int)(slope * j) + starty + i;
                    //printf("valuef of %d is %d\n",j,valuef);
                    pixels8[(valuef * width) + (j+startx)] = color;
                    taskYIELD();
                }
            }
        }
        else{
            for(int i = -(lwidth/2); i < lwidth/2; i++) {
                for(int j = 0; j < lengthx; j++) {
                    valuef = (int)(slope * j) + starty + i;
                    pixels8[(valuef * width) + (j+startx)] = color;
                    taskYIELD();
                }
            }
        }
    }
    else{
        if(lengthx == 0) {
            if(bitpp == 8) {
                for(int i = -(lwidth/2); i < lwidth/2; i++) {
                    for(int j = 0; j < lengthy; j++) {
                        pixels8[(starty + j) * width + startx] = color;
                        taskYIELD();
                    }
                }
            }else{
                for(int i = -(lwidth/2); i < lwidth/2; i++) {
                    for(int j = 0; j < lengthy; j++) {
                        pixels[(starty + j) * width + j+startx] = color;
                        taskYIELD();
                    }
                }
            }
        }
        if(lengthy == 0) {
            if(bitpp == 8) {
                for(int i = -(lwidth/2); i < lwidth/2; i++) {
                    for(int j = 0; j < lengthx; j++) {
                        pixels8[starty * width + (startx + j)] = color;
                        taskYIELD();
                    }
                }
            }else{
                for(int i = -(lwidth/2); i < lwidth/2; i++) {
                    for(int j = 0; j < lengthx; j++) {
                        pixels[starty * width + (startx + j)] = color;
                        taskYIELD();
                    }
                }
            }
        }
    }
}

void VGADisplayManager::loadPicture(std::string imgname, int posx, int posy, int xwidth, int yheight) {

    //Image parameters
    extern const uint8_t image_start8[] asm("_binary_output_raw_start");
    extern const uint16_t image_start[] asm("_binary_output_raw_start");

    //using a pointer to manipulate image data
    const uint8_t *img8 = (uint8_t)image_start8;
    const uint16_t *img = (uint16_t)image_start;

    //memcpy(pixels8, image_start, height*width);
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