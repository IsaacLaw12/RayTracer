#ifndef SPHERE_INCLUDED
#define SPHERE_INCLUDED

#include <string>
#include <vector>
#include <cmath>

// Needs to link Eigen in the makefile with g++ -I/path/to/Eigen
#include <Eigen>
#include <Core>


class Sphere: public SceneObject {
  public:
        Sphere(std::string driver_line);
  private:
        double radius;
        Eigen::Vector3d sphere_center = Eigen::Vector3d().Zero();
};
#endif
