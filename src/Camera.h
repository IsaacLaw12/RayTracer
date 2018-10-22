#ifndef CAMERA_INCLUDED
#define CAMERA_INCLUDED

// Needs to link Eigen in the makefile with g++ -I/path/to/Eigen
#include <Eigen>
#include <Core>

class Camera {
  public:
        Camera ();
        void set_eye(double x_pos, double y_pos, double z_pos);
        void set_look(double x_dir, double y_dir, double z_dir);
        void set_up(double x_dir, double y_dir, double z_dir);
        void set_focal_length(double foc_length);
        void set_bounds(double x1, double y1, double x2, double y2);
        void set_resolution(int width, int height);
        Eigen::Vector3d get_pixel_position(int x_pixel, int y_pixel);
        void calculate_axis();

        int pixel_width;
        int pixel_height;
        Eigen::Vector3d get_eye();
  private:
        Eigen::Vector3d eye = Eigen::Vector3d::Zero();
        Eigen::Vector3d look = Eigen::Vector3d::Zero();
        Eigen::Vector3d up_dir = Eigen::Vector3d::Zero();
        Eigen::Vector3d x_axis = Eigen::Vector3d::Zero();
        Eigen::Vector3d y_axis = Eigen::Vector3d::Zero();
        Eigen::Vector3d z_axis = Eigen::Vector3d::Zero();

        double focal_length = 0;

        double near_bounding_left;
        double near_bounding_right;
        double near_bounding_bottom;
        double near_bounding_top;


};
#endif
