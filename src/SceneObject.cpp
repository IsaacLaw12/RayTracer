#include "SceneObject.h"

double SceneObject::intersect_ray(Eigen::Vector3d ray_pt, Eigen::Vector3d ray_dir, Eigen::Vector3d &hit_normal){
    // Object specific function
    return -1;
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
