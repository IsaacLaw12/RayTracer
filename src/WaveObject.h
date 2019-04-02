#ifndef WAVEOBJ_INCLUDED
#define WAVEOBJ_INCLUDED

#include "SceneObject.h"
#include "BoundingBox.h"

// Needs to link Eigen in the makefile with g++ -I/path/to/Eigen
#include <Eigen>
#include <Core>
#include <math.h>
#include <random>
#include <iostream>
#include <memory>

struct wave_parameters{
    Eigen::Vector3d direction;
    double amplitude;
    double frequency;
    double phase_constant;
};

class WaveObject: public SceneObject {
  public:
      WaveObject(std::string driver_line, int current_time);
      Ray get_refracted_ray(Ray &ray, Eigen::Vector3d &n, Eigen::Vector3d &m);
      double intersect_ray(Ray& ray, Eigen::Vector3d &hit_normal);
      void set_time(int current_time);
      void extract_wave_info(std::string driver_line);

  private:
      void set_bounding_box();
      void generate_waves(int num_waves, int random_seed);
      void find_min_max(Eigen::Vector3d &min, Eigen::Vector3d &max);
      void extract_vector_extremes(Eigen::Vector3d original, Eigen::Vector3d &min, Eigen::Vector3d &max);
      Eigen::Vector3d get_wave_direction(double angle);
      Eigen::Vector3d wave_function(double offset_one, double offset_two);
      double step_search(Ray& ray, double start, double end);
      double refine_search(Ray &ray, double step_end);
      bool is_above_wave(Eigen::Vector3d loc);
      Eigen::Vector3d find_surf_norm(Ray &ray, double t_value);
      void find_offsets(Eigen::Vector3d loc, double &offset_one, double &offset_two);


      double time = 0;
      std::unique_ptr<BoundingBox> bounding_box;
      std::vector<wave_parameters> component_waves;
      double search_step = 1;
      double wave_height = .5;
      int num_waves = 1;
      double side_len_one = 0;
      double side_len_two = 0;
      Eigen::Vector3d first_corner;
      Eigen::Vector3d second_corner;
      Eigen::Vector3d direction_one;
      Eigen::Vector3d direction_two;
      Eigen::Vector3d surface_normal;
};
#endif
