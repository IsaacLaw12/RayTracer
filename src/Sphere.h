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
        double intersect_ray(Eigen::Vector3d ray_pt, Eigen::Vector3d ray_dir, Eigen::Vector3d &hit_normal);
        bool ray_intersects(Eigen::Vector3d ray_pt, Eigen::Vector3d ray_dir);
  private:
        double radius;
        Eigen::Vector3d sphere_center = Eigen::Vector3d().Zero();
};
#endif
