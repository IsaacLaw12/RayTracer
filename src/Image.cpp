#include "Image.h"

Image::Image(int width, int height){
    image_width = width;
    image_height = height;
    red_pixels.conservativeResize(image_width, image_height);
    green_pixels.conservativeResize(image_width, image_height);
    blue_pixels.conservativeResize(image_width, image_height);
}
