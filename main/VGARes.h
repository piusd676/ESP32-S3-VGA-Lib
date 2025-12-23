#include "Arduino.h"

class VGARes {
    public:
        static VGARes R640x480at60hz16bit;
        static VGARes R640x480at60hz8bit;
        static VGARes R640x480at60hz4bit;
        static VGARes R640x480at60hz3bit;
        int pixelclockhz;
        int hres;
        int vres;
        int hsyncpulsewidth;
        int hsyncbackwidth;
        int hsyncfrontwidth;
        int vsyncpulsewidth;
        int vsyncbackwidth;
        int vsyncfrontwidth;
        int hsyncidlelow
        int vsyncidlelow
        int pclkactivenegative;
        int datawidth;
        int bitsperpixel;
        int num_frbfs;
        int bouncebfrsize;
        int sramtransalign;
        int psramtransalign;
        int hsyncgpio
        int vsyncgpio
        int datagpios[16];
        int dbfb;

    Mode(int cpixelclockhz, int hres, int vres, int hsyncpw, int hsyncbw, int hsyncfw, int vsyncpw, int vsyncbw, int vsyncfw, 
        int hsyncidlelow, int vsyncidlelow, int pclkactivenegative, int datawidth, int bitsperpixel, int num_frbfs, int bouncebfrsize,
        int sramtransalign, int psramtransalign, int hsyncgpio, int vsyncgpio, int datagpios[16], int dbfb)
}