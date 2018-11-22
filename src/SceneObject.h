#ifndef SCENEOBJ_INCLUDED
#define SCENEOBJ_INCLUDED

// Needs to link Eigen in the makefile with g++ -I/path/to/Eigen
#include <Eigen>
#include <Core>
#include <cmath>

#include "Ray.h"


class SceneObject {
    public:
        virtual double intersect_ray(Ray&, Eigen::Vector3d &);
        virtual Ray get_refracted_ray(Ray&, Eigen::Vector3d&, Eigen::Vector3d&);
        virtual Eigen::Vector3d refract_ray(Ray&, Eigen::Vector3d&, Eigen::Vector3d&);

        double get_phong();
        double get_eta();
        Eigen::Matrix3d get_diffuse_color();
        Eigen::Matrix3d get_ambient_color();
        Eigen::Matrix3d get_specular_color();
        Eigen::Matrix3d get_attenuation_color();
        Eigen::Vector3d get_kr();
        Eigen::Vector3d get_ko();
    protected:
        double phong;
        Eigen::Matrix3d ambient_color = Eigen::Matrix3d().Zero();
        Eigen::Matrix3d diffuse_color = Eigen::Matrix3d().Zero();
        Eigen::Matrix3d specular_color = Eigen::Matrix3d().Zero();
        Eigen::Matrix3d attenuation_color = Eigen::Matrix3d().Zero();
};
#endif
