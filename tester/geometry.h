/**
* @file geometry.h
* @author Cem Gokmen, Vivian Thiebaut
* @date June 28, 2018
* @brief A geometry library describing a variety of geometric objects we expect
*        in drawings.
*/

// The stdint library contains the uint16_t type, which is an unsigned 16-bit
// integer that we typedef as a Pixel.
#include <stdint.h>

// We are typedefing an unsigned 16-bit integer as our Pixel type. Your code
// should use ONLY this type for reading and writing Pixels, so that if this
// typedef is changed to an 32-bit int on a system that supports deeper color,
// your code still performs accurately.
typedef uint16_t Pixel;

// The Vector struct is simply a pair of unsigned int x,y coordinates: a named
// tuple if you're thinking in Python.
typedef struct {
    unsigned int x, y;
} Vector;

// The Screen struct contains all information about a screen that you can draw
// on.
typedef struct {
    Vector size; // size of the screen: width in the x field, height in the y
    Pixel* buffer; // the video buffer / pixel array to write onto
} Screen;

// The Circle struct contains all information about a circle that you can draw.
// We will draw unfilled circles.
typedef struct {
    Vector center; // coordinates (width, height) of the center of the circle
    unsigned int radius; // radius of the circle
    Pixel color; // the color of the circle
} Circle;


// The Rectangle struct contains all information about a rectangle that you can draw. We will draw filled rectangles.
typedef struct {
    Vector top_left; // the coordinates of the top left vertex of the rectangle
    Vector size; // the size of the rectangle (width, height)
    Pixel color; // the fill color of the rectangle
} Rectangle;


// The Image struct contains all information about an image that you can draw.
typedef struct {
    Vector top_left; // the coordinates of the top left vertex of the rectangle on the screen
    Vector size; // the size of the original image (width, height)
    const Pixel* buffer; // the pixel array to read the image pixels from
} Image;

// The CropWindow struct will be used to crop a section of an image.
typedef struct {
    Vector top_left; // the coordinates of the top left vertex of the crop
                     // window, relative to the top-left of the image.
    Vector size; // the size of the crop window
} CropWindow;
