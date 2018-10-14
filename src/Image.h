#ifndef IMAGE_INCLUDED
#define IMAGE_INCLUDED

#include <iostream>
#include "Model.h"

// Needs to link Eigen in the makefile with g++ -I/path/to/Eigen
#include <Eigen>
#include <Core>


class Image {
  public:
        Image(int width, int height);
        //write_pixel(int index_x, int index_y, Eigen::Vector3d rgb);

  private:
      int image_width;
      int image_height;
      Eigen::MatrixXd red_pixels = Eigen::MatrixXd();
      Eigen::MatrixXd green_pixels = Eigen::MatrixXd();
      Eigen::MatrixXd blue_pixels = Eigen::MatrixXd();
};
#endif
