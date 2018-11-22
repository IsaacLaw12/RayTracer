#include "Sphere.h"
#include <sstream>

Sphere::Sphere(std::string driver_line){
    double x, y, z, rad, ka_red, ka_blue, ka_green, kd_red, kd_blue, kd_green,
        ks_red, ks_blue, ks_green, katt_red, katt_blue, katt_green;

    std::string driver_type;

    std::stringstream read_sphere(driver_line);
    read_sphere >> driver_type >> x >> y >> z >> rad >> ka_red >> ka_blue >> ka_green >> kd_red >> kd_blue >> kd_green >>
        ks_red >> ks_blue >> ks_green>> katt_red>> katt_blue>> katt_green;

    sphere_center << x, y, z;
    radius = rad;
    // Just a constant used for spheres
    phong = 16;

    ambient_color(0, 0) = ka_red;
    ambient_color(1, 1) = ka_blue;
    ambient_color(2, 2) = ka_green;

    diffuse_color(0, 0) = kd_red;
    diffuse_color(1, 1) = kd_blue;
    diffuse_color(2, 2) = kd_green;

    specular_color(0, 0) = ks_red;
    specular_color(1, 1) = ks_blue;
    specular_color(2, 2) = ks_green;

    attenuation_color(0, 0) = katt_red;
    attenuation_color(1, 1) = katt_blue;
    attenuation_color(2, 2) = katt_green;
}


double Sphere::intersect_ray(Ray& ray, Eigen::Vector3d &hit_normal){
    const Eigen::Vector3d &ray_pt = ray.get_point();
    const Eigen::Vector3d &ray_dir = ray.get_dir();
    double t_value = -1;
    double v = (sphere_center - ray_pt).dot(ray_dir);
    double c = (sphere_center - ray_pt).norm();
    // Either the ray went the wrong way, or there was no intersection
    if (v < 0 || ((radius*radius) < (c*c - v*v))){
        return t_value;
    }
    double dr = (ray_pt + (v * ray_dir) - sphere_center).norm();
    double intersection_offset = sqrt( radius*radius - dr*dr);
    t_value = v - intersection_offset;

    Eigen::Vector3d intersection_point = ray_pt + ray_dir * t_value;
    hit_normal = (intersection_point - sphere_center).normalized();
    return t_value;
}

Eigen::Vector3d Sphere::refract_ray(Ray& w, Eigen::Vector3d& intersect_pos, Eigen::Vector3d& normal){
    double etar = get_eta() / 1.0;
    double a = - etar;
    double wn = w.dot(normal);
    double radsq = etar*etar * (wn*wn -1) + 1;
    double b = (etar * wn) - std::sqrt(radsq);
    Eigen::Vector3d T = a * w + b * N;
    return T;
}

Ray Sphere::get_refracted_ray(Ray& w, Eigen::Vector3d& intersect_pos, Eigen::Vector3d& normal){
    Eigen::Vector3d T1, T2;
    T1 = refract_ray(w, intersect_pos, (intersect_pos - sphere_center).normalized() );
    Eigen::Vector3d exit = intersect_pos + 2 * (sphere_center - intersect_pos).dot(T1) * T1;
    Eigen::Vector3d Nin = (sphere_center - exit).normalized();
    Eigen::Vector3d T2 = refract_ray(-T1, exit, Nin);
    Ray exit_ray = Ray(exit, T2);
    return exit_ray;
}

bool Sphere::ray_intersects(Ray& ray){
    double v = (sphere_center - ray.get_point()).dot(ray.get_dir());
    double c = (sphere_center - ray.get_point()).norm();
    if ((radius*radius) > (c*c - v*v)){
        return true;
    } else{
        return false;
    }
}
