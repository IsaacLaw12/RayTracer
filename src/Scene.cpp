#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <ctype.h>
#include "Scene.h"

Scene::Scene(std::string driver_file){
    orig_driver_file = driver_file;
    load_scene();
}

Eigen::Vector3d& Scene::get_ambient(){
    return ambient;
}

std::vector<SceneObject*>& Scene::get_rendered_objects(){
    return render_objects;
}

Camera& Scene::get_camera(){
    return scene_camera;
}

Image& Scene::get_image(){
    return destination_image;
}

std::vector<Light>& Scene::get_lights(){
    return scene_lights;
}

int Scene::get_recursion(){
    return recursion_level;
}

int Scene::get_current_frame(){
    return current_frame;
}

bool Scene::advance_frame(){
    std::cout << "current: " << current_frame << " number of frames: " << number_frames << "\n";
    // Call to make the necessary changes to the scene before rendering the next frame
    if (has_next_frame()){
        render_objects.clear();
        for (auto ao:animated_objects){
            ao->advance_frame();
            render_objects.push_back(ao->get_object());
        }
        for (auto ss:scene_objects){
            render_objects.push_back(ss);
        }
        current_frame++;
        return true;
    } else{
        return false;
    }
}

bool Scene::has_next_frame(){
    // current_frame is 1 based. That means a current_frame of 0 needs to be incremented before using.
    return (current_frame < number_frames);
}

void Scene::load_scene(){
    std::ifstream in(orig_driver_file);
    std::string driver_line;
    // load line identifiers
    std::string light = "light";
    std::string model = "model";
    std::string ambient = "ambient";
    std::string sphere = "sphere";
    std::string wave = "wave";

    while (std::getline(in, driver_line)){
        if (!driver_line.compare(0, light.size(), light))
            add_light(driver_line);
        else if(!driver_line.compare(0, model.size(), model))
            add_model(driver_line);
        else if (!driver_line.compare(0, sphere.size(), sphere))
            add_sphere(driver_line);
        else if (!driver_line.compare(0, ambient.size(), ambient))
            edit_ambient(driver_line);
        else if (!driver_line.compare(0, wave.size(), wave))
            add_wave(driver_line);
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
    std::string frames = "frames";
    std::string start_frame = "start_frame";
    std::string recursion = "recursionLevel";
    std::string focus_blur = "focus_blur";
    std::string anti_alias = "anti_alias";
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
        destination_image.set_dimensions(d1, d2);
    }else if(!driver_line.compare(0, focus_blur.size(), focus_blur)){
        d_line >> d1 >> d2 >> d3;
        destination_image.set_focus_blur(d1, d2, d3);
    }else if(!driver_line.compare(0, anti_alias.size(), anti_alias)){
          d_line >> d1;
          scene_camera.set_anti_alias(d1);
          destination_image.set_anti_alias(d1);
    }else if(!driver_line.compare(0, recursion.size(), recursion)){
        d_line >> recursion_level;
    }else if(!driver_line.compare(0, frames.size(), frames)) {
        d_line >> number_frames;
        if (number_frames < 1){
            number_frames = 1;
        }
    }else if(!driver_line.compare(0, start_frame.size(), start_frame)) {
        d_line >> current_frame;
    }
}

void Scene::edit_ambient(std::string driver_line){
    std::stringstream ambient_read(driver_line);
    std::string line_type;
    double red, green, blue;
    ambient_read >> line_type >> red >> green >> blue;
    ambient << red, green, blue;
}

void Scene::add_model(std::string driver_line){
    Model* mo = new Model(driver_line);
    if (mo->is_animated()){
        AnimatedObject* ao = new AnimatedObject(mo);
        ao->set_start_frame(current_frame);
        animated_objects.push_back(ao);
    } else{
        SceneObject* so = (SceneObject*) mo;
        scene_objects.push_back(so);
    }
}

void Scene::add_sphere(std::string driver_line){
    SceneObject* so = new Sphere(driver_line);
    scene_objects.push_back(so);
}

void Scene::add_light(std::string driver_line){
    Light light(driver_line);
    scene_lights.push_back(light);
}

void Scene::add_wave(std::string driver_line){
    // wave  corner1 side_length side_length dir1 dir2 res height  num_waves material
    std::stringstream d_line(driver_line);
    std::string line_type, material_file;
    double c1x, c1y, c1z, len_one, len_two, d1x, d1y, d1z, d2x, d2y, d2z, res, height;
    int num_waves;
    d_line >>line_type>>c1x>>c1y>>c1z>>len_one>>len_two>>d1x>>d1y>>d1z>>d2x>>d2y>>d2z>>res>>height>>num_waves>>material_file;
    Eigen::Vector3d corner_one = Eigen::Vector3d(c1x, c1y, c1z);
    Eigen::Vector3d direction_one = Eigen::Vector3d(d1x, d1y, d1z);
    Eigen::Vector3d direction_two = Eigen::Vector3d(d2x, d2y, d2z);
    WaveObject* wo = new WaveObject(corner_one, len_one, len_two, direction_one, direction_two, res, material_file, height, num_waves);
    wo->set_start_frame(current_frame);
    AnimatedObject* ao = wo;
    animated_objects.push_back(ao);
}
