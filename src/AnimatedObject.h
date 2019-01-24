#ifndef ANIMATEOBJ_INCLUDED
#define ANIMATEOBJ_INCLUDED

// Needs to link Eigen in the makefile with g++ -I/path/to/Eigen
#include <Eigen>
#include <Core>
#include <cmath>

#include "Ray.h"
#include "SceneObject.h"


class AnimatedObject {
    public:
        void set_object(SceneObject* new_obj);
    protected:
        std::string directory_name;
        SceneObject* current_obj;
};
#endif
