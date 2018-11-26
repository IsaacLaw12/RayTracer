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
        double intersect_ray(Ray& ray, Eigen::Vector3d &hit_normal);
        Ray get_refracted_ray(Ray &w, Eigen::Vector3d &intersect_pos, Eigen::Vector3d &normal);
        Eigen::Vector3d refract_ray(Eigen::Vector3d &w, Eigen::Vector3d &intersect_pos, Eigen::Vector3d &normal, double first_eta, double second_eta);
        bool ray_intersects(Ray& ray);
  private:
        double radius;
        Eigen::Vector3d sphere_center = Eigen::Vector3d().Zero();
};
#endif
