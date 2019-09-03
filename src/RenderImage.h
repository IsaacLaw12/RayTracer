#ifndef RENDERIMAGE_INCLUDED
#define RENDERIMAGE_INCLUDED
#include <queue>

#include "Scene.h"

// Needs to link Eigen in the makefile with g++ -I/path/to/Eigen
#include <Eigen>
#include <Core>

struct image_tile{
    // Defines a window in the image plane, assists the split of the total render
    // into smaller parts
    int x;
    int y;
    int x_len;
    int y_len;
};

class RenderImage {
  public:
        RenderImage(Scene* attached_scene);
        void render_image(std::string save_image_file);
  private:
        Scene* scene;
        double MISSED_T_VALUE = 1000000000;
        std::queue<image_tile> image_tiles;

        std::queue<image_tile> tile_images(int window_size);
        void render_tiles();
        void render_tile(image_tile img_t);
        void ray_trace(Ray& ray, Eigen::Vector3d& accum, Eigen::Vector3d& ampl, int level, double &t_value);
        double find_intersection(Ray& ray, SceneObject*& md, Eigen::Vector3d &hit_normal);
        void calculate_color(Ray& ray, double t_value, SceneObject* hit_obj, Eigen::Vector3d &hit_normal, Eigen::Vector3d &accum, Eigen::Vector3d &ampl, int level);
        bool lightReachesObject(Light& light, Eigen::Vector3d intersect_pos);
};
#endif
