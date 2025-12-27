#ifndef VGADisplayManager_h
#define VGADisplayManager_h
#include "VGARes.h"
#include <string>
//#include "VGAESP32S3.h"

class VGADisplayManager 
{
    public:
        void initGraphics(void *framebuffer, int width, int height, int bitpp, int framebuffernr);
        void swapfbfs(void *framebuffernew);
        void initScroller(int scwidth, int scheight);
        void display(int startx, int starty, int endx, int endy);
        void displayRectangle(int color, int px, int py, int rwidth, int rheight, int layer, bool maskit);
        void displayCircle(int color, int centerx, int centery, int radius, bool fill, int layer, bool maskit);
        void displayLine(int color, int startx, int starty, int endx, int endy, int width, int layer, bool maskit);
        void loadPicture(std::string imgname, int posx, int posy, int xwidth, int yheight, int imgx, int imgy, int layer, bool maskit);
        void displayPolygon(int color, int points[][2], bool fill, bool maskit);
        void setBackground(int color, int layer);
        void displayscroll(int xscroll, int yscroll, int layer, bool maskit);

    VGADisplayManager() {}
};

#endif