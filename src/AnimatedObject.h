#ifndef ANIMATEOBJ_INCLUDED
#define ANIMATEOBJ_INCLUDED

// Needs to link Eigen in the makefile with g++ -I/path/to/Eigen
#include <Eigen>
#include <Core>
#include <cmath>

#include "Ray.h"
#include "SceneObject.h"
#include "Transformation.h"
#include <sys/types.h>
#include <dirent.h>

class AnimatedObject {
    public:
        AnimatedObject(std::string driver_line);
        void set_object(SceneObject* new_obj);
        SceneObject* get_object();
        void advance_frame();
        bool has_next_frame();
        std::string get_model_file_name(int model_index);
    private:
        void read_directory(const std::string& name, std::vector<std::string>& v);
        bool filter_obj_files();

        std::string directory_name = "";
        std::vector<std::string> model_files;
        SceneObject* current_obj;

        Transformation* model_to_scene;

        unsigned current_model = 0;
};
#endif
