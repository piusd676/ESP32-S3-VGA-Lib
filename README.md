### **VGA Library based on the ESP32-S3, driven by its LCD-CAM peripheral**
---
## Premise:

This project aims to simplify the process of interacting with a VGA display through a microcontroller, in this case, the ESP32-S3 with 16MB flash and 8MB PSRAM.
It utilizes the LCD-CAM peripheral on the microcontroller itself.
This repo aims to serve as a documentation for the thought process behind this and also a reference for anyone attempting this in the future
## 

Technical Details:
- Microcontroller: ESP32-S3 DevkitC1 N16R8
- Framework: ESP-IDF
- Libraries: ESP32-S3 LCD-CAM Peripheral Library
- DACs are external using Pmod VGA / Resistor Ladder
- Voltage dividers necessary, as VGA expects 0.7V on RGB pins and MC has a logic level of 3.3V
##

Roadmap:
- [x] Basic output signals &#8594; ESP outputs HSYNC, VSYNC, RGB
- [x] Monitor connectivity &#8594; Monitor is able to lock onto signal
- [x] Basic colors displayed &#8594; 3-Bit color can be correctly displayed on monitor
- [ ] 16-Bit color
- [ ] Dithering
- [ ] Draw shapes(squares, rectangles, circles, triangles) with methods
- [ ] Text using methods

