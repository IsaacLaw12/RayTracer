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
        void transform_object();
        void save_model(std::ostream &output);
        std::string get_model_file_name();
        bool transform_loaded();
        Model get_model();
  private:
        void create_transform_matrix(double transform_string[]);
        void generate_transform_matrix();

        std::string model_file_name = "";
        Model target_model = Model("");
        Eigen::Matrix4d normalize_z_rotate = Eigen::Matrix4d::Identity();
        Eigen::Matrix4d rotate_matrix = Eigen::Matrix4d::Identity();
        Eigen::Matrix4d scale_matrix = Eigen::Matrix4d::Identity();
        Eigen::Matrix4d translate_matrix = Eigen::Matrix4d::Identity();

        Eigen::Matrix4d final_matrix = Eigen::Matrix4d::Zero();
        bool load_successful = true;

};
#endif
