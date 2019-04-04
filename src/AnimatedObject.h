#ifndef ANIMATEOBJ_INCLUDED
#define ANIMATEOBJ_INCLUDED

// Needs to link Eigen in the makefile with g++ -I/path/to/Eigen
#include <Eigen>
#include <Core>
#include <cmath>
#include <fstream>

#include "Ray.h"
#include "Model.h"
#include "Transformation.h"
#include <sys/types.h>
#include <dirent.h>

struct frame_setting{
    int frame_number;
    std::vector<double> transform_floats;
    bool set_as_new_base_model;
};

class AnimatedObject {
    public:
        AnimatedObject(Model* animated_asset);
        AnimatedObject();
        void load_animation();
        std::vector<std::string> extract_file_lines(std::string file_name);
        void set_object(Model* new_obj);
        virtual SceneObject* get_object();
        virtual void advance_frame();
        bool has_next_frame();
        void reset_current_frame();
        void set_start_frame(unsigned starting_frame);

    protected:
        unsigned current_frame = 0;
        std::string animation_file = "";

    private:
        void add_animation_section(int section_start, int section_end, std::vector<double> transform_floats);
        std::vector<double> fraction_of_transform(double divide_by, std::vector<double> transform_floats);
        std::vector<double> multiple_of_transform(double multiply_by, std::vector<double> transform_floats);
        std::vector<frame_setting> frame_steps;
        Model* original_obj;
        Model* current_obj;
};
#endif
