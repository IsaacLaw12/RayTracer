#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <ctype.h>
#include "Scene.h"

Scene::Scene(std::string driver_file){
    orig_driver_file = driver_file;
    load_scene();
    // 42
}

void Scene::render_image(std::string save_image_file){
    ray_trace();
    destination_image.save_image(save_image_file);
}

void Scene::load_scene(){
    std::ifstream in(orig_driver_file);
    std::string driver_line;
    // load line identifiers
    std::string light = "light";
    std::string model = "model";
    std::string ambient = "ambient";
    while (std::getline(in, driver_line)){
        if (!driver_line.compare(0, light.size(), light))
            add_light(driver_line);
        else if(!driver_line.compare(0, model.size(), model))
            add_model(driver_line);
        else if (!driver_line.compare(0, ambient.size(), ambient))
            edit_ambient(driver_line);
        else if(valid_driver_line(driver_line)){
            // The only other driver lines should be camera specs
            edit_camera(driver_line);
        }
    }
    scene_camera.calculate_axis();
    in.close();
}

bool Scene::valid_driver_line(std::string driver_line){
    bool is_valid = true;
    if(driver_line.size() == 0){
        is_valid = false;
    } else if(isspace(driver_line[0])){
        is_valid = false;
    } else if(driver_line[0] == '#'){
        is_valid = false;
    }
    return is_valid;
}

void Scene::edit_camera(std::string driver_line){
    std::string eye = "eye";
    std::string look = "look";
    std::string up = "up";
    std::string focal_length = "d";
    std::string bounds = "bounds";
    std::string res = "res";
    std::stringstream d_line(driver_line);

    std::string line_type;
    d_line >> line_type;
    double d1, d2, d3, d4;
    if (!driver_line.compare(0, eye.size(), eye)){
        d_line >> d1 >> d2 >> d3;
        scene_camera.set_eye(d1, d2, d3);
    }else if(!driver_line.compare(0, look.size(), look)){
        d_line >> d1 >> d2 >> d3;
        scene_camera.set_look(d1, d2, d3);
    }else if (!driver_line.compare(0, up.size(), up)){
        d_line >> d1 >> d2 >> d3;
        scene_camera.set_up(d1, d2, d3);
    }else if(!driver_line.compare(0, focal_length.size(), focal_length)){
        d_line >> d1;
        scene_camera.set_focal_length(d1);
    }else if(!driver_line.compare(0, bounds.size(), bounds)){
        d_line >> d1 >> d2 >> d3 >> d4;
        scene_camera.set_bounds(d1,d2,d3,d4);
    }else if(!driver_line.compare(0, res.size(), res)){
        d_line >> d1 >> d2;
        scene_camera.set_resolution(d1, d2);
        destination_image = Image(d1, d2);
    }
}

void Scene::edit_ambient(std::string driver_line){
    std::stringstream ambient_read(driver_line);
    std::string line_type;
    ambient_read >> line_type >> ambient_red >> ambient_green >> ambient_blue;
    std::cout << "ambient red is " << ambient_red << "\n";
}

void Scene::add_model(std::string driver_line){
    Transformation tf(driver_line);
    if (tf.transform_loaded()){
        tf.transform_object();
        scene_models.push_back(tf.get_model());
    } else{
        std::cout << "Unable to extract transformation and object from " << driver_line << "\n";
    }



    std::cout << "Adding model with " << driver_line << "\n";
    std::cout << "There are now " << scene_models.size() << " models\n";
}

void Scene::add_light(std::string driver_line){
    std::cout << "Adding light with " << driver_line << "\n";
}

void Scene::ray_trace(){
    for (int i=0; i<2;i++){//scene_camera.pixel_width; i++){
        for (int j=0; j<2;j++){//scene_camera.pixel_height; j++){
            Eigen::Vector3d ray_pt = scene_camera.get_pixel_position(i, j);
            ray_dir = ray_pt - scene_camera.get_eye();
            ray_dir = ray_dir.normalized();
        }
    }
    std::cout << "starting ray tracing" << "\n";
}

double Scene::find_intersection(Eigen::Vector3d ray_pt, Eigen::Vectored ray_dir){

    for (Model model : scene_models){
        Eigen::MatrixXi model_faces = model.get_faces();
        for (int i=0; i<model_faces.cols(); i++){

        }
    }
}
