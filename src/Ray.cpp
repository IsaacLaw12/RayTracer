#include "Ray.h"

Ray::Ray(){
    dir = Eigen::Vector3d::Zero();
    point = Eigen::Vector3d::Zero();
}

Ray::Ray(Eigen::Vector3d start_point, Eigen::Vector3d ray_dir){
    set_point(start_point);
    ray_dir = ray_dir.normalized();
    set_dir(ray_dir);
}

void Ray::set_point(Eigen::Vector3d start_point){
    point = start_point;
}

void Ray::set_dir(Eigen::Vector3d ray_dir){
    dir = ray_dir;
}

const Eigen::Vector3d& Ray::get_point(){
    return point;
}

const Eigen::Vector3d& Ray::get_dir(){
    return dir;
}

Eigen::Vector3d Ray::get_ray_loc(double distance){
    // Returns the location that the ray reaches when it has progresssed distance along the ray
    return point + dir * distance;
}
