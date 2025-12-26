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
#include <vector>
#include "VGADisplayManager.h"
#include "VGAESP32S3.h"

static uint8_t *pixels8 = NULL;
static uint16_t *pixels = NULL;
uint8_t* layers[4];
bool* mask[3];
int bitpp = 0;
int width = 0;
int height = 0;

void VGADisplayManager::initGraphics(void *framebuffer, int pwidth, int pheight, int pbitpp, int framebuffernr) {

    width = pwidth;
    height = pheight;

    /*
    layer0 = (uint16_t *)heap_caps_calloc(width*height, sizeof(uint16_t), MALLOC_CAP_DMA | MALLOC_CAP_SPIRAM); //Bottom layer
    layer1 = (uint16_t *)heap_caps_calloc(width*height, sizeof(uint16_t), MALLOC_CAP_DMA | MALLOC_CAP_SPIRAM); //Middle layer
    layer2 = (uint16_t *)heap_caps_calloc(width*height, sizeof(uint16_t), MALLOC_CAP_DMA | MALLOC_CAP_SPIRAM); //Top layer
    */

    for(int i = 0; i < 3; i++) {
        layers[i] = (uint8_t *)heap_caps_calloc(width*height, sizeof(uint8_t), MALLOC_CAP_DMA | MALLOC_CAP_SPIRAM);
        mask[i] =  (bool *)heap_caps_calloc(width*height, sizeof(bool), MALLOC_CAP_DMA | MALLOC_CAP_SPIRAM);
    }

    pixels = (uint16_t *)framebuffer;
    pixels8 = (uint8_t *)framebuffer;
    layers[3] = pixels8;

    bitpp = pbitpp;
}

void VGADisplayManager::display(int startx, int starty, int endx, int endy) {     //writes changes to PSRAM for DMA to pick up
    
    //Variable declarations
    int currentrow = 0;
    int currentpx = 0;

    if(bitpp == 8) {
        for(int y = starty; y < endy; y++) {
            currentrow = y * width;
            for(int x = startx; x < endx; x++) {
                currentpx = currentrow + x;
                pixels8[currentpx] = layers[2][currentpx];
                if(mask[2][currentpx] == true) {
                    continue;
                }
                else{
                    pixels8[currentpx] = layers[1][currentpx];
                    if(mask[1][currentpx] == true) {
                        continue;
                    }
                    else{
                        pixels8[currentpx] = layers[0][currentpx];
                    }
                }
            }
        }
    }
    else{
        for(int y = 0; y < height; y++) {
            for(int x = 0; x < width; x++) {
                pixels[y * width + x] = layers[2][y * width + x];
                if(layers[2][y * width + x] & 0x80) {
                    continue;
                }
                else{
                    pixels[y * width + x] = layers[1][y * width + x];
                    if(layers[1][y * width + x] & 0x80) {
                        continue;
                    }
                    else{
                        pixels[y * width + x] = layers[0][y * width + x];
                    }
                }
            }
        }
    }
}

