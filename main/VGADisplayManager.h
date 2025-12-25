#ifndef VGADisplayManager_h
#define VGADisplayManager_h
#include "VGARes.h"
#include <string>
//#include "VGAESP32S3.h"

class VGADisplayManager 
{
    public:
        void initGraphics(void *framebuffer, int width, int height, int bitpp, int framebuffernr);
        void display();
        void displayRectangle(int color, int px, int py, int rwidth, int rheight);
        void displayCircle(int color, int centerx, int centery, int radius, bool fill);
        void displayLine(int color, int startx, int starty, int endx, int endy, int width);
        void loadPicture(std::string imgname, int posx, int posy, int xwidth, int yheight, int imgx, int imgy);
        void setBackground(int color);

    VGADisplayManager() {}
};

#endif