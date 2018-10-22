#ifndef IMAGE_INCLUDED
#define IMAGE_INCLUDED

#include <iostream>
#include <fstream>
#include <string>
#include "Model.h"

// Needs to link Eigen in the makefile with g++ -I/path/to/Eigen
#include <Eigen>
#include <Core>


class Image {
  public:
        Image(int width, int height);
        void write_pixel(int index_x, int index_y, Eigen::Vector3d rgb);
        void save_image(std::string file_name);

  private:
      int convert_255(double value);

      int image_width;
      int image_height;
      Eigen::MatrixXd red_pixels;
      Eigen::MatrixXd green_pixels;
      Eigen::MatrixXd blue_pixels;
};
#endif
