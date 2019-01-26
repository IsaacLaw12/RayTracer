#ifndef ANIMATEOBJ_INCLUDED
#define ANIMATEOBJ_INCLUDED

// Needs to link Eigen in the makefile with g++ -I/path/to/Eigen
#include <Eigen>
#include <Core>
#include <cmath>

#include "Ray.h"
#include "SceneObject.h"
#include "Transformation.h"
#include <boost/filesystem.hpp>

class AnimatedObject {
    public:
        AnimatedObject(std::string driver_line);
        void set_object(SceneObject* new_obj);
        SceneObject* get_object();
        void advance_frame();
        bool has_next_frame();
    private:
        struct path_leaf_string;
        void read_directory(const std::string& name, std::vector<std::string>& v);

        std::string directory_name;
        std::vector<std::string> model_files;
        SceneObject* current_obj;

        Transformation* model_to_scene;

        int current_model = 0;
};
#endif
