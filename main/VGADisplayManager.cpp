/*
    VGADisplayManager handles everything that is going on on the screen.
    It draws and erases pixels from layers, manages masking and layering into the final pixel buffer, manages drawn shapes etc
    Nothing going on on the screen besides basic timing setup should be coming from outside this class!
*/

#include "esp_lcd_panel_rgb.h"
#include "hal/lcd_ll.h"
#include "esp_lcd_panel_ops.h"
#include "esp_mac.h"
#include "esp_attr.h"
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
#include "esp_async_memcpy.h"
#include "VGAESP32S3.h"

static uint8_t *pixels8 = NULL;     //8 bit pixel array
static uint16_t *pixels = NULL;     //16 bit pixel array
uint8_t* layers[5];                 //layer array: pointers to 4 layers, 0-2 are actual layers(0 bottom, 2 top), 3 is the pixelarray itself
bool* mask[3];                      //masking for each of the 3 layers that can be combined(pixelarray itself obviously cant be masked)
uint8_t *scroller;                  //Scroll layer for scrolling on current application
bool *scrollupdater;                //mask for which pixels need updating on scroll
bool *scrollmask;                   //masking layer for scroller
int bitpp = 0;                      //global bits per pixel 
int width = 0;                      //global width
int height = 0;                     //global height
int scheight = 0;                   //Scroller height
int scwidth = 0;                    //Scroller width
int currscposx = 0;                 //Current scroller position x
int currscposy = 0;                 //Current scroller position y

void VGADisplayManager::initGraphics(void *framebuffer, int pwidth, int pheight, int pbitpp, int framebuffernr) {       //Initiator method to give this object all the relevant information

    width = pwidth;         
    height = pheight;

    for(int i = 0; i < 3; i++) {
        layers[i] = (uint8_t *)heap_caps_calloc(width*height, sizeof(uint8_t), MALLOC_CAP_DMA | MALLOC_CAP_SPIRAM);     //Allocating layers to external PSRAM
        mask[i] =  (bool *)heap_caps_calloc(width*height, sizeof(bool), MALLOC_CAP_DMA | MALLOC_CAP_SPIRAM);            //Allocating mask to external PSRAM
    }

    pixels = (uint16_t *)framebuffer;                                                                                   //Setting up both framebuffer pointers, one for 8 bit graphics, one for 16 bit graphics
    pixels8 = (uint8_t *)framebuffer;
    layers[3] = pixels8;            //topmost layer is the pixelbuffer itself, you can draw directly to it if necessary to circumvent any update logic(far faster than layer managing)

    bitpp = pbitpp;
}

void VGADisplayManager::swapfbfs(void *framebuffernew) {

    layers[3] = (uint8_t *)framebuffernew;
    pixels8 = (uint8_t *)framebuffernew;
    pixels = (uint16_t *)framebuffernew;
}

void VGADisplayManager::initScroller(int pscwidth, int pscheight) {

    //Var declarations
    scwidth = pscwidth;
    scheight = pscheight;

    //Set up scroller for required dimensions
    scroller = (uint8_t *)heap_caps_calloc(scwidth*scheight, sizeof(uint8_t), MALLOC_CAP_DMA | MALLOC_CAP_SPIRAM);
    scrollmask =  (bool *)heap_caps_calloc(scwidth*scheight, sizeof(bool), MALLOC_CAP_DMA | MALLOC_CAP_SPIRAM);
    layers[4] = (uint8_t *)scroller;
}

