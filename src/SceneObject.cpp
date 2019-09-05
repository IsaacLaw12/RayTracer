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
    // Dummy function, could be overwritten by child classes
    return true;
}


void SceneObject::load_material(std::string material_file){
    std::ifstream in(material_file);
    if (!in){
        std::cerr << "Could not open material file " << material_file << std::endl;
        return;
    }
    std::string obj_line;
    std::string line_type = "";

    double k1, k2, k3;
    while (std::getline(in, obj_line)){
        std::stringstream obj_read(obj_line);
        obj_read >> line_type;

        if (line_type[0] == 'K' && line_type[1] == 'a'){
            obj_read >> k1 >> k2 >> k3;
            ambient_color(0, 0) = k1;
            ambient_color(1, 1) = k2;
            ambient_color(2, 2) = k3;
        }

        if (line_type[0] == 'K' && line_type[1] == 'd'){
            obj_read >> k1 >> k2 >> k3;
            diffuse_color(0, 0) = k1;
            diffuse_color(1, 1) = k2;
            diffuse_color(2, 2) = k3;
        }

        if (line_type[0] == 'K' && line_type[1] == 's'){
            obj_read >> k1 >> k2 >> k3;
            specular_color(0, 0) = k1;
            specular_color(1, 1) = k2;
            specular_color(2, 2) = k3;
        }

        if (line_type[0] == 'K' && line_type[1] == 'o'){
            obj_read >> k1 >> k2 >> k3;
            refract_color(0, 0) = k1;
            refract_color(1, 1) = k2;
            refract_color(2, 2) = k3;
        }

        if (line_type[0] == 'e'){
            if (line_type[1] == 't'){
                if (line_type[2] == 'a'){
                    obj_read >> k1;
                    eta = k1;
                }
            }
        }

        if (line_type[0] == 'N' && line_type[1] == 's'){
            obj_read >> phong;
        }

    }

}
