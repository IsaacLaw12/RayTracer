#ifndef BOUNDINGBOX_INCLUDED
#define BOUNDINGBOX_INCLUDED

#include <string>
#include <iostream>
#include <vector>
#include <set>
class Model;
#include "Model.h"
#include "Ray.h"
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
        std::set<int> intersected_faces(Ray& ray);
  private:
        void find_contained_faces(std::set<int> &parent_faces);
        double max(double a, double b, double c);
        double min(double a, double b, double c);
        bool triangle_intersects(Eigen::Vector3d& v0, Eigen::Vector3d& v1, Eigen::Vector3d& v2);
        bool axis_test(const Eigen::Vector3d& axis, const Eigen::Vector3d& e, const Eigen::Vector3d& v0, const Eigen::Vector3d& v1, const Eigen::Vector3d& v2, const Eigen::Vector3d& u0, const Eigen::Vector3d& u1, const Eigen::Vector3d& u2);
        void find_corners();
        void subdivide_box();
        void calculate_bounding();
        bool ray_intersects(Ray& ray);

        Eigen::MatrixXd& Vertices;
        Eigen::MatrixXi& Faces;
        std::vector<BoundingBox* > contained_boxes;
        std::set<int> contained_faces;

        // Bounding box coordinates
        Eigen::Vector3d min_corner = Eigen::Vector3d();
        Eigen::Vector3d max_corner = Eigen::Vector3d();
        Eigen::Vector3d center = Eigen::Vector3d();

        int recursion_limit;
};
#endif
