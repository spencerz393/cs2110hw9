/**
* @file graphics.c
* @author Hanhao Zhang
* @date July 3rd 2018
* @brief A graphics library for drawing geometry, for Homework 9 of Georgia Tech
*        CS 2110, Summer 2018.
*/

// Please take a look at the header file below to understand what's required for
// each of the functions.
#include "graphics.h"

Pixel noFilter(Pixel c) {

    return c;
}

// TODO: Complete according to the prototype in graphics.h
Pixel greyscaleFilter(Pixel c) {
    uint16_t red = (c & 31);
    uint16_t green = (c & 992) >> 5;
    uint16_t blue = (c & 31744) >> 10;
    uint16_t n = ((red*77)+(green*151)+(blue*28)) >> 8;
    c = n;
    c = c | (n << 5);
    c = c | (n << 10);
    return c;
}

// TODO: Complete according to the prototype in graphics.h
Pixel redOnlyFilter(Pixel c) {
    c = c & 31;
    return c;
}

// TODO: Complete according to the prototype in graphics.h
Pixel brighterFilter(Pixel c) {
    uint16_t red = (c & 31);
    uint16_t green = (c & 992) >> 5;
    uint16_t blue = (c & 31744) >> 10;
    red = red + ((31 - red) >> 1);
    green = green + ((31 - green) >> 1);
    blue = blue + ((31 - blue) >> 1);
    c = red;
    c = c | (green << 5);
    c = c | (blue << 10);
    return c;
}


// TODO: Complete according to the prototype in graphics.h
void drawPixel(Screen *screen, Vector coordinates, Pixel pixel) {
    unsigned int width = screen -> size.x;
    unsigned int height = screen -> size.y;
    unsigned int row = coordinates.y;
    unsigned int col = coordinates.x;
    if ((row >= height) | (col >= width)) {
        return;
    }
    Pixel *start = screen -> buffer;
    start = start + row*width + col;
    *start = pixel;
}

// TODO: Complete according to the prototype in graphics.h
void drawFilledRectangle(Screen *screen, Rectangle *rectangle) {
    unsigned int topx = rectangle -> top_left.x;
    unsigned int topy = rectangle -> top_left.y;
    unsigned int rec_width = rectangle -> size.x;
    unsigned int rec_height = rectangle -> size.y;
    unsigned int width = screen -> size.x;
    unsigned int height = screen -> size.y;
    Pixel *start = screen -> buffer;
    if ((topx + rec_width) > width) {
        rec_width = width - topx;
    }
    if ((topy + rec_height) > height) {
        rec_height = height - topy;
    }
    for (unsigned int w = 0; w < rec_width; w++) {
        for (unsigned int h = 0; h < rec_height; h++) {
            unsigned int col = topx + w;
            unsigned int row = topy + h;
            *(start + row*width + col) = rectangle -> color;
        }
    }
}


// TODO: Complete according to the prototype in graphics.h
void drawCircle(Screen *screen, Circle *circle) {
    unsigned int radius = circle -> radius;
    unsigned int cx = circle -> center.x;
    unsigned int cy = circle -> center.y;
    Vector vec;
    Pixel color = circle -> color;
    int x = 0;
    int y = radius;
    int d = 1 - radius;
    while (x <= y) {
        vec.x = cx + x;
        vec.y = cy + y;
        drawPixel(screen, vec, color);
        vec.x = cx + x;
        vec.y = cy - y;
        drawPixel(screen, vec, color);
        vec.x = cx - x;
        vec.y = cy - y;
        drawPixel(screen, vec, color);
        vec.x = cx - x;
        vec.y = cy + y;
        drawPixel(screen, vec, color);
        vec.x = cx + y;
        vec.y = cy + x;
        drawPixel(screen, vec, color);
        vec.x = cx + y;
        vec.y = cy - x;
        drawPixel(screen, vec, color);
        vec.x = cx - y;
        vec.y = cy + x;
        drawPixel(screen, vec, color);
        vec.x = cx - y;
        vec.y = cy - x;
        drawPixel(screen, vec, color);
        if (d < 0) {
            d = d + 2 * x + 3;
            x += 1;
        } else {
            d = d + 2 * (x-y) + 5;
            x += 1;
            y -= 1;
        }
    }
}

// TODO: Complete according to the prototype in graphics.h
void drawImage(Screen *screen, Image *image, CropWindow *cropWindow, Pixel (*colorFilter)(Pixel)) {
    unsigned int cropWidth, cropHeight, imWidth, imHeight, imx, imy, cropx, cropy;
    imWidth = image -> size.x;
    imHeight = image -> size.y;
    imx = image -> top_left.x;
    imy = image -> top_left.y;
    Vector vec;
    swwsif (cropWindow) {
        cropWidth = cropWindow -> size.x;
        cropHeight = cropWindow -> size.y;
        cropx = cropWindow -> top_left.x;
        cropy = cropWindow -> top_left.y;
    } else {
        cropWidth = imWidth;
        cropHeight = imHeight;
        cropx = cropy = 0;
    }
    if ((cropx + cropWidth) > imWidth) {
        cropWidth = imWidth - cropx;
    }
    if((cropy + cropHeight) > imHeight) {
        cropHeight = imHeight - cropy;
    }
    const Pixel *imStart = image -> buffer;
    for (unsigned int w = 0; w < cropWidth; w++) {
        for (unsigned int h = 0; h < cropHeight; h++) {
            vec.x = imx + w;
            vec.y = imy + h;
            drawPixel(screen, vec, colorFilter(*(imStart + (cropy + h)*imWidth + cropx + w)));
        }
    }
}

// TODO: Complete according to the prototype in graphics.              
void rotateImage(Screen *screen, Image *image, Pixel (*colorFilter)(Pixel)) {
	 unsigned int width, height, imWidth, imHeight, imx, imy;
     Pixel temp;
     width = screen -> size.x;
     height = screen -> size.y;
     imWidth = image -> size.x;
     imHeight = image -> size.y;
     imx = image -> top_left.x;
     imy = image -> top_left.y;
     
     if (((imWidth + imx) > width) | ((imHeight + imy) > height)) {
         return;
     }
     Pixel *screenStart = screen -> buffer;
     const Pixel *imStart = image -> buffer;
     for (unsigned int w = 0; w < imWidth; w++) {
        for (unsigned int h = 0; h < imHeight; h++) {
            temp = colorFilter(*(imStart + (imy + h)*imWidth + imx + w));
            *(screenStart + (imy + h)*imWidth + imx + w) = colorFilter(*(imStart + (imy + imHeight - h - 1)*imWidth + imx + imWidth - w - 1));
            *(screenStart + (imy + imHeight - h - 1)*imWidth + imx + imWidth - w - 1) = temp;
        }
    }
}
