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

void Image::write_pixel(int index_x, int index_y, Eigen::Vector3d rgb){
    red_pixels(index_x, index_y) = rgb(0);
    green_pixels(index_x, index_y) = rgb(1);
    blue_pixels(index_x, index_y) = rgb(2);
}

void Image::save_image(std::string file_name){
    std::ofstream output(file_name);
    output << "P3" << "\n";
    output << image_width << " " << image_height << " 255" << "\n";

    int red, green, blue;
    for (int i=image_height-1;i>=0; i--){
        for (int j=0; j<image_width; j++){
            red = convert_255(red_pixels(j, i));
            green = convert_255(green_pixels(j, i));
            blue = convert_255(blue_pixels(j, i));
            output << red << " " << green << " " << blue << " ";
        }
        output << "\n";
    }
    output.close();
    std::cout << "Image saved at: " << file_name << "\n";
}

int Image::convert_255(double value){
    // Convert a 0-1 double value to 0-255 int value
    value = value * 255.0;
    return int(value);
}
