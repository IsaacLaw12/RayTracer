#ifndef SCENEOBJ_INCLUDED
#define SCENEOBJ_INCLUDED

// Needs to link Eigen in the makefile with g++ -I/path/to/Eigen
#include <Eigen>
#include <Core>
#include <cmath>

#include "Ray.h"


class SceneObject {
    public:
        virtual double intersect_ray(Ray&, Eigen::Vector3d &)=0;
        virtual Ray get_refracted_ray(Ray&, Eigen::Vector3d&, Eigen::Vector3d&)=0;
        Eigen::Vector3d refract_ray(Eigen::Vector3d &w, Eigen::Vector3d &normal, double first_eta, double second_eta);
 //       virtual void get_refract_exit(Eigen::Vector3d &, Eigen::Vector3d &, Eigen::Vector3d &, Eigen::Vector3d&);

        double get_phong();
        double get_eta();
        Eigen::Matrix3d get_diffuse_color();
        Eigen::Matrix3d get_ambient_color();
        Eigen::Matrix3d get_specular_color();
        Eigen::Matrix3d get_attenuation_color();
        Eigen::Matrix3d get_kr();
        Eigen::Matrix3d get_ko();
        int get_lighting_group();
    protected:
        double phong = 16;
        double eta = 1;
        int lighting_group = 0;
        Eigen::Matrix3d ambient_color = Eigen::Matrix3d().Zero();
        Eigen::Matrix3d diffuse_color = Eigen::Matrix3d().Zero();
        Eigen::Matrix3d specular_color = Eigen::Matrix3d().Zero();
        Eigen::Matrix3d attenuation_color = Eigen::Matrix3d().Identity();
        Eigen::Matrix3d refract_color = Eigen::Matrix3d().Identity();
};
#endif
