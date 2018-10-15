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
        void add_light(std::string driver_line);

        // Methods to render image
        void rayTrace();

        std::string orig_driver_file = "";
        Camera scene_camera;
        Image destination_image = Image(0, 0);
        std::vector<Model> scene_models;
        std::vector<Light> scene_lights;
        double ambient_red;
        double ambient_green;
        double ambient_blue;
};
#endif
