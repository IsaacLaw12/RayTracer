#ifndef BOUNDINGBOX_INCLUDED
#define BOUNDINGBOX_INCLUDED

#include <string>
#include <iostream>
#include <vector>
class Model;
#include "Model.h"
#include <limits>

// Needs to link Eigen in the makefile with g++ -I/path/to/Eigen
#include <Eigen>
#include <Core>


class BoundingBox {
  public:
        BoundingBox(Eigen::MatrixXd& verts, Eigen::MatrixXi& faces, int recursionLimit);
        BoundingBox(BoundingBox* parent_bb, int recursionLimit, Eigen::Vector3d min, Eigen::Vector3d max);
        
        Eigen::Vector3d get_min_corner();
        Eigen::Vector3d get_max_corner();
        Eigen::Vector3d get_vertex(int index);
        std::vector<int> get_contained_faces();
        std::vector<int> intersected_faces(Eigen::Vector3d ray_pt, Eigen::Vector3d ray_dir);
  private:
        void find_contained_faces();
        bool point_contained(Eigen::Vector3d point);
        void find_corners();
        void subdivide_box();
        void calculate_bounding();
        bool ray_intersects(Eigen::Vector3d ray_pt, Eigen::Vector3d ray_dir);

        Eigen::MatrixXd& Vertices;
        Eigen::MatrixXi& Faces;
        std::vector<int> parent_faces; // All faces contained by the parent object 
        std::vector<BoundingBox> contained_boxes;
        std::vector<int> contained_faces;
        
        // Bounding box coordinates
        Eigen::Vector3d min_corner = Eigen::Vector3d();
        Eigen::Vector3d max_corner = Eigen::Vector3d();

        int recursion_limit;
};
#endif
