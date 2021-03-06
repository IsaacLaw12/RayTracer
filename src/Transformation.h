#ifndef TRANSFORM_INCLUDED
#define TRANSFORM_INCLUDED

#include <string>
#include <vector>
#include <sstream>
#include <cmath>
#include "Model.h"

// Needs to link Eigen in the makefile with g++ -I/path/to/Eigen
#include <Eigen>
#include <Core>

class Transformation {
  public:
        Transformation(std::string driver_file);
        Transformation(std::vector<double> transform_floats);
        void transform_object(Model* new_model);
        std::string get_asset_name();
        std::string get_animation_file();
        bool get_smoothing();
        int get_lighting_group();
        bool transform_loaded();
  private:
        void create_transform_matrix(double transform_string[]);
        void generate_transform_matrix();

        std::string asset_name = "";
        std::string animation_file = "";
        Model* target_model;
        Eigen::Matrix4d normalize_z_rotate = Eigen::Matrix4d::Identity();
        Eigen::Matrix4d rotate_matrix = Eigen::Matrix4d::Identity();
        Eigen::Matrix4d scale_matrix = Eigen::Matrix4d::Identity();
        Eigen::Matrix4d translate_matrix = Eigen::Matrix4d::Identity();

        Eigen::Matrix4d final_matrix = Eigen::Matrix4d::Zero();
        bool load_successful = true;
        std::string smoothing = "";
        int lighting_group = 0;
};
#endif
