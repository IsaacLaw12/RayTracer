#include "RenderImage.h"

RenderImage::RenderImage(Scene* attached_scene){
    scene = attached_scene;
}

void RenderImage::render_image(std::string save_image_file){
    shoot_rays();
    scene->get_image().save_image(save_image_file);
}

void RenderImage::shoot_rays(){
    Eigen::Vector3d ray_pt, ray_dir, color, ampl;
    Ray ray;
    double t_value = 0;
    Camera& scene_camera = scene->get_camera();
    for (int i=0; i<scene_camera.pixel_width; i++){
        if (! (i%64)){
            std::cout << "Row: " << i << " / " << scene_camera.pixel_width << "\n";
        }
        for (int j=0; j<scene_camera.pixel_height; j++){
            ray_pt = scene_camera.get_pixel_position(i, j);
            ray_dir = (ray_pt - scene_camera.get_eye()).normalized();
            ray = Ray(ray_pt, ray_dir);
            color << 0,0,0;
            ampl << 1,1,1;
            ray_trace(ray, color, ampl, scene->get_recursion(), t_value);
            scene->get_image().write_pixel(i, j, color);
            scene->get_image().write_t_value(i, j, t_value);
        }
     }
     std::cout << "end of ray tracing \n";
}

void RenderImage::ray_trace(Ray& ray, Eigen::Vector3d& accum, Eigen::Vector3d& ampl, int level, double &t_value){
    if (scene->get_objects().size() == 0){
        return;
    }
    SceneObject* hit_obj = scene->get_objects()[0];
    Eigen::Vector3d hit_normal;
    t_value = find_intersection(ray, hit_obj, hit_normal);
    calculate_color(ray, t_value, hit_obj, hit_normal, accum, ampl, level);
}

double RenderImage::find_intersection(Ray& ray, SceneObject*& md, Eigen::Vector3d &hit_normal){
    double epsilon = .0000000001;
    double intersect_t = MISSED_T_VALUE;
    Eigen::Vector3d hit_s_normal;
    for (auto so : scene->get_objects()){
        double t_value = so->intersect_ray(ray, hit_s_normal);
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

Eigen::Vector3d RenderImage::calculate_color(Ray& ray, double t_value, SceneObject* hit_obj, Eigen::Vector3d &hit_normal, Eigen::Vector3d &accum, Eigen::Vector3d &ampl, int level){
    Eigen::Vector3d vector_to_light, intersect_pos, dif_refl, color;
    Eigen::Vector3d obj_intsct_vector, light_reflect_point;
    if (t_value == MISSED_T_VALUE){
        color << 0,0,0;
        return color;
    }
    color = hit_obj->get_ambient_color() * scene->get_ambient();
    double light_concentration, proximity_reflection;
    intersect_pos = ray.get_point() + t_value * ray.get_dir();

    for(Light light:scene->get_lights()){
        if (light.get_lighting_group() != hit_obj->get_lighting_group()){
            continue;
        }
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
        obj_intsct_vector = (ray.get_point() - intersect_pos).normalized();
        // The point opposite the light flipped on the normal
        light_reflect_point = (2 * hit_normal.dot(vector_to_light) *  hit_normal) - vector_to_light;
        proximity_reflection = obj_intsct_vector.dot(light_reflect_point);
        if (proximity_reflection > 0.0){
            color += hit_obj->get_specular_color() * light.get_color() * std::pow(proximity_reflection, hit_obj->get_phong());
        }

    }
    accum = accum + ampl.cwiseProduct(color);

    Eigen::Vector3d kr_ampl = hit_obj->get_kr() * ampl;
    Eigen::Matrix3d ko = hit_obj->get_ko();
    double opacity = (ko * Eigen::Vector3d(1,1,1)).dot(Eigen::Vector3d(1,1,1));
    double dummy_t = 0;
    // Reflections
    if (level > 0){
        Eigen::Vector3d reflect_accum = Eigen::Vector3d::Zero();
        Eigen::Vector3d w = -1 * ray.get_dir();
        Eigen::Vector3d ray_dir = (2 * hit_normal.dot(w) * hit_normal - w).normalized();
        Ray reflect_ray = Ray(intersect_pos, ray_dir);
        ray_trace(reflect_ray, reflect_accum, kr_ampl, --level, dummy_t);
        Eigen::Vector3d reflect_ampl = ko * ampl;
        accum = accum + reflect_ampl.cwiseProduct(reflect_accum);
    }
    // Calculate refraction
    if (level > 0 && opacity < 3.0){
        Eigen::Vector3d refract_accum = Eigen::Vector3d::Zero();
        Ray refract_ray = hit_obj->get_refracted_ray(ray, intersect_pos, hit_normal);
        ray_trace(refract_ray, refract_accum, kr_ampl, --level, dummy_t);
        Eigen::Vector3d refract_ampl = (Eigen::Matrix3d::Identity() - ko) * ampl;
        accum = accum + refract_ampl.cwiseProduct(refract_accum);
    }
    return color;
}

bool RenderImage::lightReachesObject(Light& light, Eigen::Vector3d intersect_pos){
    SceneObject* unused_so;
    Eigen::Vector3d unused_norm;

    double distance_to_light = (light.get_pos() - intersect_pos).norm();
    Eigen::Vector3d vector_to_light = (light.get_pos() - intersect_pos).normalized();
    Ray ray_to_light = Ray(intersect_pos, vector_to_light);
    double t_value = find_intersection(ray_to_light, unused_so, unused_norm);
    if (t_value == MISSED_T_VALUE || (distance_to_light < t_value)){
        return true;
    }
    return false;
}