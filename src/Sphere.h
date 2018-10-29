#ifndef SPHERE_INCLUDED
#define SPHERE_INCLUDED

#include <string>
#include <vector>

// Needs to link Eigen in the makefile with g++ -I/path/to/Eigen
#include <Eigen>
#include <Core>


class Sphere {
  public:
        Sphere(std::string driver_line);
  private:
        double radius;
        Eigen::Vector3d sphere_center = Eigen::Vector3d().Zero();
        Eigen::Matrix3d ambient_color = Eigen::Matrix3d().Zero();
        Eigen::Matrix3d diffuse_color = Eigen::Matrix3d().Zero();
        Eigen::Matrix3d specular_color = Eigen::Matrix3d().Zero();
        Eigen::Matrix3d attenuation_color = Eigen::Matrix3d().Zero();
};
#endif
