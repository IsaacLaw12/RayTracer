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
    std::string sphere = "sphere";

    while (std::getline(in, driver_line)){
        if (!driver_line.compare(0, light.size(), light))
            add_light(driver_line);
        else if(!driver_line.compare(0, model.size(), model))
            add_model(driver_line);
        else if (!driver_line.compare(0, ambient.size(), sphere))
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
    double red, green, blue;
    ambient_read >> line_type >> red >> green >> blue;
    ambient << red, green, blue;

}

void Scene::add_model(std::string driver_line){
    Transformation tf(driver_line);
    if (tf.transform_loaded()){
        tf.transform_object();
        Model* mod = tf.get_model();
        scene_models.push_back(mod);
    } else{
        std::cout << "Unable to extract transformation and object from " << driver_line << "\n";
    }

    std::cout << "There are now " << scene_models.size() << " models\n";
}

void Scene::add_sphere(std::string driver_line){
    Sphere sp(driver_line);
    if (sp.load_successful()){
        scene_spheres.push_back(*sp);
    } else{
        std::cout << "Unable to create sphere from: " << driver_line << "\n";
    }
}

void Scene::add_light(std::string driver_line){
    Light light(driver_line);
    scene_lights.push_back(light);
}

void Scene::ray_trace(){
    double t_value;
    Eigen::Vector3d ray_pt, ray_dir, color, hit_normal;
    Model* hit_model = scene_models[0];

    std::cout << "starting ray tracing" << "\n";
    for (int i=0; i<scene_camera.pixel_width; i++){
        if (! (i%64)){
            std::cout << "Row: " << i << " / " << scene_camera.pixel_height << "\n";
        }
        for (int j=0; j<scene_camera.pixel_height; j++){
            ray_pt = scene_camera.get_pixel_position(i, j);
            ray_dir = (ray_pt - scene_camera.get_eye()).normalized();
            t_value = find_intersection(ray_pt, ray_dir, hit_model, hit_normal);
            //std::cout << "T-value: " << t_value << "\n";
            color = calculate_color(ray_pt, ray_dir, t_value, hit_model, hit_normal);
            destination_image.write_pixel(i, j, color);
        }
     }
     std::cout << "end of ray tracing \n";
}

double Scene::find_intersection(Eigen::Vector3d ray_pt, Eigen::Vector3d ray_dir, Model* md, Eigen::Vector3d &hit_normal){
    double intersect_t = -1;
    Eigen::Vector3d hit_s_normal;
    for (Model* model:scene_models){
        double t_value = model->intersect_ray(ray_pt, ray_dir, hit_s_normal);
        if ( (intersect_t == -1) || (t_value < intersect_t)){
            intersect_t = t_value;
            md = model;
            hit_normal = hit_s_normal;
        }
    }
    return intersect_t;
}

Eigen::Vector3d Scene::calculate_color(Eigen::Vector3d ray_pt, Eigen::Vector3d ray_dir, double t_value, Model* hit_model, Eigen::Vector3d &hit_normal){
    Eigen::Vector3d vector_to_light, intersect_pos, dif_refl, color;
    if (t_value < 0){
        color << 0,0,0;
        return color;
    }
    color = hit_model->get_ambient_color() * ambient;
    double light_concentration;
    intersect_pos = ray_pt + t_value * ray_dir;
    for(Light light:scene_lights){
        // Diffuse reflection
        vector_to_light = (light.get_pos() - intersect_pos).normalized();
        light_concentration = hit_normal.dot(vector_to_light);
        if (light_concentration < 0){
            light_concentration = (-1 * hit_normal).dot(vector_to_light);
        }
        dif_refl =  hit_model->get_diffuse_color() * light.get_color() * light_concentration;
        color += dif_refl;

        // Specular reflection
    }
    return color;
}
