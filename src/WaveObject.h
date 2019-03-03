#ifndef WAVEOBJ_INCLUDED
#define WAVEOBJ_INCLUDED

class BoundingBox;
#include "Model.h"
#include "AnimatedObject.h"

// Needs to link Eigen in the makefile with g++ -I/path/to/Eigen
#include <Eigen>
#include <Core>
#include <math.h>
#include <random>

struct wave_parameters{
    Eigen::Vector3d direction;
    double amplitude;
    double frequency;
    double phase_constant;
};

class WaveObject: public AnimatedObject {
  public:
      WaveObject(Eigen::Vector3d corner_one, double len_one, double len_two, Eigen::Vector3d dir_one, Eigen::Vector3d dir_two, int res, std::string material_file, double height, int number_waves);
      void advance_frame();
      void update_model();
      void set_height(double height);
      void set_num_waves(int waves);
      Model* get_object();
  private:
      Eigen::Vector3d wave_function(int offset_one, int offset_two, double time);
      void generate_waves(int num_waves, int random_seed);
      Eigen::Vector3d get_wave_direction(double angle);
      double find_grid_cell_size();
      void find_side_lengths(double& side_one, double& side_two);
      Eigen::MatrixXi create_faces(Eigen::MatrixXd &vertices, int row_length);
      void add_face(Eigen::MatrixXi &faces, int a, int b, int c);

      Model* wave_object;
      std::vector<wave_parameters> component_waves;
      int resolution = 10;
      double wave_height = .5;
      int num_waves = 1;
      double side_one_len = 1;
      double side_two_len = 1;
      Eigen::Vector3d first_corner;
      Eigen::Vector3d second_corner;
      Eigen::Vector3d direction_one;
      Eigen::Vector3d direction_two;
};
#endif
