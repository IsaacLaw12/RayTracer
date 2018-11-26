#include "SceneObject.h"

double SceneObject::intersect_ray(Ray&, Eigen::Vector3d &){
    // Object specific function
    return -1;
}

Ray SceneObject::get_refracted_ray(Ray&, Eigen::Vector3d&, Eigen::Vector3d&){
    return Ray( Eigen::Vector3d(0,0,0), Eigen::Vector3d(0,0,0));
}

Eigen::Vector3d SceneObject::refract_ray(Eigen::Vector3d &, Eigen::Vector3d&, Eigen::Vector3d&, double, double){
    return Eigen::Vector3d(0,0,0);
}

double SceneObject::get_phong(){
    return phong;
}

double SceneObject::get_eta(){
    return eta;
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

Eigen::Matrix3d SceneObject::get_kr(){
    // The amount of light that is passed back by each level of recursion
    return attenuation_color;
}

Eigen::Matrix3d SceneObject::get_ko(){
    // The amount of light reflected vs refracted?
    return refract_color;
}
