#ifndef ANIMATEOBJ_INCLUDED
#define ANIMATEOBJ_INCLUDED

// Needs to link Eigen in the makefile with g++ -I/path/to/Eigen
#include <Eigen>
#include <Core>
#include <cmath>

#include "Ray.h"
#include "Model.h"
#include "Transformation.h"
#include <sys/types.h>
#include <dirent.h>

class AnimatedObject {
    public:
        AnimatedObject(std::string driver_line);
        AnimatedObject();
        void set_object(Model* new_obj);
        virtual Model* get_object();
        virtual void advance_frame();
        bool has_next_frame();
        std::string get_model_file_name(int model_index);

    protected:
        unsigned current_frame = 0;

    private:
        void read_directory(const std::string& name, std::vector<std::string>& v);
        bool filter_obj_files();

        std::string directory_name = "";
        std::vector<std::string> model_files;
        Model* current_obj;

        Transformation* model_to_scene;
};
#endif
