#ifndef VGADisplayManager_h
#define VGADisplayManager_h
#include "VGARes.h"
//#include "VGAESP32S3.h"

class VGADisplayManager 
{
    public:
        void initGraphics(void *framebuffer);
        void displayRectangle(int color);

    VGADisplayManager() {}
};

#endif