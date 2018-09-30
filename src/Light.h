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

  private:
      void assign_values(double spec_nums[]);

      double x_cord;
      double y_cord;
      double z_cord;
      double w_value;
      double red_color;
      double green_color;
      double blue_color;

      bool load_successful = true;
};
#endif
