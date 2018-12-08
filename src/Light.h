#ifndef LIGHT_INCLUDED
#define LIGHT_INCLUDED

#include <iostream>
#include <string>
#include <vector>
#include "Model.h"

// Needs to link Eigen in the makefile with g++ -I/path/to/Eigen
#include <Eigen>
#include <Core>


class Light {
  public:
        Light(std::string driver_line);
        bool light_valid();
        Eigen::Vector3d get_pos();
        Eigen::Vector3d get_color();
        int get_lighting_group();

  private:
      void assign_values(double spec_nums[]);
      int lighting_group = 0;
      double w_value;
      Eigen::Vector3d light_pos;
      Eigen::Vector3d light_color;

      bool load_successful = true;
};
#endif
