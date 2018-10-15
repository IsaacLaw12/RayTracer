#include "Image.h"

Image::Image(int width, int height){
    image_width = width;
    image_height = height;
    red_pixels = Eigen::MatrixXd();
    green_pixels = Eigen::MatrixXd();
    blue_pixels = Eigen::MatrixXd();
    red_pixels.conservativeResize(image_width, image_height);
    green_pixels.conservativeResize(image_width, image_height);
    blue_pixels.conservativeResize(image_width, image_height);
}

void Image::save_image(std::string file_name){
    std::cout << "Saving image at file: " << file_name << "\n";
}
