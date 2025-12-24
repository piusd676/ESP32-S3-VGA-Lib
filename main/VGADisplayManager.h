#ifndef VGADisplayManager_h
#define VGADisplayManager_h
#include "VGARes.h"
//#include "VGAESP32S3.h"

class VGADisplayManager 
{
    public:
        void initGraphics(void *framebuffer, int width, int height, int bitpp);
        void displayRectangle(int color, int px, int py, int rwidth, int rheight);
        void setBackground(int color);

    VGADisplayManager() {}
};

#endif