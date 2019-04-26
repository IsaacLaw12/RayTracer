#ifndef BOUNDINGBOX_INCLUDED
#define BOUNDINGBOX_INCLUDED

#include <string>
#include <iostream>
#include <vector>
#include <set>
#include <memory>
#include "Ray.h"
#include <limits>

// Needs to link Eigen in the makefile with g++ -I/path/to/Eigen
#include <Eigen>
#include <Core>


class BoundingBox {
  public:
        BoundingBox(Eigen::Vector3d min_corn, Eigen::Vector3d max_corn);
        BoundingBox();
        Eigen::Vector3d get_min_corner();
        Eigen::Vector3d get_max_corner();
        bool ray_intersects(Ray& ray);
        bool ray_intersects(Ray& ray, double &tmin, double & tmax);
  protected:
        Eigen::Vector3d min_corner = Eigen::Vector3d();
        Eigen::Vector3d max_corner = Eigen::Vector3d();
};
#endif
