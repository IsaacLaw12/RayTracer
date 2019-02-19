#include "Model.h"
#include "Transformation.h"
#include <string>
#include <math.h>
#include <iostream>
#include <sstream>

Transformation::Transformation(std::string driver_string){
    // driver_string "model wx wy wz angle scale tx ty tz cube.obj"
    std::stringstream driver(driver_string);
    double transform_floats[8] = {};
    std::string driver_type = "";
    driver >> driver_type;

    double temp = 0;
    int counter = 0;
    while (driver >> temp){
        transform_floats[counter++] = temp;
    }
    if (counter != 8){
        // The correct transform numbers were not found
        std::cout << "WRONG NUMBERS OF MODEL ATTRIBUTES FOUND" << "\n";
        load_successful = false;
        return;
    }
    create_transform_matrix(transform_floats);
    generate_transform_matrix();

    driver.clear();
    driver >> smoothing;
    driver >> asset_name;
    driver >> lighting_group;
}

void Transformation::create_transform_matrix(double transform_string[]){
    double rot_trans_x = transform_string[0];
    double rot_trans_y = transform_string[1];
    double rot_trans_z = transform_string[2];
    double deg_angle   = transform_string[3];
    double scale       = transform_string[4];
    double translate_x = transform_string[5];
    double translate_y = transform_string[6];
    double translate_z = transform_string[7];

    // Populate rotation matrices
    Eigen::Vector3d Wv = Eigen::Vector3d(rot_trans_x, rot_trans_y, rot_trans_z).normalized();
    Eigen::Vector3d Uv = Eigen::Vector3d(Wv);
    Eigen::Vector3d Vv;

    // Make sure Uv is not parallel to Wv
    int minindex;
    Uv.minCoeff(&minindex);
    if (Uv(minindex) == -1)
        // (-1 0 0) is a special case because it would change to (1 0 0)
        //     which is parallel
        minindex = (minindex + 1) % 3;
    Uv(minindex) = 1.0;

    Uv = Wv.cross(Uv).normalized();
    Vv = Wv.cross(Uv).normalized();

    normalize_z_rotate << Uv[0], Uv[1], Uv[2], 0,
                          Vv[0], Vv[1], Vv[2], 0,
                          Wv[0], Wv[1], Wv[2], 0,
                          0,     0,     0,     1;

    double rad_angle = (deg_angle * M_PI) / 180;
    double cos_angle = cos(rad_angle);
    double sin_angle = sin(rad_angle);
    rotate_matrix(0,0) = cos_angle;
    rotate_matrix(1,1) = cos_angle;
    rotate_matrix(0,1) = -sin_angle;
    rotate_matrix(1,0) = sin_angle;

    // Populate scale matrix
    scale_matrix(0,0) = scale;
    scale_matrix(1,1) = scale;
    scale_matrix(2,2) = scale;

    // Populate translate matrix
    translate_matrix(0,3) = translate_x;
    translate_matrix(1,3) = translate_y;
    translate_matrix(2,3) = translate_z;
}

void Transformation::generate_transform_matrix(){
    Eigen::Matrix4d normalize_transpose = Eigen::Matrix4d(normalize_z_rotate);
    normalize_transpose.transposeInPlace();
    final_matrix = translate_matrix * scale_matrix * normalize_transpose * rotate_matrix * normalize_z_rotate ;
}

void Transformation::transform_object(Model* new_model){

    Eigen::MatrixXd temp = final_matrix * new_model->get_vertices();
    new_model->set_vertices( temp );
}

std::string Transformation::get_asset_name(){
    return asset_name;
}

std::string Transformation::get_smoothing(){
    return smoothing;
}

int Transformation::get_lighting_group(){
    return lighting_group;
}

bool Transformation::transform_loaded(){
    return load_successful;
}
