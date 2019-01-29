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

std::vector<SceneObject*>& Scene::get_objects(){
    return scene_objects;
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

bool Scene::advance_frame(){
    std::cout << "current: " << current_frame << " number of frames: " << number_frames << "\n";
    // Call to make the necessary changes to the scene before rendering the next frame
    if (has_next_frame()){
        scene_objects.clear();
        current_frame++;
        for (auto ao:animated_objects){
            ao->advance_frame();
            scene_objects.push_back(ao->get_object());
        }
        for (auto ss:scene_spheres){
            scene_objects.push_back(ss);
        }
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

    while (std::getline(in, driver_line)){
        if (!driver_line.compare(0, light.size(), light))
            add_light(driver_line);
        else if(!driver_line.compare(0, model.size(), model))
            add_model(driver_line);
        else if (!driver_line.compare(0, sphere.size(), sphere))
            add_sphere(driver_line);
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
    std::string frames = "frames";
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
    AnimatedObject* ao = new AnimatedObject(driver_line);
    animated_objects.push_back(ao);
}

void Scene::add_sphere(std::string driver_line){
    Sphere* sp = new Sphere(driver_line);
    scene_spheres.push_back(sp);
}

void Scene::add_light(std::string driver_line){
    Light light(driver_line);
    scene_lights.push_back(light);
}
