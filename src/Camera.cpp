#include <Eigen>
#include <Core>
#include <iostream>
#include "Camera.h"

Camera::Camera(){}

void Camera::set_eye(double x_pos, double y_pos, double z_pos){
    eye = Eigen::Vector3d(x_pos, y_pos, z_pos);
}

void Camera::set_look(double x_dir, double y_dir, double z_dir){
    look = Eigen::Vector3d(x_dir, y_dir, z_dir);
}

void Camera::set_up(double x_dir, double y_dir, double z_dir){
    up_dir = Eigen::Vector3d(x_dir, y_dir, z_dir);
}

void Camera::set_focal_length(double foc_length){
    focal_length = foc_length;
}

void Camera::set_bounds(double x1, double y1, double x2, double y2){
    near_bounding_left = x1;
    near_bounding_bottom = y1;
    near_bounding_right = x2;
    near_bounding_top = y2;
 }

void Camera::set_resolution(int width, int height){
    pixel_width = width * (anti_alias + 1);
    pixel_height = height * (anti_alias + 1);
}

void Camera::set_anti_alias(int aa_level){
    anti_alias = aa_level;
    set_resolution(pixel_width, pixel_height);
}

void Camera::calculate_axis(){
    z_axis = eye - look;
    z_axis = z_axis.normalized();
    x_axis = up_dir.cross(z_axis).normalized();
    y_axis = z_axis.cross(x_axis).normalized();

}

Eigen::Vector3d Camera::get_pixel_position(int x_pixel, int y_pixel){

    double px = x_pixel / (pixel_width-1.0) * (near_bounding_right - near_bounding_left) + near_bounding_left;
    double py = y_pixel / (pixel_height-1.0) * (near_bounding_top - near_bounding_bottom) + near_bounding_bottom;

    Eigen::Vector3d pixpt = eye + (-1 * focal_length * z_axis) + (px * x_axis) + (py * y_axis);
    return pixpt;
}

Eigen::Vector3d Camera::get_eye(){
    return eye;
}