void VGADisplayManager::display(int startx, int starty, int endx, int endy) {     //writes changes to PSRAM for DMA to pick up
    
    //Variable declarations
    int currentrow = 0;                 //Optimization: row only has to be calculated once per loop of outer for loop
    int currentpx = 0;                  //Optimization: current pixel coordinate only has to be calculated once per inner loop

    if(bitpp == 8) {                    //Testing for current bit layout (WARNING: if 16 bit is used, colors have to be in RGB565(or equivalent) format! providing an incompatible color format will
        for(int y = starty; y < endy; y++) {   //introduce combing or produce visible gaps in output!)
            currentrow = y * width;
            for(int x = startx; x < endx; x++) {
                currentpx = currentrow + x;
                pixels8[currentpx] = layers[2][currentpx];              //Topmost layer is applied first
                if(mask[2][currentpx] == true) {                        //If current pixel is marked as masking, pixel is entirely skipped
                    continue;
                }
                else{                                                   //If pixel is either empty or just not masking, next layer is applied
                    pixels8[currentpx] = layers[1][currentpx];          //Repeating setup from before
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
    else{                                                               //Same logic but for a 16 bit pointer
        for(int y = starty; y < endy; y++) {
            currentrow = y * width;
            for(int x = startx; x < endx; x++) {
                currentpx = currentrow + x;
                pixels[currentpx] = layers[2][currentpx];              //Topmost layer is applied first
                if(mask[2][currentpx] == true) {                        //If current pixel is marked as masking, pixel is entirely skipped
                    continue;
                }
                else{                                                   //If pixel is either empty or just not masking, next layer is applied
                    pixels[currentpx] = layers[1][currentpx];          //Repeating setup from before
                    if(mask[1][currentpx] == true) {
                        continue;
                    }
                    else{
                        pixels[currentpx] = layers[0][currentpx];
                    }
                }
            }
        }
    }
}

//Logic for displaying a simple axis-oriented rectangle. points cannot be defined with angles
void VGADisplayManager::displayRectangle(EXT_RAM_BSS_ATTR int color, EXT_RAM_BSS_ATTR int px, EXT_RAM_BSS_ATTR int py, EXT_RAM_BSS_ATTR int rwidth,EXT_RAM_BSS_ATTR int rheight, EXT_RAM_BSS_ATTR int layer, bool maskit) {
    
    //Variable declarations
    int currentrow = 0;                 //Optimization: row only needs to be calculated once per outer for loop
    int currentpx = 0;
    int pheight = height;
    int pwidth = width;

    if(layer == 4) {
        pheight = scheight;
        pwidth = scwidth;
    }

    for(int y = py; y < (py + rheight); y++) {
        currentrow = y * pwidth;
        for(int x = px; x < (px + rwidth); x++) {
            currentpx = currentrow + x;
            layers[layer][currentpx] = color;
            mask[layer][currentpx] = maskit;      
        }
    }
}


void VGADisplayManager::displayCircle(int color, int centerx, int centery, int radius, bool fill, int layer, bool maskit) {

    //Calculate area of circle to figure out angles
    int circircu = 0;           //Circumference for the amount of pixels drawn
    double anglesteps = 0.0;    //Steps the angle (in radians) needs to take to reach those angles
    int pix = 0;                //x coordinate of next point to be drawn
    int piy = 0;                //y coordinate of next point to be drawn
    
    //TODO:OPTIMIZE!! DFS?
    circircu = M_PI * 4 * radius;
    anglesteps = (2*M_PI)/circircu;

    //Scroller setup
    int pheight = height;
    int pwidth = width;

    if(layer == 4) {
        pheight = scheight;
        pwidth = scwidth;
    }

    if(fill) {
        for(int i = 0; i < circircu/4; i++) {           //Filling bottom-right quadrant
            pix = centerx + radius * sin(i*anglesteps);
            piy = centery + radius * cos(i*anglesteps);
            displayRectangle(color, centerx, centery, (pix-centerx), (piy-centery), layer, maskit);
        }
        for(int i = circircu/4; i < circircu/2; i++) {  //Filling top-right quadrant
            pix = centerx + radius * sin(i*anglesteps);
            piy = centery + radius * cos(i*anglesteps);
            displayRectangle(color, centerx, piy, (pix-centerx), (centery-piy), layer, maskit);
        }                                               //Filling top-left quadrant
        for(int i = circircu/2; i < 3*(circircu/4); i++) {
            pix = centerx + radius * sin(i*anglesteps);
            piy = centery + radius * cos(i*anglesteps);
            displayRectangle(color, pix, piy, (centerx-pix), (centery-piy), layer, maskit);
        }                                               //Filling bottom-left quadrant
        for(int i = 3*(circircu/4); i < circircu; i++) {
            pix = centerx + radius * sin(i*anglesteps);
            piy = centery + radius * cos(i*anglesteps);
            displayRectangle(color, pix, centery, (centerx-pix), (piy-centery), layer, maskit);
        }
    }
    else{
        for(int i = 0; i < circircu; i++) {             //Filling entire circle at once
            pix = centerx + radius * sin(i*anglesteps);
            piy = centery + radius * cos(i*anglesteps);
            layers[layer][piy * pwidth + pix] = color;
            mask[layer][piy * pwidth + pix] = maskit;
        }   
    }
}

void VGADisplayManager::displayLine(EXT_RAM_BSS_ATTR int color, EXT_RAM_BSS_ATTR int startx, EXT_RAM_BSS_ATTR int starty, EXT_RAM_BSS_ATTR int endx, EXT_RAM_BSS_ATTR int endy, EXT_RAM_BSS_ATTR int lwidth, int layer, bool maskit) {

    //TODO: OPTIMIZATION
    //Declaration Variables
    EXT_RAM_BSS_ATTR double slope = 0;  //slope of line
    EXT_RAM_BSS_ATTR double lengthx = 0;    //pure x length of line
    EXT_RAM_BSS_ATTR double lengthy = 0;    //pure y length of line
    EXT_RAM_BSS_ATTR int valuef = 0;

    //Calculating pure axis lengths
    lengthx = endx-startx;
    lengthy = endy-starty;

    //Scroller setup
    int pheight = height;
    int pwidth = width;

    if(layer == 4) {
        pheight = scheight;
        pwidth = scwidth;
    }

    //Check that our line is atleast 1 pixel long
    if((lengthx == 0) & (lengthy == 0)) {
        return;
    }

    //Calculating slope
    if((lengthx > 0) & (lengthy > 0)) {     //make sure there isnt a division by zero and that our result wont be Infinity
        slope = lengthy/lengthx;
        //Drawing angled line
        for(int i = -(lwidth/2); i < lwidth/2; i++) {
            for(int j = 0; j <= lengthx; j++) {
                valuef = (int)(slope * j) + starty + i;                 //Calculating value of function f(x) = m*x+c
                layers[layer][(valuef * pwidth) + (j+startx)] = color;   
                mask[layer][(valuef * pwidth) + (j+startx)] = maskit;
            }
        }
    }
    else{
        if(lengthx == 0) {                                              //Vertical line is drawn differently because of calculation issues with slope(result would be infinity)
            for(int i = -(lwidth/2); i < lwidth/2; i++) {
                for(int j = 0; j < lengthy; j++) {
                    layers[layer][(valuef * pwidth) + (j+startx)] = color;
                    mask[layer][(valuef * pwidth) + (j+startx)] = maskit;
                }   
            }
        }
        if(lengthy == 0) {                                              //Vertical line is drawn differently because of calculation issues with slope(division by zero)
            for(int i = -(lwidth/2); i < lwidth/2; i++) {
                for(int j = 0; j < lengthx; j++) {
                    layers[layer][(valuef * pwidth) + (j+startx)] = color;
                    mask[layer][(valuef * pwidth) + (j+startx)] = maskit;
                }
            }
        }
    }
}

void VGADisplayManager::loadPicture(std::string imgname, int posx, int posy, int xwidth, int yheight, int imgx, int imgy, int layer, bool maskit) {  //Loading picture 

    //Image parameters
    extern const uint8_t image_start8[] asm("_binary_output_raw_start");    //"_binary_IMGNAME_ENDING_start" is equivalent to IMGNAME.ENDING start memory address, 8 bit format
    extern const uint16_t image_start[] asm("_binary_output_raw_start");    //"_binary_IMGNAME_ENDING_start" is equivalent to IMGNAME.ENDING start memory address, 16 bit format

    //Scroller setup
    int pheight = height;
    int pwidth = width;

    if(layer == 4) {
        pheight = scheight;
        pwidth = scwidth;
    }

    //Catching picture loads across the entire screen with exact screen width and height values
    if((imgx == pwidth) & (xwidth == pwidth) & (imgy == pheight) & (pheight == yheight)) {
        memcpy(layers[layer], image_start, pheight*pwidth);
    }

    //using a pointer to manipulate image data
    const uint8_t *img8 = (uint8_t *)image_start8;  //8 bit format
    const uint16_t *img = (uint16_t *)image_start;  //16 bit format

    //Assumption until metadata implemented:img is sized to width, height of display image
    //Calculating steps
    double xsteps = imgx/xwidth;
    double ysteps = imgy/yheight;
    int xval = 0;
    int yval = 0;

    for(int y = posy; y < (posy + yheight); y++) {
        for(int x = posx; x < (posx + xwidth); x++) {
            yval = (y-posy)*ysteps;                 //xsteps, ysteps to scale img
            xval = (x-posx)*xsteps;
            layers[layer][y * pwidth + x] = img8[yval * pwidth + xval];
            mask[layer][y * pwidth + x] = maskit; 
        }
    }
}

void VGADisplayManager::displayPolygon(int color, int points[][2], bool fill, bool maskit) {

    //TODO

}

void VGADisplayManager::setBackground(int color, int layer) {

    //Scroller setup
    int pheight = height;
    int pwidth = width;

    if(layer == 4) {
        pheight = scheight;
        pwidth = scwidth;
    }
    
    for(int y = 0; y < pheight; y++) {
        for(int x = 0; x < pwidth; x++) {
            memset(layers[layer], color, pheight*pwidth);
        }
    }
}

void VGADisplayManager::displayscroll(int xscroll, int yscroll, int layer, bool maskit) {

    //Scroller setup
    int pheight = scheight;
    int pwidth = scwidth;

    if(layer == 4) {
        return;
    }

    //Increase pos variables by scroll amount
    currscposx += xscroll;
    currscposy += yscroll;

    int fbcurrposy;
    int fbval;
    int fbcurrpos;
    int updateval;
    /*
    //Var declarations
    uint8_t *src = (layers[4] + (currscposy * pwidth) + currscposx);
    uint8_t *dst = layers[layer];
    uint64_t *src64 = (uint64_t *)src;
    uint64_t *dst64 = (uint64_t *)dst;
    
    for(int y = 0; y < ((width*height*8)/64); y++) {
        dst64[y] = src64[y];
    }
        */

    for(int y = 0; y < pheight; y++) {
        fbcurrposy = y * width;
        for(int x = 0; x < pwidth; x++) {
            fbcurrpos = fbcurrposy + x;
            fbval = layers[layer][fbcurrpos];
            updateval = layers[4][(y + currscposy) * pwidth + (x + currscposx)];
            if(fbval == updateval) {
                continue;
            }
            else{
                layers[layer][fbcurrpos] = updateval;
            }
        }
    }

        

}

//TODO:Circle, Oval, Line, Loading picture, layer management