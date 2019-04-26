#include "Sphere.h"
#include <sstream>

Sphere::Sphere(std::string driver_line){
    double x, y, z, rad, ka_red, ka_blue, ka_green, kd_red, kd_blue, kd_green,
        ks_red, ks_blue, ks_green, katt_red, katt_blue, katt_green;

    // If driver file does not include refraction constants default to 1, no refraction;
    double ref_red, ref_green, ref_blue;
    ref_red = 1; ref_green = 1; ref_blue = 1;

    std::string driver_type;

    std::stringstream read_sphere(driver_line);
    read_sphere >> driver_type >> x >> y >> z >> rad >> ka_red >> ka_blue >> ka_green >> kd_red >> kd_blue >> kd_green >>
        ks_red >> ks_blue >> ks_green>> katt_red>> katt_blue>> katt_green >> ref_red >> ref_green >> ref_blue >> phong >> eta >> lighting_group >> animation_file;

    sphere_center << x, y, z;
    radius = rad;
    // Just a constant used for spheres


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

    refract_color(0, 0) = ref_red;
    refract_color(1, 1) = ref_green;
    refract_color(2, 2) = ref_blue;
}

void Sphere::add_to_center(Eigen::Vector3d change){
    sphere_center = sphere_center + change;
}

void Sphere::add_to_radius(double change){
    radius = radius + change;
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

Ray Sphere::get_refracted_ray(Ray &orig_ray, Eigen::Vector3d &intersect_pos, Eigen::Vector3d &normal){
    Eigen::Vector3d T1, T2;
    double this_eta = get_eta();
    Eigen::Vector3d W = -1 * orig_ray.get_dir();
    T1 = refract_ray(W, normal, 1.0, this_eta);
    Eigen::Vector3d exit, Nin;
    get_refract_exit(intersect_pos, T1, exit, Nin);
    T1 = -1 * T1;
    T2 = refract_ray(T1, Nin, this_eta, 1.0);
    Ray exit_ray = Ray(exit, T2);
    return exit_ray;
}

void Sphere::get_refract_exit(Eigen::Vector3d &intersect_pos, Eigen::Vector3d &refract_ray, Eigen::Vector3d &exit, Eigen::Vector3d& exit_normal){
    exit = intersect_pos + 2 * (sphere_center - intersect_pos).dot(refract_ray) * refract_ray;
    exit_normal = (sphere_center - exit).normalized();
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

bool Sphere::is_animated(){
    return animation_file.size() > 0;
}

std::string Sphere::get_animation_file(){
    return animation_file;
}

Eigen::Vector3d Sphere::get_center(){
    return sphere_center;
}

double Sphere::get_radius(){
    return radius;
}
