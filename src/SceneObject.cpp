#include "SceneObject.h"

double SceneObject::intersect_ray(Ray&, Eigen::Vector3d &){
    // Object specific function
    return -1;
}

double SceneObject::get_phong(){
    return phong;
}

double SceneObject::get_eta(){
    return 2;
}

Eigen::Matrix3d SceneObject::get_diffuse_color(){
    return diffuse_color;
}

Eigen::Matrix3d SceneObject::get_ambient_color(){
    return ambient_color;
}

Eigen::Matrix3d SceneObject::get_specular_color(){
    return specular_color;
}

Eigen::Matrix3d SceneObject::get_attenuation_color(){
    return attenuation_color;
}

Eigen::Vector3d SceneObject::get_kr(){
    // The amount of light that is passed back by each level of recursion
    Eigen::Vector3d kr(1,1,1);
    return kr;
}

Eigen::Vector3d SceneObject::get_ko(){
    // The amount of light reflected vs refracted?
    Eigen::Vector3d ko(1,1,1);
    return ko;
}
