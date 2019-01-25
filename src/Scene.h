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
        Eigen::Vector3d& get_ambient();
        std::vector<SceneObject*>& get_objects();
        Camera& get_camera();
        Image& get_image();
        std::vector<Light>& get_lights();
        int get_recursion();
        bool advance_frame();
        bool has_next_frame();
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

        std::string orig_driver_file = "";
        Camera scene_camera;
        Image destination_image = Image(0, 0);
        std::vector<SceneObject*> scene_objects;
        std::vector<AnimatedObject*> animated_objects;
        std::vector<Light> scene_lights;
        Eigen::Vector3d ambient;

        int frames = 1;
        int current_frame = 0;
        int recursion_level = 0;
};
#endif
