#ifndef SCENE_INCLUDED
#define SCENE_INCLUDED

#include <string>
#include <vector>
#include <sstream>
#include "Model.h"
#include "Transformation.h"
#include "Camera.h"
#include "Image.h"
#include "Light.h"
#include "Sphere.h"
#include "SceneObject.h"
#include "Ray.h"


class Scene {
  public:
        Scene(std::string driver_file);
        void render_image(std::string save_image_file);
  private:
        // Methods to help load the scene
        void load_scene();
        bool valid_driver_line(std::string driver_line);
        void process_line(std::string driver_line);
        void edit_camera(std::string driver_line);
        void edit_ambient(std::string driver_line);
        void add_model(std::string driver_line);
        void add_sphere(std::string driver_line);
        void add_light(std::string driver_line);

        // Methods to render image
        void shoot_rays();
        void ray_trace(Ray& ray, Eigen::Vector3d& accum, Eigen::Vector3d& ampl, int level);
        double find_intersection(Ray& ray, SceneObject*& md, Eigen::Vector3d &hit_normal);
        Eigen::Vector3d calculate_color(Ray& ray, double t_value, SceneObject* hit_obj, Eigen::Vector3d &hit_normal, Eigen::Vector3d &accum, Eigen::Vector3d &ampl, int level);
        bool lightReachesObject(Light& light, Eigen::Vector3d intersect_pos);

        std::string orig_driver_file = "";
        Camera scene_camera;
        Image destination_image = Image(0, 0);
        std::vector<SceneObject*> scene_objects;
        //std::vector<Model*> scene_models;
        //std::vector<Sphere*> scene_spheres;
        std::vector<Light> scene_lights;
        Eigen::Vector3d ambient;

        int recursion_level = 0;
        double MISSED_T_VALUE = 1000000000;
};
#endif
