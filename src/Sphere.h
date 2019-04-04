#ifndef SPHERE_INCLUDED
#define SPHERE_INCLUDED

#include <string>
#include <iostream>
#include <vector>
#include <cmath>

// Needs to link Eigen in the makefile with g++ -I/path/to/Eigen
#include <Eigen>
#include <Core>
#include "SceneObject.h"


class Sphere: public SceneObject {
  public:
        Sphere(std::string driver_line);
        void add_to_center(Eigen::Vector3d change);
        void add_to_radius(double change);
        double intersect_ray(Ray& ray, Eigen::Vector3d &hit_normal);
        Ray get_refracted_ray(Ray &orig_ray, Eigen::Vector3d &intersect_pos, Eigen::Vector3d &normal);
        void get_refract_exit(Eigen::Vector3d &intersect_pos, Eigen::Vector3d &refract_ray, Eigen::Vector3d &exit, Eigen::Vector3d& exit_normal);
        bool ray_intersects(Ray& ray);
        bool is_animated();
        std::string get_animation_file();
        Eigen::Vector3d get_center();
        double get_radius();
  private:
        double radius;
        std::string animation_file = "";
        Eigen::Vector3d sphere_center = Eigen::Vector3d().Zero();
};
#endif
