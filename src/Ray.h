#ifndef RAY_INCLUDED
#define RAY_INCLUDED

// Needs to link Eigen in the makefile with g++ -I/path/to/Eigen
#include <Eigen>
#include <Core>


class Ray {
    public:
        Ray();
        Ray(Eigen::Vector3d start_point, Eigen::Vector3d ray_dir);
        void set_point(Eigen::Vector3d start_point);
        void set_dir(Eigen::Vector3d ray_dir);
        const Eigen::Vector3d& get_point();
        const Eigen::Vector3d& get_dir();
    private:
        double phong;
        Eigen::Vector3d point;
        Eigen::Vector3d dir;
};
#endif
