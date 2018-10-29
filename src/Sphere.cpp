#include "Sphere.h"
#include <sstream>

Sphere::Sphere(std::string driver_line){
    double x, y, z, rad, ka_red, ka_blue, ka_green, kd_red, kd_blue, kd_green,
        ks_red, ks_blue, ks_green, katt_red, katt_blue, katt_green;

    std::stringstream read_sphere(driver_line);
    read_sphere >> x, y, z, rad, ka_red, ka_blue, ka_green, kd_red, kd_blue, kd_green,
        ks_red, ks_blue, ks_green, katt_red, katt_blue, katt_green;

    sphere_center << x, y, z;
    radius = rad;

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
