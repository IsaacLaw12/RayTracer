#include <Eigen>
#include <Core>
#include <iostream>
#include "Camera.h"

Camera::Camera(){
    std::cout << "Creating Camera object" << std::endl;
}

void Camera::set_eye(double x_pos, double y_pos, double z_pos){
    eye = Eigen::Vector3d(x_pos, y_pos, z_pos);
    std::cout << "Camera eye is now " << eye << "\n";
}

void Camera::set_look(double x_dir, double y_dir, double z_dir){
    look = Eigen::Vector3d(x_dir, y_dir, z_dir);
    std::cout << "Camera look is now " << look << "\n";
}

void Camera::set_up(double x_dir, double y_dir, double z_dir){
    up_dir = Eigen::Vector3d(x_dir, y_dir, z_dir);
    std::cout << "Camera up is now " << up_dir << "\n";
}

void Camera::set_focal_length(double foc_length){
    focal_length = foc_length;
    std::cout << "Camera focal_length is now " << focal_length << "\n";
}

void Camera::set_bounds(double x1, double y1, double x2, double y2){
    view_plane_x1 = x1;
    view_plane_y1 = y1;
    view_plane_x2 = x2;
    view_plane_y2 = y2;
    std::cout << "Bounds are " << view_plane_x1 << " " << view_plane_y1 << " " << view_plane_x2 << " " << view_plane_y2 << " " << "\n";
}

void Camera::set_resolution(int width, int height){
    pixel_width = width;
    pixel_height = height;
    std::cout << "Camera res is now " << width << "by" << height << "\n";
}

void Camera::calculate_axis(){
    z_axis = eye - look;
    z_axis = z_axis.normalized();
    x_axis = up_dir.cross(z_axis).normalized();
    y_axis = z_axis.cross(x_axis).normalized();

}

Eigen::Vector3d Camera::get_pixel_position(int x_pixel, int y_pixel){
    double px = x_pixel / ( (pixel_width-1) * (view_plane_x2 - view_plane_x1)) + view_plane_x1;
    double py = y_pixel / ( (pixel_height-1) * (view_plane_y2 - view_plane_y1)) + view_plane_y1;
    Eigen::Vector3d pixpt = eye + (-1 * focal_length * z_axis) + (px * x_axis) + (py * y_axis);
    return pixpt;
}

Eigen::Vector3d Camera::get_eye(){
    return eye;
}
