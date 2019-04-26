#ifndef ANIMATESPH_INCLUDED
#define ANIMATESPH_INCLUDED

// Needs to link Eigen in the makefile with g++ -I/path/to/Eigen
#include <Eigen>
#include <Core>
#include <cmath>
#include <fstream>

#include "AnimatedObject.h"
#include "Sphere.h"

struct sp_frame_setting{
    int frame_number;
    Eigen::Vector3d center_change;
    double radius_change;
};

class AnimatedSphere: public AnimatedObject {
    public:
        AnimatedSphere(Sphere* animated_asset);
        void load_animation();
        void set_object(Sphere* new_obj);
        SceneObject* get_object();
        void advance_frame();
        bool has_next_frame();

    private:
        void add_animation_section(int section_start, int section_end, std::vector<double> transform_floats);
        std::vector<double> fraction_of_transform(double divide_by, std::vector<double> transform_floats);
        std::vector<sp_frame_setting> frame_steps;
        Sphere* sphere;
};
#endif
