#include "SceneObject.h"
#include <iostream>

Eigen::Vector3d SceneObject::refract_ray(Eigen::Vector3d &w, Eigen::Vector3d &normal, double first_eta, double second_eta){
    double etar = first_eta / second_eta;
    double a = - etar;
    double wn = w.dot(normal);
    double radsq = etar*etar * (wn*wn -1) + 1;
    double b = (etar * wn) - std::sqrt(radsq);
    Eigen::Vector3d T = a * w + b * normal;
    return T;
}

double SceneObject::get_phong(){
    return phong;
}

double SceneObject::get_eta(){
    return eta;
}

void SceneObject::set_lighting_group(int group_number){
    lighting_group = group_number;
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

int SceneObject::get_lighting_group(){
    return lighting_group;
}

bool SceneObject::model_loaded(){
    // Dummy function, should be overwritten by child classes
    return true;
}
