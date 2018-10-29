#ifndef SCENEOBJ_INCLUDED
#define SCENEOBJ_INCLUDED

// Needs to link Eigen in the makefile with g++ -I/path/to/Eigen
#include <Eigen>
#include <Core>


class SceneObject {
    public:
        double intersect_ray(Eigen::Vector3d ray_pt, Eigen::Vector3d ray_dir, Eigen::Vector3d &hit_normal);

        Eigen::Matrix3d get_diffuse_color();
        Eigen::Matrix3d get_ambient_color();
        Eigen::Matrix3d get_specular_color();
        Eigen::Matrix3d get_attenuation_color();

        Eigen::Matrix3d ambient_color = Eigen::Matrix3d().Zero();
        Eigen::Matrix3d diffuse_color = Eigen::Matrix3d().Zero();
        Eigen::Matrix3d specular_color = Eigen::Matrix3d().Zero();
        Eigen::Matrix3d attenuation_color = Eigen::Matrix3d().Zero();
}
#endif