void VGADisplayManager::displayRectangle(EXT_RAM_BSS_ATTR int color, EXT_RAM_BSS_ATTR int px, EXT_RAM_BSS_ATTR int py, EXT_RAM_BSS_ATTR int rwidth,EXT_RAM_BSS_ATTR int rheight, EXT_RAM_BSS_ATTR int layer) {
    
    //Variable declarations
    int currentrow = 0;
    int currentpx = 0;
    
    if(bitpp == 8) {                                    //pixels8[y * width + x] = color;Test 8 bit or 16 bit output(anything between 3-8 bit uses 8 bit, anything above uses 16 bit)
        for(int y = py; y < (py + rheight); y++) {
            currentrow = y * width;
            for(int x = px; x < (px + rwidth); x++) {
                currentpx = currentrow + x;
                layers[layer][currentpx] = color;
                mask[layer][currentpx] = true;
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


void VGADisplayManager::displayCircle(int color, int centerx, int centery, int radius, bool fill, int layer) {

    //Calculate area of circle to figure out angles
    int circircu = 0;           //Circumference for the amount of pixels drawn
    double anglesteps = 0.0;    //Steps the angle (in radians) needs to take to reach those angles
    int pix = 0;                //x coordinate of next point to be drawn
    int piy = 0;                //y coordinate of next point to be drawn
    
    //TODO:OPTIMIZE!!
    circircu = M_PI * 4 * radius;
    anglesteps = (2*M_PI)/circircu;

    if(fill) {
        for(int i = 0; i < circircu/4; i++) {           //Filling bottom-right quadrant
            pix = centerx + radius * sin(i*anglesteps);
            piy = centery + radius * cos(i*anglesteps);
            displayRectangle(color, centerx, centery, (pix-centerx), (piy-centery), layer);
        }
        for(int i = circircu/4; i < circircu/2; i++) {  //Filling top-right quadrant
            pix = centerx + radius * sin(i*anglesteps);
            piy = centery + radius * cos(i*anglesteps);
            displayRectangle(color, centerx, piy, (pix-centerx), (centery-piy), layer);
        }                                               //Filling top-left quadrant
        for(int i = circircu/2; i < 3*(circircu/4); i++) {
            pix = centerx + radius * sin(i*anglesteps);
            piy = centery + radius * cos(i*anglesteps);
            displayRectangle(color, pix, piy, (centerx-pix), (centery-piy), layer);
        }                                               //Filling bottom-left quadrant
        for(int i = 3*(circircu/4); i < circircu; i++) {
            pix = centerx + radius * sin(i*anglesteps);
            piy = centery + radius * cos(i*anglesteps);
            displayRectangle(color, pix, centery, (centerx-pix), (piy-centery), layer);
        }
    }
    else{
        for(int i = 0; i < circircu; i++) {
            pix = centerx + radius * sin(i*anglesteps);
            piy = centery + radius * cos(i*anglesteps);
            layers[layer][piy * width + pix] = color;
        }   
    }
}

void VGADisplayManager::displayLine(EXT_RAM_BSS_ATTR int color, EXT_RAM_BSS_ATTR int startx, EXT_RAM_BSS_ATTR int starty, EXT_RAM_BSS_ATTR int endx, EXT_RAM_BSS_ATTR int endy, EXT_RAM_BSS_ATTR int lwidth) {

    //Declaration Variables
    EXT_RAM_BSS_ATTR double slope = 0;  //slope of line
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

void VGADisplayManager::loadPicture(std::string imgname, int posx, int posy, int xwidth, int yheight, int imgx, int imgy, int layer) {

    //Image parameters
    extern const uint8_t image_start8[] asm("_binary_output_raw_start");
    extern const uint16_t image_start[] asm("_binary_output_raw_start");

    //Catching picture loads across the entire screen with exact good values
    if((imgx == width) & (xwidth == width) & (imgy == height) & (height == yheight)) {
        if(bitpp == 8) {
            memcpy(pixels8, image_start, height*width);
        }else{
            memcpy(pixels, image_start, height*width);
        }
    }

    //using a pointer to manipulate image data
    const uint8_t *img8 = (uint8_t *)image_start8;
    const uint16_t *img = (uint16_t *)image_start;

    //Assumption until metadata implemented:img is sized to width, height of display image
    //Calculating steps
    double xsteps = imgx/xwidth;
    double ysteps = imgy/yheight;
    int xval = 0;
    int yval = 0;

    if(bitpp == 8) {
        for(int y = posy; y < (posy + yheight); y++) {
            for(int x = posx; x < (posx + xwidth); x++) {
                yval = (y-posy)*ysteps;
                xval = (x-posx)*xsteps;
                layers[layer][y * width + x] = img8[yval * width + xval]; 
            }
        }
    }else{
        for(int y = posy; y < (posy + yheight); y++) {
            for(int x = posx; x < (posx + xwidth); x++) {
                yval = (y-posy)*ysteps;
                xval = (x-posx)*xsteps;
                pixels[y * width + x] = img8[yval * width + xval]; 
            }
        }
    }
}

void VGADisplayManager::displayPolygon(int color, int points[][2], bool fill) {

}

void VGADisplayManager::setBackground(int color) {
    if(bitpp == 8) {
        for(int y = 0; y < height; y++) {
            for(int x = 0; x < width; x++) {
                //pixels8[y * width + x] = color;
            }
        }
    }
    else{
        for(int y = 0; y < height; y++) {
            for(int x = 0; x < width; x++) {
                //pixels[y * width + x] = color;
            }
        }
    }
}

//TODO:Circle, Oval, Line, Loading picture, layer management