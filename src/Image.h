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
        void set_dimensions(int width, int height);
        void write_pixel(int index_x, int index_y, Eigen::Vector3d rgb);
        void write_t_value(int index_x, int index_y, double t_value);
        void write_t_total(int index_x, int index_y, double t_value_total);
        void save_image(std::string file_name);
        void set_focus_blur(double distance, double strength, double length);
        void set_anti_alias(int aa_level);

  private:
      int convert_255(double value);
      void blur_pixel(int index_x, int index_y, int blur_size);
      void apply_anti_alias();
      void apply_focus_blur();
      int get_pixel_blur(int index_x, int index_y);
      double get_anti_alias_val(int index_x, int index_y, int scale, Eigen::MatrixXd* cur_color);
      void prep_post_arrays();

      bool focus_blur = false;
      int anti_alias = 0;
      int image_width = 0;
      int image_height = 0;
      Eigen::MatrixXd red_pixels;
      Eigen::MatrixXd green_pixels;
      Eigen::MatrixXd blue_pixels;

      Eigen::MatrixXd post_red_pixels;
      Eigen::MatrixXd post_green_pixels;
      Eigen::MatrixXd post_blue_pixels;

      Eigen::MatrixXd t_values;
      Eigen::MatrixXd total_t_values;

      int focus_distance;
      int focus_strength; // Speed that focus blur increases with t_value
      int focus_length;  // Distance from focus_distance to start of blurring
};
#endif
