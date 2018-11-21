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
    shoot_rays();
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
    std::string recursion = "recursionLevel";
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
    }else if(!driver_line.compare(0, recursion.size(), recursion)){
        d_line >> recursion_level;
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
        SceneObject* mod = tf.get_model();
        scene_objects.push_back(mod);
    } else{
        std::cout << "Unable to extract transformation and object from " << driver_line << "\n";
    }
}

void Scene::add_sphere(std::string driver_line){
    SceneObject* sp = new Sphere(driver_line);
    //if (sp.load_successful()){
    scene_objects.push_back(sp);
    std::cout << "There are now: " << scene_objects.size() << " scene objects\n";
    //} else{
    //    std::cout << "Unable to create sphere from: " << driver_line << "\n";
    //}
}

void Scene::add_light(std::string driver_line){
    Light light(driver_line);
    scene_lights.push_back(light);
}

void Scene::shoot_rays(){
    Eigen::Vector3d ray_pt, ray_dir, color, ampl;

    std::cout << "starting ray tracing" << "\n";
    for (int i=0; i<scene_camera.pixel_width; i++){
        if (! (i%64)){
            std::cout << "Row: " << i << " / " << scene_camera.pixel_height << "\n";
        }
        for (int j=0; j<scene_camera.pixel_height; j++){
            ray_pt = scene_camera.get_pixel_position(i, j);
            ray_dir = (ray_pt - scene_camera.get_eye()).normalized();
            color << 0,0,0;
            ampl << 1,1,1;
            ray_trace(ray_pt, ray_dir, color, ampl, recursion_level);
            destination_image.write_pixel(i, j, color);
        }
     }
     std::cout << "end of ray tracing \n";
}

void Scene::ray_trace(Eigen::Vector3d& ray_pt, Eigen::Vector3d& ray_dir, Eigen::Vector3d& accum, Eigen::Vector3d& ampl, int level){
    if (scene_objects.size() == 0){
        return;
    }
    SceneObject* hit_obj = scene_objects[0];
    Eigen::Vector3d hit_normal;
    double t_value = find_intersection(ray_pt, ray_dir, hit_obj, hit_normal);
    calculate_color(ray_pt, ray_dir, t_value, hit_obj, hit_normal, accum, ampl, level);
}

double Scene::find_intersection(Eigen::Vector3d ray_pt, Eigen::Vector3d ray_dir, SceneObject*& md, Eigen::Vector3d &hit_normal){
    double epsilon = .0000000001;
    double intersect_t = MISSED_T_VALUE;
    Eigen::Vector3d hit_s_normal;
    for (auto so : scene_objects){
        double t_value = so->intersect_ray(ray_pt, ray_dir, hit_s_normal);
        if (t_value <= epsilon){
            continue;
        }
        if ((t_value < intersect_t) ){
            intersect_t = t_value;
            md = so;
            hit_normal = hit_s_normal;
        }
    }
    return intersect_t;
}

Eigen::Vector3d Scene::calculate_color(Eigen::Vector3d ray_pt, Eigen::Vector3d ray_dir, double t_value, SceneObject* hit_obj, Eigen::Vector3d &hit_normal, Eigen::Vector3d &accum, Eigen::Vector3d &ampl, int level){
    Eigen::Vector3d vector_to_light, intersect_pos, dif_refl, color;
    Eigen::Vector3d obj_intsct_vector, light_reflect_point;
    if (t_value == MISSED_T_VALUE){
        color << 0,0,0;
        return color;
    }
    color = hit_obj->get_ambient_color() * ambient;
    double light_concentration, proximity_reflection;
    intersect_pos = ray_pt + t_value * ray_dir;
    for(Light light:scene_lights){
        if (!lightReachesObject(light, intersect_pos)){
            continue;
        }
        // Diffuse reflection
        vector_to_light = (light.get_pos() - intersect_pos).normalized();
        light_concentration = hit_normal.dot(vector_to_light);
        if (light_concentration < 0){
                continue;
        }

        dif_refl =  hit_obj->get_diffuse_color() * light.get_color() * light_concentration;
        color += dif_refl;
        // Specular reflection

        // The vector from the intersect point to the camera
        obj_intsct_vector = (ray_pt - intersect_pos).normalized();
        // The point opposite the light flipped on the normal
        light_reflect_point = (2 * hit_normal.dot(vector_to_light) *  hit_normal) - vector_to_light;
        proximity_reflection = obj_intsct_vector.dot(light_reflect_point);
        if (proximity_reflection > 0.0){
            color += hit_obj->get_specular_color() * light.get_color() * std::pow(proximity_reflection, hit_obj->get_phong());
        }

    }
    accum = accum + ampl.cwiseProduct(color);
    if (level > 0){
        Eigen::Vector3d new_ray = -1 * ray_dir;
        new_ray = (2 * hit_normal.dot(new_ray) * hit_normal - new_ray).normalized();
        ampl = hit_obj->get_kr().cwiseProduct(ampl);
        ray_trace(intersect_pos, new_ray, accum, ampl, --level);
    }
    return color;
}

bool Scene::lightReachesObject(Light& light, Eigen::Vector3d intersect_pos){
    SceneObject* unused_so;
    Eigen::Vector3d unused_norm;

    double distance_to_light = (light.get_pos() - intersect_pos).norm();
    Eigen::Vector3d vector_to_light = (light.get_pos() - intersect_pos).normalized();
    double t_value = find_intersection(intersect_pos, vector_to_light, unused_so, unused_norm);
    if (t_value == MISSED_T_VALUE || (distance_to_light < t_value)){
        return true;
    }
    return false;
}
